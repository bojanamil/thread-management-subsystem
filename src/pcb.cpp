#include "pcb.h"
#include <dos.h>
#include "glb.h"

ID PCB::idd = 0;

PCB::PCB(StackSize stackSize_, Time timeSlice_, Thread* thread_) {
	stackSize = stackSize_ / sizeof(unsigned int);
	timeSlice = timeSlice_;
	thread = thread_;
	stack = 0;
	started = 0;
	finished = 0;
	listBlocked = 0;
	blockedOnSem = 0;
	blockedOnEv = 0;
	id = ++idd;
	pcbs->insert(this);
	ifFork = 0;
	count = 0;
	parent = 0;
}

PCB::~PCB() {
	pcbs->remove(this);
	delete[] stack;
	delete listBlocked;
}

void PCB::wrapper(Thread* thread) {
	asm sti;
	thread->run();
	Thread::exit();
}

void PCB::setStack() {
	stack = new unsigned int[stackSize];
	stack[stackSize-1] = FP_SEG(this->thread);
	stack[stackSize-2] = FP_OFF(this->thread);
	stack[stackSize-3] = 0;	// cs addr iz wrapper u nista
	stack[stackSize-4] = 0;	// ip addr iz wrapper u nista
	stack[stackSize-5] = 0; // asm cli pred run() u wrapper()
	stack[stackSize-6] = FP_SEG(PCB::wrapper);	// cs addr iz dispatchInterrupt u wrapper na pocetak
	stack[stackSize-7] = FP_OFF(PCB::wrapper);	// ip addr iz dispatchInterrupt u wrapper na pocetak
	stack[stackSize-16] = 0;	// bp za fork granicu za popravku bp na steku
	ss = FP_SEG(&stack[stackSize-16]);
	sp = FP_OFF(&stack[stackSize-16]);
}

void PCB::signalCompleted() {
	while (listBlocked->first) {
		resume(listBlocked->remove());
	}
}

