/***************************************************************************
                          Cache.h  -  Cache
                             -------------------
    begin                : Mon Apr 30 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#if defined(ICACHE) || defined(DCACHE)

#include <systemc.h>
#include <common.h>

#ifdef ICACHE
#define CLASSNAME ICache
#define CPU2CACHE fetch2icache
#define CACHE2MEM icache2biu
#else
#define CLASSNAME DCache
#define CPU2CACHE lsu2dcache
#define CACHE2MEM dcache2biu
#endif

SC_MODULE(CLASSNAME)
{
	struct
	{
		UInt32 tag;
		bool valid;
		#ifndef ICACHE
		bool dirty;
		#endif
	} directory[associativity][nlines];
	int plrubits[nlines];
	unsigned char storage[associativity][nlines][linesize];	
	
	sc_in<bool> inClock;
	
	/* From CPU */
#ifdef ICACHE
	sc_in<bool> inCancel;
#endif
	sc_in<UInt32> inAddr; // Adress
	sc_in<bool> inReq; // CPU request
#ifndef ICACHE
	sc_in<bool> inWrite; // Write=true / read=false
	sc_in<DataArray<CPU2CACHE> > inData; // Data to write
#endif
	sc_in<int> inSize; // data size (in bytes)
#ifndef ICACHE
	sc_in<bool> inZeroBlock;	/* true if fill cache line with zero */
#endif
	
	/* To CPU */
	sc_out<DataArray<CPU2CACHE> > outData; // the data read into the cache line
	sc_out<bool> outHit; // true if the cache access hits
#ifdef DCACHE
	sc_out<Dummy<bool> > outAck; // true if the cache access is finished
	sc_out<bool> outBusy;
#else
	sc_out<bool> outAck; // true if the cache access is finished
#endif
	
	/* To Memory */
	sc_out<bool> outMemReq; // Memory request
#ifndef ICACHE
	sc_out<bool> outMemWrite; // write=true / read=false
	sc_out<DataArray<CACHE2MEM> > outMemData; // Data to write into memory
#endif
	sc_out<UInt32> outMemAddr; // Address of the data
	
	/* From Memory */
	sc_in<DataArray<CACHE2MEM> > inMemData; // Data coming from memory
	sc_in<bool> inMemAck; // true if the memory transaction finished

	int state; // state of cache
	bool busy; // true if the cache is busy
	bool hit;
	bool waitingForAck;
	
	int bank;
	bool write;
#ifndef ICACHE
	bool zeroBlock;
#endif
	int size;
	UInt32 tag;
	UInt32 index;
	UInt32 offset;
	int bankToReplace;
	UInt32 addr;
	
	UInt8 *line;
	UInt32 lineOffset;
	int length;
	int dataLength;
	bool dataSent;
	
	UInt32 misses;
	UInt32 hits;
	
#ifndef ICACHE
	UInt32 writeOffset;
	UInt32 writeBank;
	UInt32 writeIndex;
	bool writeZeroBlock;
#endif
	
	void UpdatePLRUBits(UInt32 bank, UInt32 index);
	void DecodeAddress(UInt32 addr, UInt32& tag, UInt32& index, UInt32& offset);
	int Search(UInt32 index, UInt32 tag);
	int Choose(UInt32 index);
	void Read(UInt32 bank, UInt32 index, UInt32 offset, UInt8 *data, int size);
#ifndef ICACHE
	void Write(UInt32 bank, UInt32 index, UInt32 offset, const UInt8 *data, int size);
	void Zero(UInt32 bank, UInt32 index);
	bool IsDirty(UInt32 bank, UInt32 index);
#endif	
	UInt32 AlignToLineBoundary(UInt32 addr);
	UInt32 AlignToMemoryWord(UInt32 addr);
	bool IsValid(UInt32 bank, UInt32 index);
	UInt32 GetLineAddr(UInt32 bank, UInt32 index);
	UInt8 *GetLineStorage(UInt32 bank, UInt32 index);
	void ReplaceLine(UInt32 bank, UInt32 index, UInt32 tag);
	
	void OnFrontEdge();
	void Async();
	void Forward();
	void OnFallingEdge();
	void GetRequest();
	
	UInt32 MakeAddr(UInt32 bank, UInt32 index, UInt32 offset);
	
	void MemoryRead(UInt8 *buffer, UInt32 addr, UInt32 size);
	void MemoryWrite(UInt32 addr, UInt8 *buffer, UInt32 size);
	void MemorySet(UInt32 addr, UInt8 value, UInt32 size);
	bool ReadDWord(UInt32 addr, UInt64& value);
	void WriteDWord(UInt32 addr, UInt64 value);
	bool ReadWord(UInt32 addr, UInt32& value);
	void WriteWord(UInt32 addr, UInt32 value);
	void WriteHalfWord(UInt32 addr, UInt16 value);
	bool ReadHalfWord(UInt32 addr, UInt16& value);
	void WriteByte(UInt32 addr, UInt8 value);
	bool ReadByte(UInt32 addr, UInt8& value);
	
	void Reset();
	void ResetStats();

	int offsetMask;
	int indexMask;
	int indexShift;
	int tagShift;
	
	SC_CTOR(CLASSNAME)
	{
		SC_METHOD(OnFrontEdge);
		sensitive_pos << inClock;
		
		SC_METHOD(Async);
#ifdef ICACHE
		sensitive << inAddr << inReq;
#else
		sensitive << inAddr << inReq << inWrite << inData;
#endif	
		SC_METHOD(Forward);
		sensitive << inMemData;
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;

		int size;
		
		for(size = linesize, indexShift = 0, offsetMask = 0; size > 1; size >>= 1, indexShift++, offsetMask <<= 1, offsetMask |= 1);
		for(size = nlines, tagShift = indexShift, indexMask = 0; size > 1; size >>= 1, tagShift++, indexMask <<= 1, indexMask |= 1);
			
		Reset();
		
		int i, j;
		for(i = 0; i < associativity; i++)
		{
			for(j = 0; j < nlines; j++)
			{
				directory[i][j].valid = false;
			}
		}
		
		for(i = 0; i < nlines; i++)
		{
			plrubits[i] = 0;
		}
		
	}
};

#undef CLASSNAME
#undef CPU2CACHE
#undef CACHE2MEM

#endif
