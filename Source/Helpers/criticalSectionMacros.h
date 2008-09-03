#pragma once

class criticalSectionControl;

#define CRITICAL(fun)  {\
	CS->waitForUnlock(CSID);\
	CS->lockSection(CSID); \
	fun; \
	CS->unlockSection(CSID); \
	}

#define CRITICAL_VARS criticalSectionControl * CS;\
	unsigned int CSID

#define CRITICAL_INIT CS = criticalSectionControl::getInstance();\
	CSID = CS->getCriticalSection()

#define CRITICAL_RELEASE CS->waitForUnlock(CSID);\
	CS->releaseCriticalSection(CSID)