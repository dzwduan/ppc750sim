/***************************************************************************
                       branch.c  -  description
                         -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 * *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or *
 *   (at your option) any later version.   *
 * *
 ***************************************************************************/

#include <branch.h>
#include <string.h>
#include <stdio.h>
#include <ppcemul.h>
#include <xmlize.h>

extern UInt32 text_addr;
extern UInt32 text_size;

void bx_impl(ppc_inst_t inst)
{
	if(AA)
		NPC = SEXT26(LI << 2);
	else
		NPC = PC + SEXT26(LI << 2);
		
	if(LK)
		LR = PC + 4;	
}


void bcx_impl(ppc_inst_t inst)
{
	BOOL ctr_ok;
	BOOL cond_ok;
	
	if(CONDITIONAL_BR)				
	{
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			cond_ok = ((CR << BI) & 0x80000000) != 0;
		}
		else
		{
			/* Branch if condition is false */
			cond_ok = ((CR << BI) & 0x80000000) == 0;
		}
	}
	else
	{
		cond_ok = TRUE;
	}
					
	if(DECREMENTS_CTR)
	{
		/* Decrement CTR */
		CTR = CTR - 1;
		/* Test whether to branch if CTR is zero or not */
		if(BR_IF_CTR_ZERO)
		{
			ctr_ok = CTR == 0;
		}
		else
		{
			ctr_ok = CTR != 0;
		}
	}
	else
	{
		ctr_ok = TRUE;
	}
					
	if(ctr_ok && cond_ok)
	{
		if(AA)
			NPC = SEXT16(BD << 2);
		else
			NPC = PC + SEXT16(BD << 2);
	}
					
	if(LK)
		LR = PC + 4;
}

void bcctrx_impl(ppc_inst_t inst)
{
	BOOL cond_ok;
					
	if(CONDITIONAL_BR)				
	{
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			cond_ok = ((CR << BI) & 0x80000000) != 0;
		}
		else
		{
			/* Branch if condition is false */
			cond_ok = ((CR << BI) & 0x80000000) == 0;
		}
	}
	else
	{
		cond_ok = TRUE;
	}
				
	if(cond_ok)
	{
		NPC = CTR & 0xfffffffc;
	}

	if(LK)
		LR = PC + 4;
}

void bclrx_impl(ppc_inst_t inst)
{
	BOOL ctr_ok;
	BOOL cond_ok;
	
	if(CONDITIONAL_BR)				
	{
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			cond_ok = ((CR << BI) & 0x80000000) != 0;
		}
		else
		{
			/* Branch if condition is false */
			cond_ok = ((CR << BI) & 0x80000000) == 0;
		}
	}
	else
	{
		cond_ok = TRUE;
	}
					
	if(DECREMENTS_CTR)
	{
		/* Decrement CTR */
		CTR = CTR - 1;
		/* Test whether to branch if CTR is zero or not */
		ctr_ok = BR_IF_CTR_ZERO ? CTR == 0 : CTR != 0;
	}
	else
	{
		ctr_ok = TRUE;
	}
				
	if(ctr_ok && cond_ok)
	{
		NPC = LR & 0xfffffffc;
	}
	
	if(LK)
		LR = PC + 4;
}




















char *bx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	UInt32 npc;
	
	if(LK)
		buf += sprintf(buf, "\tLR = 0x%08x;\n", pc + 4);
	
	if(AA)
		npc = SEXT26(LI << 2);
	else
		npc = pc + SEXT26(LI << 2);
		
	if(npc >= text_addr && npc < text_addr + text_size)
	{
		buf += sprintf(buf, "\tPC = 0x%08x;\n", npc);
		buf += sprintf(buf, "\tgoto pc_%08x;\n", npc);
	}
	else
	{
		buf += sprintf(buf, "\tabort();\n");
	}
	return buf;
}


char *bcx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	UInt32 npc;
	BOOL ctr_used = FALSE;
	BOOL cond_used = FALSE;
	char ctr_ok[256];
	char cond_ok[256];
	
	if(CONDITIONAL_BR)				
	{
		cond_used = TRUE;
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) != 0", BI);
		}
		else
		{
			/* Branch if condition is false */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) == 0", BI);
		}
	}
					
	if(DECREMENTS_CTR)
	{
		ctr_used = TRUE;
		/* Decrement CTR */
		buf += sprintf(buf, "\tCTR = CTR - 1;\n");
		/* Test whether to branch if CTR is zero or not */
		if(BR_IF_CTR_ZERO)
			sprintf(ctr_ok, "CTR == 0");
		else
			sprintf(ctr_ok, "CTR != 0");
	}
					
	if(LK)
		buf += sprintf(buf, "\tLR = %u;\n", pc + 4);
	
	if(cond_used)
	{
		if(ctr_used)
			buf += sprintf(buf, "\tif(%s && %s)\n", cond_ok, ctr_ok);
		else
			buf += sprintf(buf, "\tif(%s)\n", cond_ok);
	}
	else
	{
		if(ctr_used)
			buf += sprintf(buf, "\tif(%s)\n", ctr_ok);
	}
	
	if(ctr_used || cond_used)
		buf += sprintf(buf, "\t{\n");
		
	if(AA)
		npc = SEXT16(BD << 2);
	else
		npc = pc + SEXT16(BD << 2);
	
	if(npc >= text_addr && npc < text_addr + text_size)
	{
		buf += sprintf(buf, "\t\tPC = 0x%08x;\n", npc);
		buf += sprintf(buf, "\t\tgoto pc_%08x;\n", npc);
	}
	else
	{
		buf += sprintf(buf, "\tabort();\n");
	}
	if(cond_used || ctr_used)
		buf += sprintf(buf, "\t}\n");
	return buf;
}

char *bcctrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	BOOL cond_used = FALSE;
	char cond_ok[256];
					
	if(CONDITIONAL_BR)				
	{
		cond_used = TRUE;
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) != 0", BI);
		}
		else
		{
			/* Branch if condition is false */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) == 0", BI);
		}
	}
				
	if(cond_used)
	{
		buf += sprintf(buf, "\tif(%s)\n", cond_ok);
		buf += sprintf(buf, "\t{\n");
	}
	
	buf += sprintf(buf, "\t\tPC = CTR & 0xfffffffc;\n");
	
	if(LK)
		buf += sprintf(buf, "\tLR = %u;\n", pc + 4);

	buf += sprintf(buf, "\t\tgoto *pc_table[(PC >> 2) - %u];\n", text_addr >> 2);
	
	if(cond_used)
		buf += sprintf(buf, "\t}\n");
	return buf;

}

char *bclrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	BOOL ctr_used = FALSE;
	BOOL cond_used = FALSE;
	char ctr_ok[256];
	char cond_ok[256];
	
	if(CONDITIONAL_BR)				
	{
		cond_used = TRUE;
		/* Conditional branch */
		if(BR_IF_TRUE)
		{
			/* Branch if condition is true */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) != 0", BI);
		}
		else
		{
			/* Branch if condition is false */
			sprintf(cond_ok, "((CR << %u) & 0x80000000) == 0", BI);
		}
	}
					
	if(DECREMENTS_CTR)
	{
		ctr_used = TRUE;
		/* Decrement CTR */
		buf += sprintf(buf, "\tCTR = CTR - 1;\n");
		/* Test whether to branch if CTR is zero or not */
		if(BR_IF_CTR_ZERO)
			sprintf(ctr_ok, "CTR == 0");
		else
			sprintf(ctr_ok, "CTR != 0");
	}
				
	if(cond_used)
	{
		if(ctr_used)
			buf += sprintf(buf, "\tif(%s && %s)\n", cond_ok, ctr_ok);
		else
			buf += sprintf(buf, "\tif(%s)\n", cond_ok);
	}
	else
	{
		if(ctr_used)
			buf += sprintf(buf, "\tif(%s)\n", ctr_ok);
	}
	
	if(cond_used || ctr_used)
		buf += sprintf(buf, "\t{\n");
		
	buf += sprintf(buf, "\t\tPC = LR & 0xfffffffc;\n");
	
	if(LK)
		buf += sprintf(buf, "\t\tLR = %u;\n", pc + 4);
	
	buf += sprintf(buf, "\t\tgoto *pc_table[(PC >> 2) - %u];\n", text_addr >> 2);
	
	if(cond_used || ctr_used)
		buf += sprintf(buf, "\t}\n");
	return buf;
	
	
	
	
}










char *bx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	addr_t target_addr;
	buf += sprintf(buf, "b");
	if(LK) buf += sprintf(buf, "l");
	if(AA)
	{
		buf += sprintf(buf, "a");
		target_addr = SEXT26(LI << 2);
	}
	else
	{
		target_addr = pc + SEXT26(LI << 2);
	}
	buf += sprintf(buf, " 0x%08x", target_addr);
	return buf;
}

char *bcx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	addr_t target_addr;
	
	buf += sprintf(buf, "bc");
	if(LK) buf += sprintf(buf, "l");
	if(AA)
	{
		buf += sprintf(buf, "a");
		target_addr = SEXT16(BD << 2);
	}
	else
	{
		target_addr = pc + SEXT16(BD << 2);
	}
	buf += sprintf(buf, " %u, %u, 0x%08x", BO, BI, target_addr);
	return buf;
}

char *bcctrx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "bcctr");
	if(LK) buf += sprintf(buf, "l");
	buf += sprintf(buf, " %u, %u", BO, BI);
	return buf;
}

char *bclrx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "bclr");
	if(LK) buf += sprintf(buf, "l");
	buf += sprintf(buf, " %u, %u", BO, BI);
	return buf;
}




void bx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICBranch;
	di->operation.branch.ident = ID_B;
	di->operation.branch.extra.b.aa = AA ? TRUE : FALSE;
	di->operation.branch.extra.b.lk = LK ? TRUE : FALSE;
	di->hasImmed = TRUE;
	di->immed = SEXT26(LI);
	di->io.branch.inCR = FALSE;
	di->io.branch.inLR = FALSE;
	di->io.branch.inCTR = FALSE;
	di->io.branch.outLR = LK ? TRUE : FALSE;
	di->io.branch.outCTR = FALSE;
}

void bcx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	if(DECREMENTS_CTR)
	{
		di->io.branch.inCTR = TRUE;
		di->io.branch.outCTR = TRUE;
	}
	else
	{
		di->io.branch.inCTR = FALSE;
		di->io.branch.outCTR = FALSE;
	}
	
if (CONDITIONAL_BR)
{
		di->io.branch.inCR = TRUE;
		di->io.branch.crf = BI/4;
	}
	else
	{
		di->io.branch.inCR = FALSE;
	}
	
	if(LK)
	{
		di->operation.branch.extra.bc.lk = TRUE;
		di->io.branch.outLR = TRUE;
	}
	else
	{
		di->operation.branch.extra.bc.lk = FALSE;
		di->io.branch.outLR = FALSE;
	}
	
	if(AA)
	{
		di->operation.branch.extra.bc.aa = TRUE;
	}
	else
	{
		di->operation.branch.extra.bc.aa = FALSE;
	}
	di->iclass = ICBranch;
	di->operation.branch.ident = ID_BC;
	di->operation.branch.extra.bc.bi = BI;
	di->operation.branch.extra.bc.bo = BO;
	di->hasImmed = TRUE;
	di->immed = SEXT16(BD<<2);
	di->io.branch.inLR = FALSE;
}

void bcctrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICBranch;
	di->operation.branch.ident = ID_BCCTR;
	di->operation.branch.extra.bcctr.bi = BI;
	di->operation.branch.extra.bcctr.bo = BO;
	di->operation.branch.extra.bcctr.lk = LK;
	di->hasImmed = FALSE;
	di->io.branch.inLR = FALSE;
	di->io.branch.inCTR = TRUE;
	if(LK)
		di->io.branch.outLR = TRUE;
	else
		di->io.branch.outLR = FALSE;
	di->io.branch.outCTR = FALSE;
	if(CONDITIONAL_BR)
	{
		di->io.branch.inCR = TRUE;
		di->io.branch.crf = BI/4;
	}
	else
	{
		di->io.branch.inCR = FALSE;
	}
}

void bclrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	if(DECREMENTS_CTR)
	{
		di->io.branch.inCTR = TRUE;
		di->io.branch.outCTR = TRUE;
	}
	else
	{
		di->io.branch.inCTR = FALSE;
		di->io.branch.outCTR = FALSE;
	}
	
	if(CONDITIONAL_BR)
	{
		di->io.branch.inCR = TRUE;
		di->io.branch.crf = BI/4;
	}
	else
	{
		di->io.branch.inCR = FALSE;
	}
	
	if(LK)
	{
		di->operation.branch.extra.bclr.lk = TRUE;
		di->io.branch.outLR = TRUE;
	}
	else
	{
		di->operation.branch.extra.bclr.lk = FALSE;
		di->io.branch.outLR = FALSE;
	}
	di->iclass = ICBranch;
	di->operation.branch.ident = ID_BCLR;
	di->operation.branch.extra.bclr.bi = BI;
	di->operation.branch.extra.bclr.bo = BO;
	di->hasImmed = FALSE;
	di->io.branch.inLR = TRUE;
}







void bx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(LK)
		xmlf(buf, pc, AA ? "bla" : "bl", 2, XML_DSTSPR, 8, XML_SIMM, SEXT26(LI << 2));
	else
		xmlf(buf, pc, AA ? "ba" : "b", 1, XML_SIMM, SEXT26(LI << 2));
}

void bcx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(LK)
	{
		if(DECREMENTS_CTR)
			xmlf(buf, pc, AA ? "bcla" : "bcl", 6, XML_DSTSPR, 8, XML_DSTSPR, 9, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI, XML_SIMM, SEXT16(BD << 2));
		else
			xmlf(buf, pc, AA ? "bcla" : "bcl", 4, XML_DSTSPR, 8, XML_BO, BO, XML_BI, BI, XML_SIMM, SEXT16(BD << 2));
	}
	else
	{
		if(DECREMENTS_CTR)
			xmlf(buf, pc, AA ? "bca" : "bc", 5, XML_DSTSPR, 9, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI, XML_SIMM, SEXT16(BD << 2));
		else
			xmlf(buf, pc, AA ? "bca" : "bc", 3, XML_BO, BO, XML_BI, BI, XML_SIMM, SEXT16(BD << 2));		
	}
}

void bcctrx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(LK)
		xmlf(buf, pc, "bcctrl", 4, XML_DSTSPR, 8, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI);
	else
		xmlf(buf, pc, "bcctr", 3, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI);
}

void bclrx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(LK)
	{
		if(DECREMENTS_CTR)
			xmlf(buf, pc, "bclrl", 6, XML_DSTSPR, 8, XML_DSTSPR, 9, XML_SRCSPR, 8, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI);
		else
			xmlf(buf, pc, "bclrl", 4, XML_DSTSPR, 8, XML_SRCSPR, 8, XML_BO, BO, XML_BI, BI);
	}
	else
	{
		if(DECREMENTS_CTR)
			xmlf(buf, pc, "bclr", 5, XML_DSTSPR, 9, XML_SRCSPR, 8, XML_SRCSPR, 9, XML_BO, BO, XML_BI, BI);
		else
			xmlf(buf, pc, "bclr", 3, XML_SRCSPR, 8, XML_BO, BO, XML_BI, BI);
	}		
}
