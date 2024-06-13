/***************************************************************************
                          IntegerUnit.h  -  unite de calcul entier
                             -------------------
    begin                : Thu Apr 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __INTEGERUNIT_H__
#define __INTEGERUNIT_H__

#include <systemc.h>
#include <common.h>

SC_MODULE(IntegerUnit)
{
	sc_in<bool> inClock;

	/* From Dispatch Unit */	
	sc_in<bool> inDispatched;
	sc_in<IntegerOperation> inOperation;
	sc_in<IntegerOperands> inOperands;
	sc_in<IntegerTags> inTags;
	sc_in<int> inBranchCounter;
#if defined(DEBUG) || defined(TRACE)
	sc_in<UInt32> inInstructionCounter;
	sc_in<UInt32> inProgramCounter;
	sc_in<UInt32> inInstruction;
#endif	
	/* From Branch Unit */
	sc_in<bool> inBranchFinished;
	sc_in<bool> inMispredictedBranch;
	

	/* From XER */
	sc_in<bool> inCarry;
	
	/* To Dispatch Unit and Register File */
	sc_out<bool> outResultValid;
	sc_out<tag_t> outResultTag;
	sc_out<UInt32> outResultData;
	
	/* To Dispatch Unit and Condition Register */
	sc_out<bool> outResultCRValid;
	sc_out<tag_t> outResultCRTag;
	sc_out<UInt8> outResultCRData;

	/* To Completion Unit */
	sc_out<bool> outOverflow;	/* output XER[OV] */
	sc_out<bool> outCarry;		/* output XER[CA] */

	sc_out<tag_t> outTag;
	sc_out<bool> outFinished;
	
	sc_out<bool> outBusy;

	bool writeCarry;	
	int latency;
	UInt32 result32;
	int branchCounter;
	
	/* XER */
	
	bool ov;
	bool ca;
	
	/* CR Field */
	int crf;
	bool lt;
	bool gt;
	bool eq;
	bool so;
	
	bool dispatched;
	
	void Execute();
	void OnFallingEdge();
	int GetMulLatency(UInt32 op1, UInt32 op2);
	UInt32 RotateLeft(UInt32 v, UInt8 n);
	UInt32 Mask(UInt8 mb, UInt8 me);
	bool Carry(UInt32 a, UInt32 b);
	bool Overflow(SInt32 a, SInt32 b);
	bool Underflow(SInt32 a, SInt32 b);
	
	void Reset();
	
	SC_CTOR(IntegerUnit)
	{
		SC_METHOD(Execute);
		sensitive_pos << inClock;
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;
		
		Reset();
	}
};

#endif
