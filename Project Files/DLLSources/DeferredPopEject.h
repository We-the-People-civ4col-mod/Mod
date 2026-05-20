#pragma once
#ifndef DEFERRED_POP_EJECT_H
#define DEFERRED_POP_EJECT_H

#include "TBB.h"

class CvCity;
class CvUnit;

struct DeferredPopEject
{
	CvCity* pCity;
	CvUnit* pUnit;
};

typedef tbb::concurrent_queue<DeferredPopEject> DeferredPopEjectQueue;

// Non-NULL only during the parallel CvPlayerAI::AI_assignWorkingPlots() call.
extern DeferredPopEjectQueue* g_pDeferredPopEjectQ;

#endif
