/***************************************************************************
                          SystemRegisterUnit.cpp  -  description
                             -------------------
    begin                : Fri Jun 1 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <SystemRegisterUnit.h>

void SystemRegisterUnit::Execute()
{
	const SystemRegisterOperation& operation = inOperation;
	const SystemRegisterTags& tags = inTags;
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter = inInstructionCounter;
	UInt32 dbgProgramCounter = inProgramCounter;
	UInt32 dbgInstruction = inInstruction;
#endif
	
	if(inDispatched)
	{
		branchCounter = inBranchCounter;
#ifdef DEBUG
		if(Debug(DebugSystemRegisterUnit))
		{
			cout << name() << ": dispatched (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
			cout << endl;
			cout << "CR = "; WriteHex(cout, inCR); cout << endl;
		}
#endif
#ifdef TRACE
		if(Trace(TraceSystemRegisterUnit))
		{
			trace_file->Begin("execute");
			trace_file->Value("unit", "system register");
			trace_file->Value("number", dbgInstructionCounter);
			trace_file->Value("pc", dbgInstructionCounter);
			trace_file->End("execute");
		}
#endif
		dispatched = true;
	}
	
	if(dispatched)
	{
		if(latency == 0)
		{
			switch(operation.ident)
			{
				case ID_MFSPR:
					switch(operation.extra.mfspr.spr)
					{
						case 8:
							result = inLR;
							break;
							
						case 9:
							result = inCTR;
							break;
					}
					latency = 1;
					break;
				
				case ID_MTSPR:
					outReadRegisterNumber = operation.extra.mtspr.gpr;
					latency = 2;
					break;
						
				case ID_CRANDC:
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (a & (!b)) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
						
				case ID_CRAND:
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (a & b) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
						
				case ID_CREQV:
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (!(a ^ b)) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
				
				case ID_CRNAND:						
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (!(a & b)) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
				
				case ID_CRNOR:						
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (!(a | b)) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
				
				case ID_CROR:						
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (a | b) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
				
				case ID_CRORC:
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (a | (!b)) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
				
				case ID_CRXOR:
					{
						UInt32 cr = inCR;
						UInt32 crfield = cr >> ((31 - operation.extra.crlogical.dstCrb) & (~3));
						UInt32 a = (cr >> (31 - operation.extra.crlogical.srcCrb[0])) & 1;
						UInt32 b = (cr >> (31 - operation.extra.crlogical.srcCrb[1])) & 1;
						UInt32 d = (a  ^ b) << ((31 - operation.extra.crlogical.dstCrb) % 4);
						UInt32 m = ~(1 << ((31 - operation.extra.crlogical.dstCrb) % 4));
						resultCR = (crfield & m) | d;
						latency = 1;
					}
					break;
						
				case ID_MFCR:
					result = inCR;
					latency = 1;
					break;
					
				case ID_SC:
					do_syscall();
					latency = 1;
					break;
					
				case ID_MTCRF:
					outReadRegisterNumber = operation.extra.mtcrf.gpr;
					latency = 2;
					break;
					
				case ID_MCRF:
					resultCR = (inCR >> (4 * (7 - operation.extra.mcrf.srcCrf))) & 0xf;
					latency = 1;
					break;
					
				case ID_MFTB:
					switch(operation.extra.mftb.tbr)
					{
						case 268:
						{
							UInt64 tb = (UInt64) sim_time_stamp() / 16;
							result = (UInt32) tb;
							TBL = result;
//							cout << "TBL = " << result << endl;
						}
						break;

						case 269:
						{
							UInt64 tb = (UInt64) sim_time_stamp() / 16;
							result = (UInt32)(tb >> 32);
							TBU = result;
//							cout << "TBU = " << result << endl;
						}
						break;

						default:
							cout << "Fatal: attempt to read tbr" << operation.extra.mftb.tbr << endl;
							abort();
					}
					latency = 1;
					break;


				default:
					cout << name() << ": instruction is not implemented" << endl;
					ABORT();
			}
		}

		if(--latency == 0)
		{
			if(tags.resultTag >= 0)
			{
#ifdef DEBUG
				if(Debug(DebugSystemRegisterUnit))
				{
					cout << name() << ": producing ";
					WriteHex(cout, result);
					cout << " for a destination gpr" << endl;
				}
#endif
				outResultValid = true;
				outResultTag = tags.resultTag;
				outResultData = result;
			}
			else
			{
				outResultValid = false;
			}

			if(tags.LRTag >= 0)
			{
				UInt32 resultLR = inReadRegisterData;
#ifdef DEBUG
				if(Debug(DebugSystemRegisterUnit))
				{
					cout << name() << ": producing ";
					WriteHex(cout, resultLR);
					cout << " for LR" << endl;
				}
#endif
				outResultLRValid = true;
				outResultLRTag = tags.LRTag;
				outResultLRData = resultLR;
			}
			else
			{
				outResultLRValid = false;
			}

			if(tags.CTRTag >= 0)
			{
				UInt32 resultCTR = inReadRegisterData;
#ifdef DEBUG
				if(Debug(DebugSystemRegisterUnit))
				{
					cout << name() << ": producing ";
					WriteHex(cout, resultCTR);
					cout << " for CTR" << endl;
				}
#endif
				outResultCTRValid = true;
				outResultCTRTag = tags.CTRTag;
				outResultCTRData = resultCTR;
			}
			else
			{
				outResultCTRValid = false;
			}

			if(tags.CRTag >= 0)
			{
				if(operation.ident == ID_MTCRF)
				{
					switch(operation.extra.mtcrf.crm)
					{
						case 128: resultCR = (inReadRegisterData >> 28) & 15; break;
						case 64: resultCR = (inReadRegisterData >> 24) & 15; break;
						case 32: resultCR = (inReadRegisterData >> 20) & 15; break;
						case 16: resultCR = (inReadRegisterData >> 16) & 15; break;
						case 8: resultCR = (inReadRegisterData >> 12) & 15; break;
						case 4: resultCR = (inReadRegisterData >> 8) & 15; break;
						case 2: resultCR = (inReadRegisterData >> 4) & 15; break;
						case 1: resultCR = inReadRegisterData & 15; break;
					}
				}
#ifdef DEBUG
				if(Debug(DebugSystemRegisterUnit))
				{
					cout << name() << ": producing ";
					WriteHex(cout, result);
					cout << " for a CR field" << endl;
				}
#endif
				outResultCRValid = true;
				outResultCRTag = tags.CRTag;
				outResultCRData = resultCR;
			}
			else
			{
				outResultCRValid = false;

				if(operation.ident == ID_MTCRF)
				{
					outCR = inReadRegisterData;
				}
			}

			outBusy = false;
			outTag = tags.tag;
			outFinished = true;
			dispatched = false;
#ifdef DEBUG
			if(Debug(DebugSystemRegisterUnit))
			{
				cout << name() << ": Finishing instruction (tag = " << (int) tags.tag << ")  (" << dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
				cout << endl;
			}
#endif
			return;
		}
		outResultValid = false;
		outResultCRValid = false;
		outResultLRValid = false;
		outResultCTRValid = false;
		outFinished = false;
		outBusy = true;
		return;
	}
	outResultValid = false;
	outResultCRValid = false;
	outResultLRValid = false;
	outResultCTRValid = false;
	outFinished = false;
	outBusy = false;
}

void SystemRegisterUnit::OnFallingEdge()
{
	if(inBranchFinished)
	{
		if(inMispredictedBranch && branchCounter > 0)
		{
			branchCounter = 0;
			latency = 0;
			dispatched = false;
			outResultValid = false;
			outResultCRValid = false;
			outResultLRValid = false;
			outResultCTRValid = false;
			outFinished = false;
			outBusy = false;
		}

		if(branchCounter > 0)	branchCounter--;
	}
}

void SystemRegisterUnit::Reset()
{
	latency = 0;
	execute = false;
	dispatched = false;
	branchCounter = 0;
}
