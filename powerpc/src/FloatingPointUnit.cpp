/***************************************************************************
                          FloatingPointUnit.cpp  -  description
                             -------------------
    begin                : Fri Jul 27 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <FloatingPointUnit.h>

bool FloatingPointUnit::Pipelined(OperationIdent ident)
{
	switch(ident)
	{
		case ID_FDIV:
		case ID_FDIVD:
		case ID_FDIVS:
		case ID_FDIVSD:
		case ID_FRES:
		case ID_FRESD:
		case ID_MTFSB0:
		case ID_MTFSB0D:
		case ID_MTFSB1:
		case ID_MTFSB1D:
		case ID_MTFSFI:
		case ID_MTFSFID:
		case ID_MTFSF:
		case ID_MTFSFD:
			return false;
	}
	return true;
}

void FloatingPointUnit::Execute()
{
#ifdef DEBUG	
	if(Debug(DebugFloatingPointUnit))
	{
		cout << "Pipeline content :" << endl;
		pipeline.Print(cout);
	}
#endif
	
	FloatingPointInstruction<nFloatingPointStages> *instruction;

	/* Get a finished instruction at the end of the pipeline */	
	instruction = pipeline.GetOldest();
	
	if(instruction)
	{
		outFinished = true;
		outTag = instruction->tags.tag;
	
		if(instruction->tags.resultTag >= 0)
		{
#ifdef DEBUG
			if(Debug(DebugFloatingPointUnit))
			{
				cout << name() << ": producing rfp" << (int) instruction->tags.resultTag << "(";
				WriteHex(cout, instruction->result);
				cout << "  " << *(double *) &instruction->result;
				cout << ")" << endl;
			}
#endif
			outResultValid = true;
			outResultTag = instruction->tags.resultTag;
			outResultData = instruction->result;
		}
		else
		{
			outResultValid = false;				
		}
					
		if(instruction->tags.CRTag >= 0)
		{
#ifdef DEBUG
			if(Debug(DebugFloatingPointUnit))
			{
				cout << name() << ": producing CR rename buffer #" << (int) instruction->tags.CRTag << "(";
				WriteHex(cout, (UInt8) instruction->resultCR);
				cout << ")" << endl;
			}
#endif
			outResultCRValid = true;
			outResultCRTag = instruction->tags.CRTag;
			
			outResultCRData = instruction->resultCR;
		}
		else
		{
			outResultCRValid = false;
		}
		
		pipeline.ReleaseTail();
	}
	else
	{
		outFinished = false;
		outResultValid = false;
		outResultCRValid = false;
	}
	
	pipeline.Shift();
	
	if(inDispatched)
	{
		const FloatingPointOperation& operation = inOperation;
		const FloatingPointOperands& operands = inOperands;
		const FloatingPointTags& tags = inTags;
#if defined(DEBUG) || defined(TRACE)
		UInt32 dbgInstructionCounter = inInstructionCounter;
		UInt32 dbgProgramCounter = inProgramCounter;
		UInt32 dbgInstruction = inInstruction;
#endif

#ifdef DEBUG
		if(Debug(DebugFloatingPointUnit))
		{
			cout << name() << ": dispatched tag = " << (int) tags.tag << " (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
			cout << endl;
			int i;
			for(i = 0; i < 3; i++)
			{
				cout << name() << ": operand " << i << " = "; WriteHex(cout, operands.data[i]); cout << " (" << *(double *) &operands.data[i] << ")";
				if(IsNan(operands.data[i])) cout << " NAN";
				if(IsSNan(operands.data[i])) cout << " SNAN";
				cout << endl;
			}
		}
#endif
#ifdef TRACE
		if(Trace(TraceFloatingPointUnit))
		{
			trace_file->Begin("execute");
			trace_file->Value("number", dbgInstructionCounter);
			trace_file->Value("unit", "floating point");
			trace_file->Value("pc", dbgProgramCounter);
			trace_file->End("Execute");
		}
#endif
	
		bool pipelined = Pipelined(operation.ident);		
	
		if(pipelined)
		{
#ifdef DEBUG
			if(Debug(DebugFloatingPointUnit))
				cout << name() << ": Pipelined operation" << endl;
#endif
			int branchCounter = inBranchCounter;
			instruction = pipeline.Allocate();
		
			instruction->operation = operation;
			instruction->operands = operands;
			instruction->tags = tags;
			instruction->branchCounter = branchCounter;
#ifdef DEBUG
			instruction->dbgInstructionCounter = dbgInstructionCounter;
			instruction->dbgProgramCounter = dbgProgramCounter;
			instruction->dbgInstruction = dbgInstruction;
#endif	
			
			int *latencies = instruction->latencies;
			UInt64& result = instruction->result;
				
			bool fx = false, fex = false, vx = false, ox = false;
				
			switch(operation.ident)
			{
				case ID_MFFS:
					result = 0;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;		
					break;
				case ID_FMULSD:
				case ID_FMULS:
				{
					double a = *(double *) &operands.data[0];
					double c = *(double *) &operands.data[2];
					double res = (float)(a * c);
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FCMPU:
				{
					double fa = *(double *) &operands.data[0];
					double fb = *(double *) &operands.data[1];
					UInt64 a = operands.data[0];
					UInt64 b = operands.data[1];
	
					if(IsNan(a) || IsNan(b))
					{
						ox = true;
					}
					else
					{
						if(fa < fb)
						{
							fx = true;
						}
						else
						{
							if(fa > fb)
							{
								fex = true;
							}
							else
							{
								vx = true;
							}
						}
					}
					
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FMRD:
				case ID_FMR:
				{
					result = operands.data[1];
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FRSPD:
				case ID_FRSP:
				{
					double b = *(double *) &operands.data[1];
					double res = (float) b;
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				case ID_FADDD:
				case ID_FADD:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = a + b;
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				case ID_FSUBD:
				case ID_FSUB:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = a - b;
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FCTIWZD:				
				case ID_FCTIWZ:
				{
					double b = *(double *) &operands.data[1];
					SInt32 res = (SInt32) b;
					result = (UInt64)(UInt32) res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FMULD:
				case ID_FMUL:
				{
					double a = *(double *) &operands.data[0];
					double c = *(double *) &operands.data[2];
					double res = a * c;
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				case ID_FMADDD:
				case ID_FMADD:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = (a * c) + b;
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
					
				case ID_FADDSD:
				case ID_FADDS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = (float)(a + b);
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FABSD:
				case ID_FABS:
				{
					double b = *(double *) &operands.data[1];
					double res = fabs(b);
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FNMSUBD:
				case ID_FNMSUB:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = -((a * c) - b);
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FMADDSD:
				case ID_FMADDS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = (float)((a * c) + b);
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FSUBSD:
				case ID_FSUBS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = (float)(a - b);
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FNEGD:
				case ID_FNEG:
				{
					double b = *(double *) &operands.data[1];
					double res = -b;
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FMSUBD:
				case ID_FMSUB:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = (a * c) - b;
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FNMADDD:
				case ID_FNMADD:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = -((a * c) + b);
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FMSUBSD:
				case ID_FMSUBS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = (float)((a * c) - b);
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FNMADDSD:
				case ID_FNMADDS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double c = *(double *) &operands.data[2];
					double res = (float)(-((a * c) + b));
					result = *(UInt64 *) &res;
					latencies[0] = 2;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				
				case ID_FNABSD:
				case ID_FNABS:
				{
					double b = *(double *) &operands.data[1];
					double res = -fabs(b);
					result = *(UInt64 *) &res;
					latencies[0] = 1;
					latencies[1] = 1;
					latencies[2] = 1;
					break;
				}
				default:
					cout << name() << ": Simulator does not support this instruction (tag = " << (int) tags.tag;
					
#ifdef DEBUG
					cout << ", op = " << operation.ident << ")   (" << dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
					cout << endl;
#else
					cout << ")" << endl;
#endif
					ABORT();
			}
			
			if(instruction->tags.CRTag >= 0)
				instruction->resultCR = (fx ? 8:0) | (fex ? 4:0) | (vx ? 2:0) | (ox ? 1:0);
		}
		else
		{
			dispatched = true;
		}
	}
	
	if(dispatched)
	{
		const FloatingPointOperation& operation = inOperation;
		const FloatingPointOperands& operands = inOperands;
		const FloatingPointTags& tags = inTags;
#ifdef DEBUG
			UInt32 dbgInstructionCounter = inInstructionCounter;
			UInt32 dbgProgramCounter = inProgramCounter;
			UInt32 dbgInstruction = inInstruction;
#endif
#ifdef DEBUG
		if(Debug(DebugFloatingPointUnit))
			cout << name() << ": not a pipelined operation" << endl;
#endif
		if(latency == 0)
		{
			fx = false;
			fex = false;
			vx = false;
			ox = false;
		
			switch(operation.ident)
			{
				case ID_MTFSF:
				case ID_MTFSFD:
					result = 0;
					latency = 3;
					break;
				case ID_MTFSFID:
				case ID_MTFSFI:
					result = 0;
					latency = 3;
					break;				
					
				case ID_MTFSB0:
				case ID_MTFSB0D:
				case ID_MTFSB1:
				case ID_MTFSB1D:
					latency = 3;
					break;
					
				case ID_FDIVSD:
				case ID_FDIVS:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = (float)(a / b);
					result = *(UInt64 *) &res;
					latency = 17;
					break;
				}
				
				case ID_FDIVD:
				case ID_FDIV:
				{
					double a = *(double *) &operands.data[0];
					double b = *(double *) &operands.data[1];
					double res = a / b;
					result = *(UInt64 *) &res;
					latency = 31;
					break;
				}
				
				default:
					cout << name() << ": Simulator does not support this instruction (tag = " << (int) tags.tag;
#ifdef DEBUG
					cout << ", op = " << operation.ident << ")   (" << dbgInstructionCounter << ")  ";
					ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
					cout << endl;
#else
					cout << endl;
#endif
					ABORT();
			}
		}
		
#ifdef DEBUG
		if(Debug(DebugFloatingPointUnit))
			if(latency <= 0)
			{
				cout << name() << "Latency is " << latency << endl;
				ABORT();
			}
#endif
				
		if(--latency == 0)
		{
			outFinished = true;
			outTag = tags.tag;
			dispatched = false;
		
			if(tags.resultTag >= 0)
			{
#ifdef DEBUG
				if(Debug(DebugFloatingPointUnit))
				{
					cout << name() << ": producing rfp" << (int) tags.resultTag << "(";
					WriteHex(cout, result);
					cout << "  " << *(double *) &result;
					cout << ")" << endl;
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
				
			if(tags.CRTag >= 0)
			{
#ifdef DEBUG
				if(Debug(DebugFloatingPointUnit))
				{
					cout << name() << ": producing CR rename buffer #" << (int) tags.CRTag << "(";
					WriteHex(cout, (UInt8)((fx ? 8:0) | (fex ? 4:0) | (vx ? 2:0) | (ox ? 1:0)));
					cout << ")" << endl;
				}
#endif
				outResultCRValid = true;
				outResultCRTag = tags.CRTag;
	
				outResultCRData = (fx ? 8:0) | (fex ? 4:0) | (vx ? 2:0) | (ox ? 1:0);
			}
			else
			{
				outResultCRValid = false;
			}
			outFinished = true;
			outBusy = false;
			return;					
		}
		outFinished = false;
		outResultValid = false;
		outResultCRValid = false;
		outBusy = true;
		return;
	}
	
	outFloatingPointPipelineEmpty = pipeline.Empty();
	outBusy = pipeline.Stalled() || latency > 0;

#ifdef DEBUG	
	if(Debug(DebugFloatingPointUnit))
	{
		cout << name() << ": pipeline.Stalled() = " << pipeline.Stalled() << endl;
		cout << name() << ": Latency = " << latency << endl;
	}
#endif
}

void FloatingPointUnit::OnFallingEdge()
{
	if(inBranchFinished)
	{
		if(inMispredictedBranch)
		{
			pipeline.Flush();
			
			if(branchCounter > 0)
			{
				branchCounter = 0;
				latency = 0;
				dispatched = false;
				outResultValid = false;
				outResultCRValid = false;
				outFinished = false;
			}
			outFloatingPointPipelineEmpty = pipeline.Empty();
			outBusy = pipeline.Full() || latency > 0;
		}	
	
		pipeline.DecrementBranchCounter();
		if(branchCounter > 0) branchCounter--;
	}
}

void FloatingPointUnit::Reset()
{
	dispatched = false;
	latency = 0;
	branchCounter = 0;
	pipeline.Reset();
}
