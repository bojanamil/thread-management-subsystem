#ifndef _KEV_H_
#define _KEV_H_

typedef unsigned char IVTNo;
class PCB;

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	void signal();
	IVTNo ivtNo;
	PCB* pcb;
};

#endif