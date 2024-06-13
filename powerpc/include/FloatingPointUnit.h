/***************************************************************************
                          FloatingPointUnit.h  -  description
                             -------------------
    begin                : Thu Jul 26 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __FLOATINGPOINTUNIT_H__
#define __FLOATINGPOINTUNIT_H__

#include <systemc.h>
#include <common.h>

const int nFloatingPointStages = 3;

template <class T, int depth>
struct Pipeline
{
	int size;
	T entries[depth];
	int index;
	T *content[depth];
	
	Pipeline()
	{
		Reset();
	}
	
	void Reset()
	{
		size = 0;
		index = 0;
		
		int i;
		for(i = 0; i < depth; i++)
		{
			content[i] = 0;
		}
	}
	
	T *Allocate()
	{
		size++;
		T *entry = &entries[index++];
		if(index >= nFloatingPointStages) index = 0;
		content[0] = entry;
		return entry;
	}
	
	T *GetOldest()
	{
		T *entry = content[depth - 1];
		return (entry && entry->latencies[depth - 1] == 0) ? entry : 0;
	}
	
	T *GetNewest()
	{
		return content[0];
	}
	
	void ReleaseTail()
	{
		content[depth - 1] = 0;
		size--;
	}
	
	bool Full()
	{
		return size == depth;
	}
	
	bool Stalled()
	{
		T *entry = content[0];
		if(entry && entry->latencies[0] > 0) return true;
		return false;
	}
	
	int Empty()
	{
		return size == 0;
	}
	
	int GetFreeSpace()
	{
		return depth - size;
	}
	
	void Flush()
	{
		int i;
		for(i = 0; i < depth; i++)
		{
			T *entry = content[i];
			
			if(entry && entry->branchCounter != 0)
			{
				content[i] = 0;
				size--;
			}
		}
	}
	
	void DecrementBranchCounter()
	{
		int i;
		for(i = 0; i < depth; i++)
		{
			T *entry = content[i];
		
			if(entry && entry->branchCounter > 0) entry->branchCounter--;
		}
	}
	
	void Shift()
	{
		int i;
		
		for(i = 0; i < depth; i++)
		{
			T *entry = content[i];
			if(entry && entry->latencies[i] > 0) entry->latencies[i]--;
		}
		
		for(i = depth - 1; i >= 0; i--)
		{
			T *entry = content[i];
			
			if(entry && entry->latencies[i] == 0 && i < depth - 1 && !content[i + 1])
			{
				content[i + 1] = entry;
				content[i] = 0;
			}
		}
	}
	
	void Print(ostream& os)
	{
		int i;
		
		for(i = 0; i < depth; i++)
		{
			T *entry = content[i];
			if(entry) entry->Print(os); else cout << "()";
			os << endl;
		}
	}
};



template <int depth>
struct FloatingPointInstruction
{
	FloatingPointOperation operation;
	FloatingPointOperands operands;
	FloatingPointTags tags;
	UInt64 result;
	UInt8 resultCR;
	int branchCounter;
	int latencies[depth];
#ifdef DEBUG
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif	
	
	void Print(ostream& os)
	{
		os << "(";
		int i;
		os << "latencies = ";
		for(i = 0; i < nFloatingPointStages; i++)
		{
			os << latencies[i];
			if(i < nFloatingPointStages - 1) os << "-";
		}
#ifdef DEBUG
		os << ", (" << dbgInstructionCounter << ")   ";
		ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
#endif			
		os << ")";
	}
};


SC_MODULE(FloatingPointUnit)
{
	sc_in<bool> inClock;

	/* From Dispatch Unit */	
	sc_in<bool> inDispatched;
	sc_in<FloatingPointOperation> inOperation;
	sc_in<FloatingPointOperands> inOperands;
	sc_in<FloatingPointTags> inTags;
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
	sc_out<UInt64> outResultData;
	
	/* To Dispatch Unit and Condition Register */
	sc_out<bool> outResultCRValid;
	sc_out<tag_t> outResultCRTag;
	sc_out<UInt8> outResultCRData;

	/* To Completion Unit */
	sc_out<tag_t> outTag;
	sc_out<bool> outFinished;
	
	/* To Dispatch Unit */
	sc_out<bool> outBusy;	/* Floating Point is not available for a new instruction */
	sc_out<bool> outFloatingPointPipelineEmpty; /* Floating Point Pipeline is empty */
	
	Pipeline<FloatingPointInstruction<nFloatingPointStages>, nFloatingPointStages> pipeline;
	
	int branchCounter;
	bool dispatched;
	int latency;
	UInt64 result;
	bool fx, fex, vx, ox;
	
	static bool Pipelined(OperationIdent ident);
		
	void Execute();
	void OnFallingEdge();
	void Reset();

	SC_CTOR(FloatingPointUnit)
	{
		SC_METHOD(Execute);
		sensitive_pos << inClock;
		
		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;

		Reset();		
	}
};


#endif
