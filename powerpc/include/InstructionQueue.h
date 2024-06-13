/***************************************************************************
                          InstructionQueue.h  -  Instruction Queue
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __INSTRUCTIONQUEUE_H__
#define __INSTRUCTIONQUEUE_H__

#include <common.h>
#include <Queue.h>
#include <iostream>

/*
  InstructionQueueEntry : an instruction queue entry
  Contains instructions which have been fetched (at the fetch stage) plus some informations about branch prediction
*/
struct InstructionQueueEntry
{
	UInt32 instruction;			/* instruction word */
	bool branchPredicted;        /* whether the branch has been predicted */
	bool branchPredictedTaken;	/* whether instruction is a branch (taken or predicted as taken) */
	bool branchFollow;			/* whether a taken branch follow the instruction */
	bool priority;				/* whether a taken branch is the next instruction to be dispatched */
	UInt32 pc;					/* address of the instruction */
	UInt32 nia;					/* address of the next instruction */
	int branchCounter;			/* number of branch instruction that the instruction depends on */
	
	/* Emulator code begins*/
	ppc_inst_t inst;			/* instruction word fetched by the emulator */
	/* Emulator code ends */
	
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;	/* serial number of the instruction for debugging purposes */
#endif
	
	InstructionQueueEntry()
	{
		instruction = 0;
		branchPredicted = false;
		branchPredictedTaken = false;
		branchFollow = false;
		priority = false;
		pc = 0;
		nia = 0;
		branchCounter = 0;
		inst = 0;
		
#if defined(DEBUG) || defined(TRACE)
		dbgInstructionCounter = 0;
#endif
	}
	
	int operator == (const InstructionQueueEntry& iqe) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const InstructionQueueEntry& entry)
	{
		os << "(";
		os << "instruction = ";
		WriteHex(os, entry.instruction);
		os << ", branchPredicted = " << entry.branchPredicted;
		os << ", branchPredictedTaken = " << entry.branchPredictedTaken;
		os << ", branchFollow = " << entry.branchFollow << ", pc = " << entry.pc;
		os << ", priority = " << entry.priority;
		os << ", branch counter = " << entry.branchCounter << endl;
#ifdef DEBUG
		os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
		ppc_disassemble_to_stream(entry.instruction, entry.pc, stdout);
#endif
		 os << ")";
		return os;
	}
	
	operator bool () const
	{
		return true;
	}
	
#ifdef TRACE
	void Trace();
#endif
};

extern void sc_trace(sc_trace_file *tf, const InstructionQueueEntry& entry, const sc_string& NAME);


struct InstructionQueue : public Queue<InstructionQueueEntry, nInstructionQueueEntries>
{
#ifdef TRACE
	void Trace()
	{
		trace_file->Begin("instruction_queue");
		Queue<InstructionQueueEntry, nInstructionQueueEntries>::Trace();
		trace_file->End("instruction_queue");
	}
#endif

	void DecrementBranchCounter();
};

struct BranchQueue : public Queue<InstructionQueueEntry, nBranchQueueEntries>
{
#ifdef TRACE
	void Trace()
	{
		trace_file->Begin("branch_queue");
		Queue<BranchQueueEntry, nBranchQueueEntries>::Trace();
		trace_file->End("branch_queue");
	}
#endif
	void DecrementBranchCounter();
};


#endif
