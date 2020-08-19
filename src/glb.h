#ifndef _GLB_H_
#define _GLB_H_

#define lock { asm pushf; asm cli; }
#define unlock { asm popf; }

// wrapper klase zakljucavaju da se izbegne ugnezdavanje
// ovde nije bitno jer mogu lock i unlock da se ugnezde, ali
// za inace bi bilo potrebno da se pazi, npr. ako se pisu
// neke niti koje koriste semafore i rekurzivne funkcije

// void f()
// {
	// //...
	// if (uslov)
		// f();
	// //...
// }

// void f_wrapper()
// {
	// s->wait();
	// f();
	// s->signal();
// }

typedef void interrupt (*pInterrupt) (...);

class PCB;
class IVTEntry;
class List;

// zameni running nit
void dispatch();
void interrupt dispatchInterrupt();

// blokiraj running nit
void suspend();
void interrupt suspendInterrupt();

// napusti fork nit
void quitFork();
void interrupt quitForkInterrupt();

// probudi ovu nit
void resume(PCB* pcb);

// postavi stek fork detetu
void interrupt setChildStack();

extern PCB* running;
extern PCB* idle;
extern PCB* child;

class SortList;
extern SortList* sortList;

extern IVTEntry* myEntries[256];

extern List* pcbs;


void insertTimer();
void removeTimer();
void interrupt timerInterrupt(...);

extern int lockVal;
extern int dispatchVal;
extern int suspendVal;
extern int quitForkVal;
void lockMe();
void unlockMe();

#endif