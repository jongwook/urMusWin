//
//  MachTimer.h
//

#include <assert.h>
#include <windows.h>


class MachTimer {
	DWORD t0;	
public:
	void start();
	DWORD elapsed();
	float elapsedSec();
};

