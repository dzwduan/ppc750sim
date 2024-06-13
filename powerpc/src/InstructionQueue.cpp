/***************************************************************************
                          InstructionQueue.cpp  -  File d'instructions
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <InstructionQueue.h>

void sc_trace(sc_trace_file *tf, const InstructionQueueEntry& entry, const sc_string& NAME)
{
}

#ifdef TRACE
void InstructionQueueEntry::Trace()
{
	trace_file->Begin("instruction");
	trace_file->Value("number", dbgInstructionCounter);
	trace_file->Value("pc", pc);
	trace_file->Value("branch_prediction", branchPredictedTaken ? "taken" : "not taken");
	trace_file->End("instruction");
}
#endif

void InstructionQueue::DecrementBranchCounter()
{
	int i, n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nInstructionQueueEntries, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}	
}

void BranchQueue::DecrementBranchCounter()
{
	int i, n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nBranchQueueEntries, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}	
}
