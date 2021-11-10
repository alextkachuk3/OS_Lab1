#include "queue.h"

void init(struct queue* q) {
	q->mutex = CreateMutex(
		NULL,
		FALSE,
		NULL);
	q->frnt = 1;
	q->rear = 0;
	return;
}

void insert(struct queue* q, int x) {

	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(
		q->mutex,
		INFINITE);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		__try
		{
			if (q->rear < QMAX - 1) {
				q->rear++;
				q->qu[q->rear] = x;
			}
			else
				printf("Queue out of limit!\n");
		}

		__finally
		{
			ReleaseMutex(q->mutex);
		}
		break;

	case WAIT_ABANDONED:
		return printf("Insert error\n");
	}

	return;
}

int isempty(struct queue* q) {
	if (q->rear < q->frnt)    return 1;
	else  return 0;
}

int remove(struct queue* q) {
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(
		q->mutex,
		INFINITE);

	int x = 0;

	if (isempty(q) == 1) {
		printf("Queue is empty!\n");
		return 0;
	}

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		__try
		{
			int h;
			
			x = q->qu[q->frnt];
			for (h = q->frnt; h < q->rear; h++) {
				q->qu[h] = q->qu[h + 1];
			}
			q->rear--;

		}

		__finally
		{
			ReleaseMutex(q->mutex);
		}
		break;

	case WAIT_ABANDONED:
		return printf("Delete error\n");
	}

	return x;
}