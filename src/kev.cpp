#include "kev.h"
#include "glb.h"
#include "pcb.h"

KernelEv::KernelEv(IVTNo ivtNo) {
	this->ivtNo = ivtNo;
	pcb = running;
}

void KernelEv::signal() {
	if (pcb->blockedOnEv == this) {
		pcb->blockedOnEv = 0;
		resume(pcb);
	}
}

