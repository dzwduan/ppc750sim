/***************************************************************************
                          ReoderBuffer.cpp  -  Reorder buffer
                             -------------------
    begin                : Tue Mar 20 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#include <ReorderBuffer.h>

ReorderBufferEntry::ReorderBufferEntry()
{
	executionSerialized = false;
	branchCounter = false;
	finished = false;
}

ostream& operator << (ostream& os, const ReorderBufferEntry& entry)
{
	switch(entry.unitIdent)
	{
		case IntegerUnitIdent:
			os << "(integer : tag = " << (int) entry.extra.integer.tags.tag << ", unitNumber = " << entry.extra.integer.unitNumber << ", finished = " << entry.finished << ", execution serialized = " << entry.executionSerialized << ", branch counter = " << entry.branchCounter;
#ifdef DEBUG
			os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
			os << ")";
			break;
			
		case FloatingPointUnitIdent:
			os << "(floating point : tag = " << (int) entry.extra.floatingPoint.tags.tag << ", unitNumber = " << entry.extra.floatingPoint.unitNumber << ", finished = " << entry.finished << ", execution serialized = " << entry.executionSerialized << ", branch counter = " << entry.branchCounter;
#ifdef DEBUG
			os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
			os << ")";
			break;
		
		case LoadStoreUnitIdent:
			os << "(load/store : tag = " << (int) entry.extra.loadStore.tags.tag << ", finished = " << entry.finished << ", execution serialized = " << entry.executionSerialized  << ", branch counter = " << entry.branchCounter;
#ifdef DEBUG
			os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
			os << ")";
			break;
	
		case BranchUnitIdent:
			os << "(branch : tag = " << (int) entry.extra.branch.tags.tag << ", finished = " << entry.finished << ", execution serialized = " << entry.executionSerialized << ", branch counter = " << entry.branchCounter;
#ifdef DEBUG
			os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
			os << ")";
			break;
			
		case SystemRegisterUnitIdent:
			os << "(system register : tag = " << (int) entry.extra.systemRegister.tags.tag << ", finished = " << entry.finished << ", execution serialized = " << entry.executionSerialized << ", branch counter = " << entry.branchCounter;
#ifdef DEBUG
			os << ", instruction = (" << entry.dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(entry.dbgInstruction, entry.dbgProgramCounter, stdout);
#endif
			os << ")";
			break;
	}
	return os;
}

void ReorderBuffer::DecrementBranchCounter()
{
	int i, n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nReorderBufferEntries, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}	
}
