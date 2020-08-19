#include "event.h"
#include "glb.h"
#include "kev.h"
#include "pcb.h"
#include "ivt.h"

Event::Event(IVTNo ivtNo) {
	lock
	myImpl = new KernelEv(ivtNo);
	myEntries[ivtNo]->myEv = myImpl;
	unlock
}

Event::~Event() {
	lock
	myEntries[myImpl->ivtNo]->myEv = 0;
	delete myImpl;
	unlock
}

void Event::wait() {
	lock
	if (myImpl->pcb == running) {
		myImpl->pcb->blockedOnEv = myImpl;
		suspend();
	}
	unlock
}

void Event::signal() {
	lock
	if (myImpl->pcb->blockedOnEv == myImpl) {
		myImpl->pcb->blockedOnEv = 0;
		resume(myImpl->pcb);
	}
	unlock
}

