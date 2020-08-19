#include "list.h"
#include "pcb.h"

List::List() {
	first = last = 0;
}

List::~List() {
	while (first != 0) {
		Elem* elem = first;
		first = first->next;
		delete elem;
	}
}

void List::insert(PCB* pcb) {
	Elem *elem = new Elem(pcb);
	
	if (last != 0) {
		last->next = elem;
		elem->prev = last;
		last = elem;
	}
	else {
		first = last = elem;
	}
}

PCB* List::remove() {
	if (first != 0) {
		Elem *elem = first;
		first = first->next;
		if (first != 0) {
			first->prev = 0;
		}
		else {
			last = 0;
		}
		
		elem->next = 0;
		PCB* pcb = elem->pcb;
		delete elem;
		return pcb;
	}
	else {
		return 0;
	}
}

void List::remove(PCB* pcb) {
	Elem *elem = first;
	while (elem != 0 && elem->pcb != pcb) {
		elem = elem->next;
	}
	
	if (elem != 0) {
		if (elem->prev) elem->prev->next = elem->next;
		else first = elem->next;
		
		if (elem->next) elem->next->prev = elem->prev;
		else last = elem->prev;
		
		delete elem;
	}
}

PCB* List::get(ID id) {
	Elem* cur = first;
	while (cur) {
		if (cur->pcb->id == id)
			return cur->pcb;
		else
			cur = cur->next;
	}
	
	return 0;
}

