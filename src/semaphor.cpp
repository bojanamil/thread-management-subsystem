#include "semaphor.h"
#include "ksem.h"
#include "glb.h"
#include "sortlist.h"
#include "pcb.h"

Semaphore::Semaphore(int init) {
	lock
	myImpl = new KernelSem(init);
	unlock
}

Semaphore::~Semaphore() {
	lock
	delete myImpl;
	unlock
}

int Semaphore::wait(Time maxTimeToWait) {
	lock
	myImpl->valSem--;
	if (myImpl->valSem < 0) {
		running->blockedOnSem = myImpl;
		myImpl->list.insert(running);
		sortList->insert(running, maxTimeToWait);	// nece da ubaci ako je 0
		suspend();
	}
	else
		running->val = 1;
	unlock
	return running->val;
}

void Semaphore::signal() {
	lock
	myImpl->valSem++;
	if (myImpl->valSem <= 0) {
		PCB* pcb = myImpl->list.remove();
		sortList->remove(pcb);
		pcb->blockedOnSem = 0;
		pcb->val = 1;
		resume(pcb);
	}
	unlock
}

int Semaphore::val() const {
	return myImpl->valSem;
}

