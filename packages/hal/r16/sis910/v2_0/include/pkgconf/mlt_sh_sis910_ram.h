// eCos memory layout - Fri Oct 20 09:50:36 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
#define CYGMEM_REGION_ram (0x00000000)
#ifdef __CHAOBAN_RC
#define CYGMEM_REGION_ram_SIZE (__RC_RAMSIZE)
#else
#define CYGMEM_REGION_ram_SIZE (0x02000000)
#endif
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#define CYGMEM_REGION_rom (0xa0000000)
#define CYGMEM_REGION_rom_SIZE (0x100000)
#define CYGMEM_REGION_rom_ATTR (CYGMEM_REGION_ATTR_R)
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#ifdef __CHAOBAN_RC
#define CYGMEM_SECTION_heap1_SIZE (__RC_RAMSIZE - (size_t) CYG_LABEL_NAME (__heap1))
#else
#define CYGMEM_SECTION_heap1_SIZE (0x02000000 - (size_t) CYG_LABEL_NAME (__heap1))
#endif
