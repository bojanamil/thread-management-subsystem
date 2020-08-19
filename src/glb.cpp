#include "glb.h"
#include "pcb.h"
#include "schedule.h"
#include <dos.h>
#include "sortlist.h"
#include "ksem.h"
#include "kthread.h"

#define GET_STACK_VAL(tss,tsp) (*((unsigned*)MK_FP((tss), (tsp))))

void tick();

PCB* running = 0;
PCB* idle = 0;
PCB* child = 0;
SortList* sortList = 0;
IVTEntry* myEntries[256] = {0};
pInterrupt oldTimerInterrupt = 0;
Time counter = 0;
List* pcbs = 0;
int lockVal = 0;
int dispatchVal = 0;
int suspendVal = 0;
int quitForkVal = 0;

void lockMe() {
	lockVal++;
}

void unlockMe() {
	if (--lockVal == 0) {
		if (dispatchVal)
			dispatch();
		else if (suspendVal)
			suspend();
		else if (quitForkVal)
			quitFork();
	}
}

void dispatch() {
	lock
	if (lockVal)
		dispatchVal = 1;
	else
		dispatchInterrupt();
	unlock
}

void interrupt dispatchInterrupt() {
	// OVE KOMENTARE IZBRISATI
	// prekidna rutina
	// auto push:
	// psw, cs, ip (cs i ip su kao pc, samo segment i offset posebno, adresa instrukcije)
	// ax, bx, cx, dx, es, ds, si, di, bp
	// na kraju isto, auto pop obrnutim redosledom
	
	// samo je potrebno rucno sacuvati stari vrh steka,
	// preci na novu nit i restaurirati novi vrh steka
	// pri izlasku se sa novog steka skida kontekst nove na redu niti
	
	dispatchVal = 0;
	
	static unsigned int tss, tsp;
	asm {
		mov tss, ss;
		mov tsp, sp;
	}
	running->ss = tss;
	running->sp = tsp;
	
	if (running != idle)
		Scheduler::put(running);
	running = Scheduler::get();
	if (!running)
		running = idle;
	
	tss = running->ss;
	tsp = running->sp;
	asm {
		mov ss, tss;
		mov sp, tsp;
	}
	
	counter = 0;
}

void suspend() {
	lock
	if (lockVal)
		suspendVal = 1;
	else
		suspendInterrupt();
	unlock
}

void interrupt suspendInterrupt() {
	suspendVal = 0;
	
	static unsigned int tss, tsp;
	asm {
		mov tss, ss;
		mov tsp, sp;
	}
	running->ss = tss;
	running->sp = tsp;
	
	running = Scheduler::get();
	if (!running)
		running = idle;
	
	tss = running->ss;
	tsp = running->sp;
	asm {
		mov ss, tss;
		mov sp, tsp;
	}
	
	counter = 0;
}

void quitFork() {
	lock
	if (lockVal)
		quitForkVal = 1;
	else
		quitForkInterrupt();
	unlock
}

void interrupt quitForkInterrupt() {
	quitForkVal = 0;
	
	static unsigned int tss, tsp;
	static PCB* oldPCB;
	
	oldPCB = running;
	running = Scheduler::get();
	if (!running)
		running = idle;
	
	tss = running->ss;
	tsp = running->sp;
	asm {
		mov ss, tss;
		mov sp, tsp;
	}
	
	counter = 0;
	
	delete oldPCB->thread;
}

void resume(PCB* pcb) {
	lock
	if (pcb == 0) {
		unlock
		return;
	}
	
	Scheduler::put(pcb);
	unlock
}

void interrupt setChildStack() {
	static long num;
	num = child->stackSize;
	while (num--) {
		child->stack[num] = running->stack[num];
	}
	static unsigned runSS, runBegin, runBP, childSS, childBegin, childBP;
	childSS = FP_SEG(child->stack);
	childBegin = FP_OFF(child->stack);
	runSS = FP_SEG(running->stack);
	runBegin = FP_OFF(running->stack);
	asm {
		mov runBP, bp
	}
	childBP = runBP - runBegin + childBegin;
	child->sp = runBP - runBegin + childBegin;
	child->ss = FP_SEG(child->stack);
	static unsigned tempSS, tempBP;
	tempSS = FP_SEG(child->stack);
	tempBP = childBP;
	// int* x = nesto;
	// *x = *x + 3;
	while (GET_STACK_VAL(tempSS,tempBP)) {
		GET_STACK_VAL(tempSS,tempBP) = GET_STACK_VAL(tempSS,tempBP) - runBegin + childBegin;
		tempBP = GET_STACK_VAL(tempSS,tempBP);
	}
}

void insertTimer() {
	lock
	oldTimerInterrupt = getvect(8);
	setvect(8, &timerInterrupt);
	unlock
}

void removeTimer() {
	lock
	setvect(8, oldTimerInterrupt);
	unlock
}

void interrupt timerInterrupt(...) {
	if (oldTimerInterrupt != 0)
		(*oldTimerInterrupt)();
	if (running->timeSlice > 0)
		counter++;
	tick();
	if (sortList->getFirst() && sortList->getFirstTime() > 0) {
		sortList->setFirstTime(sortList->getFirstTime() - 1);
	}
	while (sortList->getFirst() && sortList->getFirstTime() == 0) {
		// budi prvi iz sort liste
		static PCB* pcb;
		pcb = sortList->remove();
		pcb->blockedOnSem->valSem++;
		pcb->blockedOnSem->list.remove(pcb);
		pcb->blockedOnSem = 0;
		pcb->val = 0;
		resume(pcb);
	}
	if (running == idle)
		dispatch();
	else if (running->timeSlice > 0 && counter > running->timeSlice)
		dispatch();
}



int main(int argc, char* argv[]) {
	pcbs = new List();
	sortList = new SortList();
	running = new PCB(defaultStackSize, defaultTimeSlice, 0);
	running->started = 1;
	Idle* t = new Idle();
	t->start();
	idle = Scheduler::get();
	Main* m = new Main(argc, argv);
	m->start();
	insertTimer();
	m->waitToComplete();
	// int r = userMain(argc, argv);
	int r = m->r;
	removeTimer();
	delete m;
	delete t;
	delete running;
	delete sortList;
	delete pcbs;
	return r;
}

