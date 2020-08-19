#ifndef _LIST_H_
#define _LIST_H_

class PCB;
typedef int ID;

class List {
public:
	List();
	~List();
	void insert(PCB* pcb);// stavlja poslednji
	PCB* remove();// vadi prvi
	void remove(PCB* pcb);// vadi zadati
	PCB* get(ID id);
	
	struct Elem {
		PCB *pcb;
		Elem *next;
		Elem *prev;
		
		Elem(PCB *pcb_) {
			pcb = pcb_;
			next = prev = 0;
		}
	};
	
	Elem *first, *last;
	
};

#endif