/***************************************************************************
                          RSQueue.h  -  description
                             -------------------
    begin                : Thu Jul 19 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __RSQUEUE_H__
#define __RSQUEUE_H__

/*

          +--------------+   +---------------+   +---------------+
oldest -->|          next|-->|           next|-->|           next|--> null
          |      RS      |   |       RS      |   |       RS      |
  null <--|prev          |<--|prev           |<--|prev           |<-- newest
          +--------------+   +---------------+   +---------------+
*/

template <class T, int nRS>
struct RSQueue
{
	int size;			/* number of elements into the queue */
	T stations[nRS];	/* Storage for all reservation stations into the queue */
	T *newest;			/* the most recent reservation station */
	T *oldest;			/* the oldest reservation station */
	
	RSQueue()
	{
		size = 0;
		oldest = newest = 0;
	}
	
	T& Allocate()
	{
		int i;
		T *new_rs = 0;
		for(i = 0; i < nRS; i++)
		{
			if(!stations[i].full)
			{
				new_rs = &stations[i];
				break;
			}
		}
		
		new_rs->next = 0;
		new_rs->prev = newest;
		if(newest) newest->next = new_rs;
		if(!oldest) oldest = new_rs;
		newest = new_rs;
		size++;
		new_rs->full = true;
		return *new_rs;
	}
	
	T *GetOldestValid()
	{
		T *cur_rs = oldest;
		
		if(cur_rs)
		{
			if(cur_rs->Valid()) return cur_rs;
		
			for(cur_rs = cur_rs->next; cur_rs; cur_rs = cur_rs->next)
			{
				if(!cur_rs->executionSerialized && cur_rs->Valid()) return cur_rs;
			}
		}
		return 0;
	}
	
	void Remove(T *rs)
	{
		if(rs->next) rs->next->prev = rs->prev;
		if(rs->prev) rs->prev->next = rs->next;
		if(oldest == rs) oldest = rs->next;
		if(newest == rs) newest = rs->prev;
		rs->full = false;
		size--;
	}
	
	bool Full()
	{
		return size == nRS;
	}	
	
	int Empty()
	{
		return size == 0;
	}
	
	int GetFreeSpace()
	{
		return nRS - size;
	}
	
	void Flush()
	{
		T *cur_rs, *next_rs;
		
		for(cur_rs = oldest; cur_rs; cur_rs = next_rs)
		{
			next_rs = cur_rs->next;
			
			if(cur_rs->branchCounter != 0)
				Remove(cur_rs);
		}
	}
	
	void DecrementBranchCounter()
	{
		T *cur_rs;
		
		for(cur_rs = oldest; cur_rs; cur_rs = cur_rs->next)
		{
			if(cur_rs->branchCounter > 0) cur_rs->branchCounter--;
		}
	}
};

#endif
