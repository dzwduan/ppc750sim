/***************************************************************************
                          BHT.h  -  Branch History Table
                             -------------------
    begin                : Mon May 21 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BHT_H__
#define __BHT_H__

#include <common.h>

struct BHT
{
	int counters[nBHTEntries];
	
	
	BHT();
	bool Predict(UInt32 pc);
	void Update(UInt32 pc, bool taken);
};

#endif
