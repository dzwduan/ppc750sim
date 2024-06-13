/***************************************************************************
                          IntegerRS.h  -  description
                             -------------------
    begin                : Mon Mar 12 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#ifndef __INTEGERRS_H__
#define __INTEGERRS_H__

#include <common.h>
#include <RSQueue.h>

struct IntegerRS
{
	bool full;
	IntegerRS *prev;	/* previous reservation station in the queue */
	IntegerRS *next;	/* next reservation station in the queue */
	IntegerOperation operation;	/* integer operation */
	IntegerTags tags;			/* tags of the integer instruction */
	IntegerRegisterRSEntry operands[2]; /* Operands */
	bool executionSerialized;	/* true if the instruction is execution serialized */
	int branchCounter;	/* number of branchs that the instruction depends on */
	int unitNumber;		/* number of the integer unit in which the instruction will be executed */
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif
	
	IntegerRS();
	bool Valid();
	void UpdateIntegerRegister(int tag, UInt32 data);
	friend ostream& operator << (ostream& os, const IntegerRS& rs);
};

struct IntegerRSQueue : RSQueue<IntegerRS, nIntegerRS>
{
	void UpdateIntegerRegister(int tag, UInt32 data);
};

#endif
