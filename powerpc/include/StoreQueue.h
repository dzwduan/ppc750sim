/***************************************************************************
                          StoreQueue.h  -  description
                             -------------------
    begin                : Wed Apr 25 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __STOREQUEUE_H__
#define __STOREQUEUE_H__

#include <common.h>
#include <iostream>

struct StoreQueueEntry
{
	UInt32 addr; /* Adresse du store */
	LoadStoreTags tags; /* tags de l'instruction store */
	LoadStoreOperation operation;
	int branchCounter;
	UInt64 data;
	bool valid; // true si le store a �t�/est retir� du ROB
	tag_t dependentLoadTag;
	int state;
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif
	bool full;
	StoreQueueEntry *prev;
	StoreQueueEntry *next;
	
	StoreQueueEntry();

	friend ostream& operator << (ostream& os, const StoreQueueEntry& entry);
	
#ifdef TRACE
	void Trace();
#endif
};

struct StoreQueue
{
	int size;
	StoreQueueEntry entries[nStoreQueueEntries];
	StoreQueueEntry *newest;
	StoreQueueEntry *oldest;
	
	StoreQueue()
	{
		Reset();
	}
	
	void Reset()
	{
		size = 0;
		
		int i;
		for(i = 0; i < nStoreQueueEntries; i++)
		{
			entries[i].full = false;
			entries[i].next = i > 0 ? &entries[i - 1] : 0;
			entries[i].prev = i < nStoreQueueEntries - 1 ? &entries[i + 1] : 0;
		}
		newest = &entries[0];
		oldest = &entries[nStoreQueueEntries - 1];
	}
	
	StoreQueueEntry *Allocate()
	{
		size++;
		newest->full = true;
		return newest;
	}
	
	
	StoreQueueEntry *GetOldestValid()
	{
		if(oldest->full && oldest->valid) return oldest;
		return 0;
	}
	
	StoreQueueEntry *GetOldest()
	{
		if(oldest->full) return oldest;
		return 0;
	}
	
	void ReleaseTail()
	{
		oldest->full = false;
		size--;
	}
	
	bool Full()
	{
		return size == nStoreQueueEntries;
	}	
	
	int Empty()
	{
		return size == 0;
	}
	
	int GetFreeSpace()
	{
		return nStoreQueueEntries - size;
	}
	
	void Flush()
	{
		StoreQueueEntry *cur_entry, *next_entry;
		
		for(cur_entry = oldest; cur_entry; cur_entry = next_entry)
		{
			next_entry = cur_entry->next;
			
			if(cur_entry->full  && cur_entry->branchCounter != 0)
			{
				cur_entry->full = false;
				size--;
			}
		}
	}
	
	void DecrementBranchCounter()
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && entry->branchCounter > 0) entry->branchCounter--;
		}
	}
	
	void Validate(int tag)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && !entry->valid)
			{
				if(entry->tags.tag != tag)
				{
					cout << "Write Back for store not in order" << endl;
				}
				entry->valid = true;
				return;
			}
		}
	}
	
	void Shift()
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(!entry->full)
			{
				if(entry->next)
				{
					entry->next->prev = entry->prev;
					if(entry->prev) entry->prev->next = entry->next;
					if(oldest == entry) oldest = entry->next;
					newest->next = entry;
					entry->next = 0;
					entry->prev = newest;
					newest = entry;
				}
				return;
			}
		}
	}
	
	bool Contains(UInt32 addr, int size)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && ((addr >= entry->addr && addr < entry->addr + entry->operation.size) || (entry->addr >= addr && entry->addr < addr + size))) return true;
		}
		return false;
	}
	
	StoreQueueEntry *GetOldestContaining(UInt32 addr, int size)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && ((addr >= entry->addr && addr < entry->addr + entry->operation.size) || (entry->addr >= addr && entry->addr < addr + size))) return entry;
		}
		return NULL;
	}
	
	void SetLoadDependency(tag_t tag, addr_t addr, int size)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && ((addr >= entry->addr && addr < entry->addr + entry->operation.size) || (entry->addr >= addr && entry->addr < addr + size)))
			{
				entry->dependentLoadTag = tag;
			}
		}
	}
	
	void RemoveLoadDependency(tag_t tag)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && entry->dependentLoadTag == tag)
			{
				entry->dependentLoadTag = -1;
			}
		}
	}
	
	bool HasLoadDependency(tag_t tag)
	{
		StoreQueueEntry *entry;
		
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full && entry->dependentLoadTag == tag)
			{
				return true;
			}
		}
		return false;
	}
		
	friend ostream& operator << (ostream& os, const StoreQueue& queue);
	
#ifdef TRACE
	void Trace()
	{
		StoreQueueEntry *entry;

		trace_file->Begin("store_queue");	
		for(entry = oldest; entry; entry = entry->next)
		{
			if(entry->full)
			{
				entry->Trace();
			}
		}
		trace_file->End("store_queue");
	}
#endif
};

inline ostream& operator << (ostream& os, const StoreQueue& queue)
{
	StoreQueueEntry *entry;
	
	cout << "Store queue size = " << queue.size << endl;
	for(entry = queue.newest; entry; entry = entry->prev)
	{
		os << *entry << endl;
	}
	return os;
}

#endif
