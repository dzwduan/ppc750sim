/***************************************************************************
                          integer.c  -  description
                             -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <integer.h>
#include <ppcemul.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xmlize.h>

UInt32 ROTL(UInt32 v, UInt8 n)
{
	return (v << n) | ((v >> (32 - n)) & ((1 << n) - 1));
}

UInt32 MASK(UInt8 mb, UInt8 me)
{
    return(((mb > me) ?
            ~(((UInt32)-1 >> mb) ^ ((me >= 31) ? 0 : (UInt32) -1 >> (me + 1)
)):
            (((UInt32)-1 >> mb) ^ ((me >= 31) ? 0 : (UInt32) -1 >> (me + 1))))
);
}

#define COMPUTE_CR0(r) (CR = (CR & 0xfffffff) | (XER_SO ? 0x10000000 : 0) | (((sword_t)(r) < 0) ? 0x80000000 : (((sword_t)(r) > 0) ? 0x40000000 : 0x20000000)))

BOOL Carry(UInt32 a, UInt32 b)
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

#define MAX_INT 0x7fffffff

BOOL Overflow(SInt32 a, SInt32 b)
{
	return ((a > 0) && (b > 0) && (MAX_INT - a < b)) || ((a < 0) && (b < 0) && (-MAX_INT - a > b));
}

BOOL Underflow(SInt32 a, SInt32 b)
{
	return ((a > 0) && (b < 0) && (MAX_INT + b < a)) || ((a < 0) && (b > 0) && (-MAX_INT + b > a));
}


void addi_impl(ppc_inst_t inst)
{
	GPR(RD) = RA ? GPR(RA) + SEXT16(IMM) : SEXT16(IMM);
}

void addic_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = SEXT16(IMM);
	GPR(RD) = a + b;
	if(Carry(a, b))
		SET_XER_CA;
	else
		RESET_XER_CA;
}

void addicd_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = SEXT16(IMM);
	word_t result = a + b;
	GPR(RD) = result;
	if(Carry(a, b))
		SET_XER_CA;
	else
		RESET_XER_CA;
	COMPUTE_CR0(result);
}	


void addis_impl(ppc_inst_t inst)
{
	GPR(RD) = RA ? GPR(RA) + (SEXT16(IMM) << 16) : SEXT16(IMM) << 16;
}


void andid_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) & IMM;
	GPR(RA) = result;
	if((sword_t) result < 0) SET_CR0_LT; else RESET_CR0_LT;
	if((sword_t) result > 0) SET_CR0_GT; else RESET_CR0_GT;
	if(result == 0) SET_CR0_EQ; else RESET_CR0_EQ;
	if(XER_SO) SET_CR0_SO; else RESET_CR0_SO;
}

void andisd_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) & (IMM << 16);
	GPR(RA) = result;
	if((sword_t) result < 0) SET_CR0_LT; else RESET_CR0_LT;
	if((sword_t) result > 0) SET_CR0_GT; else RESET_CR0_GT;
	if(result == 0) SET_CR0_EQ; else RESET_CR0_EQ;
	if(XER_SO) SET_CR0_SO; else RESET_CR0_SO;
}


void cmpi_impl(ppc_inst_t inst)
{
	word_t result = GPR(RA) - SEXT16(IMM);
	if((sword_t) result < 0) SET_CR_LT(CRFD); else RESET_CR_LT(CRFD);
	if((sword_t) result > 0) SET_CR_GT(CRFD); else RESET_CR_GT(CRFD);
	if(result == 0) SET_CR_EQ(CRFD); else RESET_CR_EQ(CRFD);
	if(XER_SO) SET_CR_SO(CRFD); else RESET_CR_SO(CRFD);
}


void cmpli_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = IMM;
	if(a < b) SET_CR_LT(CRFD); else RESET_CR_LT(CRFD);
	if(a > b) SET_CR_GT(CRFD); else RESET_CR_GT(CRFD);
	if(a == b) SET_CR_EQ(CRFD); else RESET_CR_EQ(CRFD);
	if(XER_SO) SET_CR_SO(CRFD); else RESET_CR_SO(CRFD);
}

void mulli_impl(ppc_inst_t inst)
{
	GPR(RD) = GPR(RA) * SEXT16(IMM);
}
	

void ori_impl(ppc_inst_t inst)
{
	GPR(RA) = GPR(RS) | IMM;
}

void oris_impl(ppc_inst_t inst)
{
	GPR(RA) = GPR(RS) | (IMM << 16);
}


void subfic_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = SEXT16(IMM);
	GPR(RD) = b - a;
	if(a == 0 || Carry(-a, b)) SET_XER_CA; else RESET_XER_CA;
}

void twi_impl(ppc_inst_t inst) { abort(); }


void xori_impl(ppc_inst_t inst)
{
	GPR(RA) = GPR(RS) ^ IMM;
}

void xoris_impl(ppc_inst_t inst)
{
	GPR(RA) = GPR(RS) ^ (IMM << 16);
}


void rlwimix_impl(ppc_inst_t inst)
{
	word_t r = ROTL(GPR(RS), SH);
	word_t m = MASK(MB, ME);
	word_t result = (r & m) | (GPR(RA) & (~m));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void rlwinmx_impl(ppc_inst_t inst)
{
	word_t r = ROTL(GPR(RS), SH);
	word_t m = MASK(MB, ME);
	word_t result = r & m;
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void rlwnmx_impl(ppc_inst_t inst)
{
	word_t r = ROTL(GPR(RS), GPR(RB) & 0x1f);
	word_t m = MASK(MB, ME);
	word_t result = r & m;
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void andx_impl(ppc_inst_t inst)
{
	UInt32 result = GPR(RS) & GPR(RB);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void andcx_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) & (~GPR(RB));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void cmp_impl(ppc_inst_t inst)
{
	word_t result = GPR(RA) - GPR(RB);
	if((sword_t) result < 0) SET_CR_LT(CRFD); else RESET_CR_LT(CRFD);
	if((sword_t) result > 0) SET_CR_GT(CRFD); else RESET_CR_GT(CRFD);
	if(result == 0) SET_CR_EQ(CRFD); else RESET_CR_EQ(CRFD);
	if(XER_SO) SET_CR_SO(CRFD); else RESET_CR_SO(CRFD);
}

void cmpl_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	if(a < b) SET_CR_LT(CRFD); else RESET_CR_LT(CRFD);
	if(a > b) SET_CR_GT(CRFD); else RESET_CR_GT(CRFD);
	if(a == b) SET_CR_EQ(CRFD); else RESET_CR_EQ(CRFD);
	if(XER_SO) SET_CR_SO(CRFD); else RESET_CR_SO(CRFD);
}

void cntlzwx_impl(ppc_inst_t inst)
{
	word_t n = 0;
	word_t m = 1 << 31;
	while(n < 32)
	{
		if(GPR(RS) & m) break;
		m >>= 1;
		n++;
	}
	GPR(RA) = n;
	if(Rc) COMPUTE_CR0(n);
}

void eqvx_impl(ppc_inst_t inst)
{
	word_t result = ~(GPR(RS) ^ GPR(RB));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void extsbx_impl(ppc_inst_t inst)
{
	word_t result = (word_t)(sword_t)(sbyte_t)(byte_t) GPR(RS);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void extshx_impl(ppc_inst_t inst)
{
	word_t result = (word_t)(sword_t)(shalfword_t)(halfword_t) GPR(RS);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void nandx_impl(ppc_inst_t inst)
{
	word_t result = ~(GPR(RS) & GPR(RB));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void norx_impl(ppc_inst_t inst)
{
	word_t result = ~(GPR(RS) | GPR(RB));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void orx_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) | GPR(RB);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void orcx_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) | (~GPR(RB));
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void slwx_impl(ppc_inst_t inst)
{
	word_t n = GPR(RB);
	word_t result;
	
	if(n & 0x20)
		result = 0;
	else
		result = GPR(RS) << (n & 0x1f);
		
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void srawx_impl(ppc_inst_t inst)
{
	word_t n = GPR(RB);
	sword_t result;
	sword_t src = GPR(RS);

	if(n == 0)
	{
		result = src;
		RESET_XER_CA;
	}
	else if(n & 0x20)
	{
		if(src < 0)
		{
			result = 0xffffffff;
			if(src & 0x7fffffff) SET_XER_CA; else RESET_XER_CA;
		}
		else
		{
			result = 0;
			RESET_XER_CA;
		}
	}
	else
	{
		n = n & 0x1f;
		result = (sword_t) src >> n;
		if(src < 0 && (src << (32 - n)) != 0) SET_XER_CA; else RESET_XER_CA;
	}
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}


void srawix_impl(ppc_inst_t inst)
{
	word_t n = SH;
	sword_t result;
	sword_t src = GPR(RS);

	if(n == 0)
	{
		result = src;
		RESET_XER_CA;
	}
	else
	{
		result = src >> n;
		if(src < 0 && (src << (32 - n)) != 0) SET_XER_CA; else RESET_XER_CA;
	}
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void srwx_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) >> (GPR(RB) & 0x1f);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void tw_impl(ppc_inst_t inst) { abort(); }

void xorx_impl(ppc_inst_t inst)
{
	word_t result = GPR(RS) ^ GPR(RB);
	GPR(RA) = result;
	if(Rc) COMPUTE_CR0(result);
}

void addx_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	word_t result = a + b;
	GPR(RD) = result;
	if(OE)
	{
		if(Overflow(a, b))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else RESET_XER_OV;
	}
	if(Rc) COMPUTE_CR0(result);
}

void addcx_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	word_t result = a + b;
	GPR(RD) = result;
	if(Carry(a, b)) SET_XER_CA; else RESET_XER_CA;
	if(OE)
	{
		if(Overflow(a, b))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else RESET_XER_OV;
	}
	if(Rc) COMPUTE_CR0(result);
}

void addex_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	word_t c = XER_CA;
	word_t result = a + b + c;
	
	GPR(RD) = result;
		
	if(Carry(a, b) || (c != 0 && Carry(a + b, c))) SET_XER_CA; else RESET_XER_CA;
	
	if(OE)
	{
		if(Overflow(a, b) || Overflow(a + b, c))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
	if(Rc) COMPUTE_CR0(result);
}

void addmex_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t c = XER_CA;
	word_t result = a + c - 1;
	
	GPR(RD) = result;
	
	if(Carry(a, c - 1)) SET_XER_CA; else RESET_XER_CA;
	
	if(OE)
	{
		if(Overflow(a, c - 1))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
	if(Rc) COMPUTE_CR0(result);
}

void addzex_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t c = XER_CA;
	word_t result = a + c;
	
	GPR(RD) = result;
	
	if(Carry(a, c)) SET_XER_CA; else RESET_XER_CA;
	
	if(OE)
	{
		if(Overflow(a, c))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
	if(Rc) COMPUTE_CR0(result);
}

void divwx_impl(ppc_inst_t inst)
{
	sword_t a = GPR(RA);
	sword_t b = GPR(RB);
	
	if(b == 0)
	{
		/* divide by 0 */
		if(OE)
		{
			SET_XER_OV;
			SET_XER_SO;
		}
	}
	else
	{
		if(a == 0x80000000 && b == 0xffffffff)
		{
			if(OE)
			{
				SET_XER_OV;
				SET_XER_SO;
			}
		}
		else
		{
			sword_t result = a / b;
			
			GPR(RD) = result;
			
			if(OE) RESET_XER_OV;
			
			if(Rc) COMPUTE_CR0(result);
			
		}
	}
}

void divwux_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	
	if(b == 0)
	{
		/* divide by 0 */
		if(OE)
		{
			SET_XER_OV;
			SET_XER_SO;
		}
	}
	else
	{
		word_t result = a / b;
			
		GPR(RD) = result;
			
		if(OE) RESET_XER_OV;
			
		if(Rc) COMPUTE_CR0(result);
	}
}

void mulhwx_impl(ppc_inst_t inst)
{
	sdword_t a = (sword_t) GPR(RA);
	sdword_t b = (sword_t) GPR(RB);
	sdword_t result = a * b;
	result = (result >> 32);
	GPR(RD) = (sword_t) result;
	if(Rc) COMPUTE_CR0(result);
}

void mulhwux_impl(ppc_inst_t inst)
{
	dword_t a = GPR(RA);
	dword_t b = GPR(RB);
	dword_t result = a * b;
	result = (result >> 32);
	GPR(RD) = (word_t) result;
	if(Rc) COMPUTE_CR0(result);
}

void mullwx_impl(ppc_inst_t inst)
{
	sword_t a = GPR(RA);
	sword_t b = GPR(RB);
	sdword_t result = a * b;
	GPR(RD) = (sword_t)(result & 0x00000000ffffffff);
	if(OE)
	{
		if(result & 0xffffffff00000000)
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_SO;
		}
	}
	if(Rc) COMPUTE_CR0(result);
}

void negx_impl(ppc_inst_t inst)
{
    word_t result = -GPR(RA);
    GPR(RD) = result;
    if(OE)
    {
    	if(GPR(RA) == 0x80000000)
    	{
    		SET_XER_OV;
    		SET_XER_SO;
    	}
    	else
    	{
    		RESET_XER_OV;
    	}
    }

	if(Rc) COMPUTE_CR0(result);
}

void subfx_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	word_t result = b - a;
	
	GPR(RD) = result;
		
	if(OE)
	{
		if(Underflow(b, a))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
		
	if(Rc) COMPUTE_CR0(result);
}

void subfcx_impl(ppc_inst_t inst)
{
	word_t a = GPR(RA);
	word_t b = GPR(RB);
	word_t result = b - a;
	
	GPR(RD) = result;
	
	if(a == 0 || Carry(b, -a)) SET_XER_CA; else RESET_XER_CA;
		
	if(OE)
	{
		if(Underflow(b, a))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
		
	if(Rc) COMPUTE_CR0(result);
}

void subfex_impl(ppc_inst_t inst)
{
	word_t a = ~GPR(RA);
	word_t b = GPR(RB);
	word_t c = XER_CA;
	word_t result = a + b + c;
	
	GPR(RD) = result;
	
	if(Carry(a, b) || Carry(a + b, c)) SET_XER_CA; else RESET_XER_CA;
		
	if(OE)
	{
		if(Overflow(b, a) || Overflow(b + a, c))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
		
	if(Rc) COMPUTE_CR0(result);
}

void subfmex_impl(ppc_inst_t inst)
{
	word_t a = ~GPR(RA);
	word_t c = XER_CA;
	word_t result = a + c - 1;
	
	GPR(RD) = result;
	
	if(Carry(a, c - 1)) SET_XER_CA; else RESET_XER_CA;
		
	if(OE)
	{
		if(Overflow(a, c - 1))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
		
	if(Rc) COMPUTE_CR0(result);
}

void subfzex_impl(ppc_inst_t inst)
{
	word_t a = ~GPR(RA);
	word_t c = XER_CA;
	word_t result = a + c;
	
	GPR(RD) = result;
	
	if(Carry(a, c)) SET_XER_CA; else RESET_XER_CA;
		
	if(OE)
	{
		if(Overflow(a, c))
		{
			SET_XER_OV;
			SET_XER_SO;
		}
		else
		{
			RESET_XER_OV;
		}
	}
		
	if(Rc) COMPUTE_CR0(result);
}

















char *addi_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\tGPR(%u) = GPR(%u) + (%d);\n", RD, RA, SEXT16(IMM));
	else
		buf += sprintf(buf, "\tGPR(%u) = %d;\n", RD, SEXT16(IMM));
	return buf;
}


char *addic_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = %d;\n", SEXT16(IMM));
	buf += sprintf(buf, "\tGPR(%u) = a + b;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, b))\n");
	buf += sprintf(buf, "\t\tSET_XER_CA;\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t\tRESET_XER_CA;\n");
	return buf;
}

char *addicd_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = %d;\n", SEXT16(IMM));
	buf += sprintf(buf, "\tword_t result = a + b;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, b))\n");
	buf += sprintf(buf, "\t\tSET_XER_CA;\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t\tRESET_XER_CA;\n");
	buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}	


char *addis_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\tGPR(%u) = GPR(%u) + (%d);\n", RD, RA, (SEXT16(IMM) << 16));
	else
		buf += sprintf(buf, "\tGPR(%u) = %d;\n", RD, (SEXT16(IMM) << 16));
	return buf;
}


char *andid_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) & %u;\n", RS, IMM);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	buf += sprintf(buf, "\tif((sword_t) result < 0) SET_CR0_LT; else RESET_CR0_LT;\n");
	buf += sprintf(buf, "\tif((sword_t) result > 0) SET_CR0_GT; else RESET_CR0_GT;\n");
	buf += sprintf(buf, "\tif(result == 0) SET_CR0_EQ; else RESET_CR0_EQ;\n");
	buf += sprintf(buf, "\tif(XER_SO) SET_CR0_SO; else RESET_CR0_SO;\n");
	return buf;
}

char *andisd_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) & %u;\n", RS, (IMM << 16));
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	buf += sprintf(buf, "\tif((sword_t) result < 0) SET_CR0_LT; else RESET_CR0_LT;\n");
	buf += sprintf(buf, "\tif((sword_t) result > 0) SET_CR0_GT; else RESET_CR0_GT;\n");
	buf += sprintf(buf, "\tif(result == 0) SET_CR0_EQ; else RESET_CR0_EQ;\n");
	buf += sprintf(buf, "\tif(XER_SO) SET_CR0_SO; else RESET_CR0_SO;\n");
	return buf;
}


char *cmpi_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) - (%d);\n", RA, SEXT16(IMM));
	buf += sprintf(buf, "\tif((sword_t) result < 0) SET_CR_LT(%u); else RESET_CR_LT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif((sword_t) result > 0) SET_CR_GT(%u); else RESET_CR_GT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(result == 0) SET_CR_EQ(%u); else RESET_CR_EQ(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(XER_SO) SET_CR_SO(%u); else RESET_CR_SO(%u);\n", CRFD, CRFD);
	return buf;
}


char *cmpli_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = %u;\n", IMM);
	buf += sprintf(buf, "\tif(a < b) SET_CR_LT(%u); else RESET_CR_LT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(a > b) SET_CR_GT(%u); else RESET_CR_GT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(a == b) SET_CR_EQ(%u); else RESET_CR_EQ(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(XER_SO) SET_CR_SO(%u); else RESET_CR_SO(%u);\n", CRFD, CRFD);
	return buf;
}

char *mulli_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tGPR(%u) = GPR(%u) * (%d);\n", RD, RA, SEXT16(IMM));
	return buf;
}
	

char *ori_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tGPR(%u) = GPR(%u) | %u;\n", RA, RS, IMM);
	return buf;
}

char *oris_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tGPR(%u) = GPR(%u) | %u;\n", RA, RS, (IMM << 16));
	return buf;
}


char *subfic_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = %d;\n", SEXT16(IMM));
	buf += sprintf(buf, "\tGPR(%u) = b - a;\n", RD);
	buf += sprintf(buf, "\tif(a == 0 || Carry(-a, b)) SET_XER_CA; else RESET_XER_CA;\n");
	return buf;
}

char *twi_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tabort();\n");
	return buf;
}


char *xori_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tGPR(%u) = GPR(%u) ^ %u;\n", RA, RS, IMM);
	return buf;
}

char *xoris_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tGPR(%u) = GPR(%u) ^ %u;\n", RA, RS, (IMM << 16));
	return buf;
}


char *rlwimix_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t r = ROTL(GPR(%u), %u);\n", RS, SH);
	buf += sprintf(buf, "\tword_t m = MASK(%u, %u);\n", MB, ME);
	buf += sprintf(buf, "\tword_t result = (r & m) | (GPR(%u) & (~m));\n", RA);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *rlwinmx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t r = ROTL(GPR(%u), %u);\n", RS, SH);
	buf += sprintf(buf, "\tword_t m = MASK(%u, %u);\n", MB, ME);
	buf += sprintf(buf, "\tword_t result = r & m;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *rlwnmx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t r = ROTL(GPR(%u), GPR(%u) & 0x1f);\n", RS, RB);
	buf += sprintf(buf, "\tword_t m = MASK(%u, %u);\n", MB, ME);
	buf += sprintf(buf, "\tword_t result = r & m;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *andx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tUInt32 result = GPR(%u) & GPR(%u);\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *andcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) & (~GPR(%u));\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *cmp_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) - GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tif((sword_t) result < 0) SET_CR_LT(%u); else RESET_CR_LT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif((sword_t) result > 0) SET_CR_GT(%u); else RESET_CR_GT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(result == 0) SET_CR_EQ(%u); else RESET_CR_EQ(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(XER_SO) SET_CR_SO(%u); else RESET_CR_SO(%u);\n", CRFD, CRFD);
	return buf;
}

char *cmpl_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tif(a < b) SET_CR_LT(%u); else RESET_CR_LT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(a > b) SET_CR_GT(%u); else RESET_CR_GT(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(a == b) SET_CR_EQ(%u); else RESET_CR_EQ(%u);\n", CRFD, CRFD);
	buf += sprintf(buf, "\tif(XER_SO) SET_CR_SO(%u); else RESET_CR_SO(%u);\n", CRFD, CRFD);
	return buf;
}

char *cntlzwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t n = 0;\n");
	buf += sprintf(buf, "\tword_t m = 1 << 31;\n");
	buf += sprintf(buf, "\twhile(n < 32)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tif(GPR(%u) & m) break;\n", RS);
	buf += sprintf(buf, "\t\tm >>= 1;\n");
	buf += sprintf(buf, "\t\tn++;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\tGPR(%u) = n;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(n);\n");
	return buf;
}

char *eqvx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = ~(GPR(%u) ^ GPR(%u));\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *extsbx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = (word_t)(sword_t)(sbyte_t)(byte_t) GPR(%u);\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *extshx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = (word_t)(sword_t)(shalfword_t)(halfword_t) GPR(%u);\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *nandx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = ~(GPR(%u) & GPR(%u));\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *norx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = ~(GPR(%u) | GPR(%u));\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *orx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) | GPR(%u);\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *orcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) | (~GPR(%u));\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *slwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t n = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result;\n");
	buf += sprintf(buf, "\tif(n & 0x20)\n");
	buf += sprintf(buf, "\t\tresult = 0;\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\tresult = GPR(%u) << (n & 0x1f);\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *srawx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t n = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tsword_t result;\n");
	buf += sprintf(buf, "\tsword_t src = GPR(%u);\n", RS);
	buf += sprintf(buf, "\tif(n == 0)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tresult = src;\n");
	buf += sprintf(buf, "\t\tRESET_XER_CA;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse if(n & 0x20)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tif(src < 0)\n");
	buf += sprintf(buf, "\t\t{\n");
	buf += sprintf(buf, "\t\t\tresult = 0xffffffff;\n");
	buf += sprintf(buf, "\t\t\tif(src & 0x7fffffff) SET_XER_CA; else RESET_XER_CA;\n");
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t\telse\n");
	buf += sprintf(buf, "\t\t{\n");
	buf += sprintf(buf, "\t\t\tresult = 0;\n");
	buf += sprintf(buf, "\t\t\tRESET_XER_CA;\n");
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tn = n & 0x1f;\n");
	buf += sprintf(buf, "\t\tresult = (sword_t) src >> n;\n");
	buf += sprintf(buf, "\t\tif(src < 0 && (src << (32 - n)) != 0) SET_XER_CA; else RESET_XER_CA;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}


char *srawix_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t n = %u;\n", SH);
	buf += sprintf(buf, "\tsword_t result;\n");
	buf += sprintf(buf, "\tsword_t src = GPR(%u);\n", RS);
	buf += sprintf(buf, "\tif(n == 0)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tresult = src;\n");
	buf += sprintf(buf, "\t\tRESET_XER_CA;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tresult = src >> n;\n");
	buf += sprintf(buf, "\t\tif(src < 0 && (src << (32 - n)) != 0) SET_XER_CA; else RESET_XER_CA;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *srwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) >> (GPR(%u) & 0x1f);\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *tw_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tabort();\n");
	return buf;
	
}

char *xorx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = GPR(%u) ^ GPR(%u);\n", RS, RB);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RA);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *addx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = a + b;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, b))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse RESET_XER_OV;\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *addcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = a + b;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, b)) SET_XER_CA; else RESET_XER_CA;\n");
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, b))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse RESET_XER_OV;\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *addex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + b + c;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, b) || (c != 0 && Carry(a + b, c))) SET_XER_CA; else RESET_XER_CA;\n");
	
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, b) || Overflow(a + b, c))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *addmex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + c - 1;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, c - 1)) SET_XER_CA; else RESET_XER_CA;\n");
	
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, c - 1))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *addzex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + c;\n");
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	buf += sprintf(buf, "\tif(Carry(a, c)) SET_XER_CA; else RESET_XER_CA;\n");
	
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, c))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *divwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tsword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tsword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tif(b == 0)\n");
	buf += sprintf(buf, "\t{\n");
	if(OE)
	{
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
	}
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tif(a == 0x80000000 && b == 0xffffffff)\n");
	buf += sprintf(buf, "\t\t{\n");
	if(OE)
	{
		buf += sprintf(buf, "\t\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\t\tSET_XER_SO;\n");
	}
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t\telse\n");
	buf += sprintf(buf, "\t\t{\n");
	buf += sprintf(buf, "\t\t\tsword_t result = a / b;\n");
	buf += sprintf(buf, "\t\t\tGPR(%u) = result;\n", RD);
	if(OE) buf += sprintf(buf, "\t\t\tRESET_XER_OV;\n");
	if(Rc) buf += sprintf(buf, "\t\t\tCOMPUTE_CR0(result);\n");
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t}\n");	
	return buf;
}

char *divwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tif(b == 0)\n");
	buf += sprintf(buf, "\t{\n");
	if(OE)
	{
		buf += sprintf(buf, "\tSET_XER_OV;\n");
		buf += sprintf(buf, "\tSET_XER_SO;\n");
	}
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tword_t result = a / b;\n");
	buf += sprintf(buf, "\t\tGPR(%u) = result;\n", RD);
	if(OE) buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
	if(Rc) buf += sprintf(buf, "\t\tCOMPUTE_CR0(result);\n");
	buf += sprintf(buf, "\t}\n");
	return buf;
}

char *mulhwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tsdword_t a = (sword_t) GPR(%u);\n", RA);
	buf += sprintf(buf, "\tsdword_t b = (sword_t) GPR(%u);\n", RB);
	buf += sprintf(buf, "\tsdword_t result = a * b;\n");
	buf += sprintf(buf, "\tresult = (result >> 32);\n");
	buf += sprintf(buf, "\tGPR(%u) = (sword_t) result;\n", RD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *mulhwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tdword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tdword_t result = a * b;\n");
	buf += sprintf(buf, "\tresult = (result >> 32);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *mullwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tsword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tsword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tsdword_t result = a * b;\n");
	buf += sprintf(buf, "\tGPR(%u) = (sword_t)(result & 0x00000000ffffffff);\n", RD);
	if(OE)
	{
		buf += sprintf(buf, "\tif(result & 0xffffffff00000000)\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
	}
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *negx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t result = -GPR(%u);\n", RA);
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	if(OE)
	{
		buf += sprintf(buf, "\tif(GPR(%u) == 0x80000000)\n", RA);
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
	
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *subfx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = b - a;\n");
	
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
		
	if(OE)
	{
		buf += sprintf(buf, "\tif(Underflow(b, a))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
		
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *subfcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = b - a;\n");
	
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	
	buf += sprintf(buf, "\tif(a == 0 || Carry(b, -a)) SET_XER_CA; else RESET_XER_CA;\n");
		
	if(OE)
	{
		buf += sprintf(buf, "\tif(Underflow(b, a))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
		
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *subfex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = ~GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t b = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + b + c;\n");
	
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	
	buf += sprintf(buf, "\tif(Carry(a, b) || Carry(a + b, c)) SET_XER_CA; else RESET_XER_CA;\n");
		
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(b, a) || Overflow(b + a, c))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
		
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *subfmex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = ~GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + c - 1;\n");
	
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	
	buf += sprintf(buf, "\tif(Carry(a, c - 1)) SET_XER_CA; else RESET_XER_CA;\n");
		
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, c - 1))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
		
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}

char *subfzex_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t a = ~GPR(%u);\n", RA);
	buf += sprintf(buf, "\tword_t c = XER_CA;\n");
	buf += sprintf(buf, "\tword_t result = a + c;\n");
	
	buf += sprintf(buf, "\tGPR(%u) = result;\n", RD);
	
	buf += sprintf(buf, "\tif(Carry(a, c)) SET_XER_CA; else RESET_XER_CA;\n");
		
	if(OE)
	{
		buf += sprintf(buf, "\tif(Overflow(a, c))\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tSET_XER_OV;\n");
		buf += sprintf(buf, "\t\tSET_XER_SO;\n");
		buf += sprintf(buf, "\t}\n");
		buf += sprintf(buf, "\telse\n");
		buf += sprintf(buf, "\t{\n");
		buf += sprintf(buf, "\t\tRESET_XER_OV;\n");
		buf += sprintf(buf, "\t}\n");
	}
		
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR0(result);\n");
	return buf;
}













char *addi_disasm(ppc_inst_t inst, addr_t pc, char *buf) {	buf += sprintf(buf, "addi r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *addic_disasm(ppc_inst_t inst, addr_t pc, char *buf) {	buf += sprintf(buf, "addic r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *addicd_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "addic. r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *addis_disasm(ppc_inst_t inst, addr_t pc, char *buf) {	 buf += sprintf(buf, "addis r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *andid_disasm(ppc_inst_t inst, addr_t pc, char *buf) {	 buf += sprintf(buf, "andi. r%u, r%u, %u", RA, RS, IMM); return buf; }
char *andisd_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "andis. r%u, r%u, %u", RA, RS, IMM); return buf; }
char *cmpi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "cmpi cr%u, %u, r%u, %d", CRFD, L, RA, SEXT16(IMM)); return buf; }
char *cmpli_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "cmpli cr%u, %u, r%u, %u", CRFD, L, RA, IMM); return buf; }
char *mulli_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mulli r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *ori_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "ori r%u, r%u, %d", RA, RS, SEXT16(IMM)); return buf; }
char *oris_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "oris r%u, r%u, %u", RA, RS, IMM); return buf; }
char *subfic_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "subfic r%u, r%u, %d", RD, RA, SEXT16(IMM)); return buf; }
char *twi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "twi %u, r%u, %d", TO, RA, SEXT16(IMM)); return buf; }
char *xori_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "xori r%u, r%u, %u", RA, RS, IMM); return buf; }
char *xoris_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "xoris r%u, r%u, %u", RA, RS, IMM); return buf; }

char *rlwimix_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "rlwimi");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, %u, %u, %u", RA, RS, SH, MB, ME);
	return buf;
}

char *rlwinmx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "rlwinm");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, %u, %u, %u", RA, RS, SH, MB, ME);
	return buf;
}

char *rlwnmx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "rlwnm");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u, %u, %u", RA, RS, RB, MB, ME);
	return buf;
}

char *andx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "and");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);
	return buf;
}

char *andcx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "andc");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *cmp_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "cmp");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " cr%u, %u, r%u, r%u", CRFD, L, RA, RB);
	return buf;
}

char *cmpl_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "cmpl");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " cr%u, %u, r%u, r%u", CRFD, L, RA, RB);
	return buf;
}

char *cntlzwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "cntlzw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RA, RS);
	return buf;
}

char *eqvx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "eqv");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *extsbx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "extsb");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RA, RS);
	return buf;
}

char *extshx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "extsh");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RA, RS);
	return buf;
}

char *nandx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "nand");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *norx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "nor");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *orx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "or");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *orcx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "orc");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *slwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "slw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *srawx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "sraw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *srawix_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "srawi");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, %u", RA, RS, SH);
	return buf;
}

char *srwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "srw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *tw_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "tw");
	buf += sprintf(buf, " %u, r%u, r%u", TO, RA, RB);
	return buf;
}

char *xorx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "xor");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RA, RS, RB);
	return buf;
}

char *addx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "add");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);
	return buf;
}

char *addcx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "addc");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *addex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "adde");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *addmex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "addme");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RD, RA);		
	return buf;
}

char *addzex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "addze");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RD, RA);		
	return buf;
}

char *divwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "divw");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *divwux_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "divwu");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *mulhwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mulhw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *mulhwux_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mulhwu");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *mullwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mullw");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *negx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "neg");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u", RD, RA);	
	return buf;
}

char *subfx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "subf");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *subfcx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "subfc");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *subfex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "subfe");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *subfmex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "subfme");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}

char *subfzex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "subfze");
	if(OE) buf += sprintf(buf, "o");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " r%u, r%u, r%u", RD, RA, RB);	
	return buf;
}









void addi_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_ADDI;
	di->operation.integer.nullRA = (RA == 0);
	di->io.integer.srcReg[0] = RA ? RA : -1; /* bug */
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void addic_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_ADDIC;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = FALSE;
}

void addicd_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_ADDICD;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = 0;
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = FALSE;
}

void addis_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_ADDIS;
	di->operation.integer.nullRA = (RA == 0);
	di->io.integer.srcReg[0] = RA ? RA : -1;  /* bug */
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void andid_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_ANDID;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = 0;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void andisd_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_ANDISD;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = 0;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void cmpi_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_CMPI;
	di->operation.integer.extra.compare.crfD = CRFD;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = -1;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = CRFD;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void cmpli_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_CMPLI;
	di->operation.integer.extra.compare.crfD = CRFD;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = -1;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = CRFD;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void mulli_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	sword_t _sh;
	_sh = SEXT16(IMM);
	di->hasImmed = TRUE;
	di->immed = _sh;
	di->iclass = ICIntMul;
	di->operation.integer.ident = ID_MULLI;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void ori_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	BOOL nop = RS == RA && UIMM == 0;
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_ORI;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = nop ? -1: RA;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void oris_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_ORIS;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void subfic_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(IMM);
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_SUBFIC;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = FALSE;
}

void twi_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void xori_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_XORI;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void xoris_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = UIMM;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = ID_XORIS;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	di->io.integer.outCR = FALSE;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void rlwimix_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntRotate;
	di->operation.integer.ident = Rc ? ID_RLWIMID : ID_RLWIMI;
	di->operation.integer.extra.rotate.sh = SH;
	di->operation.integer.extra.rotate.mb = MB;
	di->operation.integer.extra.rotate.me = ME;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RA;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void rlwinmx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntRotate;
	di->operation.integer.ident = Rc ? ID_RLWINMD : ID_RLWINM;
	di->operation.integer.extra.rotate.sh = SH;
	di->operation.integer.extra.rotate.mb = MB;
	di->operation.integer.extra.rotate.me = ME;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void rlwnmx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntRotate;
	di->operation.integer.ident = Rc ? ID_RLWNMD : ID_RLWNM;
	di->operation.integer.extra.rotate.mb = MB;
	di->operation.integer.extra.rotate.me = ME;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void andx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_ANDD : ID_AND;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void andcx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_ANDCD : ID_ANDC;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void cmp_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_CMP;
	di->operation.integer.extra.compare.crfD = CRFD;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = -1;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = CRFD;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void cmpl_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = ID_CMPL;
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = -1;
	di->io.integer.outCR = TRUE;
	di->io.integer.crf = CRFD;
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void cntlzwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? ID_CNTLZWD : ID_CNTLZW;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void eqvx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? ID_EQVD : ID_EQV;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void extsbx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? ID_EXTSBD : ID_EXTSB;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void extshx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? ID_EXTSHD : ID_EXTSH;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void nandx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_NANDD : ID_NAND;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void norx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_NORD : ID_NOR;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void orx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_ORD : ID_OR;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void orcx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_ORCD : ID_ORC;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void slwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntShift;
	di->operation.integer.ident = Rc ? ID_SLWD : ID_SLW;
	di->operation.integer.extra.shift.sh = RB;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void srawx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntShift;
	di->operation.integer.ident = Rc ? ID_SRAWD : ID_SRAW;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = FALSE;
}

void srawix_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntShift;
	di->operation.integer.ident = Rc ? ID_SRAWID : ID_SRAWI;
	di->operation.integer.extra.shift.sh = SH;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = FALSE;
}

void srwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntShift;
	di->operation.integer.ident = Rc ? ID_SRWD : ID_SRW;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void tw_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void xorx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLogical;
	di->operation.integer.ident = Rc ? ID_XORD : ID_XOR;
	di->io.integer.srcReg[0] = RS;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RA;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = FALSE;
}

void addx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_ADDOD : ID_ADDD) : (OE ? ID_ADDO : ID_ADD);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void addcx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_ADDCOD : ID_ADDCD) : (OE ? ID_ADDCO : ID_ADDC);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void addex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
    di->hasImmed = FALSE;
    di->iclass = ICIntArithmetic;
    di->operation.integer.ident = Rc ? (OE ? ID_ADDEOD : ID_ADDED) : (OE ? ID_ADDEO : ID_ADDE);
    di->io.integer.srcReg[0] = RA;
    di->io.integer.srcReg[1] = RB;
    di->io.integer.inCarry = TRUE;
    di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
    di->io.integer.outCarry = TRUE;
    di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void addmex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_ADDMEOD : ID_ADDMED) : (OE ? ID_ADDMEO : ID_ADDME);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = TRUE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void addzex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_ADDZEOD : ID_ADDZED) : (OE ? ID_ADDZEO : ID_ADDZE);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = TRUE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void divwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntDiv;
	di->operation.integer.ident = Rc ? (OE ? ID_DIVWOD : ID_DIVWD) : (OE ? ID_DIVWO : ID_DIVW);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void divwux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntDiv;
	di->operation.integer.ident = Rc ? (OE ? ID_DIVWUOD : ID_DIVWUD) : (OE ? ID_DIVWUO : ID_DIVWU);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void mulhwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntMul;
	di->operation.integer.ident = Rc ? (OE ? ID_MULHWOD : ID_MULHWD) : (OE ? ID_MULHWO : ID_MULHW);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void mulhwux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntMul;
	di->operation.integer.ident = Rc ? (OE ? ID_MULHWUOD : ID_MULHWUD) : (OE ? ID_MULHWUO : ID_MULHWU);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void mullwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntMul;
	di->operation.integer.ident = Rc ? (OE ? ID_MULLWOD : ID_MULLWD) : (OE ? ID_MULLWO : ID_MULLW);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void negx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_NEGOD : ID_NEGD) : (OE ? ID_NEGO : ID_NEG);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void subfx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_SUBFOD : ID_SUBFD) : (OE ? ID_SUBFO : ID_SUBF);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = FALSE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void subfcx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_SUBFCOD : ID_SUBFCD) : (OE ? ID_SUBFCO : ID_SUBFC);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = FALSE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void subfex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_SUBFEOD : ID_SUBFED) : (OE ? ID_SUBFEO : ID_SUBFE);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = TRUE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void subfmex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_SUBFMEOD : ID_SUBFMED) : (OE ? ID_SUBFMEO : ID_SUBFME);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = -1;
	di->io.integer.inCarry = TRUE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

void subfzex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntArithmetic;
	di->operation.integer.ident = Rc ? (OE ? ID_SUBFZEOD : ID_SUBFZED) : (OE ? ID_SUBFZEO : ID_SUBFZE);
	di->io.integer.srcReg[0] = RA;
	di->io.integer.srcReg[1] = RB;
	di->io.integer.inCarry = TRUE;
	di->io.integer.dstReg = RD;
	if(Rc)
	{
		di->io.integer.outCR = TRUE;
		di->io.integer.crf = 0;
	}
	else
	{
		di->io.integer.outCR = FALSE;
	}
	di->io.integer.outCarry = TRUE;
	di->io.integer.outOverflow = OE ? TRUE : FALSE;
}

















void addi_xml(ppc_inst_t inst, addr_t pc, char *buf) {	xmlf(buf, pc, "addi", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void addic_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "addic", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void addicd_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "addic.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void addis_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "addis", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void andid_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "andi.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_UIMM, IMM); }
void andisd_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "andis.", 4, XML_DSTGPR, XML_DSTCRF, 0, RA, XML_SRCGPR, RS, XML_UIMM, IMM); }
void cmpi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "cmpi", 4, XML_DSTCRF, CRFD, XML_UIMM, L, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void cmpli_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "cmpli", 4, XML_DSTCRF, CRFD, XML_UIMM, L, XML_SRCGPR, RA, XML_UIMM, IMM); }
void mulli_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mulli", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void ori_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "ori", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void oris_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "oris", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_UIMM, IMM); }
void subfic_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "subfic", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void twi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "twi", 3, XML_UIMM, TO, XML_SRCGPR, RA, XML_SIMM, SEXT16(IMM)); }
void xori_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "xori", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_UIMM, IMM); }
void xoris_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "xoris", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_UIMM, IMM); }

void rlwimix_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "rlwimi", 5, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_UIMM, SH, XML_UIMM, MB, XML_UIMM, ME);
	else
		xmlf(buf, pc, "rlwimi.", 6, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_UIMM, SH, XML_UIMM, MB, XML_UIMM, ME);
}

void rlwinmx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "rlwinm.", 6, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_UIMM, SH, XML_UIMM, MB, XML_UIMM, ME);
	else
		xmlf(buf, pc, "rlwinm", 5, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_UIMM, SH, XML_UIMM, MB, XML_UIMM, ME);	
}

void rlwnmx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "rlwnm.", 6, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB, XML_UIMM, MB, XML_UIMM, ME);
	else
		xmlf(buf, pc, "rlwnm", 5, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB, XML_UIMM, MB, XML_UIMM, ME);
}

void andx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "and.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "and", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
}
		
void andcx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "andc.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "andc", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);	
}

void cmp_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "cmp", 4, XML_DSTCRF, CRFD, XML_UIMM, L, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void cmpl_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "cmpl", 4, XML_DSTCRF, CRFD, XML_UIMM, L, XML_SRCGPR, RA, XML_SRCGPR, RB); }

void cntlzwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "cntlzw.", 3, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS);
	else
		xmlf(buf, pc, "cntlzw", 2, XML_DSTGPR, RA, XML_SRCGPR, RS);
}

void eqvx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "eqv.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "eqv", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void extsbx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "extsb.", 3, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS);
	else
		xmlf(buf, pc, "extsb", 2, XML_DSTGPR, RA, XML_SRCGPR, RS);
}

void extshx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "extsh.", 3, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS);
	else
		xmlf(buf, pc, "extsh", 2, XML_DSTGPR, RA, XML_SRCGPR, RS);
}

void nandx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "nand.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "nand", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void norx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "nor.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "nor", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void orx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "or.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "or", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void orcx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "orc.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "orc", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void slwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "slw.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "slw", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void srawx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "sraw.", 5, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "sraw", 4, XML_DSTGPR, RA, XML_DSTXER, XML_XERCA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void srawix_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "srawi.", 5, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RS, XML_UIMM, SH);
	else
		xmlf(buf, pc, "srawi", 4, XML_DSTGPR, RA, XML_DSTXER, XML_XERCA, XML_SRCGPR, RS, XML_UIMM, SH);
}

void srwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "srw.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "srw", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void tw_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	xmlf(buf, pc, "tw", 3, XML_UIMM, TO, XML_DSTGPR, RA, XML_SRCGPR, RB);
}

void xorx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "xor.", 4, XML_DSTGPR, RA, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "xor", 3, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RB);
}

void addx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "add.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "add", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
}

void addcx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "addc.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "addc", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
}

void addex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "addeo.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "adde.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "addeo", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "adde", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
}

void addmex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "addme.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "addme.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "addmeo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "addme", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
	}
}

void addzex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "addze.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "addze.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "addzeo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "addze", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCXER, XML_XERCA);
	}
}

void divwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "divwo.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "divw.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "divwo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "divw", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}		
}

void divwux_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "divwuo.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "divwu.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "divwuo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "divwu", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}		
}

void mulhwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mulhw.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "mulhw", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
}

void mulhwux_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mulhwu.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	else
		xmlf(buf, pc, "mulhwu", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
}

void mullwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "mullwo.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "mullw.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "mullwo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "mullw", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}		
}

void negx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "nego.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA);
		else
			xmlf(buf, pc, "neg.", 3, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "nego", 3, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA);
		else
			xmlf(buf, pc, "neg", 2, XML_DSTGPR, RD, XML_SRCGPR, RA);
	}
}

void subfx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "subfo.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "subf.", 4, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "subfo", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "subf", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
}

void subfcx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "subfco.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "subfc.", 5, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "subfco", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
		else
			xmlf(buf, pc, "subfc", 4, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB);
	}
}

void subfex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "subfeo.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfe.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "subfeo", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfe", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
}

void subfmex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "subfmeo.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfme.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "subfmeo", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfme", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
}

void subfzex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
	{
		if(OE)
			xmlf(buf, pc, "subfzeo.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfze.", 6, XML_DSTGPR, RD, XML_DSTCRF, 0, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
	else
	{
		if(OE)
			xmlf(buf, pc, "subfzeo", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XEROV | XML_XERSO | XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
		else
			xmlf(buf, pc, "subfze", 5, XML_DSTGPR, RD, XML_DSTXER, XML_XERCA, XML_SRCGPR, RA, XML_SRCGPR, RB, XML_SRCXER, XML_XERCA);
	}
}
