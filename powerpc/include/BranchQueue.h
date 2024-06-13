/***************************************************************************
                          BranchQueue.h  -  description
                             -------------------
    begin                : Mon Mar 4 2002
    copyright            : (C) 2002 CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BRANCHQUEUE_H__
#define __BRANCHQUEUE_H__

#include <common.h>
#include <Queue.h>
#include <iostream>

struct BranchQueueEntry
{
	UInt32 instruction;			/* instruction word */
	UInt32 pc;					/* address of the instruction */
	bool priority;				/* true if branch is the next instruction to be dispatched */
	
	/* Emulator code begins*/
	ppc_inst_t inst;			/* instruction word fetched by the emulator */
	/* Emulator code ends */
	
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;	/* serial number of the instruction for debugging purposes */
#endif
	
	BranchQueueEntry()
	{
		instruction = 0;
		pc = 0;
		priority = false;
	}
	
	int operator == (const BranchQueueEntry& bqe) const
	{
		return 0;
	}
	
	friend ostream& operator << (ostream& os, const BranchQueueEntry& entry)
	{
		os << "(";
		os << "instruction = ";
		WriteHex(os, entry.instruction);
#ifdef DEBUG
		os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
		ppc_disassemble_to_stream(entry.instruction, entry.pc, stdout);
#endif
		os << ", priority = " << entry.priority;
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

extern void sc_trace(sc_trace_file *tf, const BranchQueueEntry& entry, const sc_string& NAME);


struct BranchQueue : public Queue<BranchQueueEntry, nBranchQueueEntries>
{
#ifdef TRACE
	void Trace()
	{
		trace_file->Begin("branch_queue");
		Queue<BranchQueueEntry, nBranchQueueEntries>::Trace();
		trace_file->End("branch_queue");
	}
#endif
};

#endif
