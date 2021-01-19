#include "pch.h"
#include "fps.h"

bool changeFps(double const& fps) {
	auto hMod = LoadLibrary(L"libcocos2d.dll");
	if (!hMod) return false;
	auto sharedApplication = reinterpret_cast<sharedApplication_t>(GetProcAddress(hMod, "?sharedApplication@CCApplication@cocos2d@@SAPAV12@XZ"));
	auto setAnimationInterval = reinterpret_cast<setAnimationInterval_t>(GetProcAddress(hMod, "?setAnimationInterval@CCApplication@cocos2d@@UAEXN@Z"));
	if (!sharedApplication || !setAnimationInterval) return false;
	setAnimationInterval(sharedApplication(), 1. / fps);
	return true;
}