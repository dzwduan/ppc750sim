/***************************************************************************
                          BTIC.h  -  Branch Target Instruction Cache
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BTIC_H__
#define __BTIC_H__

#include <common.h>

struct BTIC
{
	struct
	{
		bool valid;
		UInt32 tag;
	} directory[BTICAssociativity][nBTICLines];
	
	int plrubits[nBTICLines];
	
	struct
	{
		int instructionsCount;
		UInt32 instructions[nInstructionsPerBTICEntry];
	} entries[BTICAssociativity][nBTICLines];

	BTIC();
	void Init();
	void DecodeAddress(UInt32 addr, UInt32& tag, UInt32& index);
	void UpdatePLRUBits(UInt32 bank, UInt32 index);
	int Search(UInt32 index, UInt32 tag);
	int Choose(UInt32 index);	
	bool Read(UInt32 pc, int& instructionsCount, UInt32 *instructions);
	void Update(UInt32 pc, int instructionsCount, UInt32 *instructions);
};

#endif
