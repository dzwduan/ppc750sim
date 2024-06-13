/***************************************************************************
                          LoadStoreRS.h  -  description
                             -------------------
    begin                : Fri Mar 16 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#ifndef __LOADSTORERS_H__
#define __LOADSTORERS_H__

#include <common.h>
#include <Queue.h>

struct LoadStoreRS
{
	LoadStoreOperation operation;	/* Load/store operation */
	LoadStoreTags tags;				/* tags of the instruction */
	Register64RSEntry operand0;		/* first operand of the instruction (either integer or floating point operand) */
	IntegerRegisterRSEntry operand1;	/* second operand (integer) */
	IntegerRegisterRSEntry operand2;	/* third operand (integer) */
	int branchCounter;					/* number of branch instruction that the instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif
	
	LoadStoreRS();
	bool Valid();
	void UpdateIntegerRegister(int tag, UInt32 data);
	void UpdateFloatingPointRegister(int tag, UInt64 data);
	friend ostream& operator << (ostream& os, const LoadStoreRS& rs);
};

struct LoadStoreRSQueue : Queue<LoadStoreRS, nLoadStoreRS>
{
	void UpdateIntegerRegister(int tag, UInt32 data);
	void UpdateFloatingPointRegister(int tag, UInt64 data);
	void DecrementBranchCounter();
};


#endif
