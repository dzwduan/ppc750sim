/***************************************************************************
                          Queue.h  -  description
                             -------------------
    begin                : Fri Jun 8 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <iostream>

template <class T,int nQueueEntries>
struct Queue
{
	T queue[nQueueEntries];
	
	int head;
	int tail;
	int size;

	Queue()
	{
		head = -1;
		tail = 0;
		size = 0;
	}
	
	bool Full()
	{
		return size == nQueueEntries;
	}
	
	bool Empty()
	{
		return size == 0;
	}
	
	int Size()
	{
		return size;
	}
	
	int GetFreeSpace()
	{
		return nQueueEntries - size;
	}
	
	T& Allocate()
	{
#ifdef DEBUG
		if(size >= nQueueEntries)
		{
			cout << "No space left in queue" << endl;
			sc_stop();
		}
#endif
		head = (head + 1) % nQueueEntries;
		size++;
		return queue[head];
	}
	
	T& GetTail()
	{
		return queue[tail];
	}
	
	void ReleaseTail()
	{
#ifdef DEBUG
		if(size <= 0)
		{
			cout << "Attempt to release tail while the queue is empty" << endl;
			ABORT();
		}
#endif
		tail = (tail + 1) % nQueueEntries;
		size--;
	}
	
	void Pop(int n)
	{
#ifdef DEBUG
		if(size < n)
		{
			cout << "Attempt to release " << n << " elements while the queue size is " << size << endl;
			ABORT();
		}
#endif
		tail = (tail + n) % nQueueEntries;
		size -= n;
	}
	
	T& GetHead()
	{
		return queue[head];
	}
	
	void ReleaseHead()
	{
#ifdef DEBUG
		if(size <= 0)
		{
			cout << "Attempt to release head while the queue is empty" << endl;
			ABORT();
		}
#endif
		head = (head + nQueueEntries - 1) % nQueueEntries;
		size--;
	}
	
	T& operator [] (int i)
	{
#ifdef DEBUG
		if(i < 0 || i >= size)
		{
			cout << "index for operator [ ] is out of range" << endl;
			sc_stop();
		}
#endif
		return queue[(tail + i) % nQueueEntries];
	}
	
	T& GetRecentEntry(int i)
	{
#ifdef DEBUG
		if(i < 0 || i >= size)
		{
			cout << "index for operator [ ] is out of range" << endl;
			sc_stop();
		}
#endif
		return queue[(head + nQueueEntries - i) % nQueueEntries];
	}
	
	void Flush()
	{
		head = -1;
		tail = 0;
		size = 0;	
	}
	
	void Flush(int n)
	{
#ifdef DEBUG
		if(size < n)
		{
			cout << "Attempt to flush " << n << " elements while the queue size is " << size << endl;
			ABORT();
		}
#endif
		head = (head + nQueueEntries - n) % nQueueEntries;
		size -= n;
	}
	
	T& GetEntry(int i)
	{
#ifdef DEBUG
		if(i < 0 || i >= nQueueEntries)
		{
			cout << "index for member function GetEntry is out of range" << endl;
			sc_stop();
		}
#endif
		return queue[i];
	}
	
	friend ostream& operator << (ostream& os, const Queue& queue)
	{
		int i, n;
		
		os << "Queue size = " << queue.size << ", tail = " << queue.tail << ", head = " << queue.head << endl;
		for(i = queue.head, n = queue.size; n > 0; n--,i = (i + nQueueEntries - 1) % nQueueEntries)
		{
			os << queue.queue[i] << endl;
		}
		return os;
	}
	
	int GetIndex(const T& entry)
	{
#ifdef DEBUG
		if(&entry < queue || &entry >= queue + nQueueEntries)
		{
			cout << "entry parameter passed to member function GetIndex is not into the queue" << endl;
			sc_stop();
		}
#endif
		return &entry - queue;
	}
	
	int GetTailIndex()
	{
		return tail;
	}
	
	bool Contains(int index)
	{
		if(tail <= head)
			return (index >= tail && index <= head);
			
		return (index >= tail && index < nQueueEntries) || (index <= head && index >= 0);
	}

#ifdef TRACE
	void Trace()
	{
		int i;
		for(i = 0; i < size; i++)
		{
			queue[(tail + i) % nQueueEntries].Trace();
		}
	}
#endif
};

#endif
