/***************************************************************************
                          BusInterfaceUnit.cpp  -  description
                             -------------------
    begin                : Thu May 10 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BusInterfaceUnit.h>
#include <string.h>

UInt32 BusInterfaceUnit::AlignToLineBoundary(UInt32 addr)
{
	return addr - (addr % linesize);
}

void BusInterfaceUnit::GetRequest()
{
	if(inICacheReq)
	{
		UInt32 addr = inICacheAddr;
#ifdef DEBUG
		if(Debug(DebugBusInterfaceUnit))
		{
			cout << name() << ": Getting an icache request at ";
			WriteHex(cout, addr);
			cout << endl;
		}
#endif		
		offset = addr % linesize;
		memoryPtr = GetStorage(AlignToLineBoundary(addr));
		length = 0;

									
		latency = memoryReadLatency;
		busActivity += memoryReadLatency;
				
		if(--latency > 0)
		{
			state = 5;
		}
		else
		{
			DataArray<icache2biu> data;
						
			outICacheAck = true;
			memcpy(data.buffer, &memoryPtr[offset], icache2biu);
			offset = (offset + icache2biu) % linesize;
			length += icache2biu;
				
#ifdef DEBUG
			if(Debug(DebugBusInterfaceUnit))
				cout << name() << ": Sending ack" << endl;
#endif
			
			if(length == linesize)
			{
				state = 0;
			}
			else
			{
				state = 6;
			}
			
			outICacheData = data;
		}
	} else if(inDCacheReq)
	{
		UInt32 addr = inDCacheAddr;
#ifdef DEBUG
		if(Debug(DebugBusInterfaceUnit))
		{
			cout << name() << ": Getting a dcache request at ";
			WriteHex(cout, addr);
			cout << endl;
		}
#endif
		offset = addr % linesize;
		memoryPtr = GetStorage(AlignToLineBoundary(addr));
		length = 0;
				
		if(inDCacheWrite)
		{
			latency = memoryWriteLatency;
			busActivity += memoryWriteLatency;
					
			if(--latency > 0)
			{				
				state = 1;
			}
			else
			{
				outDCacheAck = true;
				state = 2;
			}
		}
		else
		{
			latency = memoryReadLatency;
			busActivity += memoryReadLatency;
			
			if(--latency > 0)
			{
				state = 3;
			}
			else
			{
				DataArray<dcache2biu> data;
					
				outDCacheAck = true;
				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;
			
				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 4;
				}
				
				outDCacheData = data;
			}
		}
	}
}

void BusInterfaceUnit::Async()
{
	if(state == 0) GetRequest();
}

void BusInterfaceUnit::OnFrontEdge()
{
#ifdef DEBUG
	if(Debug(DebugBusInterfaceUnit))
		cout << name() << ": state = " << state << endl;
#endif
	switch(state)
	{
		case 0:
			GetRequest();
			outDCacheAck = false;
			break;
			
		case 1: // Waiting for the memory write latency
			if(--latency == 0)
			{
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Sending ack to DCache" << endl;
#endif
				outDCacheAck = true;
				state = 2;
			}
			break;
			
		case 2:
			outDCacheAck = false;
			break;
		
		case 3:
			if(--latency == 0)
			{
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Sending ack to DCache" << endl;
#endif
				outDCacheAck = true;
				
				DataArray<dcache2biu> data;
						
				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;
				
				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 4;
				}
				
				outDCacheData = data;
			}
			break;
			
		case 4:
			{
				outDCacheAck = false;
			
				DataArray<dcache2biu> data;
						
				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;
				
				if(length == linesize)
				{
					state = 0;
				}
				
				outDCacheData = data;
			}
			break;
			
		case 5:
			if(--latency == 0)
			{
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Sending ack to ICache" << endl;
#endif
				outICacheAck = true;
				
				DataArray<icache2biu> data;
						
				memcpy(data.buffer, &memoryPtr[offset], icache2biu);
				
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Reading " << icache2biu << " bytes (" << data << ")" << endl;
#endif
				offset = (offset + icache2biu) % linesize;
				length += icache2biu;
				
				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 6;
				}
				
				outICacheData = data;
			}
			break;
			
		case 6:
			{
				outICacheAck = false;
			
				DataArray<icache2biu> data;
						
				memcpy(data.buffer, &memoryPtr[offset], icache2biu);
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Reading " << icache2biu << " bytes (" << data << ")" << endl;
#endif
				offset = (offset + icache2biu) % linesize;
				length += icache2biu;
				
				if(length == linesize)
				{
					state = 0;
				}
				
				outICacheData = data;
			}
			break;
			
	}
}

void BusInterfaceUnit::OnFallingEdge()
{
	switch(state)
	{
		case 2: // Write Data into memory
			{
				outDCacheAck = false;
				
				const DataArray<dcache2biu>& data = inDCacheData;
				
				memcpy(&memoryPtr[offset], data.buffer, dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;
				
				if(length == linesize)
				{
					state = 0;
				}
			}
			break;
	}
}

UInt32 BusInterfaceUnit::hash1(UInt32 addr)
{
	return (addr / MemoryPageSize) % PrimaryMemoryHashTableSize;
//	return (39 * (addr / MemoryPageSize) + 11) % PrimaryMemoryHashTableSize;
}

UInt32 BusInterfaceUnit::hash2(UInt32 addr)
{
	return (addr / MemoryPageSize / PrimaryMemoryHashTableSize) % SecondaryMemoryHashTableSize;
//	return (41 *(addr / MemoryPageSize) + 7) % SecondaryMemoryHashTableSize;
}

void BusInterfaceUnit::InitializeHashTable()
{
	int i;
	
	for(i = 0; i < PrimaryMemoryHashTableSize; i++)
	{
		primaryHashTable[i] = 0;
	}
}

MemoryPageTableEntry *BusInterfaceUnit::AllocatePage(UInt32 addr)
{
	addr = addr - (addr % MemoryPageSize);
	MemoryPageTableEntry *pte = SearchPage(addr);
	if(!pte)
	{
		UInt32 h1 = hash1(addr);
		SecondaryMemoryHashTable *secondaryHashTable = primaryHashTable[h1];
		
		if(!secondaryHashTable)
		{
			secondaryHashTable = new SecondaryMemoryHashTable;
			int i;
			for(i = 0; i < SecondaryMemoryHashTableSize; i++)
			{
				secondaryHashTable->pte[i] = 0;
			}
			primaryHashTable[h1] = secondaryHashTable;
		}
		
		UInt32 h2 = hash2(addr);
		pte = new MemoryPageTableEntry;	
		pte->addr = addr;
		pte->storage = new UInt8[MemoryPageSize];
		memset(pte->storage, 0, MemoryPageSize);
		pte->next = secondaryHashTable->pte[h2];
		secondaryHashTable->pte[h2] = pte;
	}
	return pte;
}

MemoryPageTableEntry *BusInterfaceUnit::SearchPage(UInt32 addr)
{
	addr = addr - (addr % MemoryPageSize);
	
	UInt32 h1 = hash1(addr);
	SecondaryMemoryHashTable *secondaryHashTable = primaryHashTable[h1];
	
	if(secondaryHashTable)
	{
		UInt32 h2 = hash2(addr);
		MemoryPageTableEntry *pte = secondaryHashTable->pte[h2];
		
		if(pte)
		{
			do
			{
				if(pte->addr == addr) return pte;
			} while((pte = pte->next) != 0);
		}
	}
	return 0;
}

void BusInterfaceUnit::ZeroMemory(UInt32 addr, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;	
		if(size > sz)
		{
			memset(page->storage + offset, 0, sz);
			size -= sz;
			addr += sz;
			
			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memset(page->storage, 0, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}
		
			if(size > 0)
			{
				page = AllocatePage(addr);
				memset(page->storage, 0, size);
			}
		}
		else
		{
			memset(page->storage + offset, 0, size);
		}
	}
}

void BusInterfaceUnit::MemorySet(UInt32 addr, UInt8 value, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;	
		if(size > sz)
		{
			memset(page->storage + offset, value, sz);
			size -= sz;
			addr += sz;
			
			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memset(page->storage, value, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}
		
			if(size > 0)
			{
				page = AllocatePage(addr);
				memset(page->storage, value, size);
			}
		}
		else
		{
			memset(page->storage + offset, value, size);
		}
	}
}

void BusInterfaceUnit::MemoryWrite(UInt32 addr, UInt8 *buffer, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;	
		if(size > sz)
		{
			memcpy(page->storage + offset, buffer, sz);
			size -= sz;
			addr += sz;
			buffer += sz;
			
			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memcpy(page->storage, buffer, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
					buffer += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}
		
			if(size > 0)
			{
				page = AllocatePage(addr);
				memcpy(page->storage, buffer, size);
			}
		}
		else
		{
			memcpy(page->storage + offset, buffer, size);
		}
	}
}

void BusInterfaceUnit::MemoryRead(UInt8 *buffer, UInt32 addr, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;	
		if(size > sz)
		{
			memcpy(buffer, page->storage + offset, sz);
			size -= sz;
			addr += sz;
			buffer += sz;
			
			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memcpy(buffer, page->storage, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
					buffer += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}
		
			if(size > 0)
			{
				page = AllocatePage(addr);
				memcpy(buffer, page->storage, size);
			}
		}
		else
		{
			memcpy(buffer, page->storage + offset, size);
		}
	}
}

void BusInterfaceUnit::WriteDWord(UInt32 addr, UInt64 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset] = value >> 56;
	pte->storage[offset + 1] = value >> 48;
	pte->storage[offset + 2] = value >> 40;
	pte->storage[offset + 3] = value >> 32;
	pte->storage[offset + 4] = value >> 24;
	pte->storage[offset + 5] = value >> 16;
	pte->storage[offset + 6] = value >> 8;
	pte->storage[offset + 7] = value;
}

UInt64 BusInterfaceUnit::ReadDWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return ((UInt64) pte->storage[offset] << 56)
	     | ((UInt64) pte->storage[offset + 1] << 48)
	     | ((UInt64) pte->storage[offset + 2] << 40)
	     | ((UInt64) pte->storage[offset + 3] << 32)
	     | ((UInt64) pte->storage[offset + 4] << 24)
	     | ((UInt64) pte->storage[offset + 5] << 16)
	     | ((UInt64) pte->storage[offset + 6] << 8)
	     | ((UInt64) pte->storage[offset + 7]);	
}

void BusInterfaceUnit::WriteWord(UInt32 addr, UInt32 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset] = value >> 24;
	pte->storage[offset + 1] = value >> 16;
	pte->storage[offset + 2] = value >> 8;
	pte->storage[offset + 3] = value;
}

UInt32 BusInterfaceUnit::ReadWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return (pte->storage[offset] << 24) | (pte->storage[offset + 1] << 16) | (pte->storage[offset + 2] << 8) | (pte->storage[offset + 3]);
}

void BusInterfaceUnit::WriteHalfWord(UInt32 addr, UInt16 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset] = value >> 8;
	pte->storage[offset + 1] = value;
}

UInt16 BusInterfaceUnit::ReadHalfWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return (pte->storage[offset] << 8) | (pte->storage[offset + 1]);
}

void BusInterfaceUnit::WriteByte(UInt32 addr, UInt8 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset] = value;
}

UInt8 BusInterfaceUnit::ReadByte(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return pte->storage[offset];
}

UInt8 *BusInterfaceUnit::GetStorage(UInt32 addr)
{
	MemoryPageTableEntry *pte = AllocatePage(addr);
	
	if(pte) return pte->storage + (addr % MemoryPageSize);
	
	cout << name() << ": Error : address ";
	WriteHex(cout, addr);
	cout << " is not mapped" << endl;
	StopSimulation(); return 0;
	return 0;
}

UInt32 BusInterfaceUnit::AlignToPageBoundary(UInt32 addr)
{
	return addr - (addr % MemoryPageSize);
}

void BusInterfaceUnit::Reset()
{
	state = 0;
	busActivity = 0;
}
