#include "CvGameCoreDLL.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"

void CvDLLEngineIFaceBase::TriggerEffect(const TriggerEffectArguments& arguments)
{
	FAssertMsg(VARINFO<EffectTypes>::isInRange(arguments.effect), "CvDLLEngineIFaceBase::TriggerEffect called without any effect to trigger");
	if (!VARINFO<EffectTypes>::isInRange(arguments.effect))
	{
		// avoid a crash in the exe
		return;
	}

	TriggerEffect(arguments.effect, arguments.pt3Point, arguments.rotation);

	// technically audio has nothing to do with trigger effect
	// however at the time of introducing this function, all calls had play audio as the following line
	if (arguments.audio != NULL)
	{
		gDLL->getInterfaceIFace()->playGeneralSound(arguments.audio, arguments.pt3Point);
	}
}
