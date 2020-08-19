#include "thread.h"
#include "glb.h"
#include "pcb.h"
#include "schedule.h"
#include "list.h"

ID Thread::fork() {
	lock
	Thread* myClone = running->thread->clone();
	if (myClone != 0) {
		myClone->start();
		child = myClone->myPCB;
		child->val = 0;
		running->val = child->id;
		child->ifFork = 1;
		++running->count;
		child->parent = running;
		setChildStack();
		unlock
		return running->val;
	}
	else {
		unlock
		return -1;
	}
}

void Thread::exit() {
	lock
	running->finished = 1;
	running->signalCompleted();
	if (running->ifFork) {
		--running->parent->count;
		if (running->parent->count == 0 && running->parent->waitChildren)
			resume(running->parent);
		quitFork();
	}
	else
		suspend();
	// ovde nikad nece doci, ne moram da stavljam unlock
}

void Thread::waitForForkChildren() {
	lock
	if (running->count == 0) {
		unlock
		return;
	}
	
	while (running->count > 0) {
		running->waitChildren = 1;
		suspend();
	}
	running->waitChildren = 0;
	unlock
}

Thread* Thread::clone() const {
	return 0;
}

void Thread::start() {
	// lock
	lockMe();
	if (myPCB->started == 0) {
		myPCB->setStack();
		resume(myPCB);
		myPCB->started = 1;
		myPCB->listBlocked = new List();
	}
	// unlock
	unlockMe();
}

void Thread::waitToComplete() {
	// lock
	lockMe();
	if (myPCB != running && myPCB != idle && !myPCB->finished && myPCB->started) {
		myPCB->listBlocked->insert(running);
		unlockMe();
		suspend();
	}
	else
		unlockMe();
	// unlock
}

Thread::~Thread() {
	// lock
	waitToComplete();
	lockMe();
	delete myPCB;
	// unlock
	unlockMe();
}

ID Thread::getId() {
	return myPCB->id;
}

ID Thread::getRunningId() {
	return running->id;
}

Thread* Thread::getThreadById(ID id) {
	// lock
	lockMe();
	PCB* p = pcbs->get(id);
	Thread* t = 0;
	if (p)
		t = p->thread;
	// unlock
	unlockMe();
	return t;
}

Thread::Thread(StackSize stackSize, Time timeSlice) {
	// lock
	lockMe();
	myPCB = new PCB(stackSize, timeSlice, this);
	// unlock
	unlockMe();
}

