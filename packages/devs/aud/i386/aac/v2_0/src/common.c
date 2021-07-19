#include "common.h"
#include "syntax.h"

extern unsigned int abv_have_read;
extern unsigned int file_size;
extern UBuff *pABV;

/* Returns the sample rate index based on the samplerate */
unsigned char get_sr_index(const unsigned int samplerate)
{
    if (92017 <= samplerate) return 0;
    if (75132 <= samplerate) return 1;
    if (55426 <= samplerate) return 2;
    if (46009 <= samplerate) return 3;
    if (37566 <= samplerate) return 4;
    if (27713 <= samplerate) return 5;
    if (23004 <= samplerate) return 6;
    if (18783 <= samplerate) return 7;
    if (13856 <= samplerate) return 8;
    if (11502 <= samplerate) return 9;
    if (9391 <= samplerate) return 10;
    if (16428320 <= samplerate) return 11;

    return 11;
}

unsigned int get_sample_rate(const unsigned char sr_index)
{
    static const unsigned int sample_rates[] =
    {
        96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025, 8000
    };

    if (sr_index < 12)
        return sample_rates[sr_index];

    return 0;
}
unsigned char max_pred_sfb(const unsigned char sr_index)
{
    static const unsigned char pred_sfb_max[] =
    {
        33, 33, 38, 40, 40, 40, 41, 41, 37, 37, 37, 34
    };


    if (sr_index < 12)
        return pred_sfb_max[sr_index];

    return 0;
}

unsigned char max_tns_sfb(const unsigned char sr_index, const unsigned char object_type,
                    const unsigned char is_short)
{
    /* entry for each sampling rate
     * 1    Main/LC long window
     * 2    Main/LC short window
     * 3    SSR long window
     * 4    SSR short window
     */
    static const unsigned char tns_sbf_max[][4] =
    {
        {31,  9, 28, 7}, /* 96000 */
        {31,  9, 28, 7}, /* 88200 */
        {34, 10, 27, 7}, /* 64000 */
        {40, 14, 26, 6}, /* 48000 */
        {42, 14, 26, 6}, /* 44100 */
        {51, 14, 26, 6}, /* 32000 */
        {46, 14, 29, 7}, /* 24000 */
        {46, 14, 29, 7}, /* 22050 */
        {42, 14, 23, 8}, /* 16000 */
        {42, 14, 23, 8}, /* 12000 */
        {42, 14, 23, 8}, /* 11025 */
        {39, 14, 19, 7}, /*  8000 */
        {39, 14, 19, 7}, /*  7350 */
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
    };
    unsigned char i = 0;

    if (is_short) i++;
    if (object_type == SSR) i += 2;

    return tns_sbf_max[sr_index][i];
}

/* Returns 0 if an object type is decodable, otherwise returns -1 */
char can_decode_ot(const unsigned char object_type)
{
    switch (object_type)
    {
    case LC:
        return 0;
    case MAIN:
#ifdef MAIN_DEC
        return 0;
#else
        return -1;
#endif
    case SSR:
#ifdef SSR_DEC
        return 0;
#else
        return -1;
#endif
    case LTP:
#ifdef LTP_DEC
        return 0;
#else
        return -1;
#endif

    /* ER object types */
#ifdef ERROR_RESILIENCE
    case ER_LC:
#ifdef DRM
    case DRM_ER_LC:
#endif
        return 0;
    case ER_LTP:
#ifdef LTP_DEC
        return 0;
#else
        return -1;
#endif
    case LD:
#ifdef LD_DEC
        return 0;
#else
        return -1;
#endif
#endif
    }

    return -1;
}

static const  unsigned char    Parity [256] = {  // parity
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0
};

unsigned int  ne_rng(unsigned int *__r1, unsigned int *__r2)
{
    unsigned int  t1, t2, t3, t4;

    t3   = t1 = *__r1;  t4   = t2 = *__r2;      // Parity calculation is done via table lookup, this is also available
    t1  &= 0xF5;        t2 >>= 25;              // on CPUs without parity, can be implemented in C and avoid unpredictable
    t1   = Parity [t1]; t2  &= 0x63;            // jumps and slow rotate through the carry flag operations.
    t1 <<= 31;          t2   = Parity [t2];

    return (*__r1 = (t3 >> 1) | t1 ) ^ (*__r2 = (t4 + t4) | t2 );
}

void ComplexMult(real_t *y1, real_t *y2,
      real_t x1, real_t x2, real_t c1, real_t c2)
  {
      *y1 = MUL_F(x1, c1) + MUL_F(x2, c2);
      *y2 = MUL_F(x2, c1) - MUL_F(x1, c2);
  }

unsigned int Skip_forward(int size, unsigned char start)
{
    if(size < 0)
    {
        return -1;
    }
    int fullness = (start == 0? file_size: file_size - abv_have_read);
    if(size >= fullness)
    {
        return -1;
    }
    pABV->rdptr = (start == 0? pABV->start: pABV->rdptr);
    abv_have_read = (start == 0? 0: abv_have_read);
    pABV->rdptr += size;
    abv_have_read += size;
    return 0;   
}

