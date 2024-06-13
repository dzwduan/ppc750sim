/***************************************************************************
                          BTIC.cpp  -  Branch Target Instruction Cache
                             -------------------
    begin                : Tue May 22 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <systemc.h>
#include <BTIC.h>
#include <stdlib.h>

BTIC::BTIC()
{
	Init();
}

void BTIC::Init()
{
	int bank, i, j;
	
	for(bank = 0; bank < BTICAssociativity; bank++)
	{
		for(i = 0; i < nBTICLines; i++)
		{
			directory[bank][i].valid = false;
			directory[bank][i].tag = 0;
			entries[bank][i].instructionsCount = 0;
			for(j = 0; j < nInstructionsPerBTICEntry; j++)
				entries[bank][i].instructions[j] = 0;
		}
	}
	
	for(i = 0; i < nBTICLines; i++)
	{
		plrubits[i] = 0;
	}
}

void BTIC::DecodeAddress(UInt32 addr, UInt32& tag, UInt32& index)
{
	addr = addr >> 2;
	tag = addr / nBTICLines;
	index = addr % nBTICLines;
}

int BTIC::Search(UInt32 index, UInt32 tag)
{
	int bank;
	for(bank = 0; bank < BTICAssociativity; bank++)
	{
		if(directory[bank][index].tag == tag)
		{
			return bank;
		}
	}
	return -1;
}

const int B0 = 1 << 0;
const int B1 = 1 << 1;
const int B2 = 1 << 2;
const int B3 = 1 << 3;
const int B4 = 1 << 4;
const int B5 = 1 << 5;
const int B6 = 1 << 6;

void BTIC::UpdatePLRUBits(UInt32 bank, UInt32 index)
{
	switch(BTICAssociativity)
	{
		case 4:
			switch(bank)
			{
				case 0:
					plrubits[index] |= B0  + B1;
					break;
					
				case 1:
					plrubits[index] |= B0;
					plrubits[index] &= B1;
					break;
					
				case 2:
					plrubits[index] |= B2;
					plrubits[index] &= B0;
					break;
					
				case 3:
					plrubits[index] &= B0 + B2;
					break;
			}
			break;
	
		case 8:
			switch(bank)
			{
				case 0:
					plrubits[index] |= B0 + B1 + B3;
					break;
				case 1:
					plrubits[index] |= B0 + B1;
					plrubits[index] &= ~B3;
					break;
			
				case 2:
					plrubits[index] |= B0 + B4;
					plrubits[index] &= ~B1;
					break;
			
				case 3:
					plrubits[index] |= B0;
					plrubits[index] &= ~(B1 + B4);
					break;
			
				case 4:
					plrubits[index] |= B2 + B5;
					plrubits[index] &= ~B0;
					break;
			
				case 5:
					plrubits[index] |= B2;
					plrubits[index] &= ~(B0 + B5);
					break;
				
				case 6:
					plrubits[index] |=B6;
					plrubits[index] &=~(B0 + B2);
					break;
			
				case 7:
					plrubits[index] &= ~(B0 + B2 + B6);
					break;
			}
			break;
	}
}


int BTIC::Choose(UInt32 index)
{
	/* !!!! Not specified into the PowerPC 750 manual */
	int bank;
	
	for(bank = 0; bank < BTICAssociativity; bank++)
	{
		if(!directory[bank][index].valid)
		{
			return bank;
		}
	}
	
	switch(BTICAssociativity)
	{
		case 4:
			if(plrubits[index] & B0)
			{
				if(plrubits[index] & B2) return 3; else return 2;
			}
			else
			{
				if(plrubits[index] & B1) return 1; else return 0;
			}
			break;
			
		case 8:
			if(plrubits[index] & B0)
			{
				if(plrubits[index] & B2)
				{
					if(plrubits[index] & B5) return 5; else return 4;
				}
				else
				{
					if(plrubits[index] & B6) return 7; else return 6;
				}
			}
			else
			{
				if(plrubits[index] & B1)
				{
					if(plrubits[index] & B4) return 3; else return 2;
				}
				else
				{
					if(plrubits[index] & B3) return 1; else return 0;
				}
			}
	}
	
	return -1;	
}

bool BTIC::Read(UInt32 pc, int& instructionsCount, UInt32 *instructions)
{
	UInt32 tag, index;
	int bank;
	
//	cout << "BTIC: read pc = "; WriteHex(cout, pc); cout << endl;
	
	DecodeAddress(pc, tag, index);
	
	bank = Search(index, tag);
	
	if(bank >= 0)
	{
		// BTIC Hit
		UpdatePLRUBits(bank, index);
		
		int i;
		instructionsCount = entries[bank][index].instructionsCount;
		
		for(i = 0; i < instructionsCount; i++)
			instructions[i] = entries[bank][index].instructions[i];
			
		return true;
	}
	// Miss
	return false;
}

void BTIC::Update(UInt32 pc, int instructionsCount, UInt32 *instructions)
{
//	cout << "BTIC: update pc = "; WriteHex(cout, pc); cout << endl;
	UInt32 tag, index;
	int bank;
	
	DecodeAddress(pc, tag, index);
	
	bank = Choose(index);
	
	directory[bank][index].valid = true;
	directory[bank][index].tag = tag;
	entries[bank][index].instructionsCount = instructionsCount;
	
	int i;
		
#ifdef DEBUG
	if(instructionsCount > nInstructionsPerBTICEntry)
	{
		cout << "Too many instructions to fit into a BTIC entry" << endl;
		ABORT();
	}
#endif
	for(i = 0; i < instructionsCount; i++)
		entries[bank][index].instructions[i] = instructions[i];
}
