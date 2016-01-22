#ifndef __HELPERS__
#define __HELPERS__

#ifdef _WIN32
#include <Windows.h>
#include <time.h>
#else
#define Sleep(milisecs) usleep(milisecs*1000)
#endif

#define WAITUNTIL(maxtime, boolExp) \
for (int time = 0; time < maxtime && !(boolExp); time += 50) \
	Sleep(50); \
	ASSERT_TRUE(boolExp); \

/*A way to sleep that can be adapted to other platforms if Sleep is not available*/
#define WAIT(maxtime) \
for (int time = 0; time < maxtime; time += 50) \
	Sleep(50);\

#endif //_HELPERS__