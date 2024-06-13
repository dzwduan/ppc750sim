/***************************************************************************
                          BranchRS.h  -  description
                             -------------------
    begin                : Mon May 28 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BRANCHRS_H__
#define __BRANCHRS_H__

#include <common.h>
#include <Queue.h>

struct BranchRS
{
	BranchOperation operation;		/* branch operation */
	BranchTags tags;				/* tags of the instruction */
	
	/* Operands */
	UInt32 pc;						/* program counter of the branch instruction */
	ConditionRegisterRSEntry cr;	/* the CR field value if needed by the branch instruction */
	LinkRegisterRSEntry lr;			/* the LR value if needed by the branch instruction */
	CountRegisterRSEntry ctr;		/* the CTR value if needed by the branch instruction */
	UInt32 immed;					/* the immediate value if need by the branch instruction */
	bool branchPredictedTaken;		/* true if the branch has been predicted taken */
	int branchCounter;				/* number of previous that the branch instruction depends on */
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgInstruction;
#endif
	
	BranchRS();
	bool Valid();
	void UpdateLinkRegister(int tag, UInt32 data);
	void UpdateConditionRegister(int tag, UInt8 crfield);
	void UpdateCountRegister(int tag, UInt32 data);
};

struct BranchRSQueue : Queue<BranchRS, nBranchRS>
{
	void UpdateLinkRegister(int tag, UInt32 data);
	void UpdateConditionRegister(int tag, UInt8 data);
	void UpdateCountRegister(int tag, UInt32 data);
	void DecrementBranchCounter();
};

#endif
