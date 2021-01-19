#pragma once
typedef void* (__stdcall* sharedApplication_t)();
typedef void(__thiscall* setAnimationInterval_t)(void* instance, double delay);
bool changeFps(double const& fps);