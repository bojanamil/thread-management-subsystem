#ifndef _KSEM_H_
#define _KSEM_H_

#include "list.h"

class KernelSem {
public:
	KernelSem(int init);
	int valSem;
	List list;
};

#endif