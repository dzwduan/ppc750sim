/***************************************************************************
                          FetchUnit.h  -  Unite de Fetch
                             -------------------
    begin                : Thu Apr 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __FETCHUNIT_H__
#define __FETCHUNIT_H__

#include <systemc.h>
#include <InstructionQueue.h>

SC_MODULE(FetchUnit)
{
	sc_in<bool> inClock;
	
	InstructionQueue instructionQueue;
	BranchQueue branchQueue;
	
	/* To Dispatch unit */
	sc_out<int> outInstructionsInQueue;
	sc_out<InstructionQueueEntry> outIQ[nIQ];
	sc_out<int> outBranchsInQueue;
	sc_out<InstructionQueueEntry> outBranchQueue[nBranchQueueEntries];
	
	/* From Dispatch unit */
	sc_in<int> inDispatchedInstructions;
	sc_in<int> inDispatchedBranchs;
	
	/* To Instruction Cache */
	sc_out<bool> outICacheCancel;
	sc_out<bool> outICacheReq;
	sc_out<UInt32> outICacheAddr;
	sc_out<int> outICacheSize;
	
	/* From Instruction Cache */
	sc_in<bool> inICacheAck;
	sc_in<DataArray<fetch2icache> > inICacheData;
	sc_in<bool> inICacheHit;
	
	/* To Branch Prediction Unit */
	sc_out<UInt32> outReadBHTPC;
	sc_out<UInt32> outReadBTICPC;

	/* From Branch Prediction Unit */	
	sc_in<bool> inReadBHTTakenBranch; /* true si le branchement est predit pris */
	
	sc_in<bool> inReadBTICHit; /* true si hit dans le Branch Target Instruction Cache */
	sc_in<int> inReadBTICInstructionsCount;
	sc_in<UInt32> inReadBTICInstructions[nInstructionsPerBTICEntry]; /* Instructions lues dans le Branch Target Instruction Cache */
	
	/* From Branch Unit */
	sc_in<bool> inBranchFinished;
	sc_in<bool> inMispredictedBranch;
	sc_in<UInt32> inPC;
	
	/* To Branch Prediction Unit */
	sc_out<Dummy<bool> > outUpdateBTIC;
	sc_out<UInt32> outUpdateBTICPC;
	sc_out<UInt32> outUpdateBTICInstructionsCount;
	sc_out<UInt32> outUpdateBTICInstructions[nInstructionsPerBTICEntry];
	
	UInt32 programCounter;
	UInt32 nInstructionsToFetch;
	UInt32 nFetchedInstructions;
	UInt32 nInstructionsToFlush;
	bool waitingForAck;
	bool mispredictedBranch;
	bool updateBTIC;
//	bool takenBranch;
	UInt32 branchPC;
	UInt32 updateBTICPC;
	
	UInt32 bticHits;
	UInt32 bticMisses;
	int branchCounter;
//	bool stall;
//	bool ack;
//	bool readBTIC;
	int state;
	int nInstructionsToPredecode;
	InstructionQueueEntry *branchToPredict;
	UInt32 targetPC;
	
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
#endif
		
	void Fetch();
	void FillQueue();
	
	UInt32 AlignToLineBoundary(UInt32 addr);
	bool ReadBTIC(); /* return true if Hit into BTIC */
	/* Emulator code begins */
	UInt32 EmulatorFetch(UInt32 programCounter, UInt32 instruction);
	/* Emulator code ends */
	
	void Reset();

	SC_CTOR(FetchUnit)
	{
		SC_METHOD(Fetch);
		sensitive_pos << inClock;
		
		SC_METHOD(FillQueue);
		sensitive_neg << inClock;
		
		programCounter = 0;
		Reset();
	}
};

#endif
