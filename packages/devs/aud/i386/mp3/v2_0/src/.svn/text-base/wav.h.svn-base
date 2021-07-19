#ifndef WAV_H_INCLUDED
#define WAV_H_INCLUDED



#define Write16BitsLowHigh(fp, i)		do{	\
	putc(i&0xff,fp);			\
	putc((i>>8)&0xff,fp);		\
}while(0)


#define Write32BitsLowHigh(fp, i)		{	\
	Write16BitsLowHigh(fp,((int)(i&0xffffL)));		\
	Write16BitsLowHigh(fp,((int)((i>>16)&0xffffL)));		\
}


int
WriteWaveHeader(FILE * const fp, const int pcmbytes,
                const int freq, const int channels, const int bits)
{
    int     bytes = (bits + 7) / 8;
 
    fwrite("RIFF", 1, 4, fp); /* label */
    Write32BitsLowHigh(fp, (pcmbytes + 44 - 8)); /* length in bytes without header */
    fwrite("WAVEfmt ", 2, 4, fp); /* 2 labels */
    Write32BitsLowHigh(fp, 16); /* length of PCM format declaration area */
    Write16BitsLowHigh(fp, 1); /* PCM */
    Write16BitsLowHigh(fp, channels); /* number of channels */
    Write32BitsLowHigh(fp, freq); /* sample frequency in [Hz] */
    Write32BitsLowHigh(fp, freq * channels * bytes); /* bytes per second */
    Write16BitsLowHigh(fp, channels * bytes); /* bytes per sample time */
    Write16BitsLowHigh(fp, bits); /* bits per sample */
    fwrite("data", 1, 4, fp); /* label */
    Write32BitsLowHigh(fp, pcmbytes); /* length in bytes of raw PCM data */

    return ferror(fp) ? -1 : 0;
}

#endif

