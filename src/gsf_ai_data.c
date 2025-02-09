#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hi_math.h"
#include <sys/time.h>
#include <stdlib.h>
#include "devsdk.h"

int gsf_get_ai_data(AI_DATA_INFO_S *pAiData, int nSize)
{
	static int idx = 0;
	static int cnt = 0;
#if 0
	AI_DATA_INFO_S stTempAiData[] = {
		{{120, 624, 400, 220}, {0xE4,0xBA,0xAC,0xE9,0x83,0xBD,0x34,0x38,0x30,0x35,0xE3,0x82,0x8F,0x35,0x32,0x2D,0x37,0x33,0x0}},
		{{120, 624, 400, 220}, {0xE4,0xBA,0xAC,0xE9,0x83,0xBD,0x34,0x38,0x30,0xE3,0x82,0x8F,0x35,0x32,0x2D,0x37,0x33,0x0}},
		{{120, 624, 400, 220}, {0xE4,0xBA,0xAC,0xE9,0x83,0xBD,0x34,0xE3,0x81,0xA4,0x32,0x2D,0x31,0x0}},
		{{120, 624, 400, 220}, {0xE5,0xB2,0xA1,0xE5,0xB4,0x8E,0x33,0x33,0xE3,0x81,0xA4,0x34,0x35,0x2D,0x31,0x0}},
	};
#else
	AI_DATA_INFO_S stTempAiData[] = {
		{{340, 424, 400, 220}, "京都9999わ52-734"},
		{{340, 424, 400, 220}, "京都4805わ52-73"},
		{{340, 424, 400, 220}, "京都3333つ45-12345"},
		{{340, 424, 400, 220}, "京都8888つ22-1123"},
	};
	/*AI_DATA_INFO_S stTempAiData[] = {
		{{340, 424, 400, 220}, "京都4805わ52-73"},
		{{340, 424, 400, 220}, "京都480わ52-73"},
		{{340, 424, 400, 220}, "京都4つ2-1"},
		{{340, 424, 400, 220}, "岡崎33つ45-1"},
	};*/
#endif
	memset(pAiData, 0, sizeof(AI_DATA_INFO_S));
	memcpy(pAiData, &stTempAiData[idx%4], sizeof(AI_DATA_INFO_S));
	if (++cnt % 60 == 0)
	{
		idx++;
	}
	return 1;
}