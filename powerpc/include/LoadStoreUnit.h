/***************************************************************************
                          LoadStoreUnit.h  -  description
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __LOADSTOREUNIT_H__
#define __LOADSTOREUNIT_H__

#include <systemc.h>
#include <common.h>
#include <StoreQueue.h>

const int NORMAL_ACCESS = 0;
const int MSB_ACCESS = 1;
const int LSB_ACCESS = 2;

SC_MODULE(LoadStoreUnit)
{
	sc_in<bool> inClock;
	
	/* From Dispatch Unit */
	sc_in<bool> inDispatched;
	sc_in<LoadStoreOperation> inOperation;
	sc_in<LoadStoreOperands> inOperands;
	sc_in<LoadStoreTags> inTags;
	sc_in<int> inBranchCounter;
#if defined(DEBUG) || defined(TRACE)
	sc_in<UInt32> inInstructionCounter;
	sc_in<UInt32> inProgramCounter;
	sc_in<UInt32> inInstruction;
#endif
	/* To Dispatch Unit */
	sc_out<bool> outStoreQueueStalled;
	sc_out<bool> outLoadQueueStalled;

	/* To Dispatch Unit and Register File */
	sc_out<bool> outResultValid[2];
	sc_out<bool> outFloatingPointResultValid;
	sc_out<tag_t> outResultTag[2];
	sc_out<UInt32> outResultData[2];
	sc_out<UInt64> outFloatingPointResultData;
	
	/* To Completion Unit */
	sc_out<tag_t> outTag[2];
	sc_out<bool> outFinished[2];
	
	/* To Dispatch Unit */
	sc_out<bool> outBusy;
	
	/* To System Register Unit */
	sc_out<bool> outEmpty;
	
	/* To L1 Data Cache */
	sc_out<bool> outDCacheReq;
	sc_out<DataArray<lsu2dcache> > outDCacheData;
	sc_out<bool> outDCacheWrite;
	sc_out<UInt32> outDCacheAddr;
	sc_out<int> outDCacheSize;
	sc_out<bool> outDCacheZeroBlock;

	/* From L1 Data Cache */
	sc_in<Dummy<bool> > inDCacheAck;
	sc_in<DataArray<lsu2dcache> > inDCacheData;
	sc_in<bool> inDCacheHit;
	
	/* From Completion Unit */
	sc_in<Dummy<bool> > inWriteBack[nMemoryWriteBackPorts];
	sc_in<tag_t> inWriteBackTag[nMemoryWriteBackPorts];
	
	/* From Branch Unit */
	sc_in<bool> inBranchFinished;
	sc_in<bool> inMispredictedBranch;
	
	bool loadWaitingForAck;
	bool storeWaitingForAck;
	
	bool stall;
	
	struct
	{
		int state;
		LoadStoreOperation operation;
		LoadStoreTags tags;
		UInt32 addr;
		bool valid;
		UInt64 data;
		int branchCounter;
#if defined(DEBUG) || defined(TRACE)
		UInt32 dbgInstructionCounter;
		UInt32 dbgProgramCounter;
		UInt32 dbgInstruction;
#endif
	} stage1;
	
	struct
	{
		struct
		{
			LoadStoreOperation operation;
			LoadStoreTags tags;
			UInt32 addr;
			bool valid;
			bool independent;
			int branchCounter;
			int state;
			DataArray<lsu2dcache> data;
#if defined(DEBUG) || defined(TRACE)
			UInt32 dbgInstructionCounter;
			UInt32 dbgProgramCounter;
			UInt32 dbgInstruction;
#endif
		} load;
		
		StoreQueue storeQueue;
	} stage2;
	
	
	void OnFrontEdge();
	void Forward();
	void WriteBack();
	void OnFallingEdge();
	void DecrementBranchCounter();
	void Flush();
	void Load();
	void Store(StoreQueueEntry *entry);
	
	bool Misalignment(UInt32 addr, int size);
	bool CrossLineBoundary(UInt32 addr, int size);
	
	void Reset();
			
	SC_CTOR(LoadStoreUnit)
	{
		int i;
		
		SC_METHOD(OnFrontEdge);
		sensitive_pos << inClock;
		
		SC_METHOD(Forward);
		sensitive << inDCacheAck;
		
		SC_METHOD(WriteBack);
		for(i = 0; i < nMemoryWriteBackPorts; i++)
			sensitive << inWriteBack[i];
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;

		Reset();	
	}
};

#endif
