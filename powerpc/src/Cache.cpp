/***************************************************************************
                          Cache.cpp  -  Cache
                             -------------------
    begin                : Mon Apr 30 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <Cache.h>

#if defined(ICACHE) || defined(DCACHE)

#ifdef ICACHE
#define CLASSNAME ICache
#define CPU2CACHE fetch2icache
#define CACHE2MEM icache2biu
#else
#define CLASSNAME DCache
#define CPU2CACHE lsu2dcache
#define CACHE2MEM dcache2biu
#endif

void CLASSNAME::DecodeAddress(UInt32 addr, UInt32& tag, UInt32& index, UInt32& offset)
{
	offset = addr & offsetMask;
	index = (addr >> indexShift) & indexMask;
	tag = addr >> tagShift;
#ifdef DEBUG
	if(Debug(DebugCache))
	{
		cout << name() << ": Decoding address ";
		WriteHex(cout, addr);
		cout << " (tag = " << tag << ", index = " << index << ", offset = " << offset << ")" << endl;
	}
	if(offset >= linesize)
	{
		cout << name() << ": Error while decoding address : offset is out of range" << endl;
		ABORT();
	}
	if(index >= nlines)
	{
		cout << name() << ": Error while decoding address : index is out of range" << endl;
		ABORT();
	}
#endif
}

int CLASSNAME::Search(UInt32 index, UInt32 tag)
{
	int bank;
	
#ifdef DEBUG
	if(Debug(DebugCache))
		cout << name() << ": Searching for tag " << tag << " at index " << index << endl;
#endif
	
	for(bank = 0; bank < associativity; bank++)
	{
		if(directory[bank][index].valid && directory[bank][index].tag == tag)
		{
#ifdef DEBUG
	if(Debug(DebugCache))
		cout << name() << ": tag found in bank " << bank << endl;
#endif
			return bank;
		}
	}
#ifdef DEBUG
	if(Debug(DebugCache))
		cout << name() << ": tag not found" << endl;
#endif
	return -1;
}

const int B0 = 1 << 0;
const int B1 = 1 << 1;
const int B2 = 1 << 2;
const int B3 = 1 << 3;
const int B4 = 1 << 4;
const int B5 = 1 << 5;
const int B6 = 1 << 6;

void CLASSNAME::UpdatePLRUBits(UInt32 bank, UInt32 index)
{
	switch(associativity)
	{
		case 4:
			switch(bank)
			{
				case 0:
					plrubits[index] |= B0  + B1;
					break;
					
				case 1:
					plrubits[index] |= B0;
					plrubits[index] &= B1;
					break;
					
				case 2:
					plrubits[index] |= B2;
					plrubits[index] &= B0;
					break;
					
				case 3:
					plrubits[index] &= B0 + B2;
					break;
			}
			break;
	
		case 8:
			switch(bank)
			{
				case 0:
					plrubits[index] |= B0 + B1 + B3;
					break;
				case 1:
					plrubits[index] |= B0 + B1;
					plrubits[index] &= ~B3;
					break;
			
				case 2:
					plrubits[index] |= B0 + B4;
					plrubits[index] &= ~B1;
					break;
			
				case 3:
					plrubits[index] |= B0;
					plrubits[index] &= ~(B1 + B4);
					break;
			
				case 4:
					plrubits[index] |= B2 + B5;
					plrubits[index] &= ~B0;
					break;
			
				case 5:
					plrubits[index] |= B2;
					plrubits[index] &= ~(B0 + B5);
					break;
				
				case 6:
					plrubits[index] |=B6;
					plrubits[index] &=~(B0 + B2);
					break;
			
				case 7:
					plrubits[index] &= ~(B0 + B2 + B6);
					break;
			}
			break;
	}
}

void CLASSNAME::Read(UInt32 bank, UInt32 index, UInt32 offset, UInt8 *data, int size)
{
	memcpy(data, &storage[bank][index][offset], size);
	
	UpdatePLRUBits(bank, index);
}

UInt32 CLASSNAME::MakeAddr(UInt32 bank, UInt32 index, UInt32 offset)
{
	return (directory[bank][index].tag << tagShift) | (index << indexShift) | offset;
}

#ifndef ICACHE
void CLASSNAME::Write(UInt32 bank, UInt32 index, UInt32 offset, const UInt8 *data, int size)
{
	memcpy(&storage[bank][index][offset], data, size);
	
	UpdatePLRUBits(bank, index);
	directory[bank][index].dirty = true;
}

void CLASSNAME::Zero(UInt32 bank, UInt32 index)
{
	memset(&storage[bank][index], 0, linesize);
	
	UpdatePLRUBits(bank, index);
	directory[bank][index].dirty = true;
}
#endif

int CLASSNAME::Choose(UInt32 index)
{
	/* See figure 3-5. PLRU Replacement Algorithm  */
	/*     table 3-2. PLRU Bit Update Rules */
	/* of MPC750 RISC Microprocessor User's Manual */
	int bank;
	
	for(bank = 0; bank < associativity; bank++)
	{
		if(!directory[bank][index].valid)
		{
			return bank;
		}
	}
	
	switch(associativity)
	{
		case 4:
			if(plrubits[index] & B0)
			{
				if(plrubits[index] & B2) return 3; else return 2;
			}
			else
			{
				if(plrubits[index] & B1) return 1; else return 0;
			}
			break;
			
		case 8:
			if(plrubits[index] & B0)
			{
				if(plrubits[index] & B2)
				{
					if(plrubits[index] & B5) return 5; else return 4;
				}
				else
				{
					if(plrubits[index] & B6) return 7; else return 6;
				}
			}
			else
			{
				if(plrubits[index] & B1)
				{
					if(plrubits[index] & B4) return 3; else return 2;
				}
				else
				{
					if(plrubits[index] & B3) return 1; else return 0;
				}
			}
	}
	
	return -1;
}

bool CLASSNAME::IsValid(UInt32 bank, UInt32 index)
{
	return directory[bank][index].valid;
}

#ifndef ICACHE
bool CLASSNAME::IsDirty(UInt32 bank, UInt32 index)
{
	return directory[bank][index].dirty;
}
#endif

UInt32 CLASSNAME::GetLineAddr(UInt32 bank, UInt32 index)
{
	return (directory[bank][index].tag << tagShift) | (index << indexShift);
}

UInt32 CLASSNAME::AlignToLineBoundary(UInt32 addr)
{
	return addr - (addr % linesize);
}

UInt32 CLASSNAME::AlignToMemoryWord(UInt32 addr)
{
	return addr - (addr % memoryWord);
}

UInt8 *CLASSNAME::GetLineStorage(UInt32 bank, UInt32 index)
{
	return &storage[bank][index][0];
}

void CLASSNAME::ReplaceLine(UInt32 bank, UInt32 index, UInt32 tag)
{
	directory[bank][index].tag = tag;
	directory[bank][index].valid = true;
#ifndef ICACHE
	directory[bank][index].dirty = false;
#endif
}

void CLASSNAME::GetRequest()
{
	bool ack = false;

	if(inReq)
	{
		// CPU has made a request
		UInt32 /*curInstructionTag,*/ curTag, curIndex, curOffset, curSize, curAddr;
#ifndef ICACHE
		bool curZeroBlock;
#endif
		int curBank;
		
		curAddr = inAddr;
//		curInstructionTag = inTag;
		curSize = inSize;
#ifndef ICACHE
		curZeroBlock = inZeroBlock;
#endif
		
#ifdef DEBUG
		if(Debug(DebugCache))
		{
			cout << name() << ": Getting a CPU request at ";
			WriteHex(cout, curAddr);
			cout << " for " << curSize << " bytes " << endl;
		}
#endif
		DecodeAddress(curAddr, curTag, curIndex, curOffset);
		
		curBank = Search(curIndex, curTag);
		
		hit = curBank >= 0;
		
		if(hit)
		{
			hits++;
			// Hit
			ack = true;
//			outTag = curInstructionTag;
			
#ifndef ICACHE
			if(!inWrite)
			{
#endif
				// Read
				DataArray<CPU2CACHE> data;
				
				Read(curBank, curIndex, curOffset, data.buffer, curSize);
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Reading " << curSize << " bytes (" << data << ")" << endl;
#endif
				outData = data;
#ifndef ICACHE
			}
			else
			{
				writeBank = curBank;
				writeIndex = curIndex;
				writeOffset = curOffset;
				writeZeroBlock = curZeroBlock;
			}
#endif
		}
		else
		{
			misses++;
#ifdef DEBUG
			if(Debug(DebugCache))
				cout << name() << ": Miss" << endl;
#endif
			// Miss
			
			if(!busy)
			{
				busy = true;
				
				tag = curTag;
				offset = curOffset;
				index = curIndex;
				addr = curAddr;
				size = curSize;
#ifndef ICACHE
				zeroBlock = curZeroBlock;
#endif
//				instructionTag = curInstructionTag;
				
				bankToReplace = Choose(index);
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Choosing bank " << bankToReplace << endl;
#endif				
			
		
#ifndef ICACHE		
				if(inWrite)
				{
//			ack = true;
					// Miss on write
					if(IsValid(bankToReplace, index) && IsDirty(bankToReplace, index))
					{
						// Cache line eject
						state = 1;
					}
					else
					{
						if(zeroBlock)
						{
							// Cache line fill with zeros
							state = 5;
						}
						else
						{
							// Cache line fill
							state = 3;
						}
					}
				}
				else
#endif
				{
					// Miss on read
#ifndef ICACHE
					if(IsValid(bankToReplace, index) && IsDirty(bankToReplace, index))
					{
						// Cache line eject
						state = 6;
					}
					else
#endif
					{
						// Cache line fill
#ifdef DEBUG
						if(Debug(DebugCache))
							cout << name() << ": Filling line" << endl;
#endif
						state = 8;
					}
				}
			}
		}
	}
#ifdef DEBUG
	if(Debug(DebugCache))
		if(ack)
			cout << name() << ": Sending an ack" << endl;
#endif
	outAck = ack;
}

void CLASSNAME::Async()
{
	if(state == 0) GetRequest();
}

void CLASSNAME::OnFrontEdge()
{
	hit = false;
	
#ifdef ICACHE
	if((state == 6 || state == 8) && inCancel)
	{
#ifdef DEBUG
		if(Debug(DebugCache))
			cout << name() << ": canceling previous request" << endl;
#endif
		busy = false;
		state = 0;
	}
#endif
#ifdef DEBUG
	if(Debug(DebugCache))
		cout << name() << ": state " << state << endl;
#endif
	switch(state)
	{
		case 0:
			GetRequest();
			break;
#ifndef ICACHE
		case 1: // Write request to eject the cache line before a write
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Memory write request for ejecting a cache line before a write" << endl;
#endif
				DataArray<CACHE2MEM> data;
							
				line = GetLineStorage(bankToReplace, index);
				lineOffset = 0;
				length = 0;
				memcpy(data.buffer, &line[lineOffset], CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;						
				if(length == linesize)
				{
					if(zeroBlock)
						state = 5;
					else
						state = 3;
				}
				else
				{
					state = 2;
				}
							
				outMemReq = true;
				outMemAddr = GetLineAddr(bankToReplace, index);
				outMemWrite = true;
				outMemData = data;
				waitingForAck = true;
			}
			break;
		case 2: // Cache line eject before a write
			if(!waitingForAck)
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Ejecting a cache line before a write" << endl;
#endif
				DataArray<CACHE2MEM> data;
				
				memcpy(data.buffer, &line[lineOffset], CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;
				if(length == linesize)
				{
					if(zeroBlock)
						state = 5;
					else
						state = 3;
				}
				
				outMemReq = false;
				outMemData = data;
			}
			break;
			
		case 3: // Read reqest to fill the cache line before a write
			if(!waitingForAck)
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Memory read request for filling a cache line before a write" << endl;
#endif
				line = GetLineStorage(bankToReplace, index);
				length = 0;
				lineOffset = 0;
				
				outMemReq = true;
				outMemWrite = false;
				outMemAddr = AlignToLineBoundary(inAddr);
				waitingForAck = true;
				state = 4;		
#ifdef DEBUG
				if(Debug(DebugCache))
				{
					cout << name() << ": Reading " << linesize << " bytes at ";
					WriteHex(cout, AlignToLineBoundary(inAddr));
					cout  << endl;
				}
#endif
			}
			break;
			
		case 4:
			if(!waitingForAck)
			{
				outMemReq = false;
			}
			break;
			
		case 5:
			outAck = true;
			break;
			
		case 6: // write request to eject the cache line before a read
			{
				DataArray<CACHE2MEM> data;
							
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Memory write request for ejecting a cache line before a read" << endl;
#endif
				line = GetLineStorage(bankToReplace, index);
				lineOffset = 0;
				length = 0;
				memcpy(data.buffer, &line[lineOffset], CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;						
				if(length == linesize)
				{
					state = 8;
				}
				else
				{
					state = 7;
				}
							
				outMemReq = true;
				outMemAddr = GetLineAddr(bankToReplace, index);
				outMemWrite = true;
				outMemData = data;
				waitingForAck = true;
			}
			break;		
	
		case 7: // Cache line eject before a read
			if(!waitingForAck)
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Ejecting a cache line before a read" << endl;
#endif
				DataArray<CACHE2MEM> data;
				memcpy(data.buffer, &line[lineOffset], CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;
				if(length == linesize)
				{
					state = 8;
				}
				
				outMemReq = false;
				outMemData = data;
			}
			break;
#endif			
		case 8: // read request to fill the cache line
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Memory read request for filling a cache line before a read" << endl;
#endif
			line = GetLineStorage(bankToReplace, index);
			length = 0;
			lineOffset = AlignToMemoryWord(offset);
			dataLength = 0 - (offset % CACHE2MEM);
				
			outMemReq = true;
#ifndef ICACHE
			outMemWrite = false;
#endif
			outMemAddr = AlignToMemoryWord(addr);
			dataSent = false;
			waitingForAck = true;
			state = 9;
			break;
			
		case 9:
			outAck = false;
			break;
	}
}

void CLASSNAME::Forward()
{
	/* Forward data to LSU while filling cache line */
	if(state == 9 && !dataSent)
	{
		if(size <= CACHE2MEM)
		{
			DataArray<CPU2CACHE> data;
			const DataArray<CACHE2MEM>& memdata = inMemData;

			memcpy(data.buffer, &memdata.buffer[offset % CACHE2MEM], size);
#ifdef DEBUG
			if(Debug(DebugCache))
				cout << name() << ": Forwarding " << size << " bytes to cpu (" << data << ")" << endl;
#endif
			outData = data;
#ifdef DEBUG
			if(Debug(DebugCache))
				cout << name() << ": Sending an ack" << endl;
#endif
			outAck = true;
			dataSent = true;			
		}
		else
		{
			if(dataLength + CACHE2MEM >= size)
			{
				DataArray<CPU2CACHE> data;
				memcpy(data.buffer, &line[offset], dataLength);
				const DataArray<CACHE2MEM>& memdata = inMemData;
				memcpy(&data.buffer[dataLength], memdata.buffer, size - dataLength);
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Forwarding " << size << " bytes to cpu (" << data << ")" << endl;
#endif
				outData = data;
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Sending an ack" << endl;
#endif
				outAck = true;
				dataSent = true;
			}
			dataLength += CACHE2MEM;
		}
	}
}

void CLASSNAME::OnFallingEdge()
{
#ifdef DEBUG
	if(Debug(DebugCache))
		cout << name() << ": state " << state << endl;
#endif
	
	if(waitingForAck)
	{
#ifdef DEBUG
		if(Debug(DebugCache))
			cout << name() << ": Waiting for an ack" << endl;
#endif
		if(inMemAck)
		{
#ifdef DEBUG
			if(Debug(DebugCache))
				cout << name() << ": Getting an ack" << endl;
#endif
			waitingForAck = false;
			outMemReq = false;
		}
	}
	
	switch(state)
	{
#ifndef ICACHE
		case 4: // filling the cache line before a write
			if(!waitingForAck)
			{
#ifdef DEBUG
				if(Debug(DebugCache))
				{
					cout << name() << ": cache line filling before a write" << endl;
					cout << name() << ": Writing " << CACHE2MEM << " bytes into cache line (lineOffset = " << lineOffset << ")" << endl;
				}
#endif
				const DataArray<CACHE2MEM>& data = inMemData;
				memcpy(&line[lineOffset], data.buffer, CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;
				
				if(length == linesize)
				{
					state = 5;
				}
			}
			break;
					
		case 5: // write into the cache line
			{
				/* directory update */
				ReplaceLine(bankToReplace, index, tag);
										
				if(zeroBlock)
				{
#ifdef DEBUG
					if(Debug(DebugCache))
					{
						cout << name() << ": writing into cache line" << endl;
						cout << name() << ": Writing " << linesize << " zeros into cache line" << endl;
					}
#endif
					Zero(bankToReplace, index);
				}
				else
				{
#ifdef DEBUG
					if(Debug(DebugCache))
					{
						cout << name() << ": writing into cache line" << endl;
						cout << name() << ": Writing " << size << " bytes into cache line" << endl;
					}
#endif
					const DataArray<CPU2CACHE>& data = inData;
					
					Write(bankToReplace, index, offset, data.buffer, size);
				}
				state = 0;
				busy = false;
			}
			break;
#endif
			
		case 9: // filling the cache line before a read
			if(!waitingForAck)
			{
				const DataArray<CACHE2MEM>& data = inMemData;
#ifdef DEBUG
				if(Debug(DebugCache))
				{
					cout << name() << ": filling a cache line before for a read" << endl;
					cout << name() << ": Writing " << CACHE2MEM << " bytes (" << data << ") into cache line (lineOffset = " << lineOffset << ")" << endl;
				}
#endif
				memcpy(&line[lineOffset], data.buffer, CACHE2MEM);
				lineOffset = (lineOffset + CACHE2MEM) % linesize;
				length += CACHE2MEM;
				
				if(length == linesize)
				{
					/* directory update */
					ReplaceLine(bankToReplace, index, tag);
					state = 0;
					busy = false;
				}
			}
			break;
	}
	
#ifndef ICACHE
#ifdef DEBUG
	if(Debug(DebugCache))
	{
		cout << name() << ": ##### inReq = " <<(bool)  inReq << endl;
		cout << name() << ": ##### hit = " << hit << endl;
		cout << name() << ": ##### inWrite = " << (bool) inWrite << endl;
	}
#endif
	if(inReq && hit)
	{
		if(inWrite)
		{
			// Write
			const DataArray<CPU2CACHE>& data = inData;	
			if(writeZeroBlock)
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Writing " << linesize << " zeros" << endl;
#endif
				Zero(writeBank, writeIndex);
			}
			else
			{
#ifdef DEBUG
				if(Debug(DebugCache))
					cout << name() << ": Writing " << inSize << " bytes (" << data << ")" << endl;
#endif
				Write(writeBank, writeIndex, writeOffset, data.buffer, inSize);
			}
		}
	}
#endif

	outAck = false;
#ifdef DCACHE
	outBusy = state != 0;
#endif
}

void CLASSNAME::MemoryRead(UInt8 *buffer, UInt32 addr, UInt32 size)
{
	while(size > 0)
	{
		UInt32 tag;
		UInt32 index;
		UInt32 offset;
		DecodeAddress(addr, tag, index, offset);
		int bank = Search(index, tag);
		UInt32 count = (offset + size > linesize) ? linesize - offset : size;
		
		if(bank >= 0)
			memcpy(buffer, &storage[bank][index][offset], count);

		buffer += count;			
		size -= count;
		addr += count;
	}
}

void CLASSNAME::MemoryWrite(UInt32 addr, UInt8 *buffer, UInt32 size)
{
	while(size > 0)
	{
		UInt32 tag;
		UInt32 index;
		UInt32 offset;
		DecodeAddress(addr, tag, index, offset);
		int bank = Search(index, tag);
		UInt32 count = (offset + size > linesize) ? linesize - offset : size;
		
		if(bank >= 0)
			memcpy(&storage[bank][index][offset], buffer, count);

		buffer += count;			
		size -= count;
		addr += count;
	}
}

void CLASSNAME::MemorySet(UInt32 addr, UInt8 value, UInt32 size)
{
	while(size > 0)
	{
		UInt32 tag;
		UInt32 index;
		UInt32 offset;
		DecodeAddress(addr, tag, index, offset);
		int bank = Search(index, tag);
		UInt32 count = (offset + size > linesize) ? linesize - offset : size;
		
		if(bank >= 0)
			memset(&storage[bank][index][offset], value, count);

		size -= count;
		addr += count;
	}
}

bool CLASSNAME::ReadDWord(UInt32 addr, UInt64& value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		value = ((UInt64) storage[bank][index][offset] << 56)
		      | ((UInt64) storage[bank][index][offset + 1] << 48)
		      | ((UInt64) storage[bank][index][offset + 2] << 40)
		      | ((UInt64) storage[bank][index][offset + 3] << 32)
		      | ((UInt64) storage[bank][index][offset + 4] << 24)
		      | ((UInt64) storage[bank][index][offset + 5] << 16)
		      | ((UInt64) storage[bank][index][offset + 6] << 8)
		      | ((UInt64) storage[bank][index][offset + 7]);
		return true;
	}
	return false;
}

void CLASSNAME::WriteDWord(UInt32 addr, UInt64 value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		storage[bank][index][offset] = value >> 56;
		storage[bank][index][offset + 1] = value >> 48;
		storage[bank][index][offset + 2] = value >> 40;
		storage[bank][index][offset + 3] = value >> 32;
		storage[bank][index][offset + 4] = value >> 24;
		storage[bank][index][offset + 5] = value >> 16;
		storage[bank][index][offset + 6] = value >> 8;
		storage[bank][index][offset + 7] = value;
	}
}

bool CLASSNAME::ReadWord(UInt32 addr, UInt32& value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		value = (storage[bank][index][offset] << 24) | (storage[bank][index][offset + 1] << 16) | (storage[bank][index][offset + 2] << 8) | storage[bank][index][offset + 3];
		return true;
	}
	return false;
}

void CLASSNAME::WriteWord(UInt32 addr, UInt32 value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		storage[bank][index][offset] = value >> 24;
		storage[bank][index][offset + 1] = value >> 16;
		storage[bank][index][offset + 2] = value >> 8;
		storage[bank][index][offset + 3] = value;
	}
}

bool CLASSNAME::ReadHalfWord(UInt32 addr, UInt16& value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		value = (storage[bank][index][offset] << 8) | (storage[bank][index][offset + 1]);
		return true;
	}
	return false;
}

void CLASSNAME::WriteHalfWord(UInt32 addr, UInt16 value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		storage[bank][index][offset] = value >> 8;
		storage[bank][index][offset + 1] = value;
	}
}

bool CLASSNAME::ReadByte(UInt32 addr, UInt8& value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		value = storage[bank][index][offset];
		return true;
	}
	return false;
}

void CLASSNAME::WriteByte(UInt32 addr, UInt8 value)
{
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	DecodeAddress(addr, tag, index, offset);
	int bank = Search(index, tag);
	if(bank >= 0)
	{
		storage[bank][index][offset] = value;
	}
}

void CLASSNAME::Reset()
{
	state = 0;
	busy = false;
	hit = false;
	waitingForAck = false;
	hits = misses = 0;
}

void CLASSNAME::ResetStats()
{
	hits = misses = 0;
}

#endif
