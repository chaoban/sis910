#ifndef __COMMON_H__
#define __COMMON_H__

/* Allow decoding of MAIN profile AAC */
#define MAIN_DEC
/* Allow decoding of SSR profile AAC */
//#define SSR_DEC
/* Allow decoding of LTP profile AAC */
#define LTP_DEC
/* Allow decoding of LD profile AAC */
#define LD_DEC
/* Allow decoding of Digital Radio Mondiale (DRM) */
//#define DRM
//#define DRM_PS

/* LD can't do without LTP */
#ifdef LD_DEC
#ifndef ERROR_RESILIENCE
#define ERROR_RESILIENCE
#endif
#ifndef LTP_DEC
#define LTP_DEC
#endif
#endif

#ifdef DRM
#ifndef ALLOW_SMALL_FRAMELENGTH
#define ALLOW_SMALL_FRAMELENGTH
#endif
#undef LD_DEC
#undef LTP_DEC
#undef MAIN_DEC
#undef SSR_DEC
#endif

//#define SBR_DEC
//#define SBR_LOW_POWER
//#define PS_DEC

#ifdef SBR_LOW_POWER
#undef PS_DEC
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if 0
#define assert(ex) do { \
    if (!(ex)) { \
	(void)printf("Assertion failed: file \"%s\", \
    		line %d\n", __FILE__, __LINE__); \
    } \
} while (0)
#endif

  typedef float real_t;
  #define lrintf(f) ((int)(f))

  #define MUL_R(A,B) ((A)*(B))
  #define MUL_C(A,B) ((A)*(B))
  #define MUL_F(A,B) ((A)*(B))

  #define DIV_R(A, B) ((A)/(B))
  #define DIV_C(A, B) ((A)/(B))

  #define REAL_CONST(A) ((real_t)(A))
  #define COEF_CONST(A) ((real_t)(A))
  #define Q2_CONST(A) ((real_t)(A))
  #define FRAC_CONST(A) ((real_t)(A)) /* pure fractional part */

  /* Complex multiplication */
  void ComplexMult(real_t *y1, real_t *y2,
      real_t x1, real_t x2, real_t c1, real_t c2);

typedef real_t complex_t[2];
#define RE(A) A[0]
#define IM(A) A[1]

#ifndef SBR_LOW_POWER
#define qmf_t complex_t
#define QMF_RE(A) RE(A)
#define QMF_IM(A) IM(A)
#else
#define qmf_t real_t
#define QMF_RE(A) (A)
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2 /* PI/2 */
#define M_PI_2 1.57079632679489661923
#endif

unsigned char get_sr_index(const unsigned int samplerate);
unsigned int get_sample_rate(const unsigned char sr_index);
unsigned char max_pred_sfb(const unsigned char sr_index);
unsigned char max_tns_sfb(const unsigned char sr_index, const unsigned char object_type,
                    const unsigned char is_short);
char can_decode_ot(const unsigned char object_type);
unsigned int ne_rng(unsigned int *__r1, unsigned int *__r2);
unsigned int Skip_forward(int size, unsigned char dis);

#endif
