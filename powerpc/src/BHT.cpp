/***************************************************************************
                          BHT.cpp  -  Branch History Table
                             -------------------
    begin                : Mon May 21 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BHT.h>

BHT::BHT()
{
	int i;
	
	for(i = 0; i < nBHTEntries; i++)
	{
		counters[i] = 0;
	}
}

bool BHT::Predict(UInt32 pc)
{
	UInt32 index = (pc >> 2) % nBHTEntries;
#ifdef DEBUG
	if(Debug(DebugBHT))
	{
		cout << "Branch at "; WriteHex(cout, pc); cout << " (index " << index << ") is predicted " << (counters[index] >= 2 ? "taken" : "not taken") << endl;
	}
#endif
	return counters[index] >= 2;
}

void BHT::Update(UInt32 pc, bool taken)
{
	UInt32 index = (pc >> 2) % nBHTEntries;
//	cerr << "Branch at "; WriteHex(cerr, pc); cerr << " (index " << index << ") is " << (taken ? "taken" : "not taken") << endl;
	int counter = counters[index];

	if(taken)
	{
		if(counter < nBHTLevels - 1) counters[index] = counter + 1;
	}
	else
	{
		if(counter > 0) counters[index] = counter - 1;
	}
}
