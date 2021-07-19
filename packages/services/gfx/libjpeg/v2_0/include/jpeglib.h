/*
 * jpeglib.h
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the application interface for the JPEG library.
 * Most applications using the library need only include this file,
 * and perhaps jerror.h if they want to know the exact error codes.
 */

#ifndef JPEGLIB_H
#define JPEGLIB_H
#include <cyg/infra/diag.h>
//weijpeg
#define __wei_jpeg 1
//Debugkey
#define __DEBUG_JPEG //debug on
//#define __JPG_HW_BITBLT //use hw bitblt
#define __JPG_HW_SCALE 1 //use hw scale
//#define __JPG_R16_BITBLT//use cpu bitblt
//#define __DBG_LOG    //log
//#define __DBG_SOF //get sof
//#define __DBG_SOS
//#define __DBG_DHT
//#define __DBG_DQT
//#define __DBG_HWINF
//#define __DBG_BITSTREAM
//#define __DBG_BITSTREAM_FILEOUT
//#define __DBG_HOPPER_FILEOUT
//#define __DBG_QMATRIX_FILEOUT

#ifdef __DEBUG_JPEG
#define EPRINTF(a)  diag_printf a
#else
#define EPRINTF(a)
#endif

/*
 * First we include the configuration files that record how this
 * installation of the JPEG library is set up.  jconfig.h can be
 * generated automatically for many systems.  jmorecfg.h contains
 * manual configuration options that most people need not worry about.
 */

#ifndef JCONFIG_INCLUDED	/* in case jinclude.h already did */
//#include <jpeg/jconfig.h>		/* widely used configuration options */
#define HAVE_PROTOTYPES

/* Does your compiler support the declaration "unsigned char" ?
 * How about "unsigned short" ?
 */
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT

/* Define "void" as "char" if your compiler doesn't know about type void.
 * NOTE: be sure to define void such that "void *" represents the most general
 * pointer type, e.g., that returned by malloc().
 */
/* #define void char */

/* Define "const" as empty if your compiler doesn't know the "const" keyword.
 */
/* #define const */

/* Define this if an ordinary "char" type is unsigned.
 * If you're not sure, leaving it undefined will work at some cost in speed.
 * If you defined HAVE_UNSIGNED_CHAR then the speed difference is minimal.
 */
#undef CHAR_IS_UNSIGNED

/* Define this if your system has an ANSI-conforming <stddef.h> file.
 */
#define HAVE_STDDEF_H

/* Define this if your system has an ANSI-conforming <stdlib.h> file.
 */
#define HAVE_STDLIB_H

/* Define this if your system does not have an ANSI/SysV <string.h>,
 * but does have a BSD-style <strings.h>.
 */
#undef NEED_BSD_STRINGS

/* Define this if your system does not provide typedef size_t in any of the
 * ANSI-standard places (stddef.h, stdlib.h, or stdio.h), but places it in
 * <sys/types.h> instead.
 */
#undef NEED_SYS_TYPES_H

/* For 80x86 machines, you need to define NEED_FAR_POINTERS,
 * unless you are using a large-data memory model or 80386 flat-memory mode.
 * On less brain-damaged CPUs this symbol must not be defined.
 * (Defining this symbol causes large data structures to be referenced through
 * "far" pointers and to be allocated with a special version of malloc.)
 */
#undef NEED_FAR_POINTERS

/* Define this if your linker needs global names to be unique in less
 * than the first 15 characters.
 */
#undef NEED_SHORT_EXTERNAL_NAMES

/* Although a real ANSI C compiler can deal perfectly well with pointers to
 * unspecified structures (see "incomplete types" in the spec), a few pre-ANSI
 * and pseudo-ANSI compilers get confused.  To keep one of these bozos happy,
 * define INCOMPLETE_TYPES_BROKEN.  This is not recommended unless you
 * actually get "missing structure definition" warnings or errors while
 * compiling the JPEG code.
 */
#undef INCOMPLETE_TYPES_BROKEN


/*
 * The following options affect code selection within the JPEG library,
 * but they don't need to be visible to applications using the library.
 * To minimize application namespace pollution, the symbols won't be
 * defined unless JPEG_INTERNALS has been defined.
 */

#ifdef JPEG_INTERNALS

/* Define this if your compiler implements ">>" on signed values as a logical
 * (unsigned) shift; leave it undefined if ">>" is a signed (arithmetic) shift,
 * which is the normal and rational definition.
 */
#undef RIGHT_SHIFT_IS_UNSIGNED


#endif /* JPEG_INTERNALS */


/*
 * The remaining options do not affect the JPEG library proper,
 * but only the sample applications cjpeg/djpeg (see cjpeg.c, djpeg.c).
 * Other applications can ignore these.
 */

#ifdef JPEG_CJPEG_DJPEG

/* These defines indicate which image (non-JPEG) file formats are allowed. */

#define BMP_SUPPORTED		/* BMP image file format */
#define GIF_SUPPORTED		/* GIF image file format */
#define PPM_SUPPORTED		/* PBMPLUS PPM/PGM image file format */
#undef RLE_SUPPORTED		/* Utah RLE image file format */
#define TARGA_SUPPORTED		/* Targa image file format */

/* Define this if you want to name both input and output files on the command
 * line, rather than using stdout and optionally stdin.  You MUST do this if
 * your system can't cope with binary I/O to stdin/stdout.  See comments at
 * head of cjpeg.c or djpeg.c.
 */
#undef TWO_FILE_COMMANDLINE

/* Define this if your system needs explicit cleanup of temporary files.
 * This is crucial under MS-DOS, where the temporary "files" may be areas
 * of extended memory; on most other systems it's not as important.
 */
#undef NEED_SIGNAL_CATCHER

/* By default, we open image files with fopen(...,"rb") or fopen(...,"wb").
 * This is necessary on systems that distinguish text files from binary files,
 * and is harmless on most systems that don't.  If you have one of the rare
 * systems that complains about the "b" spec, define this symbol.
 */
#undef DONT_USE_B_MODE

/* Define this if you want percent-done progress reports from cjpeg/djpeg.
 */
#undef PROGRESS_REPORT


#endif /* JPEG_CJPEG_DJPEG */

#endif
//#include <jpeg/jmorecfg.h>		/* seldom changed options */
#define BITS_IN_JSAMPLE  8	/* use 8 or 12 */


/*
 * Maximum number of components (color channels) allowed in JPEG image.
 * To meet the letter of the JPEG spec, set this to 255.  However, darn
 * few applications need more than 4 channels (maybe 5 for CMYK + alpha
 * mask).  We recommend 10 as a reasonable compromise; use 4 if you are
 * really short on memory.  (Each allowed component costs a hundred or so
 * bytes of storage, whether actually used in an image or not.)
 */

#define MAX_COMPONENTS  10	/* maximum number of image components */


/*
 * Basic data types.
 * You may need to change these if you have a machine with unusual data
 * type sizes; for example, "char" not 8 bits, "short" not 16 bits,
 * or "long" not 32 bits.  We don't care whether "int" is 16 or 32 bits,
 * but it had better be at least 16.
 */

/* Representation of a single sample (pixel element value).
 * We frequently allocate large arrays of these, so it's important to keep
 * them small.  But if you have memory to burn and access to char or short
 * arrays is very slow on your hardware, you might want to change these.
 */

#if BITS_IN_JSAMPLE == 8
/* JSAMPLE should be the smallest type that will hold the values 0..255.
 * You can use a signed char by having GETJSAMPLE mask it with 0xFF.
 */

#ifdef HAVE_UNSIGNED_CHAR

typedef unsigned char JSAMPLE;
#define GETJSAMPLE(value)  ((int) (value))

#else /* not HAVE_UNSIGNED_CHAR */

typedef char JSAMPLE;
#ifdef CHAR_IS_UNSIGNED
#define GETJSAMPLE(value)  ((int) (value))
#else
#define GETJSAMPLE(value)  ((int) (value) & 0xFF)
#endif /* CHAR_IS_UNSIGNED */

#endif /* HAVE_UNSIGNED_CHAR */

#define MAXJSAMPLE	255
#define CENTERJSAMPLE	128

#endif /* BITS_IN_JSAMPLE == 8 */


#if BITS_IN_JSAMPLE == 12
/* JSAMPLE should be the smallest type that will hold the values 0..4095.
 * On nearly all machines "short" will do nicely.
 */

typedef short JSAMPLE;
#define GETJSAMPLE(value)  ((int) (value))

#define MAXJSAMPLE	4095
#define CENTERJSAMPLE	2048

#endif /* BITS_IN_JSAMPLE == 12 */


/* Representation of a DCT frequency coefficient.
 * This should be a signed value of at least 16 bits; "short" is usually OK.
 * Again, we allocate large arrays of these, but you can change to int
 * if you have memory to burn and "short" is really slow.
 */

typedef short JCOEF;


/* Compressed datastreams are represented as arrays of JOCTET.
 * These must be EXACTLY 8 bits wide, at least once they are written to
 * external storage.  Note that when using the stdio data source/destination
 * managers, this is also the data type passed to fread/fwrite.
 */

#ifdef HAVE_UNSIGNED_CHAR

typedef unsigned char JOCTET;
#define GETJOCTET(value)  (value)

#else /* not HAVE_UNSIGNED_CHAR */

typedef char JOCTET;
#ifdef CHAR_IS_UNSIGNED
#define GETJOCTET(value)  (value)
#else
#define GETJOCTET(value)  ((value) & 0xFF)
#endif /* CHAR_IS_UNSIGNED */

#endif /* HAVE_UNSIGNED_CHAR */


/* These typedefs are used for various table entries and so forth.
 * They must be at least as wide as specified; but making them too big
 * won't cost a huge amount of memory, so we don't provide special
 * extraction code like we did for JSAMPLE.  (In other words, these
 * typedefs live at a different point on the speed/space tradeoff curve.)
 */

/* UINT8 must hold at least the values 0..255. */

#ifdef HAVE_UNSIGNED_CHAR
typedef unsigned char UINT8;
#else /* not HAVE_UNSIGNED_CHAR */
#ifdef CHAR_IS_UNSIGNED
typedef char UINT8;
#else /* not CHAR_IS_UNSIGNED */
typedef short UINT8;
#endif /* CHAR_IS_UNSIGNED */
#endif /* HAVE_UNSIGNED_CHAR */

/* UINT16 must hold at least the values 0..65535. */

#ifdef HAVE_UNSIGNED_SHORT
typedef unsigned short UINT16;
#else /* not HAVE_UNSIGNED_SHORT */
typedef unsigned int UINT16;
#endif /* HAVE_UNSIGNED_SHORT */

/* INT16 must hold at least the values -32768..32767. */

#ifndef XMD_H			/* X11/xmd.h correctly defines INT16 */
typedef short INT16;
#endif

/* INT32 must hold at least signed 32-bit values. */

#ifndef XMD_H			/* X11/xmd.h correctly defines INT32 */
typedef long INT32;
#endif

/* Datatype used for image dimensions.  The JPEG standard only supports
 * images up to 64K*64K due to 16-bit fields in SOF markers.  Therefore
 * "unsigned int" is sufficient on all machines.  However, if you need to
 * handle larger images and you don't mind deviating from the spec, you
 * can change this datatype.
 */

typedef unsigned int JDIMENSION;

#define JPEG_MAX_DIMENSION  65500L  /* a tad under 64K to prevent overflows */


/* These macros are used in all function definitions and extern declarations.
 * You could modify them if you need to change function linkage conventions;
 * in particular, you'll need to do that to make the library a Windows DLL.
 * Another application is to make all functions global for use with debuggers
 * or code profilers that require it.
 */

/* a function called through method pointers: */
#define METHODDEF(type)		static type
/* a function used only in its module: */
#define LOCAL(type)		static type
/* a function referenced thru EXTERNs: */
#define GLOBAL(type)		type
/* a reference to a GLOBAL function: */
#define EXTERN(type)		extern type


/* This macro is used to declare a "method", that is, a function pointer.
 * We want to supply prototype parameters if the compiler can cope.
 * Note that the arglist parameter must be parenthesized!
 * Again, you can customize this if you need special linkage keywords.
 */

#ifdef HAVE_PROTOTYPES
#define JMETHOD(type,methodname,arglist)  type (*methodname) arglist
#else
#define JMETHOD(type,methodname,arglist)  type (*methodname) ()
#endif


/* Here is the pseudo-keyword for declaring pointers that must be "far"
 * on 80x86 machines.  Most of the specialized coding for 80x86 is handled
 * by just saying "FAR *" where such a pointer is needed.  In a few places
 * explicit coding is needed; see uses of the NEED_FAR_POINTERS symbol.
 */

#ifdef NEED_FAR_POINTERS
#define FAR  far
#else
#define FAR
#endif


/*
 * On a few systems, type boolean and/or its values FALSE, TRUE may appear
 * in standard header files.  Or you may have conflicts with application-
 * specific header files that you want to include together with these files.
 * Defining HAVE_BOOLEAN before including jpeglib.h should make it work.
 */

#ifndef HAVE_BOOLEAN
typedef int boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif


/*
 * The remaining options affect code selection within the JPEG library,
 * but they don't need to be visible to most applications using the library.
 * To minimize application namespace pollution, the symbols won't be
 * defined unless JPEG_INTERNALS or JPEG_INTERNAL_OPTIONS has been defined.
 */

#ifdef JPEG_INTERNALS
#define JPEG_INTERNAL_OPTIONS
#endif

#ifdef JPEG_INTERNAL_OPTIONS


/*
 * These defines indicate whether to include various optional functions.
 * Undefining some of these symbols will produce a smaller but less capable
 * library.  Note that you can leave certain source files out of the
 * compilation/linking process if you've #undef'd the corresponding symbols.
 * (You may HAVE to do that if your compiler doesn't like null source files.)
 */

/* Arithmetic coding is unsupported for legal reasons.  Complaints to IBM. */

/* Capability options common to encoder and decoder: */

#define DCT_ISLOW_SUPPORTED	/* slow but accurate integer algorithm */
#define DCT_IFAST_SUPPORTED	/* faster, less accurate integer method */
#define DCT_FLOAT_SUPPORTED	/* floating-point: accurate, fast on fast HW */
/* Encoder capability options: */

#undef  C_ARITH_CODING_SUPPORTED    /* Arithmetic coding back end? */
#define C_MULTISCAN_FILES_SUPPORTED /* Multiple-scan JPEG files? */
#define C_PROGRESSIVE_SUPPORTED	    /* Progressive JPEG? (Requires MULTISCAN)*/
#define ENTROPY_OPT_SUPPORTED	    /* Optimization of entropy coding parms? */
/* Note: if you selected 12-bit data precision, it is dangerous to turn off
 * ENTROPY_OPT_SUPPORTED.  The standard Huffman tables are only good for 8-bit
 * precision, so jchuff.c normally uses entropy optimization to compute
 * usable tables for higher precision.  If you don't want to do optimization,
 * you'll have to supply different default Huffman tables.
 * The exact same statements apply for progressive JPEG: the default tables
 * don't work for progressive mode.  (This may get fixed, however.)
 */
#define INPUT_SMOOTHING_SUPPORTED   /* Input image smoothing option? */

/* Decoder capability options: */

#undef  D_ARITH_CODING_SUPPORTED    /* Arithmetic coding back end? */
#define D_MULTISCAN_FILES_SUPPORTED /* Multiple-scan JPEG files? */
#define D_PROGRESSIVE_SUPPORTED	    /* Progressive JPEG? (Requires MULTISCAN)*/
#define SAVE_MARKERS_SUPPORTED	    /* jpeg_save_markers() needed? */
#define BLOCK_SMOOTHING_SUPPORTED   /* Block smoothing? (Progressive only) */
#define IDCT_SCALING_SUPPORTED	    /* Output rescaling via IDCT? */
#undef  UPSAMPLE_SCALING_SUPPORTED  /* Output rescaling at upsample stage? */
#define UPSAMPLE_MERGING_SUPPORTED  /* Fast path for sloppy upsampling? */
#define QUANT_1PASS_SUPPORTED	    /* 1-pass color quantization? */
#define QUANT_2PASS_SUPPORTED	    /* 2-pass color quantization? */

/* more capability options later, no doubt */


/*
 * Ordering of RGB data in scanlines passed to or from the application.
 * If your application wants to deal with data in the order B,G,R, just
 * change these macros.  You can also deal with formats such as R,G,B,X
 * (one extra byte per pixel) by changing RGB_PIXELSIZE.  Note that changing
 * the offsets will also change the order in which colormap data is organized.
 * RESTRICTIONS:
 * 1. The sample applications cjpeg,djpeg do NOT support modified RGB formats.
 * 2. These macros only affect RGB<=>YCbCr color conversion, so they are not
 *    useful if you are using JPEG color spaces other than YCbCr or grayscale.
 * 3. The color quantizer modules will not behave desirably if RGB_PIXELSIZE
 *    is not 3 (they don't understand about dummy color components!).  So you
 *    can't use color quantization if you change that value.
 */

#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
#define RGB_GREEN	1	/* Offset of Green */
#define RGB_BLUE	2	/* Offset of Blue */
#define RGB_PIXELSIZE	3	/* JSAMPLEs per RGB scanline element */
#define RGB_ALPHA       4

/* Definitions for speed-related optimizations. */


/* If your compiler supports inline functions, define INLINE
 * as the inline keyword; otherwise define it as empty.
 */

#ifndef INLINE
#ifdef __GNUC__			/* for instance, GNU C knows about inline */
#define INLINE __inline__
#endif
#ifndef INLINE
#define INLINE			/* default is to define it as empty */
#endif
#endif


/* On some machines (notably 68000 series) "int" is 32 bits, but multiplying
 * two 16-bit shorts is faster than multiplying two ints.  Define MULTIPLIER
 * as short on such a machine.  MULTIPLIER must be at least 16 bits wide.
 */

#ifndef MULTIPLIER
#define MULTIPLIER  int		/* type for fastest integer multiply */
#endif


/* FAST_FLOAT should be either float or double, whichever is done faster
 * by your compiler.  (Note that this type is only used in the floating point
 * DCT routines, so it only matters if you've defined DCT_FLOAT_SUPPORTED.)
 * Typically, float is faster in ANSI C compilers, while double is faster in
 * pre-ANSI compilers (because they insist on converting to double anyway).
 * The code below therefore chooses float if we have ANSI-style prototypes.
 */

#ifndef FAST_FLOAT
#ifdef HAVE_PROTOTYPES
#define FAST_FLOAT  float
#else
#define FAST_FLOAT  double
#endif
#endif

#endif /* JPEG_INTERNAL_OPTIONS */

/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEG_LIB_VERSION >= 60".
 */

#define JPEG_LIB_VERSION  62	/* Version 6b */


/* Various constants determining the sizes of things.
 * All of these are specified by the JPEG standard, so don't change them
 * if you want to be compatible.
 */

#define DCTSIZE		    8	/* The basic DCT block is 8x8 samples */
#define DCTSIZE2	    64	/* DCTSIZE squared; # of elements in a block */
#define NUM_QUANT_TBLS      4	/* Quantization tables are numbered 0..3 */
#define NUM_HUFF_TBLS       4	/* Huffman tables are numbered 0..3 */
#define NUM_ARITH_TBLS      16	/* Arith-coding tables are numbered 0..15 */
#define MAX_COMPS_IN_SCAN   4	/* JPEG limit on # of components in one scan */
#define MAX_SAMP_FACTOR     4	/* JPEG limit on sampling factors */
/* Unfortunately, some bozo at Adobe saw no reason to be bound by the standard;
 * the PostScript DCT filter can emit files with many more than 10 blocks/MCU.
 * If you happen to run across such a file, you can up D_MAX_BLOCKS_IN_MCU
 * to handle it.  We even let you do this from the jconfig.h file.  However,
 * we strongly discourage changing C_MAX_BLOCKS_IN_MCU; just because Adobe
 * sometimes emits noncompliant files doesn't mean you should too.
 */
#define C_MAX_BLOCKS_IN_MCU   10 /* compressor's limit on blocks per MCU */
#ifndef D_MAX_BLOCKS_IN_MCU
#define D_MAX_BLOCKS_IN_MCU   10 /* decompressor's limit on blocks per MCU */
#endif


/* Data structures for images (arrays of samples and of DCT coefficients).
 * On 80x86 machines, the image arrays are too big for near pointers,
 * but the pointer arrays can fit in near memory.
 */

typedef JSAMPLE FAR *JSAMPROW;	/* ptr to one image row of pixel samples. */
typedef JSAMPROW *JSAMPARRAY;	/* ptr to some rows (a 2-D sample array) */
typedef JSAMPARRAY *JSAMPIMAGE;	/* a 3-D sample array: top index is color */

typedef JCOEF JBLOCK[DCTSIZE2];	/* one block of coefficients */
typedef JBLOCK FAR *JBLOCKROW;	/* pointer to one row of coefficient blocks */
typedef JBLOCKROW *JBLOCKARRAY;		/* a 2-D array of coefficient blocks */
typedef JBLOCKARRAY *JBLOCKIMAGE;	/* a 3-D array of coefficient blocks */

typedef JCOEF FAR *JCOEFPTR;	/* useful in a couple of places */


/* Types for JPEG compression parameters and working tables. */


/* DCT coefficient quantization tables. */

typedef struct {
  /* This array gives the coefficient quantizers in natural array order
   * (not the zigzag order in which they are stored in a JPEG DQT marker).
   * CAUTION: IJG versions prior to v6a kept this array in zigzag order.
   */
  UINT16 quantval[DCTSIZE2];	/* quantization step for each coefficient */
  /* This field is used only during compression.  It's initialized FALSE when
   * the table is created, and set TRUE when it's been output to the file.
   * You could suppress output of a table by setting this to TRUE.
   * (See jpeg_suppress_tables for an example.)
   */
  boolean sent_table;		/* TRUE when table has been output */
} JQUANT_TBL;


/* Huffman coding tables. */

typedef struct {
  /* These two fields directly represent the contents of a JPEG DHT marker */
  UINT8 bits[17];		/* bits[k] = # of symbols with codes of */
				/* length k bits; bits[0] is unused */
  UINT8 huffval[256];		/* The symbols, in order of incr code length */
  /* This field is used only during compression.  It's initialized FALSE when
   * the table is created, and set TRUE when it's been output to the file.
   * You could suppress output of a table by setting this to TRUE.
   * (See jpeg_suppress_tables for an example.)
   */
  boolean sent_table;		/* TRUE when table has been output */
} JHUFF_TBL;


/* Basic info about one component (color channel). */

typedef struct {
  /* These values are fixed over the whole image. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOF marker. */
  int component_id;		/* identifier for this component (0..255) */
  int component_index;		/* its index in SOF or cinfo->comp_info[] */
  int h_samp_factor;		/* horizontal sampling factor (1..4) */
  int v_samp_factor;		/* vertical sampling factor (1..4) */
  int quant_tbl_no;		/* quantization table selector (0..3) */
  /* These values may vary between scans. */
  /* For compression, they must be supplied by parameter setup; */
  /* for decompression, they are read from the SOS marker. */
  /* The decompressor output side may not use these variables. */
  int dc_tbl_no;		/* DC entropy table selector (0..3) */
  int ac_tbl_no;		/* AC entropy table selector (0..3) */
  
  /* Remaining fields should be treated as private by applications. */
  
  /* These values are computed during compression or decompression startup: */
  /* Component's size in DCT blocks.
   * Any dummy blocks added to complete an MCU are not counted; therefore
   * these values do not depend on whether a scan is interleaved or not.
   */
  JDIMENSION width_in_blocks;
  JDIMENSION height_in_blocks;
  /* Size of a DCT block in samples.  Always DCTSIZE for compression.
   * For decompression this is the size of the output from one DCT block,
   * reflecting any scaling we choose to apply during the IDCT step.
   * Values of 1,2,4,8 are likely to be supported.  Note that different
   * components may receive different IDCT scalings.
   */
  int DCT_scaled_size;
  /* The downsampled dimensions are the component's actual, unpadded number
   * of samples at the main buffer (preprocessing/compression interface), thus
   * downsampled_width = ceil(image_width * Hi/Hmax)
   * and similarly for height.  For decompression, IDCT scaling is included, so
   * downsampled_width = ceil(image_width * Hi/Hmax * DCT_scaled_size/DCTSIZE)
   */
  JDIMENSION downsampled_width;	 /* actual width in samples */
  JDIMENSION downsampled_height; /* actual height in samples */
  /* This flag is used only for decompression.  In cases where some of the
   * components will be ignored (eg grayscale output from YCbCr image),
   * we can skip most computations for the unused components.
   */
  boolean component_needed;	/* do we need the value of this component? */

  /* These values are computed before starting a scan of the component. */
  /* The decompressor output side may not use these variables. */
  int MCU_width;		/* number of blocks per MCU, horizontally */
  int MCU_height;		/* number of blocks per MCU, vertically */
  int MCU_blocks;		/* MCU_width * MCU_height */
  int MCU_sample_width;		/* MCU width in samples, MCU_width*DCT_scaled_size */
  int last_col_width;		/* # of non-dummy blocks across in last MCU */
  int last_row_height;		/* # of non-dummy blocks down in last MCU */

  /* Saved quantization table for component; NULL if none yet saved.
   * See jdinput.c comments about the need for this information.
   * This field is currently used only for decompression.
   */
  JQUANT_TBL * quant_table;

  /* Private per-component storage for DCT or IDCT subsystem. */
  void * dct_table;
} jpeg_component_info;


/* The script for encoding a multiple-scan file is an array of these: */

typedef struct {
  int comps_in_scan;		/* number of components encoded in this scan */
  int component_index[MAX_COMPS_IN_SCAN]; /* their SOF/comp_info[] indexes */
  int Ss, Se;			/* progressive JPEG spectral selection parms */
  int Ah, Al;			/* progressive JPEG successive approx. parms */
} jpeg_scan_info;

/* The decompressor can save APPn and COM markers in a list of these: */

typedef struct jpeg_marker_struct FAR * jpeg_saved_marker_ptr;

struct jpeg_marker_struct {
  jpeg_saved_marker_ptr next;	/* next in list, or NULL */
  UINT8 marker;			/* marker code: JPEG_COM, or JPEG_APP0+n */
  unsigned int original_length;	/* # bytes of data in the file */
  unsigned int data_length;	/* # bytes of data saved at data[] */
  JOCTET FAR * data;		/* the data contained in the marker */
  /* the marker length word is not counted in data_length or original_length */
};

/* Known color spaces. */

typedef enum {
	JCS_UNKNOWN,		/* error/unspecified */
	JCS_GRAYSCALE,		/* monochrome */
	JCS_RGB,		/* red/green/blue */
	JCS_YCbCr,		/* Y/Cb/Cr (also known as YUV) */
	JCS_CMYK,		/* C/M/Y/K */
	JCS_YCCK		/* Y/Cb/Cr/K */
} J_COLOR_SPACE;

/* DCT/IDCT algorithm options. */

typedef enum {
	JDCT_ISLOW,		/* slow but accurate integer algorithm */
	JDCT_IFAST,		/* faster, less accurate integer method */
	JDCT_FLOAT		/* floating-point: accurate, fast on fast HW */
} J_DCT_METHOD;

#ifndef JDCT_DEFAULT		/* may be overridden in jconfig.h */
#define JDCT_DEFAULT  JDCT_ISLOW
#endif
#ifndef JDCT_FASTEST		/* may be overridden in jconfig.h */
#define JDCT_FASTEST  JDCT_IFAST
#endif

/* Dithering options for decompression. */

typedef enum {
	JDITHER_NONE,		/* no dithering */
	JDITHER_ORDERED,	/* simple ordered dither */
	JDITHER_FS		/* Floyd-Steinberg error diffusion dither */
} J_DITHER_MODE;


/* Common fields between JPEG compression and decompression master structs. */

#define jpeg_common_fields \
  struct jpeg_error_mgr * err;	/* Error handler module */\
  struct jpeg_memory_mgr * mem;	/* Memory manager module */\
  struct jpeg_progress_mgr * progress; /* Progress monitor, or NULL if none */\
  void * client_data;		/* Available for use by application */\
  boolean is_decompressor;	/* So common code can tell which is which */\
  int global_state		/* For checking call sequence validity */

/* Routines that are to be used by both halves of the library are declared
 * to receive a pointer to this structure.  There are no actual instances of
 * jpeg_common_struct, only of jpeg_compress_struct and jpeg_decompress_struct.
 */
struct jpeg_common_struct {
  jpeg_common_fields;		/* Fields common to both master struct types */
  /* Additional fields follow in an actual jpeg_compress_struct or
   * jpeg_decompress_struct.  All three structs must agree on these
   * initial fields!  (This would be a lot cleaner in C++.)
   */
};
#ifdef __wei_jpeg
//add type for sis910 hwused
typedef enum 
{
    JFmt_Unknow,
    JFmt_YUV411,
    JFmt_YUV422,
    JFmt_YUV444,
    JFmt_YUV422v,
    JFmt_YUV400
}J_Sample_Fmt;

struct bitstream_src_mgr
{
    size_t nsize;
    void* base;
};

struct  hw_jpeg_dst_mgr
{
   unsigned int nwidth;
   unsigned int nheight;
   unsigned int nbpp;//1 2 4
   unsigned int npitch;
   unsigned int nmcuwidth;
   void*    base;    
};

struct jpeg_hw_struct 
{
    struct bitstream_src_mgr srcbitstream;
    struct bitstream_src_mgr hopper;
    struct bitstream_src_mgr qmatrix;
    unsigned int nMcuwidth; //mcu number using y comp
    unsigned int nMcuheight;//mcu number using y comp
    unsigned int nblock_height;//each mcu height
    unsigned int nblock_width;
    unsigned int ninterval;
    unsigned int nleading; //due to 32B DMA align
    unsigned int nbloknum; //64 pixel per block = 4 * 16 mcu
    J_Sample_Fmt eSampleFmt;//422 or others
    struct hw_jpeg_dst_mgr dst_rgba;
    struct hw_jpeg_dst_mgr dst_y;
    struct hw_jpeg_dst_mgr dst_uv;
    int bdma_8x;//dma limited to 2k*2k, if breakup, setup this
    int nSafeMode;//for hw safe mode
    cyg_uint8 down_sample_rate;
    boolean filter_used;
};
typedef struct jpeg_hw_struct * j_hw_ptr;
#endif //__wei_jpeg

typedef struct jpeg_common_struct * j_common_ptr;
typedef struct jpeg_compress_struct * j_compress_ptr;
typedef struct jpeg_decompress_struct * j_decompress_ptr;

/* Master record for a compression instance */

struct jpeg_compress_struct {
  jpeg_common_fields;		/* Fields shared with jpeg_decompress_struct */

  /* Destination for compressed data */
  struct jpeg_destination_mgr * dest;

  /* Description of source image --- these fields must be filled in by
   * outer application before starting compression.  in_color_space must
   * be correct before you can even call jpeg_set_defaults().
   */

  JDIMENSION image_width;	/* input image width */
  JDIMENSION image_height;	/* input image height */
  int input_components;		/* # of color components in input image */
  J_COLOR_SPACE in_color_space;	/* colorspace of input image */

  double input_gamma;		/* image gamma of input image */

  /* Compression parameters --- these fields must be set before calling
   * jpeg_start_compress().  We recommend calling jpeg_set_defaults() to
   * initialize everything to reasonable defaults, then changing anything
   * the application specifically wants to change.  That way you won't get
   * burnt when new parameters are added.  Also note that there are several
   * helper routines to simplify changing parameters.
   */

  int data_precision;		/* bits of precision in image data */

  int num_components;		/* # of color components in JPEG image */
  J_COLOR_SPACE jpeg_color_space; /* colorspace of JPEG image */

  jpeg_component_info * comp_info;
  /* comp_info[i] describes component that appears i'th in SOF */
  
  JQUANT_TBL * quant_tbl_ptrs[NUM_QUANT_TBLS];
  /* ptrs to coefficient quantization tables, or NULL if not defined */
  
  JHUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
  JHUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
  /* ptrs to Huffman coding tables, or NULL if not defined */
  
  UINT8 arith_dc_L[NUM_ARITH_TBLS]; /* L values for DC arith-coding tables */
  UINT8 arith_dc_U[NUM_ARITH_TBLS]; /* U values for DC arith-coding tables */
  UINT8 arith_ac_K[NUM_ARITH_TBLS]; /* Kx values for AC arith-coding tables */

  int num_scans;		/* # of entries in scan_info array */
  const jpeg_scan_info * scan_info; /* script for multi-scan file, or NULL */
  /* The default value of scan_info is NULL, which causes a single-scan
   * sequential JPEG file to be emitted.  To create a multi-scan file,
   * set num_scans and scan_info to point to an array of scan definitions.
   */

  boolean raw_data_in;		/* TRUE=caller supplies downsampled data */
  boolean arith_code;		/* TRUE=arithmetic coding, FALSE=Huffman */
  boolean optimize_coding;	/* TRUE=optimize entropy encoding parms */
  boolean CCIR601_sampling;	/* TRUE=first samples are cosited */
  int smoothing_factor;		/* 1..100, or 0 for no input smoothing */
  J_DCT_METHOD dct_method;	/* DCT algorithm selector */

  /* The restart interval can be specified in absolute MCUs by setting
   * restart_interval, or in MCU rows by setting restart_in_rows
   * (in which case the correct restart_interval will be figured
   * for each scan).
   */
  unsigned int restart_interval; /* MCUs per restart, or 0 for no restart */
  int restart_in_rows;		/* if > 0, MCU rows per restart interval */

  /* Parameters controlling emission of special markers. */

  boolean write_JFIF_header;	/* should a JFIF marker be written? */
  UINT8 JFIF_major_version;	/* What to write for the JFIF version number */
  UINT8 JFIF_minor_version;
  /* These three values are not used by the JPEG code, merely copied */
  /* into the JFIF APP0 marker.  density_unit can be 0 for unknown, */
  /* 1 for dots/inch, or 2 for dots/cm.  Note that the pixel aspect */
  /* ratio is defined by X_density/Y_density even when density_unit=0. */
  UINT8 density_unit;		/* JFIF code for pixel size units */
  UINT16 X_density;		/* Horizontal pixel density */
  UINT16 Y_density;		/* Vertical pixel density */
  boolean write_Adobe_marker;	/* should an Adobe marker be written? */
  
  /* State variable: index of next scanline to be written to
   * jpeg_write_scanlines().  Application may use this to control its
   * processing loop, e.g., "while (next_scanline < image_height)".
   */

  JDIMENSION next_scanline;	/* 0 .. image_height-1  */

  /* Remaining fields are known throughout compressor, but generally
   * should not be touched by a surrounding application.
   */

  /*
   * These fields are computed during compression startup
   */
  boolean progressive_mode;	/* TRUE if scan script uses progressive mode */
  int max_h_samp_factor;	/* largest h_samp_factor */
  int max_v_samp_factor;	/* largest v_samp_factor */

  JDIMENSION total_iMCU_rows;	/* # of iMCU rows to be input to coef ctlr */
  /* The coefficient controller receives data in units of MCU rows as defined
   * for fully interleaved scans (whether the JPEG file is interleaved or not).
   * There are v_samp_factor * DCTSIZE sample rows of each component in an
   * "iMCU" (interleaved MCU) row.
   */
  
  /*
   * These fields are valid during any one scan.
   * They describe the components and MCUs actually appearing in the scan.
   */
  int comps_in_scan;		/* # of JPEG components in this scan */
  jpeg_component_info * cur_comp_info[MAX_COMPS_IN_SCAN];
  /* *cur_comp_info[i] describes component that appears i'th in SOS */
  
  JDIMENSION MCUs_per_row;	/* # of MCUs across the image */
  JDIMENSION MCU_rows_in_scan;	/* # of MCU rows in the image */
  
  int blocks_in_MCU;		/* # of DCT blocks per MCU */
  int MCU_membership[C_MAX_BLOCKS_IN_MCU];
  /* MCU_membership[i] is index in cur_comp_info of component owning */
  /* i'th block in an MCU */

  int Ss, Se, Ah, Al;		/* progressive JPEG parameters for scan */

  /*
   * Links to compression subobjects (methods and private variables of modules)
   */
  struct jpeg_comp_master * master;
  struct jpeg_c_main_controller * main;
  struct jpeg_c_prep_controller * prep;
  struct jpeg_c_coef_controller * coef;
  struct jpeg_marker_writer * marker;
  struct jpeg_color_converter * cconvert;
  struct jpeg_downsampler * downsample;
  struct jpeg_forward_dct * fdct;
  struct jpeg_entropy_encoder * entropy;
  jpeg_scan_info * script_space; /* workspace for jpeg_simple_progression */
  int script_space_size;
};


/* Master record for a decompression instance */

struct jpeg_decompress_struct {
  jpeg_common_fields;		/* Fields shared with jpeg_compress_struct */

  /* Source of compressed data */
  struct jpeg_source_mgr * src;

  /* Basic description of image --- filled in by jpeg_read_header(). */
  /* Application may inspect these values to decide how to process image. */

  JDIMENSION image_width;	/* nominal image width (from SOF marker) */
  JDIMENSION image_height;	/* nominal image height */
  int num_components;		/* # of color components in JPEG image */
  J_COLOR_SPACE jpeg_color_space; /* colorspace of JPEG image */

  /* Decompression processing parameters --- these fields must be set before
   * calling jpeg_start_decompress().  Note that jpeg_read_header() initializes
   * them to default values.
   */

  J_COLOR_SPACE out_color_space; /* colorspace for output */

  unsigned int scale_num, scale_denom; /* fraction by which to scale image */

  double output_gamma;		/* image gamma wanted in output */

  boolean buffered_image;	/* TRUE=multiple output passes */
  boolean raw_data_out;		/* TRUE=downsampled data wanted */

  J_DCT_METHOD dct_method;	/* IDCT algorithm selector */
  boolean do_fancy_upsampling;	/* TRUE=apply fancy upsampling */
  boolean do_block_smoothing;	/* TRUE=apply interblock smoothing */

  boolean quantize_colors;	/* TRUE=colormapped output wanted */
  /* the following are ignored if not quantize_colors: */
  J_DITHER_MODE dither_mode;	/* type of color dithering to use */
  boolean two_pass_quantize;	/* TRUE=use two-pass color quantization */
  int desired_number_of_colors;	/* max # colors to use in created colormap */
  /* these are significant only in buffered-image mode: */
  boolean enable_1pass_quant;	/* enable future use of 1-pass quantizer */
  boolean enable_external_quant;/* enable future use of external colormap */
  boolean enable_2pass_quant;	/* enable future use of 2-pass quantizer */

  /* Description of actual output image that will be returned to application.
   * These fields are computed by jpeg_start_decompress().
   * You can also use jpeg_calc_output_dimensions() to determine these values
   * in advance of calling jpeg_start_decompress().
   */

  JDIMENSION output_width;	/* scaled image width */
  JDIMENSION output_height;	/* scaled image height */
  int out_color_components;	/* # of color components in out_color_space */
  int output_components;	/* # of color components returned */
  /* output_components is 1 (a colormap index) when quantizing colors;
   * otherwise it equals out_color_components.
   */
  int rec_outbuf_height;	/* min recommended height of scanline buffer */
  /* If the buffer passed to jpeg_read_scanlines() is less than this many rows
   * high, space and time will be wasted due to unnecessary data copying.
   * Usually rec_outbuf_height will be 1 or 2, at most 4.
   */

  /* When quantizing colors, the output colormap is described by these fields.
   * The application can supply a colormap by setting colormap non-NULL before
   * calling jpeg_start_decompress; otherwise a colormap is created during
   * jpeg_start_decompress or jpeg_start_output.
   * The map has out_color_components rows and actual_number_of_colors columns.
   */
  int actual_number_of_colors;	/* number of entries in use */
  JSAMPARRAY colormap;		/* The color map as a 2-D pixel array */

  /* State variables: these variables indicate the progress of decompression.
   * The application may examine these but must not modify them.
   */

  /* Row index of next scanline to be read from jpeg_read_scanlines().
   * Application may use this to control its processing loop, e.g.,
   * "while (output_scanline < output_height)".
   */
  JDIMENSION output_scanline;	/* 0 .. output_height-1  */

  /* Current input scan number and number of iMCU rows completed in scan.
   * These indicate the progress of the decompressor input side.
   */
  int input_scan_number;	/* Number of SOS markers seen so far */
  JDIMENSION input_iMCU_row;	/* Number of iMCU rows completed */

  /* The "output scan number" is the notional scan being displayed by the
   * output side.  The decompressor will not allow output scan/row number
   * to get ahead of input scan/row, but it can fall arbitrarily far behind.
   */
  int output_scan_number;	/* Nominal scan number being displayed */
  JDIMENSION output_iMCU_row;	/* Number of iMCU rows read */

  /* Current progression status.  coef_bits[c][i] indicates the precision
   * with which component c's DCT coefficient i (in zigzag order) is known.
   * It is -1 when no data has yet been received, otherwise it is the point
   * transform (shift) value for the most recent scan of the coefficient
   * (thus, 0 at completion of the progression).
   * This pointer is NULL when reading a non-progressive file.
   */
  int (*coef_bits)[DCTSIZE2];	/* -1 or current Al value for each coef */

  /* Internal JPEG parameters --- the application usually need not look at
   * these fields.  Note that the decompressor output side may not use
   * any parameters that can change between scans.
   */

  /* Quantization and Huffman tables are carried forward across input
   * datastreams when processing abbreviated JPEG datastreams.
   */

  JQUANT_TBL * quant_tbl_ptrs[NUM_QUANT_TBLS];
  /* ptrs to coefficient quantization tables, or NULL if not defined */

  JHUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
  JHUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
  /* ptrs to Huffman coding tables, or NULL if not defined */

  /* These parameters are never carried across datastreams, since they
   * are given in SOF/SOS markers or defined to be reset by SOI.
   */

  int data_precision;		/* bits of precision in image data */

  jpeg_component_info * comp_info;
  /* comp_info[i] describes component that appears i'th in SOF */

  boolean progressive_mode;	/* TRUE if SOFn specifies progressive mode */
  boolean arith_code;		/* TRUE=arithmetic coding, FALSE=Huffman */

  UINT8 arith_dc_L[NUM_ARITH_TBLS]; /* L values for DC arith-coding tables */
  UINT8 arith_dc_U[NUM_ARITH_TBLS]; /* U values for DC arith-coding tables */
  UINT8 arith_ac_K[NUM_ARITH_TBLS]; /* Kx values for AC arith-coding tables */

  unsigned int restart_interval; /* MCUs per restart interval, or 0 for no restart */

  /* These fields record data obtained from optional markers recognized by
   * the JPEG library.
   */
  boolean saw_JFIF_marker;	/* TRUE iff a JFIF APP0 marker was found */
  /* Data copied from JFIF marker; only valid if saw_JFIF_marker is TRUE: */
  UINT8 JFIF_major_version;	/* JFIF version number */
  UINT8 JFIF_minor_version;
  UINT8 density_unit;		/* JFIF code for pixel size units */
  UINT16 X_density;		/* Horizontal pixel density */
  UINT16 Y_density;		/* Vertical pixel density */
  boolean saw_Adobe_marker;	/* TRUE iff an Adobe APP14 marker was found */
  UINT8 Adobe_transform;	/* Color transform code from Adobe marker */

  boolean CCIR601_sampling;	/* TRUE=first samples are cosited */

  /* Aside from the specific data retained from APPn markers known to the
   * library, the uninterpreted contents of any or all APPn and COM markers
   * can be saved in a list for examination by the application.
   */
  jpeg_saved_marker_ptr marker_list; /* Head of list of saved markers */

  /* Remaining fields are known throughout decompressor, but generally
   * should not be touched by a surrounding application.
   */

  /*
   * These fields are computed during decompression startup
   */
  int max_h_samp_factor;	/* largest h_samp_factor */
  int max_v_samp_factor;	/* largest v_samp_factor */

  int min_DCT_scaled_size;	/* smallest DCT_scaled_size of any component */

  JDIMENSION total_iMCU_rows;	/* # of iMCU rows in image */
  /* The coefficient controller's input and output progress is measured in
   * units of "iMCU" (interleaved MCU) rows.  These are the same as MCU rows
   * in fully interleaved JPEG scans, but are used whether the scan is
   * interleaved or not.  We define an iMCU row as v_samp_factor DCT block
   * rows of each component.  Therefore, the IDCT output contains
   * v_samp_factor*DCT_scaled_size sample rows of a component per iMCU row.
   */

  JSAMPLE * sample_range_limit; /* table for fast range-limiting */

  /*
   * These fields are valid during any one scan.
   * They describe the components and MCUs actually appearing in the scan.
   * Note that the decompressor output side must not use these fields.
   */
  int comps_in_scan;		/* # of JPEG components in this scan */
  jpeg_component_info * cur_comp_info[MAX_COMPS_IN_SCAN];
  /* *cur_comp_info[i] describes component that appears i'th in SOS */

  JDIMENSION MCUs_per_row;	/* # of MCUs across the image */
  JDIMENSION MCU_rows_in_scan;	/* # of MCU rows in the image */

  int blocks_in_MCU;		/* # of DCT blocks per MCU */
  int MCU_membership[D_MAX_BLOCKS_IN_MCU];
  /* MCU_membership[i] is index in cur_comp_info of component owning */
  /* i'th block in an MCU */

  int Ss, Se, Ah, Al;		/* progressive JPEG parameters for scan */

  /* This field is shared between entropy decoder and marker parser.
   * It is either zero or the code of a JPEG marker that has been
   * read from the data source, but has not yet been processed.
   */
  int unread_marker;
#ifdef __wei_jpeg
  j_hw_ptr phwset;
#endif //__wei_jpeg

  /*
   * Links to decompression subobjects (methods, private variables of modules)
   */
  struct jpeg_decomp_master * master;
  struct jpeg_d_main_controller * main;
  struct jpeg_d_coef_controller * coef;
  struct jpeg_d_post_controller * post;
  struct jpeg_input_controller * inputctl;
  struct jpeg_marker_reader * marker;
  struct jpeg_entropy_decoder * entropy;
  struct jpeg_inverse_dct * idct;
  struct jpeg_upsampler * upsample;
  struct jpeg_color_deconverter * cconvert;
  struct jpeg_color_quantizer * cquantize;
};


/* "Object" declarations for JPEG modules that may be supplied or called
 * directly by the surrounding application.
 * As with all objects in the JPEG library, these structs only define the
 * publicly visible methods and state variables of a module.  Additional
 * private fields may exist after the public ones.
 */


/* Error handler object */

struct jpeg_error_mgr {
  /* Error exit handler: does not return to caller */
  JMETHOD(void, error_exit, (j_common_ptr cinfo));
  /* Conditionally emit a trace or warning message */
  JMETHOD(void, emit_message, (j_common_ptr cinfo, int msg_level));
  /* Routine that actually outputs a trace or error message */
  JMETHOD(void, output_message, (j_common_ptr cinfo));
  /* Format a message string for the most recent JPEG error or message */
  JMETHOD(void, format_message, (j_common_ptr cinfo, char * buffer));
#define JMSG_LENGTH_MAX  200	/* recommended size of format_message buffer */
  /* Reset error state variables at start of a new image */
  JMETHOD(void, reset_error_mgr, (j_common_ptr cinfo));
  
  /* The message ID code and any parameters are saved here.
   * A message can have one string parameter or up to 8 int parameters.
   */
  int msg_code;
#define JMSG_STR_PARM_MAX  80
  union {
    int i[8];
    char s[JMSG_STR_PARM_MAX];
  } msg_parm;
  
  /* Standard state variables for error facility */
  
  int trace_level;		/* max msg_level that will be displayed */
  
  /* For recoverable corrupt-data errors, we emit a warning message,
   * but keep going unless emit_message chooses to abort.  emit_message
   * should count warnings in num_warnings.  The surrounding application
   * can check for bad data by seeing if num_warnings is nonzero at the
   * end of processing.
   */
  long num_warnings;		/* number of corrupt-data warnings */

  /* These fields point to the table(s) of error message strings.
   * An application can change the table pointer to switch to a different
   * message list (typically, to change the language in which errors are
   * reported).  Some applications may wish to add additional error codes
   * that will be handled by the JPEG library error mechanism; the second
   * table pointer is used for this purpose.
   *
   * First table includes all errors generated by JPEG library itself.
   * Error code 0 is reserved for a "no such error string" message.
   */
  const char * const * jpeg_message_table; /* Library errors */
  int last_jpeg_message;    /* Table contains strings 0..last_jpeg_message */
  /* Second table can be added by application (see cjpeg/djpeg for example).
   * It contains strings numbered first_addon_message..last_addon_message.
   */
  const char * const * addon_message_table; /* Non-library errors */
  int first_addon_message;	/* code for first string in addon table */
  int last_addon_message;	/* code for last string in addon table */
};


/* Progress monitor object */

struct jpeg_progress_mgr {
  JMETHOD(void, progress_monitor, (j_common_ptr cinfo));

  long pass_counter;		/* work units completed in this pass */
  long pass_limit;		/* total number of work units in this pass */
  int completed_passes;		/* passes completed so far */
  int total_passes;		/* total number of passes expected */
};


/* Data destination object for compression */

struct jpeg_destination_mgr {
  JOCTET * next_output_byte;	/* => next byte to write in buffer */
  size_t free_in_buffer;	/* # of byte spaces remaining in buffer */

  JMETHOD(void, init_destination, (j_compress_ptr cinfo));
  JMETHOD(boolean, empty_output_buffer, (j_compress_ptr cinfo));
  JMETHOD(void, term_destination, (j_compress_ptr cinfo));
};


/* Data source object for decompression */

struct jpeg_source_mgr {
  const JOCTET * next_input_byte; /* => next byte to read from buffer */
  size_t bytes_in_buffer;	/* # of bytes remaining in buffer */

  JMETHOD(void, init_source, (j_decompress_ptr cinfo));
  JMETHOD(boolean, fill_input_buffer, (j_decompress_ptr cinfo));
  JMETHOD(void, skip_input_data, (j_decompress_ptr cinfo, long num_bytes));
  JMETHOD(boolean, resync_to_restart, (j_decompress_ptr cinfo, int desired));
  JMETHOD(void, term_source, (j_decompress_ptr cinfo));
};


/* Memory manager object.
 * Allocates "small" objects (a few K total), "large" objects (tens of K),
 * and "really big" objects (virtual arrays with backing store if needed).
 * The memory manager does not allow individual objects to be freed; rather,
 * each created object is assigned to a pool, and whole pools can be freed
 * at once.  This is faster and more convenient than remembering exactly what
 * to free, especially where malloc()/free() are not too speedy.
 * NB: alloc routines never return NULL.  They exit to error_exit if not
 * successful.
 */

#define JPOOL_PERMANENT	0	/* lasts until master record is destroyed */
#define JPOOL_IMAGE	1	/* lasts until done with image/datastream */
#define JPOOL_NUMPOOLS	2

typedef struct jvirt_sarray_control * jvirt_sarray_ptr;
typedef struct jvirt_barray_control * jvirt_barray_ptr;


struct jpeg_memory_mgr {
  /* Method pointers */
  JMETHOD(void *, alloc_small, (j_common_ptr cinfo, int pool_id,
				size_t sizeofobject));
  JMETHOD(void FAR *, alloc_large, (j_common_ptr cinfo, int pool_id,
				     size_t sizeofobject));
  JMETHOD(JSAMPARRAY, alloc_sarray, (j_common_ptr cinfo, int pool_id,
				     JDIMENSION samplesperrow,
				     JDIMENSION numrows));
  JMETHOD(JBLOCKARRAY, alloc_barray, (j_common_ptr cinfo, int pool_id,
				      JDIMENSION blocksperrow,
				      JDIMENSION numrows));
  JMETHOD(jvirt_sarray_ptr, request_virt_sarray, (j_common_ptr cinfo,
						  int pool_id,
						  boolean pre_zero,
						  JDIMENSION samplesperrow,
						  JDIMENSION numrows,
						  JDIMENSION maxaccess));
  JMETHOD(jvirt_barray_ptr, request_virt_barray, (j_common_ptr cinfo,
						  int pool_id,
						  boolean pre_zero,
						  JDIMENSION blocksperrow,
						  JDIMENSION numrows,
						  JDIMENSION maxaccess));
  JMETHOD(void, realize_virt_arrays, (j_common_ptr cinfo));
  JMETHOD(JSAMPARRAY, access_virt_sarray, (j_common_ptr cinfo,
					   jvirt_sarray_ptr ptr,
					   JDIMENSION start_row,
					   JDIMENSION num_rows,
					   boolean writable));
  JMETHOD(JBLOCKARRAY, access_virt_barray, (j_common_ptr cinfo,
					    jvirt_barray_ptr ptr,
					    JDIMENSION start_row,
					    JDIMENSION num_rows,
					    boolean writable));
  JMETHOD(void, free_pool, (j_common_ptr cinfo, int pool_id));
  JMETHOD(void, self_destruct, (j_common_ptr cinfo));

  /* Limit on memory allocation for this JPEG object.  (Note that this is
   * merely advisory, not a guaranteed maximum; it only affects the space
   * used for virtual-array buffers.)  May be changed by outer application
   * after creating the JPEG object.
   */
  long max_memory_to_use;

  /* Maximum allocation request accepted by alloc_large. */
  long max_alloc_chunk;
};


/* Routine signature for application-supplied marker processing methods.
 * Need not pass marker code since it is stored in cinfo->unread_marker.
 */
typedef JMETHOD(boolean, jpeg_marker_parser_method, (j_decompress_ptr cinfo));


/* Declarations for routines called by application.
 * The JPP macro hides prototype parameters from compilers that can't cope.
 * Note JPP requires double parentheses.
 */

#ifdef HAVE_PROTOTYPES
#define JPP(arglist)	arglist
#else
#define JPP(arglist)	()
#endif


/* Short forms of external names for systems with brain-damaged linkers.
 * We shorten external names to be unique in the first six letters, which
 * is good enough for all known systems.
 * (If your compiler itself needs names to be unique in less than 15 
 * characters, you are out of luck.  Get a better compiler.)
 */

#ifdef NEED_SHORT_EXTERNAL_NAMES
#define jpeg_std_error		jStdError
#define jpeg_CreateCompress	jCreaCompress
#define jpeg_CreateDecompress	jCreaDecompress
#define jpeg_destroy_compress	jDestCompress
#define jpeg_destroy_decompress	jDestDecompress
#define jpeg_stdio_dest		jStdDest
#define jpeg_stdio_src		jStdSrc
#define jpeg_set_defaults	jSetDefaults
#define jpeg_set_colorspace	jSetColorspace
#define jpeg_default_colorspace	jDefColorspace
#define jpeg_set_quality	jSetQuality
#define jpeg_set_linear_quality	jSetLQuality
#define jpeg_add_quant_table	jAddQuantTable
#define jpeg_quality_scaling	jQualityScaling
#define jpeg_simple_progression	jSimProgress
#define jpeg_suppress_tables	jSuppressTables
#define jpeg_alloc_quant_table	jAlcQTable
#define jpeg_alloc_huff_table	jAlcHTable
#define jpeg_start_compress	jStrtCompress
#define jpeg_write_scanlines	jWrtScanlines
#define jpeg_finish_compress	jFinCompress
#define jpeg_write_raw_data	jWrtRawData
#define jpeg_write_marker	jWrtMarker
#define jpeg_write_m_header	jWrtMHeader
#define jpeg_write_m_byte	jWrtMByte
#define jpeg_write_tables	jWrtTables
#define jpeg_read_header	jReadHeader
#define jpeg_start_decompress	jStrtDecompress
#define jpeg_read_scanlines	jReadScanlines
#define jpeg_finish_decompress	jFinDecompress
#define jpeg_read_raw_data	jReadRawData
#define jpeg_has_multiple_scans	jHasMultScn
#define jpeg_start_output	jStrtOutput
#define jpeg_finish_output	jFinOutput
#define jpeg_input_complete	jInComplete
#define jpeg_new_colormap	jNewCMap
#define jpeg_consume_input	jConsumeInput
#define jpeg_calc_output_dimensions	jCalcDimensions
#define jpeg_save_markers	jSaveMarkers
#define jpeg_set_marker_processor	jSetMarker
#define jpeg_read_coefficients	jReadCoefs
#define jpeg_write_coefficients	jWrtCoefs
#define jpeg_copy_critical_parameters	jCopyCrit
#define jpeg_abort_compress	jAbrtCompress
#define jpeg_abort_decompress	jAbrtDecompress
#define jpeg_abort		jAbort
#define jpeg_destroy		jDestroy
#define jpeg_resync_to_restart	jResyncRestart
#endif /* NEED_SHORT_EXTERNAL_NAMES */


/* Default error-management setup */
EXTERN(struct jpeg_error_mgr *) jpeg_std_error
	JPP((struct jpeg_error_mgr * err));

/* Initialization of JPEG compression objects.
 * jpeg_create_compress() and jpeg_create_decompress() are the exported
 * names that applications should call.  These expand to calls on
 * jpeg_CreateCompress and jpeg_CreateDecompress with additional information
 * passed for version mismatch checking.
 * NB: you must set up the error-manager BEFORE calling jpeg_create_xxx.
 */
#define jpeg_create_compress(cinfo) \
    jpeg_CreateCompress((cinfo), JPEG_LIB_VERSION, \
			(size_t) sizeof(struct jpeg_compress_struct))
#define jpeg_create_decompress(cinfo) \
    jpeg_CreateDecompress((cinfo), JPEG_LIB_VERSION, \
			  (size_t) sizeof(struct jpeg_decompress_struct))
EXTERN(void) jpeg_CreateCompress JPP((j_compress_ptr cinfo,
				      int version, size_t structsize));
EXTERN(void) jpeg_CreateDecompress JPP((j_decompress_ptr cinfo,
					int version, size_t structsize));
/* Destruction of JPEG compression objects */
EXTERN(void) jpeg_destroy_compress JPP((j_compress_ptr cinfo));
EXTERN(void) jpeg_destroy_decompress JPP((j_decompress_ptr cinfo));

/* Standard data source and destination managers: stdio streams. */
/* Caller is responsible for opening the file before and closing after. */
EXTERN(void) jpeg_stdio_dest JPP((j_compress_ptr cinfo, FILE * outfile));
EXTERN(void) jpeg_stdio_src JPP((j_decompress_ptr cinfo, FILE * infile));

/* Default parameter setup for compression */
EXTERN(void) jpeg_set_defaults JPP((j_compress_ptr cinfo));
/* Compression parameter setup aids */
EXTERN(void) jpeg_set_colorspace JPP((j_compress_ptr cinfo,
				      J_COLOR_SPACE colorspace));
EXTERN(void) jpeg_default_colorspace JPP((j_compress_ptr cinfo));
EXTERN(void) jpeg_set_quality JPP((j_compress_ptr cinfo, int quality,
				   boolean force_baseline));
EXTERN(void) jpeg_set_linear_quality JPP((j_compress_ptr cinfo,
					  int scale_factor,
					  boolean force_baseline));
EXTERN(void) jpeg_add_quant_table JPP((j_compress_ptr cinfo, int which_tbl,
				       const unsigned int *basic_table,
				       int scale_factor,
				       boolean force_baseline));
EXTERN(int) jpeg_quality_scaling JPP((int quality));
EXTERN(void) jpeg_simple_progression JPP((j_compress_ptr cinfo));
EXTERN(void) jpeg_suppress_tables JPP((j_compress_ptr cinfo,
				       boolean suppress));
EXTERN(JQUANT_TBL *) jpeg_alloc_quant_table JPP((j_common_ptr cinfo));
EXTERN(JHUFF_TBL *) jpeg_alloc_huff_table JPP((j_common_ptr cinfo));

/* Main entry points for compression */
EXTERN(void) jpeg_start_compress JPP((j_compress_ptr cinfo,
				      boolean write_all_tables));
EXTERN(JDIMENSION) jpeg_write_scanlines JPP((j_compress_ptr cinfo,
					     JSAMPARRAY scanlines,
					     JDIMENSION num_lines));
EXTERN(void) jpeg_finish_compress JPP((j_compress_ptr cinfo));

/* Replaces jpeg_write_scanlines when writing raw downsampled data. */
EXTERN(JDIMENSION) jpeg_write_raw_data JPP((j_compress_ptr cinfo,
					    JSAMPIMAGE data,
					    JDIMENSION num_lines));

/* Write a special marker.  See libjpeg.doc concerning safe usage. */
EXTERN(void) jpeg_write_marker
	JPP((j_compress_ptr cinfo, int marker,
	     const JOCTET * dataptr, unsigned int datalen));
/* Same, but piecemeal. */
EXTERN(void) jpeg_write_m_header
	JPP((j_compress_ptr cinfo, int marker, unsigned int datalen));
EXTERN(void) jpeg_write_m_byte
	JPP((j_compress_ptr cinfo, int val));

/* Alternate compression function: just write an abbreviated table file */
EXTERN(void) jpeg_write_tables JPP((j_compress_ptr cinfo));

/* Decompression startup: read start of JPEG datastream to see what's there */
EXTERN(int) jpeg_read_header JPP((j_decompress_ptr cinfo,
				  boolean require_image));
/* Return value is one of: */
#define JPEG_SUSPENDED		0 /* Suspended due to lack of input data */
#define JPEG_HEADER_OK		1 /* Found valid image datastream */
#define JPEG_HEADER_TABLES_ONLY	2 /* Found valid table-specs-only datastream */
/* If you pass require_image = TRUE (normal case), you need not check for
 * a TABLES_ONLY return code; an abbreviated file will cause an error exit.
 * JPEG_SUSPENDED is only possible if you use a data source module that can
 * give a suspension return (the stdio source module doesn't).
 */

/* Main entry points for decompression */
EXTERN(boolean) jpeg_start_decompress JPP((j_decompress_ptr cinfo));
EXTERN(JDIMENSION) jpeg_read_scanlines JPP((j_decompress_ptr cinfo,
					    JSAMPARRAY scanlines,
					    JDIMENSION max_lines));
EXTERN(boolean) jpeg_finish_decompress JPP((j_decompress_ptr cinfo));

/* Replaces jpeg_read_scanlines when reading raw downsampled data. */
EXTERN(JDIMENSION) jpeg_read_raw_data JPP((j_decompress_ptr cinfo,
					   JSAMPIMAGE data,
					   JDIMENSION max_lines));

/* Additional entry points for buffered-image mode. */
EXTERN(boolean) jpeg_has_multiple_scans JPP((j_decompress_ptr cinfo));
EXTERN(boolean) jpeg_start_output JPP((j_decompress_ptr cinfo,
				       int scan_number));
EXTERN(boolean) jpeg_finish_output JPP((j_decompress_ptr cinfo));
EXTERN(boolean) jpeg_input_complete JPP((j_decompress_ptr cinfo));
EXTERN(void) jpeg_new_colormap JPP((j_decompress_ptr cinfo));
EXTERN(int) jpeg_consume_input JPP((j_decompress_ptr cinfo));
/* Return value is one of: */
/* #define JPEG_SUSPENDED	0    Suspended due to lack of input data */
#define JPEG_REACHED_SOS	1 /* Reached start of new scan */
#define JPEG_REACHED_EOI	2 /* Reached end of image */
#define JPEG_ROW_COMPLETED	3 /* Completed one iMCU row */
#define JPEG_SCAN_COMPLETED	4 /* Completed last iMCU row of a scan */

/* Precalculate output dimensions for current decompression parameters. */
EXTERN(void) jpeg_calc_output_dimensions JPP((j_decompress_ptr cinfo));

/* Control saving of COM and APPn markers into marker_list. */
EXTERN(void) jpeg_save_markers
	JPP((j_decompress_ptr cinfo, int marker_code,
	     unsigned int length_limit));

/* Install a special processing method for COM or APPn markers. */
EXTERN(void) jpeg_set_marker_processor
	JPP((j_decompress_ptr cinfo, int marker_code,
	     jpeg_marker_parser_method routine));

/* Read or write raw DCT coefficients --- useful for lossless transcoding. */
EXTERN(jvirt_barray_ptr *) jpeg_read_coefficients JPP((j_decompress_ptr cinfo));
EXTERN(void) jpeg_write_coefficients JPP((j_compress_ptr cinfo,
					  jvirt_barray_ptr * coef_arrays));
EXTERN(void) jpeg_copy_critical_parameters JPP((j_decompress_ptr srcinfo,
						j_compress_ptr dstinfo));

/* If you choose to abort compression or decompression before completing
 * jpeg_finish_(de)compress, then you need to clean up to release memory,
 * temporary files, etc.  You can just call jpeg_destroy_(de)compress
 * if you're done with the JPEG object, but if you want to clean it up and
 * reuse it, call this:
 */
EXTERN(void) jpeg_abort_compress JPP((j_compress_ptr cinfo));
EXTERN(void) jpeg_abort_decompress JPP((j_decompress_ptr cinfo));

/* Generic versions of jpeg_abort and jpeg_destroy that work on either
 * flavor of JPEG object.  These may be more convenient in some places.
 */
EXTERN(void) jpeg_abort JPP((j_common_ptr cinfo));
EXTERN(void) jpeg_destroy JPP((j_common_ptr cinfo));

/* Default restart-marker-resync procedure for use by data source modules */
EXTERN(boolean) jpeg_resync_to_restart JPP((j_decompress_ptr cinfo,
					    int desired));

#ifdef __wei_jpeg
typedef struct {
    cyg_uint16 height;
    cyg_uint16 width;        // Pixels
    cyg_uint16 pitch;
    cyg_uint8  *addr;              // Frame buffer
}display_info;

EXTERN(j_hw_ptr) jpeg_alloc_hwset JPP((j_common_ptr cinfo));
EXTERN(int) __sishwdecode_jpg JPP((j_decompress_ptr cinfo, display_info *display_buffer));
#endif //__wei_jpeg
/* These marker codes are exported since applications and data source modules
 * are likely to want to use them.
 */

#define JPEG_RST0	0xD0	/* RST0 marker code */
#define JPEG_EOI	0xD9	/* EOI marker code */
#define JPEG_APP0	0xE0	/* APP0 marker code */
#define JPEG_COM	0xFE	/* COM marker code */


/* If we have a brain-damaged compiler that emits warnings (or worse, errors)
 * for structure definitions that are never filled in, keep it quiet by
 * supplying dummy definitions for the various substructures.
 */

#ifdef INCOMPLETE_TYPES_BROKEN
#ifndef JPEG_INTERNALS		/* will be defined in jpegint.h */
struct jvirt_sarray_control { long dummy; };
struct jvirt_barray_control { long dummy; };
struct jpeg_comp_master { long dummy; };
struct jpeg_c_main_controller { long dummy; };
struct jpeg_c_prep_controller { long dummy; };
struct jpeg_c_coef_controller { long dummy; };
struct jpeg_marker_writer { long dummy; };
struct jpeg_color_converter { long dummy; };
struct jpeg_downsampler { long dummy; };
struct jpeg_forward_dct { long dummy; };
struct jpeg_entropy_encoder { long dummy; };
struct jpeg_decomp_master { long dummy; };
struct jpeg_d_main_controller { long dummy; };
struct jpeg_d_coef_controller { long dummy; };
struct jpeg_d_post_controller { long dummy; };
struct jpeg_input_controller { long dummy; };
struct jpeg_marker_reader { long dummy; };
struct jpeg_entropy_decoder { long dummy; };
struct jpeg_inverse_dct { long dummy; };
struct jpeg_upsampler { long dummy; };
struct jpeg_color_deconverter { long dummy; };
struct jpeg_color_quantizer { long dummy; };
#endif /* JPEG_INTERNALS */
#endif /* INCOMPLETE_TYPES_BROKEN */


/*
 * The JPEG library modules define JPEG_INTERNALS before including this file.
 * The internal structure declarations are read only when that is true.
 * Applications using the library should not include jpegint.h, but may wish
 * to include jerror.h.
 */

#ifdef JPEG_INTERNALS
#include <jpeg/jpegint.h>		/* fetch private declarations */
#include <jpeg/jerror.h>		/* fetch error codes too */
#endif
#endif /* JPEGLIB_H */
