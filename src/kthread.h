#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include "thread.h"

class Idle : public Thread {
public:
	void run() {
		while (1) {
			// lezi spavaj
			dispatch();
		}
	}
};

int userMain(int argc, char* argv[]);
class Main : public Thread {
public:
	void run() {
		r = userMain(argc, argv);
	}
	virtual ~Main() {
		waitToComplete();
	}
	Thread* clone() const {
		return new Main(argc, argv);
	}
	Main(int argc_, char** argv_) {
		argc = argc_;
		argv = argv_;
	}
	int r;
	int argc;
	char** argv;
};

#endif