
//
//  MachTimer.m
//

#include "MachTimer.h"

void MachTimer::start() {
	t0=GetTickCount();
}

DWORD MachTimer::elapsed() {
	return GetTickCount()-t0;
}

float MachTimer::elapsedSec() {
	return (float)elapsed()/1000.0f;
}
