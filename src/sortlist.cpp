#include "sortlist.h"

SortList::SortList() {
	first = 0;
}

SortList::~SortList() {
	while (first) {
		Elem* next = first->next;
		delete first;
		first = next;
	}
}

void SortList::insert(PCB* pcb, Time time) {
	if (pcb == 0 || time == 0)
		return;
	
	Elem *cur, *prev;
	
	for (prev = 0, cur = first; cur != 0; /*nista*/) {
		if (cur->time >= time)
			break;
		
		prev = cur;
		cur = cur->next;
		time -= prev->time;
	}
	
	if (cur)
		cur->time -= time;
	
	Elem* e = new Elem(pcb, time);
	
	if (prev)
		prev->next = e;
	
	e->next = cur;
	
	if (!prev)
		first = e;
}

PCB* SortList::remove() {
	PCB* p = 0;
	if (first) {
		Elem* next = first->next;
		p = first->pcb;
		delete first;
		first = next;
	}
	return p;
}

PCB* SortList::getFirst() {
	PCB* p = 0;
	if (first)
		p = first->pcb;
	return p;
}

Time SortList::getFirstTime() {
	Time t = 0;
	if (first)
		t = first->time;
	return t;
}

void SortList::setFirstTime(Time time) {
	if (first)
		first->time = time;
}

void SortList::remove(PCB* pcb) {
	Elem* prev = 0;
	Elem* cur = first;
	
	while (cur && cur->pcb != pcb) {
		prev = cur;
		cur = cur->next;
	}
	
	if (cur) {
		if (cur->next)
			cur->next->time += cur->time;
		
		if (prev)
			prev->next = cur->next;
		else
			first = cur->next;
		
		delete cur;
	}
}

