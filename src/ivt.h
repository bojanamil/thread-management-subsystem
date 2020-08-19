#ifndef _IVT_H_
#define _IVT_H_

#include "glb.h"
typedef unsigned char IVTNo;
#include "kev.h"

class IVTEntry {
public:
	IVTEntry(IVTNo ivtNo, pInterrupt myInterrupt);
	~IVTEntry();
	IVTNo ivtNo;
	pInterrupt oldInterrupt;
	KernelEv* myEv;
};

#define PREPAREENTRY(ivtNo,flag)\
void interrupt myInterrupt##ivtNo(...);\
IVTEntry myEntry##ivtNo(ivtNo,myInterrupt##ivtNo);\
void interrupt myInterrupt##ivtNo(...) {\
	if (myEntry##ivtNo.oldInterrupt != 0 && flag != 0)\
		myEntry##ivtNo.oldInterrupt();\
	if (myEntry##ivtNo.myEv != 0)\
		myEntry##ivtNo.myEv->signal();\
	dispatch();\
}

#endif