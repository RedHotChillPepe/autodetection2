/******************************************************************************

  Copyright (C), 2021, SZZHST Tech. Co., Ltd.

 ******************************************************************************
  File Name     : gsf_type.h

  Version       : Initial Draft
  Author        : szzhst developer group
  Created       : 2021/06/23
  Description   :
  History       :
  1.Date        : 2021/06/23
    Author      :
    Modification: Created file
******************************************************************************/
#ifndef __GSF_TYPE_H__
#define __GSF_TYPE_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdio.h>

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           GSF_U8;
typedef unsigned short          GSF_U16;
typedef unsigned int            GSF_U32;
typedef unsigned long long      GSF_U64;
typedef unsigned long           GSF_UL;

typedef char                    GSF_CHAR;
typedef  char            		GSF_S8;
typedef short                   GSF_S16;
typedef int                     GSF_S32;
typedef signed long long        GSF_S64;
typedef signed long             GSF_SL;

#define GSF_SUCCESS 0
#define GSF_FAILURE -1

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
