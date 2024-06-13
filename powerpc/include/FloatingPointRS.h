/***************************************************************************
                          FloatingPointRS.h  -  description
                             -------------------
    begin                : Mon Jul 30 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __FLOATINGPOINT_H__
#define __FLOATINGPOINT_H__

#include <common.h>
#include <RSQueue.h>

struct FloatingPointRS
{
	bool full;
	FloatingPointRS *prev;	/* previous reservation station */
	FloatingPointRS *next;	/* next reservation station */
	FloatingPointOperation operation;	/* Floating point operation */
	FloatingPointTags tags;				/* tags of the instruction */
	FloatingPointRegisterRSEntry operands[3]; /* Operands */
	bool executionSerialized;			/* true if the instruction is execution serialized */
	int branchCounter;					/* number of branch instruction that the instruction depends on */
	int unitNumber;						/* number of the floating point unit in which the instruction will be executed */
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif
	
	FloatingPointRS();
	bool Valid();
	void UpdateFloatingPointRegister(int tag, UInt64 data);
	friend ostream& operator << (ostream& os, const FloatingPointRS& rs);
};

struct FloatingPointRSQueue : RSQueue<FloatingPointRS, nFloatingPointRS>
{
	void UpdateFloatingPointRegister(int tag, UInt64 data);
};

#endif
