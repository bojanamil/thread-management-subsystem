#ifndef _PCB_H_
#define _PCB_H_

#include "thread.h"
#include "list.h"
class KernelSem;
class KernelEv;

class PCB {
public:
	StackSize stackSize;
	Time timeSlice;
	Thread* thread;
	unsigned int ss, sp;
	unsigned int* stack;
	
	int started;
	int finished;
	int waitChildren;
	
	List* listBlocked;
	KernelSem* blockedOnSem;
	int val;// kad se zavrsi wait semafora ili fork, ovo vraca
	KernelEv* blockedOnEv;
	
	ID id;
	static ID idd;
	
	int ifFork;
	int count;	// koliko dece ima
	PCB* parent;
	
	PCB(StackSize stackSize_, Time timeSlice_, Thread* thread_);
	~PCB();
	static void wrapper(Thread* thread);
	void setStack();
	void signalCompleted();
};

#endif