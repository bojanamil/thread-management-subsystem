#include "ivt.h"
#include <dos.h>

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt myInterrupt) {
	lock
	this->ivtNo = ivtNo;
	this->oldInterrupt = getvect(ivtNo);
	setvect(ivtNo, myInterrupt);
	this->myEv = 0;
	myEntries[ivtNo] = this;
	unlock
}

IVTEntry::~IVTEntry() {
	lock
	myEntries[ivtNo] = 0;
	setvect(ivtNo, oldInterrupt);
	unlock
}

