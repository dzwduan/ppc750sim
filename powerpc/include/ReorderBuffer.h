/***************************************************************************
                          ReorderBuffer.h  -  Reorder Buffer
                             -------------------
    begin                : Tue Mar 20 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@cea.fr, mouchard@lri.fr
 ***************************************************************************/

#ifndef __REORDERBUFFER_H__
#define __REORDERBUFFER_H__

#include <common.h>
#include <Queue.h>
#include <iostream>

/* ReorderBufferEntry is a reorder buffer entry.
   It contains informations about an instruction in order to write back results to the physical registers and to memory */
struct ReorderBufferEntry
{
	UnitIdent unitIdent;			/* Type of unit (Integer, Floating Point, Load/Store, Branch, etc.... */
	
	union
	{
		/* the following struct are defined depending on the content of unitIdent */
		
		/* IntegerUnitIdent */
		struct
		{
			IntegerTags tags;		/* tags of the integer instruction */
			int unitNumber;			/* Integer unit number */
			bool writeOverflow;		/* true if the instruction modifies the overflow bit XER[OV] */
			bool overflow;			/* value to write to the overflow bit XER[OV] */
			bool writeCarry;		/* true if the instruction modifies the carry bit XER[CA] */
			bool carry;				/* value to write to the carry bit XER[CA] */
		} integer;
		
		/* FloatingPointUnitIdent */
		struct
		{
			FloatingPointTags tags;		/* tags of the floating point instruction */
			int unitNumber;				/* Floating Point unit number */
		} floatingPoint;
		
		/* LoadStoreUnitIdent */
		struct
		{
			bool write;				/* true if the instruction is a store */
			bool floatingPoint;		/* true if the instruction is a floating point load/store */
			LoadStoreTags tags;		/* tags of the load/store instructions */
		} loadStore;
		
		/* BranchUnitIdent */
		struct
		{
			BranchTags tags;	/* tags of the branch instruction */
			bool mispredictedBranch;
			UInt32 pc;
		} branch;
		
		/* SystemRegisterUnitIdent */
		struct
		{
			SystemRegisterTags tags;	/* tags of the system register unit instruction */
			bool interlock;				/* true if the interlock bit is set */
			bool writeCR;				/* true if the instruction potentially modifies all fields of the CR register */
			UInt8 crm;					/* binary mask containing which CR fields must be written */
			UInt32 cr;					/* value which is anded with crm, and written to CR */
		} systemRegister;
	} extra;
	
	/* Emulator code begins */
	ppc_inst_t inst;		/* instruction word fetched by the emulator (for test purposes) */
	addr_t instPC;			/* instruction address (for test purposes) */
	/* Emulator code ends */
	UInt32 nia;					/* Next instruction address */
	
	bool executionSerialized;	/* true if the instruction is serialized at execution */
								/* (execution begins when it is the oldest instruction into the processor) */
	int branchCounter;			/* Number of branch instructions that this instruction depends on */
	bool finished;				/* true if the instruction is finished */
	
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;	/* instruction serial number (debugging purposes) */
	UInt32 dbgProgramCounter;		/* instruction address (debugging purposes) */
	UInt32 dbgInstruction;			/* instruction word (debugging purposes) */
#endif
	
	ReorderBufferEntry();
	friend ostream& operator << (ostream& os, const ReorderBufferEntry& entry);
};

/* ReorderBuffer is a reorder buffer. It is implemented as a specialized queue */
struct ReorderBuffer : Queue<ReorderBufferEntry, nReorderBufferEntries>
{
	void DecrementBranchCounter();
};

#endif
