#ifndef LIBMAD_SIS_HWMAC
#define LIBMAD_SIS_HWMAC
#define iiow(a,b)  *(volatile unsigned int*)(a)=b
#define iior(a)    *(volatile unsigned int*)(a)
#define stall5 {asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");} 
#define stall20 {stall5;stall5;stall5;stall5;} 
#define stall100 {stall20;stall20;stall20;stall20;}


#define CONTROL_ADDR		0x90002500
#define CONTROL_BDDR		0x90002504
#define CONTROL 			0x90002508
#define MACL				0x90002510
#define MACH				0x90002514

#define A_ADDR  			0x90040000
#define B_ADDR  			0x90040400

#define ADDR_START_MSK  	0x000003FF
#define ADDR_INDEX_MSK  	0x03FF0000
#define ADDR_INDEX_DIR_MSK  0x20000000 //0:next +, 1next -
#define ADDR_DWORD_MSK  	0x40000000 //0:DWORD, 1: WORD
#define ADDR_CARRY_MSK  	0x80000000

#define CONTROL_LOOP_MSK	0x000003FF //loop count
#define CONTROL_SHR_MSK 	0x003F0000 //right shift count

#define HW_MAC_CLEAR		  iiow(CONTROL, 0x80000000)

#define HW_MAC_SET  									\
							iiow(CONTROL_ADDR, 0x40010);\
							iiow(CONTROL_BDDR, 0x40010);\
							iiow(CONTROL, 0x0008) //control
#define HW_MAC_SET_II   								   \
							iiow(CONTROL_ADDR, 0x4000c);\
							iiow(CONTROL_BDDR, 0x4000c);\
							iiow(CONTROL, 0x0009) //control
#define HW_MAC_SET_III  								\
							iiow(CONTROL_ADDR, 0x40010);\
							iiow(CONTROL_BDDR, 0x40010);\
							iiow(CONTROL, 0x0010) //control

#define HW_MAC_FX_ARRAY_WRITE   						\
							iiow(0x90040010,(*fx)[0]);  \
							iiow(0x90040014,(*fx)[1]);  \
							iiow(0x90040018,(*fx)[2]);  \
							iiow(0x9004001c,(*fx)[3]);  \
							iiow(0x90040020,(*fx)[4]);  \
							iiow(0x90040024,(*fx)[5]);  \
							iiow(0x90040028,(*fx)[6]);  \
							iiow(0x9004002c,(*fx)[7])
#define HW_MAC_PTR_BRRAY_WRITE  						\   						 
							iiow(0x90040410, ptr[ 0]);  \
							iiow(0x90040414, ptr[ 14]); \
							iiow(0x90040418, ptr[ 12]); \
							iiow(0x9004041c, ptr[ 10]); \
							iiow(0x90040420, ptr[ 8]);  \
							iiow(0x90040424, ptr[ 6]);  \
							iiow(0x90040428, ptr[ 4]);  \
							iiow(0x9004042c, ptr[ 2])
#define HW_MAC_PTR_R_BRRAY_WRITE						  \ 						   
							iiow(0x90040410, ptr[ 2]);  \
							iiow(0x90040414, ptr[ 4]); \
							iiow(0x90040418, ptr[ 6]); \
							iiow(0x9004041c, ptr[ 8]); \
							iiow(0x90040420, ptr[ 10]);  \
							iiow(0x90040424, ptr[ 12]);  \
							iiow(0x90040428, ptr[ 14]);  \
							iiow(0x9004042c, ptr[ 0])
#define HW_MAC_PTR_II_BRRAY_WRITE   					   \							
							iiow(0x90040410, ptr[ 15]);  \
							iiow(0x90040414, ptr[ 17]); \
							iiow(0x90040418, ptr[ 19]); \
							iiow(0x9004041c, ptr[ 21]); \
							iiow(0x90040420, ptr[ 23]);  \
							iiow(0x90040424, ptr[ 25]);  \
							iiow(0x90040428, ptr[ 27]);  \
							iiow(0x9004042c, ptr[ 29])
#define HW_MAC_PTR_III_BRRAY_WRITE  						\   						 
							iiow(0x90040430, ptr[ 29]);  \
							iiow(0x90040434, ptr[ 27]); \
							iiow(0x90040438, ptr[ 25]); \
							iiow(0x9004043c, ptr[ 23]); \
							iiow(0x90040440, ptr[ 21]);  \
							iiow(0x90040444, ptr[ 19]);  \
							iiow(0x90040448, ptr[ 17]);  \
							iiow(0x9004044c, ptr[ 15])

#define HW_MAC_FE_ARRAY_WRITE   						\
							iiow(0x90040010,(*fe)[0]);  \
							iiow(0x90040014,(*fe)[1]);  \
							iiow(0x90040018,(*fe)[2]);  \
							iiow(0x9004001c,(*fe)[3]);  \
							iiow(0x90040020,(*fe)[4]);  \
							iiow(0x90040024,(*fe)[5]);  \
							iiow(0x90040028,(*fe)[6]);  \
							iiow(0x9004002c,(*fe)[7])
#define HW_MAC_FO_ARRAY_WRITE   						\
							iiow(0x90040010,(*fo)[0]);  \
							iiow(0x90040014,(*fo)[1]);  \
							iiow(0x90040018,(*fo)[2]);  \
							iiow(0x9004001c,(*fo)[3]);  \
							iiow(0x90040020,(*fo)[4]);  \
							iiow(0x90040024,(*fo)[5]);  \
							iiow(0x90040028,(*fo)[6]);  \
							iiow(0x9004002c,(*fo)[7])
#define HW_MAC_FE_R_ARRAY_WRITE 						  \
							iiow(0x90040010,(*fe)[7]);  \
							iiow(0x90040014,(*fe)[6]);  \
							iiow(0x90040018,(*fe)[5]);  \
							iiow(0x9004001c,(*fe)[4]);  \
							iiow(0x90040020,(*fe)[3]);  \
							iiow(0x90040024,(*fe)[2]);  \
							iiow(0x90040028,(*fe)[1]);  \
							iiow(0x9004002c,(*fe)[0])
#define HW_MAC_FO_R_ARRAY_WRITE 						  \
							iiow(0x90040030,(*fo)[7]);  \
							iiow(0x90040034,(*fo)[6]);  \
							iiow(0x90040038,(*fo)[5]);  \
							iiow(0x9004003c,(*fo)[4]);  \
							iiow(0x90040040,(*fo)[3]);  \
							iiow(0x90040044,(*fo)[2]);  \
							iiow(0x90040048,(*fo)[1]);  \
							iiow(0x9004004c,(*fo)[0])
#endif