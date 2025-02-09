#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "hi_math.h"
#include "gsf_vpss_comm.h"
#include "gsf_param.h"
#include "devsdk.h"
#include "hi_mpi_sys.h"


int gsf_get_unique_id(hi_unique_id *unique_id)
{
	
	int i = 0;
	hi_s32 s32Ret = 0;

	if(0 != (s32Ret = hi_mpi_sys_get_unique_id(unique_id)))
	{
		printf("[%s %d] hi_mpi_sys_get_unique_id failed with with %d!\n", __func__, __LINE__, s32Ret);
		return -1;
	}
	
	return 0;
}

