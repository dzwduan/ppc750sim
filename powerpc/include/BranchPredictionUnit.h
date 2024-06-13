/***************************************************************************
                          BranchPredictionUnit.h  -  description
                             -------------------
    begin                : Fri May 25 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BRANCHPREDICTIONUNIT_H__
#define __BRANCHPREDICTIONUNIT_H__

#include <systemc>
#include <common.h>
#include <BTIC.h>
#include <BHT.h>

SC_MODULE(BranchPredictionUnit)
{
	BTIC btic;
	BHT bht;
	
	sc_in<bool> inClock;

	/* From Fetch Unit */
	sc_in<UInt32> inReadBHTPC; /* addresse du branchement */
	sc_in<UInt32> inReadBTICPC;
	
	/* To Fetch Unit */
	sc_out<bool> outReadBHTTakenBranch; /* true si le branchement est predit pris (du plus recent au plus ancien) */
	sc_out<bool> outReadBTICHit; /* true si hit dans le Branch Target Instruction Cache */
	sc_out<int> outReadBTICInstructionsCount;
	sc_out<UInt32> outReadBTICInstructions[nInstructionsPerBTICEntry]; /* Instructions lues dans le Branch Target Instruction Cache */
	
	/* From Branch Unit */
	sc_in<Dummy<bool> > inUpdateBHT;
	sc_in<UInt32> inUpdateBHTPC;
	sc_in<bool> inUpdateBHTTakenBranch;
	
	/* From Fetch Unit */
	sc_in<Dummy<bool> > inUpdateBTIC;
	sc_in<UInt32> inUpdateBTICPC;
	sc_in<UInt32> inUpdateBTICInstructionsCount;
	sc_in<UInt32> inUpdateBTICInstructions[nInstructionsPerBTICEntry];
	
	
	void Predict(UInt32 pc);
	void ReadBHT();
	void ReadBTIC();
	void UpdateBHT();
	void UpdateBTIC();
	
	SC_CTOR(BranchPredictionUnit)
	{
		SC_METHOD(ReadBHT);
		sensitive << inReadBHTPC;
		
		SC_METHOD(ReadBTIC);
		sensitive << inReadBTICPC;
	
		SC_METHOD(UpdateBHT);
		sensitive << inUpdateBHT;
		
		SC_METHOD(UpdateBTIC);
		sensitive << inUpdateBTIC;

	}
};

#endif
