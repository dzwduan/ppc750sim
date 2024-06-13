/***************************************************************************
                          BranchUnit.h  -  description
                             -------------------
    begin                : Mon May 21 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BRANCHUNIT_H__
#define __BRANCHUNIT_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(BranchUnit)
{
	sc_in<bool> inClock;
	
	/* From Dispatch Unit */
	sc_in<bool> inDispatched;
	sc_in<BranchOperands> inOperands;
	sc_in<BranchOperation> inOperation;
	sc_in<BranchTags> inTags;
#if defined(DEBUG) || defined(TRACE)
	sc_in<UInt32> inInstructionCounter;
	sc_in<UInt32> inInstruction;
#endif
	
	/* To Link Register and Dispatch Unit */
	sc_out<bool> outResultLRValid;
	sc_out<tag_t> outResultLRTag;
	sc_out<UInt32> outResultLRData;
	
	/* To Count Register and Dispatch Unit */
	sc_out<bool> outResultCTRValid;
	sc_out<tag_t> outResultCTRTag;
	sc_out<UInt32> outResultCTRData;
	
	/* To Dispatch Unit */
	sc_out<bool> outBusy;
	
	/* To Completion Unit */
	sc_out<bool> outFinished;
	sc_out<bool> outMispredictedBranch;
	sc_out<tag_t> outTag;
	
	// To Fetch Unit
	sc_out<UInt32> outPC;
	
	/* To Branch Prediction Unit */
	sc_out<Dummy<bool> > outUpdateBHT;
	sc_out<UInt32> outBHTPC;
	sc_out<bool> outTakenBranch;
	
	UInt32 mispredictedBranches;
	UInt32 wellPredictedBranches;
	UInt32 nBC;
	UInt32 nMispredictedBC;
	UInt32 nBCLR;
	UInt32 nBCCTR;
	
	void Execute();
	void Reset();

	SC_CTOR(BranchUnit)
	{
		SC_METHOD(Execute);
		sensitive_pos << inClock;
		
		Reset();
	}
};

#endif
