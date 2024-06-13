/***************************************************************************
                          BranchPredictionUnit.cpp  -  description
                             -------------------
    begin                : Fri May 25 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BranchPredictionUnit.h>

void BranchPredictionUnit::ReadBHT()
{
	int i;
	UInt32 pc = inReadBHTPC;
	
	bool taken = bht.Predict(pc);
	outReadBHTTakenBranch = taken;
}

void BranchPredictionUnit::ReadBTIC()
{
	UInt32 instructions[nInstructionsPerBTICEntry];
	int instructionsCount;
	
	if(btic.Read(inReadBTICPC, instructionsCount, instructions))
	{
		outReadBTICHit = true;
		outReadBTICInstructionsCount = instructionsCount;

		int j;
		for(j = 0; j < instructionsCount; j++)
			outReadBTICInstructions[j] = instructions[j];
	}
	else
	{
		outReadBTICHit = false;
	}
}

void BranchPredictionUnit::UpdateBHT()
{
	if((Dummy<bool>) inUpdateBHT)
	{
		bht.Update(inUpdateBHTPC, inUpdateBHTTakenBranch);
		if(inUpdateBHTPC == inReadBHTPC) ReadBHT();
	}
}

void BranchPredictionUnit::UpdateBTIC()
{
	if((Dummy<bool>) inUpdateBTIC)
	{
		UInt32 instructions[nInstructionsPerBTICEntry];
		
		int i;
		int instructionsCount = inUpdateBTICInstructionsCount;
		
		for(i = 0; i < instructionsCount; i++)
			instructions[i] = inUpdateBTICInstructions[i];
		
		btic.Update(inUpdateBTICPC, instructionsCount, instructions);
		
		if(inUpdateBTICPC == inReadBTICPC) ReadBTIC();
	}	
}

