/***************************************************************************
                          SystemRegisterUnit.h  -  description
                             -------------------
    begin                : Wed May 30 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __SYSTEMREGISTERUNIT_H__
#define __SYSTEMREGISTERUNIT_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(SystemRegisterUnit)
{
	sc_in<bool> inClock;
	
	/* From Dispatch Unit */
	sc_in<bool> inDispatched;
	sc_in<SystemRegisterOperation> inOperation;
	sc_in<SystemRegisterTags> inTags;
	sc_in<int> inBranchCounter;
#if defined(DEBUG) || defined(TRACE)
	sc_in<UInt32> inInstructionCounter;
	sc_in<UInt32> inProgramCounter;
	sc_in<UInt32> inInstruction;
#endif
	/* From Branch Unit */
	sc_in<bool> inBranchFinished;
	sc_in<bool> inMispredictedBranch;
	
	
	/* To Dispatch Unit and Register File */		
	sc_out<bool> outResultValid;
	sc_out<tag_t> outResultTag;
	sc_out<UInt32> outResultData;
	
	sc_out<bool> outResultLRValid;
	sc_out<tag_t> outResultLRTag;
	sc_out<UInt32> outResultLRData;
	
	sc_out<bool> outResultCTRValid;
	sc_out<tag_t> outResultCTRTag;
	sc_out<UInt32> outResultCTRData;
	
	sc_out<bool> outResultCRValid;
	sc_out<tag_t> outResultCRTag;
	sc_out<UInt8> outResultCRData;
	
	/* To Register File */
	sc_out<regnum_t> outReadRegisterNumber;

	/* From Register File */
	sc_in<UInt32> inReadRegisterData;
	
	/* From Condition Register */
	sc_in<UInt32> inCR;
	
	/* From Link Register */
	sc_in<UInt32> inLR;
	
	/* From Count Register */
	sc_in<UInt32> inCTR;
	
	/* To Completion Unit */
	sc_out<bool> outFinished;
	sc_out<tag_t> outTag;
	sc_out<UInt32> outCR;

	/* To Dispatch Unit */	
	sc_out<bool> outBusy;
	
	void Execute();
	void ReadRegister();
	void OnFallingEdge();

	int branchCounter;
	UInt32 result;
	UInt8 resultCR;
	int latency;
	bool dispatched;
	bool execute;

	void Reset();

	SC_CTOR(SystemRegisterUnit)
	{
		SC_METHOD(Execute);
		sensitive_pos << inClock;

		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;

		Reset();
	}

};

#endif
