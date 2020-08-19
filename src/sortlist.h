#ifndef _SORTLIST_H_
#define _SORTLIST_H_

typedef unsigned int Time;
class PCB;

class SortList {
public:
	SortList();
	~SortList();
	void insert(PCB* pcb, Time time);
	PCB* remove(); // izvadi prvi
	PCB* getFirst(); // samo dohvati prvi
	Time getFirstTime(); // dohvati vreme prvog
	void setFirstTime(Time time);
	void remove(PCB* pcb);
	
	struct Elem {
		PCB *pcb;
		Time time;
		Elem *next;
		
		Elem(PCB *pcb_, Time time_) {
			pcb = pcb_;
			time = time_;
			next = 0;
		}
	};
	
	Elem *first;
};

#endif