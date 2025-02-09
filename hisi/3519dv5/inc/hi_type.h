/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_TYPE_H
#define HI_TYPE_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_NULL TD_NULL
#define HI_SUCCESS TD_SUCCESS
#define HI_FAILURE TD_FAILURE
typedef td_uchar hi_uchar;
typedef td_u8 hi_u8;
typedef td_u16 hi_u16;
typedef td_u32 hi_u32;
typedef td_ulong hi_ulong;
typedef td_char hi_char;
typedef td_s8 hi_s8;
typedef td_s16 hi_s16;
typedef td_s32 hi_s32;
typedef td_slong hi_slong;
typedef td_float hi_float;
typedef td_double hi_double;
typedef td_void hi_void;
typedef td_u64 hi_u64;
typedef td_s64 hi_s64;
typedef td_phys_addr_t hi_phys_addr_t;
typedef td_handle hi_handle;
typedef td_uintptr_t hi_uintptr_t;
typedef td_fr32 hi_fr32;
#define HI_FALSE TD_FALSE
#define HI_TRUE TD_TRUE
typedef td_bool hi_bool;


typedef unsigned char           HI_UCHAR;
typedef unsigned char           HI_U8;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;
typedef unsigned long           HI_UL;
typedef uintptr_t               HI_UINTPTR_T;

typedef char                    HI_CHAR;
typedef signed char             HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;

/*----------------------------------------------*
 * The fixed-point data type, used to represent float data in hardware calculations.*
 *----------------------------------------------*/
/*--u8bit---------------------------------------*/
typedef unsigned char           HI_U0Q8;
typedef unsigned char           HI_U1Q7;
typedef unsigned char           HI_U5Q3;

/*--u16bit---------------------------------------*/
typedef unsigned short          HI_U0Q16;
typedef unsigned short          HI_U4Q12;
typedef unsigned short          HI_U6Q10;
typedef unsigned short          HI_U8Q8;
typedef unsigned short          HI_U12Q4;
typedef unsigned short          HI_U14Q2;

/*--s16bit---------------------------------------*/
typedef short                   HI_S9Q7;
typedef short                   HI_S14Q2;
typedef short                   HI_S1Q15;

/*--u32bit---------------------------------------*/
typedef unsigned int            HI_U22Q10;
typedef unsigned int            HI_U25Q7;

/*--s32bit---------------------------------------*/
typedef int                     HI_S25Q7;
typedef int                     HI_S16Q16;

/*----------------------------------------------*
 * The fixed-point data type combine with FLAG bits.*
 *----------------------------------------------*/

/*8bits unsigned integer£¬4bits decimal fraction£¬4bits flag bits*/
typedef unsigned short          HI_U8Q4F4;

/*float*/
typedef float               HI_FLOAT;
/*double*/
typedef double                  HI_DOUBLE;


#ifndef _M_IX86
    typedef unsigned long long  HI_U64;
    typedef long long           HI_S64;
#else
    typedef unsigned __int64    HI_U64;
    typedef __int64             HI_S64;
#endif

typedef char                    HI_CHAR;
#define HI_VOID                 void

typedef unsigned long           HI_SIZE_T;
typedef unsigned long           HI_LENGTH_T;
typedef long int                HI_PHYS_ADDR_T;

typedef unsigned int            HI_HANDLE;

#ifdef __cplusplus
}
#endif
#endif /* HI_TYPE_H */
