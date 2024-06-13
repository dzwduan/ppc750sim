/***************************************************************************
                          BranchQueue.cpp  -  description
                             -------------------
    begin                : Mon Mar 4 2002
    copyright            : (C) 2002 CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BranchQueue.h>

void sc_trace(sc_trace_file *tf, const BranchQueueEntry& entry, const sc_string& NAME)
{
}

#ifdef TRACE
void BranchQueueEntry::Trace()
{
	trace_file->Begin("instruction");
	trace_file->Value("number", dbgInstructionCounter);
	trace_file->Value("pc", pc);
//	trace_file->Value("branch_prediction", branchPredictedTaken ? "taken" : "not taken");
	trace_file->End("instruction");
}
#endif
