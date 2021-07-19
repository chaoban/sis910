#ifndef SISHWJPEG_H
#define SISHWJPEG_H

typedef unsigned int uint;
#define HW_NUM_QUANT_TBLS 2
#define DCTSIZE           8
#define iiow(a,b)  *(volatile unsigned int*)(a)=b
#define iior(a)    *(volatile unsigned int*)(a)
#define stall5 {asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");} 
#define stall20 {stall5;stall5;stall5;stall5;} 
#define stall100 {stall20;stall20;stall20;stall20;}
#define MIN_SLOP  50
/**************************************
**  Register Definition  ******
***************************************/
#define gw_rff_val0             0x90001760
#define gw_rd0_init0            0x90001480
#define gw_rd0_init1            0x90001484
#define gw_lpc0                 0x90001500
#define gw_lpc1                 0x90001504
#define gw_lpc2                 0x90001508
#define gw_lpc7                 0x9000151c
#define gw_reset                0x90001710

#define JPG_Enable              0x90002680
#define JPG_SET0                0x90006000
#define JPG_DIMSET              0x90006004
#define JPG_DST_RGBA            0x90006008
#define JPG_DST_Y               0x9000600c
#define JPG_DST_UV              0x90006010
#define JPG_STATUS0             0x90006014
#define JPG_STATUS1             0x90006018
#define JPG_STATUS_MB           0x9000601c
#define JPG_STATUS_YPOS         0x90006020
#define JPG_STATUS_UVPOS        0x90006024
#define JPG_STATUS_RGBPOS       0x90006028
#define JPG_SAFE_MODE           0x9000602c
#define JPG_FLITER_COEF         0x90006030
#define JPG_IDLE_CYC            0x90006034
#define JPG_BZ_CYC              0x90006038

//0x1480
#define GATEWAY_delx(x)       ((x & 0x7F) << 2)  // 32 byte uint
//0x2680
#define EN_FORMAT(x)          (x & 0xF)
#define JPG_RESET_ENABLE      (0x0 << 8)
#define JPG_RESET_DISABLE     (0x1 << 8)
#define NR_H_BLK(x)           ((x & 0x1FF) << 16)
//0x6000
#define JPG_MCU_Restart(x)    (x & 0xFFFF)
#define JPG_Format(x)         ((x & 0x3F) << 16)
#define JPG_Leading(x)        ((x & 0x1F) << 22)
#define JPG_RGB_ENABLE        (0x1 << 27)
#define JPG_DMA_RQ_LVL(x)     ((x & 0x3) << 28)
#define JPG_DMA_8X_ENABLE(x)  ((x & 0x1) << 30)
#define JPG_ENGINE_ENABLE     (0x1 << 31)
//0x6004
#define JPG_Frame_width(x)    (x & 0xFFFF)
#define JPG_Frame_height(x)   ((x & 0xFFFF) << 16)
//0x6008 0x600c 0x6010
#define JPG_Frame_base(x)     (x & 0x1FFFF)
//0x602c
#define JPG_OUTBUF_SIZE(x)    (x & 0xFFFF)
#define JPG_SAVING_ENABLE     (0x1 << 16)
#define JPG_DMA_RESTART       (0x1 << 17)
#define JPG_OUTPUT_FULL       (0x1 << 18)
#define JPG_HANG_ENABLE       (0x1 << 19)

#define JPG_HW_DONE           (0x1 << 27)

#define JPG_PERF_C_ENABLE     (0x1 << 20)
#define JPG_SWIZZLE_ENABLE    (0x0 << 21)
#define JPG_DOWN_SAMPLE(x)    ((x & 0x3) << 22)
#define JPG_HFLITER_ENABLE(x) ((x & 0x1) << 24)
//0x6030 //for 910B
#define TAB_FILTER(x, y, z, w)((x & 0xF) | ((y & 0xF) << 4) | ((z & 0xF) << 8) | ((w & 0xF) << 12))
//=====================================================
#define JPG_HW_YUV411_VAL     (0x3F << 16)
#define JPG_HW_YUV422_VAL     (0x33 << 16)
#define JPG_HW_YUV444_VAL     (0x31 << 16)
#define JPG_HW_YUV422V_VAL    (0x35 << 16)
#define JPG_HW_YUV400_VAL     (0x31 << 16)

#define MAX_DEPTH               16
#define JPG_HW_Y_DC             12
#define JPG_HW_UV_DC            12
#define JPG_HW_Y_AC             50
#define JPG_HW_UV_AC            54
#define JPG_HW_HOPPER_ELEMENT_NUM 128
#define HW_QUANT_TABLE_ELEMENT_NUMBER   32
#define HW_QUANT_TABLE_ELEMENT_SIZE     2

#define JPG_QUANT_TABLE_NSIZE  (HW_NUM_QUANT_TBLS *             \
                                HW_QUANT_TABLE_ELEMENT_NUMBER * \
                                HW_QUANT_TABLE_ELEMENT_SIZE)
#define JPG_HOPPER_TABLE_NSIZE (JPG_HW_HOPPER_ELEMENT_NUM * 4)
#define BYTEPERPIXEL  4

EXTERN(long) jdiv_round_up JPP((long a, long b));
#endif //SISHWJPEG_H
