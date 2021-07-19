//==========================================================================
//
//      memmanage.h
//
//      Provide some useful macro or func to manage memory or sync/flush
//
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author:       RS
// Author:       Sidney Shih
// Date:         2009-03-05
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

// Define the D-cache size
#ifdef CYGPKG_SERVICE_MEM_MANAGE_SIS910_DCACHE_SIZE
#define DCACHE_SIZE    (CYGPKG_SERVICE_MEM_MANAGE_SIS910_DCACHE_SIZE * 1024)
#else
#define DCACHE_SIZE    (4 * 1024)
#endif

// 0x6000_0000 ~ 0x6FFF_FFFF    D-cache SYNC space (128MB, 32byte align)
// 0x7000_0000 ~ 0x7FFF_FFFF    D-cache FLUSH space (128MB, 32byte align)
#define CACHE_SYNC_ADDR     0x60000000
#define CACHE_FLUSH_ADDR    0x70000000

////////////////////////////////////////////////////////////////////////////////
//               About D-cache and DRAM SYNC/FLUSH mechanism                  //
////////////////////////////////////////////////////////////////////////////////
//
// To FLUSH a DRAM address (Y), performs a write to address (0x70000000 | Y).
// The write data is ignored and the cache line become invalid after the flush
// (so all subsequent access cause cache-miss).
// If the line was modified, a DRAM write-back happens.
// If the line was clean, no DRAM operations.
//
// To SYNC a DRAM address (Y), performs a write to address (0x60000000 | Y).
// The write data is ignored and the cache line remains valid in the cache
// (so the subsequent access may be cache-hit).
// If the line was modified, a DRAM write-back happens.
// If the line was clean, no DRAM operations.
//
// (1) Be careful with the SYNC operations.
//     S/W must have clear idea of the memory usage before using it.
//     If data is sent from RISC to external master, SYNC/FLUSH is processed
//     before master transfer. Both operations work.
//     If data is sent from external master to RISC, FLUSH is processed before
//     master transfer, SYNC will not work in this case.
//
// (2) To SYNC/FLUSH a line, RISC just need to write one data every 32-byte
//     boundary (because one line is 32-byte) .
//     To SYNC/FLUSH a continuous 4KB 32-byte aligned memory, perform
//     4096/32 = 128 write cycles.
//
#define R16_CACHE_FLUSH(_buf_, _len_)                                           \
    CYG_MACRO_START                                                             \
        cyg_uint8 *_p_ = (cyg_uint8 *)((cyg_uint32)_buf_ | CACHE_FLUSH_ADDR);   \
        cyg_uint8 *_upper_ = (cyg_uint8 *)_p_ + _len_;                          \
        if (_len_ > DCACHE_SIZE)                                                \
        {                                                                       \
            _p_ = (cyg_uint8 *)(_upper_ - DCACHE_SIZE);                         \
        }                                                                       \
        _p_ = (cyg_uint8 *)((cyg_uint32)_p_ & ~0x1F);                           \
        do                                                                      \
        {                                                                       \
            HAL_WRITE_UINT32(_p_, 0x0);                                         \
            _p_ += 32;                                                          \
        } while (_p_ <= _upper_);                                               \
    CYG_MACRO_END

#define R16_CACHE_SYNC(_buf_, _len_)                                            \
    CYG_MACRO_START                                                             \
        cyg_uint8 *_p_ = (cyg_uint8 *)((cyg_uint32)_buf_ | CACHE_SYNC_ADDR);    \
        cyg_uint8 *_upper_ = (cyg_uint8 *)_p_ + _len_;                          \
        if (_len_ > DCACHE_SIZE)                                                \
        {                                                                       \
            _p_ = (cyg_uint8 *)(_upper_ - DCACHE_SIZE);                         \
        }                                                                       \
        _p_ = (cyg_uint8 *)((cyg_uint32)_p_ & ~0x1F);                           \
        do                                                                      \
        {                                                                       \
            HAL_WRITE_UINT32(_p_, 0x0);                                         \
            _p_ += 32;                                                          \
        } while (_p_ <= _upper_);                                               \
    CYG_MACRO_END

#define HW_READ_DRAM_ONLY_BEGIN(_buf_, _len_)                                   \
        R16_CACHE_SYNC(_buf_, _len_)
#define HW_READ_DRAM_ONLY_END CYG_EMPTY_STATEMENT

#define HW_WRITE_DRAM_ONLY_BEGIN(_buf_, _len_)                                  \
        R16_CACHE_FLUSH(_buf_, _len_)
#define HW_WRITE_DRAM_ONLY_END CYG_EMPTY_STATEMENT

#define HW_READ_WRITE_DRAM_BEGIN(_buf_, _len_)                                  \
        R16_CACHE_FLUSH(_buf_, _len_)
#define HW_READ_WRITE_DRAM_END CYG_EMPTY_STATEMENT
