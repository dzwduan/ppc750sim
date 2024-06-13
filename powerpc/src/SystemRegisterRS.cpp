/***************************************************************************
                          SystemRegisterRS.cpp  -  description
                             -------------------
    begin                : Tue Jun 5 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <SystemRegisterRS.h>

/*** SystemRegisterRS ***/

SystemRegisterRS::SystemRegisterRS()
{
	branchCounter = 0;
	memset(&operation, 0, sizeof(operation));
	memset(&tags, -1, sizeof(tags));
}

bool SystemRegisterRS::Valid()
{
	return true;
}


/*** SystemRegisterRSQueue ***/

void SystemRegisterRSQueue::DecrementBranchCounter()
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nSystemRegisterRS, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}
}
