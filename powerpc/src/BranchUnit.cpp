/***************************************************************************
                          BranchUnit.cpp  -  description
                             -------------------
    begin                : Mon May 21 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BranchUnit.h>

void BranchUnit::Execute()
{
	if(inDispatched)
	{
		const BranchOperation& operation = inOperation;
		const BranchOperands& operands = inOperands;
		const BranchTags& tags = inTags;
#if defined(DEBUG) || defined(TRACE)
		UInt32 dbgInstructionCounter = inInstructionCounter;
		UInt32 dbgInstruction = inInstruction;
#endif	
		
#ifdef DEBUG
		if(Debug(DebugBranchUnit))
		{
			cout << name() << ": dispatched (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, operands.pc, stdout);
			cout << endl;
		}
#endif
#ifdef TRACE
		if(Trace(TraceBranchUnit))
		{
			trace_file->Begin("execute");
			trace_file->Value("unit", "branch");
			trace_file->Value("number", dbgInstructionCounter);
			trace_file->Value("pc", dbgInstructionCounter);
			trace_file->End("execute");
		}
#endif
		UInt32 cia = operands.pc;
		UInt32 nia = cia + 4;
		UInt32 lr;
		UInt32 ctr;
		bool takenBranch;
		bool updateBHT;
		bool misprediction = false;
		
		switch(operation.ident)
		{
			case ID_B:
#ifdef DEBUG
				if(Debug(DebugBranchUnit))
				{
					cout << name() << ": immed = ";
					WriteHex(cout, operands.immed);
					if(operation.extra.b.aa)
						cout << "(absolute)";
					else
						cout << "(relative)";
					cout << endl;
				}
#endif
				takenBranch = true;
				if(operation.extra.b.aa)
					nia = operands.immed;
				else
					nia = cia + operands.immed;
			
				if(operation.extra.b.lk)
					lr = cia + 4; //lr = (cia + 4) >> 2;
				
				updateBHT = false;
#ifdef DEBUG
				if(Debug(DebugBranchUnit))
				{
					cout << name() << ": Unconditional branch (cia = ";
					WriteHex(cout, cia);
					cout << ", nia = ";
					WriteHex(cout, nia);
					cout << ", in lr = ";
					WriteHex(cout, operands.lr);
					cout << ", out lr = ";
					WriteHex(cout, lr);
					cout << ")" << endl;
				}
#endif
				break;
			
			case ID_BC:
				{
					bool ctr_ok;
					bool cond_ok;
					
					if((operation.extra.bc.bo & 16) == 0)
					{
#ifdef DEBUG
						if(Debug(DebugBranchUnit))
							cout << name() << ": conditional branch" << endl;
#endif
						/* Conditional branch */
						if(operation.extra.bc.bo & 8)
						{
#ifdef DEBUG
							if(Debug(DebugBranchUnit))
								cout << name() << ": branch if condition is true" << endl;
#endif
							/* Branch if condition is true */
							cond_ok = ((operands.crfield << (operation.extra.bc.bi %4)) & 8) != 0;
						}
						else
						{
#ifdef DEBUG
							if(Debug(DebugBranchUnit))
								cout << name() << ": branch if condition is false" << endl;
#endif
							/* Branch if condition is false */
							cond_ok = ((operands.crfield << (operation.extra.bc.bi %4)) & 8) == 0;
						}
					}
					else
					{
						cond_ok = true;
					}
					
					if((operation.extra.bc.bo & 4) == 0)
					{
#ifdef DEBUG
						if(Debug(DebugBranchUnit))
							cout << name() << ": branch decrement CTR" << endl;
#endif
						/* Decrement CTR */
						ctr = operands.ctr - 1;
						/* Test whether to branch if CTR is zero or not */
						if(operation.extra.bc.bo & 2)
						{
#ifdef DEBUG
							if(Debug(DebugBranchUnit))
								cout << name() << ": branch if ctr == 0" << endl;
#endif
							ctr_ok = ctr == 0;
						}
						else
						{
#ifdef DEBUG
							if(Debug(DebugBranchUnit))
								cout << name() << ": branch if ctr != 0" << endl;
#endif
							ctr_ok = ctr != 0;
						}
					}
					else
					{
						ctr_ok = true;
					}
					
					takenBranch = ctr_ok && cond_ok;
					
#ifdef DEBUG
					if(Debug(DebugBranchUnit))
					{
						cout << name() << ": immed = ";
						WriteHex(cout, operands.immed);
						if(operation.extra.bc.aa)
							cout << "(absolute)";
						else
							cout << "(relative)";
						cout << endl;
					}
#endif
					if(takenBranch)
					{
#ifdef DEBUG
						if(Debug(DebugBranchUnit))
							cout << name() << ": Taken branch" << endl;
#endif
						if(operation.extra.bc.aa)
							nia = operands.immed;
						else
							nia = cia + operands.immed;				
					}
					
					if(operation.extra.bc.lk)
						lr = cia + 4; //lr = (cia + 4) >> 2;
#ifdef DEBUG
					if(Debug(DebugBranchUnit))
					{
						cout << name() << ": Conditional branch (cr = ";
						WriteHex(cout, operands.crfield);
						cout << ", cia = ";
						WriteHex(cout, cia);
						cout << ", nia = ";
						WriteHex(cout, nia);
						cout << ", in lr = ";
						WriteHex(cout, operands.lr);
						cout << ", out lr = ";
						WriteHex(cout, lr);
						cout << ", in ctr = ";
						WriteHex(cout, operands.ctr);
						cout << ", out ctr = ";
						WriteHex(cout, ctr);
						cout << ")" << endl;
					}
#endif
				}

				updateBHT = true;
				misprediction = operands.branchPredictedTaken != takenBranch;
				nBC++;
				if(misprediction) nMispredictedBC++;
				break;
			
			case ID_BCLR:
				{
					bool ctr_ok;
					bool cond_ok;
					
					if((operation.extra.bclr.bo & 16) == 0)
					{
						/* Conditional branch */
						if(operation.extra.bclr.bo & 8)
						{
							/* Branch if condition is true */
							cond_ok = ((operands.crfield << (operation.extra.bclr.bi %4)) & 8) != 0;
						}
						else
						{
							/* Branch if condition is false */
							cond_ok = ((operands.crfield << (operation.extra.bclr.bi %4)) & 8) == 0;
						}
					}
					else
					{
						cond_ok = true;
					}
					
					if((operation.extra.bclr.bo & 4) == 0)
					{
						/* Decrement CTR */
						ctr = operands.ctr - 1;
						/* Test whether to branch if CTR is zero or not */
						ctr_ok = (operation.extra.bclr.bo & 2) ? ctr == 0 : ctr != 0;
					}
					else
					{
						ctr_ok = true;
					}
				
					takenBranch = ctr_ok && cond_ok;
					
					if(takenBranch)
					{
#ifdef DEBUG
						if(Debug(DebugBranchUnit))
							cout << "Taken branch" << endl;
#endif
						nia = operands.lr & 0xfffffffc; //nia = operands.lr << 2;					
					}
					
					if(operation.extra.bclr.lk)
						lr = cia + 4; //lr = (cia + 4) >> 2;
				
					updateBHT = false;
					misprediction = operands.branchPredictedTaken != takenBranch;
					
#ifdef DEBUG
					if(Debug(DebugBranchUnit))
					{
						cout << name() << ": Branch To Link Register (cia = ";
						WriteHex(cout, cia);
						cout << ", nia = ";
						WriteHex(cout, nia);
						cout << ", CR =";
						WriteHex(cout, operands.crfield);
						cout << ", in lr = ";
						WriteHex(cout, operands.lr);
						cout << ", out lr = ";
						WriteHex(cout, lr);
						cout << ")" << endl;
					}
#endif
				}
				nBCLR++;
				break;
			
			case ID_BCCTR:
				{
					bool cond_ok;
					
					if((operation.extra.bcctr.bo & 16) == 0)
					{
						/* Conditional branch */
						if(operation.extra.bcctr.bo & 8)
						{
							/* Branch if condition is true */
							cond_ok = (operands.crfield << (operation.extra.bcctr.bi %4) & 8) != 0;
						}
						else
						{
							/* Branch if condition is false */
							cond_ok = (operands.crfield << (operation.extra.bcctr.bi %4) & 8) == 0;
						}
					}
					else
					{
						cond_ok = true;
					}
				
					takenBranch = cond_ok;
					
					if(takenBranch)
					{
#ifdef DEBUG
						if(Debug(DebugBranchUnit))
							cout << "Taken branch" << endl;
#endif
						nia = operands.ctr & 0xfffffffc; //nia = operands.ctr << 2;				
					}
					
					if(operation.extra.bcctr.lk)
						lr = cia + 4; //lr = (cia + 4) >> 2;
				
					updateBHT = false;
					misprediction = operands.branchPredictedTaken != takenBranch;

#ifdef DEBUG
					if(Debug(DebugBranchUnit))
					{
						cout << name() << ": Branch To Count Register (cia = ";
						WriteHex(cout, cia);
						cout << ", nia = ";
						WriteHex(cout, nia);
						cout << ", in lr = ";
						WriteHex(cout, operands.lr);
						cout << ", out lr = ";
						WriteHex(cout, lr);
						cout << ")" << endl;
					}
#endif
				}
				nBCCTR++;
				break;
		}
		
		outMispredictedBranch = misprediction;
		if(misprediction)
			mispredictedBranches++;
		else
			wellPredictedBranches++;
/*
		if(misprediction)
		{
			cerr << "Branch at "; WriteHex(cerr, operands.pc); cerr << " is mispredicted (" << (operands.branchPredictedTaken ? "taken" : "not taken") << " instead of " << (takenBranch ? "taken" : "not taken") <<")" << endl;
		}
*/		
#ifdef DEBUG
		if(Debug(DebugBranchUnit))
			if(misprediction)
				cout << name() << ": Mispredicted branch" << endl;
#endif
		
		outUpdateBHT = updateBHT;
		if(updateBHT)
		{
#ifdef DEBUG
			if(Debug(DebugBranchUnit))
				cout << name() << ": Updating BHT" << endl;
#endif
			outBHTPC = operands.pc;
			outTakenBranch = takenBranch;
		}
		
		if(tags.LRTag >= 0)
		{
#ifdef DEBUG
			if(Debug(DebugBranchUnit))
				cout << name() << ": result LR = " << lr << endl;
#endif
			outResultLRValid = true;
			outResultLRTag = tags.LRTag;
			outResultLRData = lr;
		}
		else
		{
			outResultLRValid = false;
		}
	
		if(tags.CTRTag >= 0)
		{
#ifdef DEBUG
			if(Debug(DebugBranchUnit))
				cout << name() << ": result CTR = " << ctr << endl;
#endif
			outResultCTRValid = true;
			outResultCTRTag = tags.CTRTag;
			outResultCTRData = ctr;
		}
		else
		{
			outResultCTRValid = false;
		}

#ifdef DEBUG
		if(Debug(DebugBranchUnit))
		{
			cout << name() << ": Branch finished (tag =" << (int) tags.tag << ")  (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, operands.pc, stdout);
			cout << endl;
		}
#endif	
		outFinished = true;
		outTag = tags.tag;
		outPC = nia;
	}
	else
	{
		outUpdateBHT = false;
		outFinished = false;
		outResultLRValid = false;
		outResultCTRValid = false;
	}
	outBusy = false;
}

void BranchUnit::Reset()
{
	mispredictedBranches = 0;
	wellPredictedBranches = 0;
	nBC = 0;
	nMispredictedBC = 0;
	nBCLR = 0;
	nBCCTR = 0;
}
