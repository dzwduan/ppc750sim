/***************************************************************************
                          StoreQueue.cpp  -  description
                             -------------------
    begin                : Wed Apr 25 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <StoreQueue.h>

#ifdef DEBUG
#define CONDITION
#endif

StoreQueueEntry::StoreQueueEntry()
{
	addr = 0;
	memset(&tags, -1, sizeof(tags));
	memset(&operation, 0, sizeof(operation));
	branchCounter = 0;
	data = 0;
	valid = false;
}

ostream& operator << (ostream& os, const StoreQueueEntry& entry)
{
	if(entry.full)
	{
		os << "(addr = ";
		WriteHex(os, entry.addr);
		os << ", tag = " << (int) entry.tags.tag;
		os << ", size = " << (int) entry.operation.size;
		os << ", branchCounter = " << entry.branchCounter;
		os << ", valid = " << entry.valid;
		os << ", dependent load tag = " << (int) entry.dependentLoadTag;
#ifdef DEBUG
		os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
		ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
		os << ")";
	}
	else
	{
		cout << "(empty)";
	}
	return os;
}

#ifdef TRACE
void StoreQueueEntry::Trace()
{
	trace_file->Begin("instruction");
	trace_file->Value("number", dbgProgramCounter);
	trace_file->Value("pc", dbgProgramCounter);
	trace_file->Value("size", operation.size);
	trace_file->Value("addr", addr);
	trace_file->End("instruction");
}
#endif
