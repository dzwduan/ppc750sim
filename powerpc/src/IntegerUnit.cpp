/***************************************************************************
                          IntegerUnit.cpp  -  unite de calcul entier
                             -------------------
    begin                : Fri Apr 6 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <IntegerUnit.h>

inline int IntegerUnit::GetMulLatency(UInt32 op1, UInt32 op2)
{
	op2 = (SInt32) op2 > 0 ? op2 : -op2;
	
	if(op2 & 0xff000000) return 6;
	if(op2 & 0x00ff0000) return 5;
	if(op2 & 0x0000ff00) return 4;
	if(op2 & 0x000000ff) return 3;
	return 2;
}

inline UInt32 IntegerUnit::RotateLeft(UInt32 v, UInt8 n)
{
	return (v << n) | ((v >> (32 - n)) & ((1 << n) - 1));
}

inline UInt32 IntegerUnit::Mask(UInt8 mb, UInt8 me)
{
    return(((mb > me) ?
            ~(((UInt32)-1 >> mb) ^ ((me >= 31) ? 0 : (UInt32) -1 >> (me + 1)
)):
            (((UInt32)-1 >> mb) ^ ((me >= 31) ? 0 : (UInt32) -1 >> (me + 1))))
);
}

bool IntegerUnit::Carry(UInt32 a, UInt32 b)
{
	UInt32 c, x, y;
	int i;

	if((a == 0) || (b == 0)) return 0;

	c = 0;
	for(i = 0; i < 32; i++)
	{
	    x = (a >> i) & 0x1;
		y = (b >> i) & 0x1;
		c = x * y | x * c | y * c;
	}
	return c;
}

const SInt32 MaxInt = 0x7fffffff;

bool IntegerUnit::Overflow(SInt32 a, SInt32 b)
{
	return ((a > 0) && (b > 0) && (MaxInt - a < b)) || ((a < 0) && (b < 0) && (-MaxInt - a > b));
}

bool IntegerUnit::Underflow(SInt32 a, SInt32 b)
{
	return ((a > 0) && (b < 0) && (MaxInt + b < a)) || ((a < 0) && (b > 0) && (-MaxInt + b > a));
}

void IntegerUnit::Execute()
{
	const IntegerOperation& operation = inOperation;
	const IntegerOperands& operands = inOperands;
	const IntegerTags& tags = inTags;
#if defined(DEBUG) || defined(TRACE)
	UInt32 dbgInstructionCounter = inInstructionCounter;
	UInt32 dbgProgramCounter = inProgramCounter;
	UInt32 dbgInstruction = inInstruction;
#endif
	
	if(inDispatched)
	{
		/* A new integer instruction has been started */
		branchCounter = inBranchCounter;
#ifdef DEBUG
		if(Debug(DebugIntegerUnit))
		{
			cout << name() << ": dispatched (" << dbgInstructionCounter << ")  ";
			ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
			cout << endl;
		}
#endif
#ifdef TRACE
		if(Trace(TraceIntegerUnit))
		{
			trace_file->Begin("execute");
			trace_file->Value("unit", "integer");
			trace_file->Value("number", dbgInstructionCounter);
			trace_file->Value("pc", dbgInstructionCounter);
			trace_file->End("execute");
		}
#endif
		dispatched = true;
	}
	
	if(dispatched)
	{
		/* An instruction is being executed */
		if(latency == 0)
		{
			/* The instruction is beginning its execution */
#ifdef DEBUG
			if(Debug(DebugIntegerUnit))
			{
				cout << name() << ": Starting execution (" << dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
				cout << endl;
				cout << name() << ": (RA/RS) = ";
				WriteHex(cout, operands.data[0]);
				cout << ", (RA/RB) = ";
				WriteHex(cout, operands.data[1]);
				cout << endl;
			}
#endif
			/* RA : operands.data[0] */
			/* RB, immed : operands.data[1] */
			UInt64 result64;
			ca = ov = false;
			
			switch(operation.ident)
			{		
				case ID_MULLI:
					result32 = operands.data[0] * operands.data[1];
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
				
				case ID_SUBFIC:
					result32 = operands.data[1] - operands.data[0];
					ca = operands.data[0] == 0 || Carry(-operands.data[0], operands.data[1]);
					latency = 1;
					break;
				
				case ID_ADDIC:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					latency = 1;
					break;
					
				case ID_CMPLI:
					lt = operands.data[0] < operands.data[1];
					gt = operands.data[0] > operands.data[1];
					eq = operands.data[0] == operands.data[1];
					latency = 1;
					break;
				
				case ID_CMPI:
					result32 = operands.data[0] - operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_ADDICD:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_ADDI:
					result32 = operation.nullRA ? operands.data[1] : operands.data[0] + operands.data[1];
					latency = 1;
					break;
			
				case ID_ADDIS:
					result32 = operation.nullRA ? (operands.data[1] << 16) : operands.data[0] + (operands.data[1] << 16);
					latency = 1;
					break;

				case ID_ORI:
					result32 = operands.data[0] | operands.data[1];
					latency = 1;
					break;
			
				case ID_ORIS:
					result32 = operands.data[0] | (operands.data[1] << 16);
					latency = 1;
					break;
					
				case ID_XORI:
					result32 = operands.data[0] ^ operands.data[1];
					latency = 1;
					break;
				
				case ID_XORIS:
					result32 = operands.data[0] ^ (operands.data[1] << 16);
					latency = 1;
					break;
				
				case ID_ANDID:
					result32 = operands.data[0] & operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_ANDISD:
					result32 = operands.data[0] & (operands.data[1] << 16);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_RLWIMI:
					{
						UInt32 r = RotateLeft(operands.data[0], operation.extra.rotate.sh);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = (r & m) | (operands.data[1] & (~m));
						latency = 1;
					}
					break;
			
				case ID_RLWIMID:
					{
						UInt32 r = RotateLeft(operands.data[0], operation.extra.rotate.sh);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = (r & m) | (operands.data[1] & (~m));
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
					
				case ID_RLWINM:
					{
						UInt32 r = RotateLeft(operands.data[0], operation.extra.rotate.sh);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = r & m;
						latency = 1;
					}
					break;
			
				case ID_RLWINMD:
					{
						UInt32 r = RotateLeft(operands.data[0], operation.extra.rotate.sh);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = r & m;
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
			
				case ID_RLWNM:
					{
						UInt32 r = RotateLeft(operands.data[0], operands.data[1] & 31);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = r & m;
						latency = 1;
					}
					break;
			
				case ID_RLWNMD:
					{
						UInt32 r = RotateLeft(operands.data[0], operands.data[1] & 31);
						UInt32 m = Mask(operation.extra.rotate.mb, operation.extra.rotate.me);
						result32 = r & m;
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
				
				case ID_CMP:
					result32 = operands.data[0] - operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_SUBFC:
					result32 = operands.data[1] - operands.data[0];
					ca = operands.data[0] == 0 || Carry(-operands.data[0], operands.data[1]);
					latency = 1;
					break;
			
				case ID_SUBFCD:
					result32 = operands.data[1] - operands.data[0];
					ca = operands.data[0] == 0 || Carry(-operands.data[0], operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
		
				case ID_SUBFCO:
					result32 = operands.data[1] - operands.data[0];
					ca = operands.data[0] == 0 || Carry(-operands.data[0], operands.data[1]);
					ov = Underflow(operands.data[1], operands.data[0]);
					latency = 1;
					break;
		
				case ID_SUBFCOD:
					result32 = operands.data[1] - operands.data[0];
					ca = operands.data[0] == 0 || Carry(-operands.data[0], operands.data[1]);
					ov = Underflow(operands.data[1], operands.data[0]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_CMPL:
					lt = operands.data[0] < operands.data[1];
					gt = operands.data[0] > operands.data[1];
					eq = operands.data[0] == operands.data[1];
					latency = 1;
					break;
			
				case ID_ADDC:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					latency = 1;
					break;
		
				case ID_ADDCD:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_ADDCO:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					ov = Overflow(operands.data[0], operands.data[1]);
					latency = 1;
					break;
			
				case ID_ADDCOD:
					result32 = operands.data[0] + operands.data[1];
					ca = Carry(operands.data[0], operands.data[1]);
					ov = Overflow(operands.data[0], operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_MULHWU:
					{
						UInt64 res = (UInt64) operands.data[0] * (UInt64) operands.data[1];
						res = (res >> 32);
						result32 = (UInt32) res;
						latency = GetMulLatency(operands.data[0], operands.data[1]);
					}
					break;
					
				case ID_MULHWUD:
					result32 = ((UInt64) operands.data[0] * (UInt64) operands.data[1]) >> 32;			
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
			
				case ID_SLW:
					result32 = (operands.data[1] & 32) ? 0 : (operands.data[0] << (operands.data[1] & 31));
					latency = 1;
					break;
				
				case ID_SLWD:
					result32 = (operands.data[1] & 32) ? 0 : (operands.data[0] << (operands.data[1] & 31));
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;

				case ID_SRW:
					result32 = (operands.data[1] & 32) ? 0 : (operands.data[0] >> (operands.data[1] & 31));
					latency = 1;
					break;							
			
				case ID_SRWD:
  					result32 = (operands.data[1] & 32) ? 0 : (operands.data[0] >> (operands.data[1] & 31));
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_SRAW:
					{
						SInt32 src = operands.data[0];
						UInt32 n = operands.data[1];
						
						if(n == 0)
						{
							result32 = src;
							ca = false;
						}
						else if(n & 0x20)
						{
							if(src < 0)
							{
								result32 = 0xffffffff;
								if(src & 0x7fffffff) ca = true; else ca = false;
							}
							else
							{
								result32 = 0;
								ca = false;
							}
						}
						else
						{
							n = n & 0x1f;
							result32 = (sword_t) src >> n;
							if(src < 0 && (src << (32 - n)) != 0) ca = true; else ca = false;
						}
					}
					
					latency = 1;
					break;							
		
				case ID_SRAWD:
					{
						SInt32 src = operands.data[0];
						UInt32 n = operands.data[1];
						
						if(n == 0)
						{
							result32 = src;
							ca = false;
						}
						else if(n & 0x20)
						{
							if(src < 0)
							{
								result32 = 0xffffffff;
								if(src & 0x7fffffff) ca = true; else ca = false;
							}
							else
							{
								result32 = 0;
								ca = false;
							}
						}
						else
						{
							n = n & 0x1f;
							result32 = (sword_t) src >> n;
							if(src < 0 && (src << (32 - n)) != 0) ca = true; else ca = false;
						}
						
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
						break;							
					}
		
				case ID_SRAWI:
					{
						int b = operation.extra.shift.sh & 31;
						
						result32 = (SInt32) operands.data[0] >> b;
						ca = (b != 0) && ((SInt32) result32 < 0) && ((operands.data[0] << (32 - operation.extra.shift.sh)) != 0);
						latency = 1;
					}
					break;
				
				case ID_SRAWID:
					{
						int b = operation.extra.shift.sh & 31;
						
						result32 = (SInt32) operands.data[0] >> b;
						ca = (b != 0) && ((SInt32) result32 < 0) && ((operands.data[0] << (32 - operation.extra.shift.sh)) != 0);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);						
						latency = 1;
					}
					break;

				case ID_CNTLZW:
					{
						UInt32 n = 0;
						UInt32 m = 1 << 31;
						while(n < 32)
						{
							if(operands.data[0] & m) break;
							m >>= 1;
							n++;
						}
						result32 = n;
					}
					latency = 1;
					break;
	
				case ID_CNTLZWD:
					{
						UInt32 n = 0;
						UInt32 m = 1 << 31;
						while(n < 32)
						{
							if(operands.data[0] & m) break;
							m >>= 1;
							n++;
						}
						result32 = n;
					}
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
					
				case ID_AND:
					result32 = operands.data[0] & operands.data[1];
					latency = 1;
					break;
					
				case ID_ANDD:
					result32 = operands.data[0] & operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_ANDC:
					result32 = operands.data[0] & ~(operands.data[1]);
					latency = 1;
					break;
			
				case ID_ANDCD:
					result32 = operands.data[0] & ~(operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
					
				case ID_MULHW:
					result32 = (UInt32)(((SInt64)(SInt32) operands.data[0] * (SInt64)(SInt32) operands.data[1]) >> 32);
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
		
				case ID_MULHWD:
					result64 = (UInt64)(((SInt64)(SInt32) operands.data[0] * (SInt64)(SInt32) operands.data[1]) >> 32);					
					lt = ((SInt32) result64 < 0);
					gt = ((SInt32) result64 > 0);
					eq = ((UInt32) result64 == 0);
					result32 = (UInt32) result64;
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
					
				case ID_NOR:
					result32 = ~(operands.data[0] | operands.data[1]);
					latency = 1;
					break;
		
				case ID_NORD:
					result32 = ~(operands.data[0] | operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_SUBF:
					result32 = operands.data[1] - operands.data[0];
					latency = 1;
					break;
			
				case ID_SUBFD:
					result32 = operands.data[1] - operands.data[0];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_SUBFO:
					result32 = operands.data[1] - operands.data[0];
					ov = Underflow(operands.data[1], operands.data[0]);
					latency = 1;
					break;
			
				case ID_SUBFOD:
					result32 = operands.data[1] - operands.data[0];
					ov = Underflow(operands.data[1], operands.data[0]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
					
				case ID_NEG:
					if(operands.data[0] != 0x80000000)
					{
						result32 = (~operands.data[0]) + 1;
					}
					else
					{
						result32 = 0x80000000;
					}
					latency = 1;
					break;
		
				case ID_NEGD:
					result32 = -operands.data[0];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
		
				case ID_NEGO:
					result32 = -operands.data[0];
					ov = operands.data[0] == 0x80000000;
					latency = 1;
					break;
			
				case ID_NEGOD:
					result32 = -operands.data[0];
					ov = operands.data[0] == 0x80000000;
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
					
				case ID_SUBFE:
					{
						UInt32 a = ~operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						latency = 1;
					}
					break;
			
				case ID_SUBFED:
					{
						UInt32 a = ~operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
			
				case ID_SUBFEO:
					{
						UInt32 a = ~operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						ov = Overflow(b, a) || Overflow(b + a, c);
						latency = 1;
					}
					break;
			
				case ID_SUBFEOD:
					{
						UInt32 a = ~operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						ov = Overflow(b, a) || Overflow(b + a, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;

				case ID_ADDE:
					{
						UInt32 a = operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						latency = 1;
					}
					break;
		
				case ID_ADDED:
					{
						UInt32 a = operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
			
				case ID_ADDEO:
					{
						UInt32 a = operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						ov = Overflow(a, b) || Overflow(a + b, c);
						latency = 1;
					}
					break;
			
				case ID_ADDEOD:
					{
						UInt32 a = operands.data[0];
						UInt32 b = operands.data[1];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + b + c;
						ca = Carry(a, b) || Carry(a + b, c);
						ov = Overflow(a, b) || Overflow(a + b, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
					
				case ID_SUBFZE:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						latency = 1;
					}
					break;
					
				case ID_SUBFZED:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;									
				
				case ID_SUBFZEO:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						ov = Overflow(a, c);
						latency = 1;
					}
					break;

				case ID_SUBFZEOD:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						ov = Overflow(a, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
						
				case ID_ADDZE:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						latency = 1;
					}
					break;
					
				case ID_ADDZED:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;									
				
				case ID_ADDZEO:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						ov = Overflow(a, c);
						latency = 1;
					}
					break;

				case ID_ADDZEOD:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c;
						ca = Carry(a, c);
						ov = Overflow(a, c);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
						
				case ID_SUBFME:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						latency = 1;
					}
					break;
					
				case ID_SUBFMED:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;									
					
				case ID_SUBFMEO:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						ov = Overflow(a, c - 1);
						latency = 1;
					}
					break;
	
				case ID_SUBFMEOD:
					{
						UInt32 a = ~operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						ov = Overflow(a, c - 1);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
													
				case ID_ADDME:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						latency = 1;
					}
					break;
					
				case ID_ADDMED:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;									
					
				case ID_ADDMEO:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						ov = Overflow(a, c - 1);
						latency = 1;
					}
					break;

				case ID_ADDMEOD:
					{
						UInt32 a = operands.data[0];
						UInt32 c = inCarry ? 1 : 0;
						result32 = a + c - 1;
						ca = Carry(a, c - 1);
						ov = Overflow(a, c - 1);
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 1;
					}
					break;
					
				case ID_MULLW:
					result32 = operands.data[0] * operands.data[1];
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
					
				case ID_MULLWD:
					result32 = operands.data[0] * operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
			
				case ID_MULLWO:
					result64 = operands.data[0] * operands.data[1];
					ov = (SInt64) result64 > 0x7fffffff || (SInt64) result64 < 0x80000000;
					result32 = (UInt32) result64;
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
					
				case ID_MULLWOD:
					result64 = operands.data[0] * operands.data[1];
					ov = (SInt64) result64 > 0x7fffffff || (SInt64) result64 < 0x80000000;
					lt = ((SInt32) result64 < 0);
					gt = ((SInt32) result64 > 0);
					eq = ((UInt32) result64 == 0);
					result32 = (UInt32) result64;
					latency = GetMulLatency(operands.data[0], operands.data[1]);
					break;
			
				case ID_ADD:
					result32 = operands.data[0] + operands.data[1];
					latency = 1;
					break;
					
				case ID_ADDD:
					result32 = operands.data[0] + operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;

				case ID_ADDO:
					result64 = operands.data[0] + operands.data[1];
					ca = (result64 > 0xffffffff);
					ov = ((ca != 0) != (((UInt32) result64 & 0x8000000) != 0));
					result32 = (UInt32) result64;
					latency = 1;
					break;					
					
				case ID_ADDOD:
					result64 = operands.data[0] + operands.data[1];
					ca = (result64 > 0xffffffff);
					ov = ((ca != 0) != (((UInt32) result64 & 0x8000000) != 0));
					lt = ((SInt32) result64 < 0);
					gt = ((SInt32) result64 > 0);
					eq = ((UInt32) result64 == 0);
					result32 = (UInt32) result64;
					latency = 1;
					break;
					
				case ID_EQV:
					result32 = ~(operands.data[0] ^ operands.data[1]);
					latency = 1;
					break;
			
				case ID_EQVD:
					result32 = ~(operands.data[0] ^ operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_XOR:
					result32 = operands.data[0] ^ operands.data[1];
					latency = 1;
					break;
		
				case ID_XORD:
					result32 = operands.data[0] ^ operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_ORC:
					result32 = operands.data[0] | (~operands.data[1]);
					latency = 1;
					break;
			
				case ID_ORCD:
					result32 = operands.data[0] | (~operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_OR:
					result32 = operands.data[0] | operands.data[1];
					latency = 1;
					break;
		
				case ID_ORD:
					result32 = operands.data[0] | operands.data[1];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
		
				case ID_DIVWU:
					if(operands.data[1] == 0)
					{
						result32 = 0;
						latency = 2;
					}
					else
					{
						result32 = operands.data[0] / operands.data[1];
						latency = 19;
					}
					break;
				
				case ID_DIVWUD:
					if(operands.data[1] == 0)
					{
						result32 = 0;
						latency = 2;
					}
					else
					{
						result32 = operands.data[0] / operands.data[1];
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 19;
					}
					break;
				
				case ID_DIVWUO:
					if(operands.data[1] == 0)
					{
						ov = true;
						result32 = 0;
						latency = 2;
					}
					else
					{
						result32 = operands.data[0] / operands.data[1];
						ov = false;
						latency = 19;
					}
					break;
				
				case ID_DIVWUOD:
					if(operands.data[1] == 0)
					{
						ov = true;
						result32 = 0;
						latency = 2;
					}
					else
					{
						result32 = operands.data[0] / operands.data[1];
						ov = false;
						lt = ((SInt32) result32 < 0);
						gt = ((SInt32) result32 > 0);
						eq = ((UInt32) result32 == 0);
						latency = 19;
					}
					break;
					
				case ID_NAND:
					result32 = ~(operands.data[0] & operands.data[1]);
					latency = 1;
					break;
					
				case ID_NANDD:
					result32 = ~(operands.data[0] & operands.data[1]);
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_EXTSB:
					result32 = (SInt32)(SInt8) operands.data[0];
					latency = 1;
					break;
				
				case ID_EXTSBD:
					result32 = (SInt32)(SInt8) operands.data[0];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
			
				case ID_EXTSH:
					result32 = (SInt32)(SInt16) operands.data[0];
					latency = 1;
					break;
				
				case ID_EXTSHD:
					result32 = (SInt32)(SInt16) operands.data[0];
					lt = ((SInt32) result32 < 0);
					gt = ((SInt32) result32 > 0);
					eq = ((UInt32) result32 == 0);
					latency = 1;
					break;
				
				case ID_DIVW:
					{
						SInt32 a = operands.data[0];
						SInt32 b = operands.data[1];
						
						if(b == 0)
						{
							result32 = 0;
							latency = 2;
						}
						else
						{
							if(a == 0x80000000 && b == 0xffffffff)
							{
							}
							else
							{
								result32 = a / b;
							}
							latency = 19;
						}
					}
					break;
			
				case ID_DIVWD:
					{
						SInt32 a = operands.data[0];
						SInt32 b = operands.data[1];
						
						if(b == 0)
						{
							result32 = 0;
							latency = 2;
						}
						else
						{
							if(a == 0x80000000 && b == 0xffffffff)
							{
							}
							else
							{
								result32 = a / b;
								lt = ((SInt32) result32 < 0);
								gt = ((SInt32) result32 > 0);
								eq = ((UInt32) result32 == 0);
							}
							latency = 19;
						}
					}
					break;
				
				case ID_DIVWO:
					{
						SInt32 a = operands.data[0];
						SInt32 b = operands.data[1];
						
						if(b == 0)
						{
							ov = true;
							result32 = 0;
							latency = 2;
						}
						else
						{
							if(a == 0x80000000 && b == 0xffffffff)
							{
								ov = true;
							}
							else
							{
								result32 = a / b;
								ov = false;
							}
							latency = 19;
						}
					}
					break;
		
				case ID_DIVWOD:
					{
						SInt32 a = operands.data[0];
						SInt32 b = operands.data[1];
						
						if(b == 0)
						{
							ov = true;
							result32 = 0;
							latency = 2;
						}
						else
						{
							if(a == 0x80000000 && b == 0xffffffff)
							{
								ov = true;
							}
							else
							{
								result32 = a / b;
								ov = false;
								lt = ((SInt32) result32 < 0);
								gt = ((SInt32) result32 > 0);
								eq = ((UInt32) result32 == 0);
							}
							latency = 19;
						}
					}
					break;
					
				default:
#ifdef DEBUG
					if(Debug(DebugIntegerUnit))
					{
						cout << name() << ": Simulator does not support this instruction (tag = " << (int) tags.tag << ", op = " << operation.ident << ")   (" << dbgInstructionCounter << ")  ";
						ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
						cout << endl;
					}
#endif
					ABORT();
			} /* switch */
				
#ifdef DEBUG
			if(Debug(DebugIntegerUnit))
				if(latency <= 0)
				{
					cout << name() << "Latency is " << latency << endl;
					ABORT();
				}
#endif
		} /* if(latency == 0) */
			
		if(--latency == 0)
		{
			/* the instruction has finished its execution */
			if(tags.resultTag >= 0)
			{
#ifdef DEBUG
				if(Debug(DebugIntegerUnit))
				{
					cout << name() << ": producing rr" << (int) tags.resultTag << "(";
					WriteHex(cout, result32);
					cout << ")" << endl;
				}
#endif
				outResultValid = true;
				outResultTag = tags.resultTag;
				outResultData = result32;
			}
			else
			{
				outResultValid = false;				
			}

			if(tags.CRTag >= 0)
			{
#ifdef DEBUG
				if(Debug(DebugIntegerUnit))
				{
					cout << name() << ": producing CR rename buffer #" << (int) tags.CRTag << "(";
					WriteHex(cout, (UInt8)((lt ? 8:0) | (gt ? 4:0) | (eq ? 2:0)));
					cout << ")" << endl;
				}
#endif
				outResultCRValid = true;
				outResultCRTag = tags.CRTag;
		
				outResultCRData = (lt ? 8:0) | (gt ? 4:0) | (eq ? 2:0);
			}
			else
			{
				outResultCRValid = false;
			}
		
			outCarry = ca;
			outOverflow = ov;

			outBusy = false;
			outTag = tags.tag;
			outFinished = true;
			dispatched = false;
#ifdef DEBUG
			if(Debug(DebugIntegerUnit))
			{
				cout << name() << ": Finishing an instruction (tag = " << (int) tags.tag << ")   (" << dbgInstructionCounter << ")  ";
				ppc_disassemble_to_stream(dbgInstruction, dbgProgramCounter, stdout);
				cout << endl;
			}
#endif
			return;
		}
		outResultValid = false;
		outResultCRValid = false;
		outFinished = false;
		outBusy = true;
		return;
	}
	outResultValid = false;
	outResultCRValid = false;
	outFinished = false;
	outBusy = false;
}

void IntegerUnit::OnFallingEdge()
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
			outFinished = false;
			outBusy = false;
		}
	
		if(branchCounter > 0) branchCounter--;
	}
}

void IntegerUnit::Reset()
{
	branchCounter = 0;
	latency = 0;
	dispatched = false;
}
