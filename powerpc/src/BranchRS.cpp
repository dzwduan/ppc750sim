/***************************************************************************
                          BranchRS.cpp  -  description
                             -------------------
    begin                : Mon May 28 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BranchRS.h>

/*** BranchRS ***/

BranchRS::BranchRS()
{
	pc = 0;
	immed = 0;
	branchPredictedTaken = false;
	branchCounter = 0;
	memset(&operation, 0, sizeof(operation));
	memset(&tags, -1, sizeof(tags));
}

bool BranchRS::Valid()
{
	if(cr.tag >= 0 && !cr.valid) return false;
	if(lr.tag >= 0 && !lr.valid) return false;
	if(ctr.tag >= 0 && !ctr.valid) return false;
	return true;
}
	
void BranchRS::UpdateLinkRegister(int tag, UInt32 data)
{
	if(!lr.valid && lr.tag == tag)
	{
		lr.valid = true;
		lr.data = data;
	}
}

void BranchRS::UpdateConditionRegister(int tag, UInt8 crfield)
{
	if(!cr.valid && cr.tag == tag)
	{
		cr.valid = true;
		cr.crfield = crfield;
	}
}

void BranchRS::UpdateCountRegister(int tag, UInt32 data)
{
	if(!ctr.valid && ctr.tag == tag)
	{
		ctr.valid = true;
		ctr.data = data;
	}
}

/*** BranchRSQueue ***/

void BranchRSQueue::UpdateLinkRegister(int tag, UInt32 data)
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nBranchRS, n--)
	{
		queue[i].UpdateLinkRegister(tag, data);
	}
}
	
void BranchRSQueue::UpdateConditionRegister(int tag, UInt8 data)
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nBranchRS, n--)
	{
		queue[i].UpdateConditionRegister(tag, data);
	}		
}
	
void BranchRSQueue::UpdateCountRegister(int tag, UInt32 data)
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nBranchRS, n--)
	{
		queue[i].UpdateCountRegister(tag, data);
	}
}

void BranchRSQueue::DecrementBranchCounter()
{
	int i; /* numéro de station de réservation */
	int n;
	
	for(i = tail, n = size; n > 0; i = (i + 1) % nBranchRS, n--)
	{
		if(queue[i].branchCounter > 0) queue[i].branchCounter--;
	}
}
