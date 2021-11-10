#pragma once
#include <Windows.h>


#define QMAX 100


typedef struct queue {
	int qu[QMAX];
	int rear, frnt;
	HANDLE mutex;
}QUEUE, *PQUEUE;

void init(struct queue* q);

void insert(struct queue* q, int x);

int isempty(struct queue* q);

int remove(struct queue* q);