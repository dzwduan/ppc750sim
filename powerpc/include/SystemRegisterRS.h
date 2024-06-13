/***************************************************************************
                          SystemRegisterRS.h  -  description
                             -------------------
    begin                : Tue Jun 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __SYSTEMREGISTERRS_H__
#define __SYSTEMREGISTERRS_H__

#include <common.h>
#include <Queue.h>

struct SystemRegisterRS
{
	/* Informations de decodage */
	SystemRegisterOperation operation;
	SystemRegisterTags tags;
	bool executionSerialized;
	bool interlock; /* instruction set the interlock bit */
	
	int branchCounter;
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter;
	UInt32 dbgProgramCounter;
	UInt32 dbgInstruction;
#endif
	
	SystemRegisterRS();
	bool Valid();
};

struct SystemRegisterRSQueue : Queue<SystemRegisterRS, nSystemRegisterRS>
{
	void DecrementBranchCounter();	
};

#endif
