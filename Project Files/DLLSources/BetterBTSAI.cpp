#include "CvGameCoreDLL.h"

#include "BetterBTSAI.h"

// AI decision making logging

void logBBAI(TCHAR* format, ... )
{
#ifdef LOG_AI
	static char buf[2048];
	va_list args;
	va_start(args, format);
	_vsnprintf(buf, 2048-4, format, args);
	va_end(args); // kmodx
	gDLL->logMsg("BBAI.log", buf);
#endif
}
