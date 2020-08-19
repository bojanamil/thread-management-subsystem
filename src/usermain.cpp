// NI SLUCAJNO NE PREDAVATI OVAJ FAJL
// SALIM SE
// NE SALIM SE

#include "thread.h"
#include "semaphor.h"
#include "event.h"

#include <iostream.h>
#include <stdlib.h>

// PREPAREENTRY(9,1);

// #define TEST_KEY

void tick() {}


Semaphore* ioMutex = 0;
Semaphore* sleepSem = 0;
Semaphore* randMutex = 0;


class Znak : public Thread {
public:
	Znak(char znak, int n) : Thread(), znak(znak), n(n) {}
	virtual ~Znak() { waitToComplete(); }
	
	void run() {
		// for (long i = 0; i < 100000; i++) {
		for (int i = 0; i < n; i++) {
			if (ioMutex->wait(1)) {
				cout << znak;
				ioMutex->signal();
			}
			
			// for (int j = 0; j < 10000; j++)
				// for (int k = 0; k < 10000; k++);
			
			// sleepSem->wait(1);
			
			randMutex->wait(0);
			Time sleepTime = 1 + rand() % 4;
			randMutex->signal();
			sleepSem->wait(sleepTime);
			
			// dispatch();
		}
		
		cout << endl << znak << " finished" << endl;
	}
	
private:
	char znak;
	int n;
	
};


class Key : public Thread {
public:
	Key(int n) : Thread(), n(n) {}
	virtual ~Key() { waitToComplete(); }
	
	void run() {
		Event e(9);
		
		for (int i = 0; i < n; i++) {
			ioMutex->wait(0);
			cout << endl << "key waiting " << (i + 1) << endl;
			ioMutex->signal();
			
			e.wait();
			
			ioMutex->wait(0);
			cout << endl << "key continue " << (i + 1) << endl;
			ioMutex->signal();
		}
		
		ioMutex->wait(0);
		cout << endl << "key finished" << endl;
		ioMutex->signal();
	}
	
private:
	int n;
	
};



int userMain(int argc, char* argv[]) {
	cout << "userMain began" << endl;
	
	ioMutex = new Semaphore(1);
	sleepSem = new Semaphore(0);
	randMutex = new Semaphore(1);
	
	int i, n = 26;
	Znak** zn = new Znak*[n];
	for (i = 0; i < n; i++)
		zn[i] = new Znak('a' + i, 10 + 5 * i);
#ifdef TEST_KEY
	Key* k = new Key(80);
#endif
	
	for (i = 0; i < n; i++)
		zn[i]->start();
#ifdef TEST_KEY
	k->start();
#endif
	
	for (i = 0; i < n; i++)
		delete zn[i];
	delete[] zn;
#ifdef TEST_KEY
	delete k;
#endif
	
	cout << endl << "userMain finished" << endl;
	
	delete randMutex;
	delete sleepSem;
	delete ioMutex;
	
	return 0;
}

