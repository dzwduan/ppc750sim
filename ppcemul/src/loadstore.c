/***************************************************************************
                          loadstore.c  -  description
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

#include <loadstore.h>
#include <ppcemul.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmlize.h>

#define EA_U (GPR(RA) + SEXT16(OFS))
#define EA (RA ? GPR(RA) + SEXT16(OFS) : SEXT16(OFS))
#define EA_UX (GPR(RA) + GPR(RB))
#define EA_X (RA ? GPR(RA) + GPR(RB) : GPR(RB))


void lbz_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	byte_t result = MEM_READ_BYTE(ea);
	GPR(RD) = (word_t) result;
}	

void lbzu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	byte_t result = MEM_READ_BYTE(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}

void lfd_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	dword_t result = MEM_READ_DWORD(ea);
	FPR_DW(FD) = result;
}

void lfdu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	dword_t result = MEM_READ_DWORD(ea);
	FPR_DW(FD) = result;
	GPR(RA) = ea;
}

void lfs_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	word_t result = MEM_READ_WORD(ea);
	FPR_DBL(FD) = (double) *(float *) &result;
}

void lfsu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	word_t result = MEM_READ_WORD(ea);
	FPR_DBL(FD) = (double) *(float *) &result;
	GPR(RA) = ea;
}

void lha_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	shalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);
	GPR(RD) = SEXT16(result);
}

void lhau_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	shalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);
	GPR(RD) = SEXT16(result);
	GPR(RA) = ea;
}


void lhz_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	halfword_t result = MEM_READ_HALF_WORD(ea);
	GPR(RD) = (word_t) result;
}

void lhzu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	halfword_t result = MEM_READ_HALF_WORD(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}

void lmw_impl(ppc_inst_t inst) { abort(); }


void lwz_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	word_t result = MEM_READ_WORD(ea);
	GPR(RD) = (word_t) result;
}


void lwzu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	word_t result = MEM_READ_WORD(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}


void stb_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	MEM_WRITE_BYTE(ea, GPR(RS));
}


void stbu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	MEM_WRITE_BYTE(ea, GPR(RS));
	GPR(RA) = ea;
}

void stfd_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	MEM_WRITE_DWORD(ea, FPR_DW(FS));
}

void stfdu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	MEM_WRITE_DWORD(ea, FPR_DW(FS));
	GPR(RA) = ea;
}

void stfs_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	float tmp = (float) FPR_DBL(FS);
	MEM_WRITE_WORD(ea, *(word_t *) &tmp);	
}

void stfsu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	float tmp = (float) FPR_DBL(FS);
	MEM_WRITE_WORD(ea, *(word_t *) &tmp);	
	GPR(RA) = ea;
}

void sth_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	MEM_WRITE_HALF_WORD(ea, GPR(RS));
}

void sthu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	MEM_WRITE_HALF_WORD(ea, GPR(RS));
	GPR(RA) = ea;
}

void stmw_impl(ppc_inst_t inst) { abort(); }


void stw_impl(ppc_inst_t inst)
{
	addr_t ea = EA;
	MEM_WRITE_WORD(ea, GPR(RS));
}


void stwu_impl(ppc_inst_t inst)
{
	addr_t ea = EA_U;
	MEM_WRITE_WORD(ea, GPR(RS));
	GPR(RA) = ea;
}

void dcba_impl(ppc_inst_t inst) { abort(); }
void dcbf_impl(ppc_inst_t inst) { abort(); }
void dcbi_impl(ppc_inst_t inst) { abort(); }
void dcbst_impl(ppc_inst_t inst) { abort(); }
void dcbt_impl(ppc_inst_t inst) { abort(); }
void dcbtst_impl(ppc_inst_t inst) { abort(); }

void dcbz_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_SET(ea - (ea % 32), 0, 32);
}

void eciwx_impl(ppc_inst_t inst) { abort(); }
void ecowx_impl(ppc_inst_t inst) { abort(); }
void eieio_impl(ppc_inst_t inst) { abort(); }
void icbi_impl(ppc_inst_t inst) { abort(); }

void lbzux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	byte_t result = MEM_READ_BYTE(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}

void lbzx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	byte_t result = MEM_READ_BYTE(ea);
	GPR(RD) = (word_t) result;
}

void lfdux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	dword_t result = MEM_READ_DWORD(ea);
	FPR_DW(FD) = result;
	GPR(RA) = ea;
}

void lfdx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	dword_t result = MEM_READ_DWORD(ea);
	FPR_DW(FD) = result;
}

void lfsux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	word_t result = MEM_READ_WORD(ea);
	FPR_DBL(FD) = (double) *(float *) &result;
	GPR(RA) = ea;
}

void lfsx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	word_t result = MEM_READ_WORD(ea);
	FPR_DBL(FD) = (double) *(float *) &result;
}


void lhaux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	shalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);
	GPR(RD) = SEXT16(result);
	GPR(RA) = ea;
}

void lhax_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	shalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);
	GPR(RD) = SEXT16(result);
}

void lhbrx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	halfword_t result = MEM_READ_HALF_WORD_BYTE_REVERSE(ea);
	GPR(RD) = (word_t) result;
}

void lhzux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	halfword_t result = MEM_READ_HALF_WORD(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}

void lhzx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	halfword_t result = MEM_READ_HALF_WORD(ea);
	GPR(RD) = (word_t) result;
}

void lswi_impl(ppc_inst_t inst) { abort(); }
void lswx_impl(ppc_inst_t inst) { abort(); }
void lwarx_impl(ppc_inst_t inst) { abort(); }

void lwbrx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	word_t result = MEM_READ_WORD_BYTE_REVERSE(ea);
	GPR(RD) = (word_t) result;
}

void lwzux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	word_t result = MEM_READ_WORD(ea);
	GPR(RD) = (word_t) result;
	GPR(RA) = ea;
}
	
void lwzx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	word_t result = MEM_READ_WORD(ea);
	GPR(RD) = (word_t) result;
}

void stbux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	MEM_WRITE_BYTE(ea, GPR(RS));
	GPR(RA) = ea;
}

void stbx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_BYTE(ea, GPR(RS));
}

void stfdux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	MEM_WRITE_DWORD(ea, FPR_DW(FS));
	GPR(RA) = ea;
}

void stfdx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_DWORD(ea, FPR_DW(FS));
}

void stfiwx_impl(ppc_inst_t inst) { abort(); }

void stfsux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	float tmp = (float) FPR_DBL(FS);
	MEM_WRITE_WORD(ea, *(word_t *) &tmp);
	GPR(RA) = ea;
}

void stfsx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	float tmp = (float) FPR_DBL(FS);
	MEM_WRITE_WORD(ea, *(word_t *) &tmp);
}

void sthbrx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_HALF_WORD_BYTE_REVERSE(ea, GPR(RS));
}
	
void sthux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	MEM_WRITE_HALF_WORD(ea, GPR(RS));
	GPR(RA) = ea;
}

void sthx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_HALF_WORD(ea, GPR(RS));
}

void stswi_impl(ppc_inst_t inst) { abort(); }
void stswx_impl(ppc_inst_t inst) { abort(); }

void stwbrx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_WORD_BYTE_REVERSE(ea, GPR(RS));
}

void stwcxd_impl(ppc_inst_t inst) { abort(); }

void stwux_impl(ppc_inst_t inst)
{
	addr_t ea = EA_UX;
	MEM_WRITE_WORD(ea, GPR(RS));
	GPR(RA) = ea;
}

void stwx_impl(ppc_inst_t inst)
{
	addr_t ea = EA_X;
	MEM_WRITE_WORD(ea, GPR(RS));
}

void tlbia_impl(ppc_inst_t inst) { abort(); }
void tlbie_impl(ppc_inst_t inst) { abort(); }
void tlbsync_impl(ppc_inst_t inst) { abort(); }

















char *lbz_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tbyte_t result = MEM_READ_BYTE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}	

char *lbzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tbyte_t result = MEM_READ_BYTE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lfd_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tdword_t result = MEM_READ_DWORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = result;\n", FD);
	return buf;
}

char *lfdu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tdword_t result = MEM_READ_DWORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = result;\n", FD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = (double) *(float *) &result;\n", FD);
	return buf;
}

char *lfsu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = (double) *(float *) &result;\n", FD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lha_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tshalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = SEXT16(result);\n", RD);
	return buf;
}

char *lhau_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tshalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = SEXT16(result);\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}


char *lhz_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\thalfword_t result = MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *lhzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\thalfword_t result = MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lmw_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }


char *lwz_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}


char *lwzu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}


char *stb_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_BYTE(ea, GPR(%u));\n", RS);
	return buf;
}


char *stbu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_BYTE(ea, GPR(%u));\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stfd_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_DWORD(ea, FPR_DW(%u));\n", FS);
	return buf;
}

char *stfdu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_DWORD(ea, FPR_DW(%u));\n", FS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tfloat tmp = (float) FPR_DBL(%u);\n", FS);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, *(word_t *) &tmp);\n");	
	return buf;
}

char *stfsu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tfloat tmp = (float) FPR_DBL(%u);\n", FS);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, *(word_t *) &tmp);\n");	
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *sth_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_HALF_WORD(ea, GPR(%u));\n", RS);
	return buf;
}

char *sthu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_HALF_WORD(ea, GPR(%u));\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stmw_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }


char *stw_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	else
		buf += sprintf(buf, "\taddr_t ea = %d;\n", SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, GPR(%u));\n", RS);
	return buf;
}


char *stwu_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + (%d);\n", RA, SEXT16(OFS));
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, GPR(%u));\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *dcba_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *dcbf_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *dcbi_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *dcbst_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *dcbt_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *dcbtst_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *dcbz_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_SET(ea - (ea %% 32), 0, 32);\n");
	return buf;
}

char *eciwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *ecowx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *eieio_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *icbi_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *lbzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tbyte_t result = MEM_READ_BYTE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lbzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tbyte_t result = MEM_READ_BYTE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *lfdux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tdword_t result = MEM_READ_DWORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = result;\n", FD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lfdx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tdword_t result = MEM_READ_DWORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = result;\n", FD);
	return buf;
}

char *lfsux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = (double) *(float *) &result;\n", FD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lfsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = (double) *(float *) &result;\n", FD);
	return buf;
}


char *lhaux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tshalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = SEXT16(result);\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lhax_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tshalfword_t result = (shalfword_t) MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = SEXT16(result);\n", RD);
	return buf;
}

char *lhbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\thalfword_t result = MEM_READ_HALF_WORD_BYTE_REVERSE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *lhzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\thalfword_t result = MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *lhzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\thalfword_t result = MEM_READ_HALF_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *lswi_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *lswx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *lwarx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *lwbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD_BYTE_REVERSE(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *lwzux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}
	
char *lwzx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tword_t result = MEM_READ_WORD(ea);\n");
	buf += sprintf(buf, "\tGPR(%u) = (word_t) result;\n", RD);
	return buf;
}

char *stbux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tMEM_WRITE_BYTE(ea, GPR(RS));\n");
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stbx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_BYTE(ea, GPR(%u));\n", RS);
	return buf;
}

char *stfdux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tMEM_WRITE_DWORD(ea, FPR_DW(%u));\n", FS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stfdx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_DWORD(ea, FPR_DW(%u));\n", FS);
	return buf;
}

char *stfiwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *stfsux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tfloat tmp = (float) FPR_DBL(%u);\n", FS);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, *(word_t *) &tmp);\n");
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stfsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tfloat tmp = (float) FPR_DBL(%u);\n", FS);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, *(word_t *) &tmp);\n");
	return buf;
}

char *sthbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_HALF_WORD_BYTE_REVERSE(ea, GPR(%u));\n", RS);
	return buf;
}
	
char *sthux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tMEM_WRITE_HALF_WORD(ea, GPR(%u));\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *sthx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_HALF_WORD(ea, GPR(%u));\n", RS);
	return buf;
}

char *stswi_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *stswx_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *stwbrx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_WORD_BYTE_REVERSE(ea, GPR(%u));\n", RS);
	return buf;
}

char *stwcxd_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *stwux_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, GPR(%u));\n", RS);
	buf += sprintf(buf, "\tGPR(%u) = ea;\n", RA);
	return buf;
}

char *stwx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(RA)
		buf += sprintf(buf, "\taddr_t ea = GPR(%u) + GPR(%u);\n", RA, RB);
	else
		buf += sprintf(buf, "\taddr_t ea = GPR(%u);\n", RB);
	buf += sprintf(buf, "\tMEM_WRITE_WORD(ea, GPR(%u));\n", RS);
	return buf;
}

char *tlbia_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *tlbie_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *tlbsync_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }











char *lbz_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lbz r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lbzu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lbzu r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lfd_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfd f%u, %d(r%u)", FD, SEXT16(IMM), RA); return buf; }
char *lfdu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfdu f%u, %d(r%u)", FD, SEXT16(IMM), RA); return buf; }
char *lfs_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfs f%u, %d(r%u)", FD, SEXT16(IMM), RA); return buf; }
char *lfsu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfsu f%u, %d(r%u)", FD, SEXT16(IMM), RA); return buf; }
char *lha_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lha r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lhau_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhau r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lhz_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhz r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lhzu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhzu r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lmw_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lmw r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lwz_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwz r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *lwzu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwzu r%u, %d(r%u)", RD, SEXT16(IMM), RA); return buf; }
char *stb_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stb r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *stbu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stbu r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *stfd_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfd f%u, %d(r%u)", FS, SEXT16(IMM), RA); return buf; }
char *stfdu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfdu f%u, %d(r%u)", FS, SEXT16(IMM), RA); return buf; }
char *stfs_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfs f%u, %d(r%u)", FS, SEXT16(IMM), RA); return buf; }
char *stfsu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfsu f%u, %d(r%u)", FS, SEXT16(IMM), RA); return buf; }
char *sth_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sth r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *sthu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sthu r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *stmw_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stmw r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *stw_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stw r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *stwu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stwu r%u, %d(r%u)", RS, SEXT16(IMM), RA); return buf; }
char *dcba_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcba r%u, r%u", RA, RB); return buf; }
char *dcbf_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbf r%u, r%u", RA, RB); return buf; }
char *dcbi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbi r%u, r%u", RA, RB); return buf; }
char *dcbst_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbst r%u, r%u", RA, RB); return buf; }
char *dcbt_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbt r%u, r%u", RA, RB); return buf; }
char *dcbtst_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbtst r%u, r%u", RA, RB); return buf; }
char *dcbz_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "dcbz r%u, r%u", RA, RB); return buf; }
char *eciwx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "eciw r%u, r%u, r%u", RD, RA, RB); return buf; }
char *ecowx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "eciw r%u, r%u, r%u", RS, RA, RB); return buf; }
char *eieio_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "eieio"); return buf; }
char *icbi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "icbi r%u, r%u", RA, RB); return buf; }
char *lbzux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lbzux r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lbzx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lbzx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lfdux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfdux f%u, r%u, r%u", FD, RA, RB); return buf; }
char *lfdx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfdx f%u, r%u, r%u", FD, RA, RB); return buf; }
char *lfsux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfsux f%u, r%u, r%u", FD, RA, RB); return buf; }
char *lfsx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfsx f%u, r%u, r%u", FD, RA, RB); return buf; }
char *lhaux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lfdux f%u, r%u, r%u", FD, RA, RB); return buf; }
char *lhax_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhax r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lhbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhbrx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lhzux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhzux r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lhzx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lhzx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lswi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lswi r%u, r%u, %u", RD, RA, NB); return buf; }
char *lswx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lswx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lwarx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwarx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lwbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwbrx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lwzux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwzux r%u, r%u, r%u", RD, RA, RB); return buf; }
char *lwzx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "lwzx r%u, r%u, r%u", RD, RA, RB); return buf; }
char *stbux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stbux r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stbx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stbx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stfdux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfdux f%u, r%u, r%u", FS, RA, RB); return buf; }
char *stfdx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfdx f%u, r%u, r%u", FS, RA, RB); return buf; }
char *stfiwx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfiwx f%u, r%u, r%u", FS, RA, RB); return buf; }
char *stfsux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfsux f%u, r%u, r%u", FS, RA, RB); return buf; }
char *stfsx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stfsx f%u, r%u, r%u", FS, RA, RB); return buf; }
char *sthbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sthbrx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *sthux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sthux r%u, r%u, r%u", RS, RA, RB); return buf; }
char *sthx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sthx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stswi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stswi r%u, r%u, %u", RS, RA, NB); return buf; }
char *stswx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stswx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stwbrx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stwbrx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stwcxd_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stwcx. r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stwux_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stwux r%u, r%u, r%u", RS, RA, RB); return buf; }
char *stwx_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "stwx r%u, r%u, r%u", RS, RA, RB); return buf; }
char *tlbia_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "tlbia"); return buf; }
char *tlbie_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "tlbie r%u", RB); return buf; }
char *tlbsync_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "tlbsync"); return buf; }






void lbz_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LBZ;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lbzu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LBZU;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lfd_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFD;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = -1;
}

void lfdu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFDU;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = RA;
}

void lfs_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFS;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lfsu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFSU;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = RA;
}

void lha_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHA;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = TRUE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lhau_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHAU;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = TRUE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lhz_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHZ;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lhzu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHZU;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lmw_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LMW;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lwz_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWZ;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;	\
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lwzu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWZU;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void stb_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STB;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stbu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STBU;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stfd_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFD;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stfdu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFDU;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stfs_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFS;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stfsu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFSU;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void sth_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STH;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void sthu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STHU;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stmw_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STMW;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stw_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STW;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stwu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = TRUE;
	di->immed = SEXT16(OFS);
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STWU;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void dcba_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void dcbf_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void dcbi_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void dcbst_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void dcbt_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void dcbtst_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void dcbz_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_DCBZ;
	di->operation.loadStore.size = -1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void eciwx_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void ecowx_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void eieio_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void icbi_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void lbzux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LBZUX;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lbzx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LBZX;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lfdux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFDUX;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = RA;
}

void lfdx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFDX;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = -1;
}

void lfsux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFSUX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = RA;
}

void lfsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatLoad;
	di->operation.loadStore.ident = ID_LFSX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = FD;
	di->io.loadStore.dstReg[1] = -1;
}

void lhaux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHAUX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = TRUE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lhax_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHAX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = TRUE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lhbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LHBRX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lhzux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_LHZUX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lhzx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_LHZX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lswi_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LSWI;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.extra.string.nb = NB;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void lswx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LSWX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void lwarx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWARX;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lwbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWBRX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void lwzux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWZUX;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = RA;
}

void lwzx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntLoad;
	di->operation.loadStore.ident = ID_LWZX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.write = FALSE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = -1;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = RD;
	di->io.loadStore.dstReg[1] = -1;
}

void stbux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STBUX;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stbx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STBX;
	di->operation.loadStore.size = 1;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stfdux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFDUX;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stfdx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFDX;
	di->operation.loadStore.size = 8;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stfiwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFIWX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stfsux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFSUX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stfsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatStore;
	di->operation.loadStore.ident = ID_STFSX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = TRUE;
	di->io.loadStore.srcReg[0] = FS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void sthbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STHBRX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void sthux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STHUX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void sthx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STHX;
	di->operation.loadStore.size = 2;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stswi_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->operation.loadStore.ident = ID_STSWI;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.extra.string.nb = NB;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stswx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STSWX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = -1;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stwbrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->operation.loadStore.ident = ID_STWBRX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);	
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stwcxd_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STWCXD;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);	
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}

void stwux_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STWUX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = FALSE;
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = RA;
}

void stwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICIntStore;
	di->operation.loadStore.ident = ID_STWX;
	di->operation.loadStore.size = 4;
	di->operation.loadStore.algebraic = FALSE;
	di->operation.loadStore.write = TRUE;
	di->operation.loadStore.nullRA = (RA == 0);
	di->operation.loadStore.floatingPoint = FALSE;
	di->io.loadStore.srcReg[0] = RS;
	di->io.loadStore.srcReg[1] = RA ? RA : -1;
	di->io.loadStore.srcReg[2] = RB;
	di->io.loadStore.dstReg[0] = -1;
	di->io.loadStore.dstReg[1] = -1;
}


void tlbia_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void tlbie_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void tlbsync_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }














void lbz_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lbz", 3, XML_DSTGPR, RD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lbzu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lbzu", 4, XML_DSTGPR, RD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lfd_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfd", 3, XML_DSTFPR, FD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lfdu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfdu", 4, XML_DSTFPR, FD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lfs_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfs", 3, XML_DSTFPR, FD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lfsu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfsu", 4, XML_DSTFPR, FD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lha_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lha", 3, XML_DSTGPR, RD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lhau_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhau", 4, XML_DSTGPR, RD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lhz_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhz", 3, XML_DSTGPR, RD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lhzu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhzu", 4, XML_DSTGPR, RD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lmw_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lmw", 3, XML_DSTGPR, RD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lwz_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwz", 3, XML_DSTGPR, RD, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void lwzu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwzu", 4, XML_DSTGPR, RD, XML_DSTGPR, RA, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stb_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stb", 3, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stbu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stbu", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stfd_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfd", 3, XML_SRCFPR, FS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stfdu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfdu", 4, XML_DSTGPR, RA, XML_SRCFPR, FS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stfs_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfs", 3, XML_SRCFPR, FS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stfsu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfsu", 4, XML_DSTGPR, RA, XML_SRCFPR, FS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void sth_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sth", 3, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void sthu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sthu", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stmw_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stmw", 3, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stw_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stw", 3, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void stwu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stwu", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SIMM, SEXT16(IMM), XML_SRCGPR, RA); }
void dcba_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcba", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void dcbf_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbf", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void dcbi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbi", 1, XML_SRCGPR, RB); }
void dcbst_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbst", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void dcbt_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbt", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void dcbtst_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbtst", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void dcbz_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "dcbz", XML_SRCGPR, RA, XML_SRCGPR, RB); }
void eciwx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "eciw", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void ecowx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "ecow", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void eieio_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "eieio", 0); }
void icbi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "icbi", 2, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lbzux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lbzux", 4, XML_DSTGPR, RD, XML_DSTGPR, RA, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lbzx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lbzx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lfdux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfdux", 4, XML_DSTGPR, RA, XML_DSTFPR, FD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lfdx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfdx f%u, r%u, r%u", 3, XML_DSTFPR, FD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lfsux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfsux", 4, XML_DSTGPR, RA, XML_DSTFPR, FD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lfsx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfsx", 3, XML_DSTFPR, FD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lhaux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lfdux", 4, XML_DSTGPR, RA, XML_DSTFPR, FD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lhax_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhax", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lhbrx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhbrx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lhzux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhzux", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lhzx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lhzx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lswi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lswi", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_UIMM, NB); }
void lswx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lswx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lwarx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwarx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lwbrx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwbrx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lwzux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwzux", 4, XML_DSTGPR, RA, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void lwzx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "lwzx", 3, XML_DSTGPR, RD, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stbux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stbux", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stbx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stbx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stfdux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfdux", 4, XML_DSTGPR, RA, XML_SRCFPR, FS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stfdx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfdx", 3, XML_SRCFPR, FS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stfiwx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfiwx", 3, XML_SRCFPR, FS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stfsux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfsux", 4, XML_DSTGPR, RA, XML_SRCFPR, FS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stfsx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stfsx", 3, XML_SRCFPR, FS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void sthbrx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sthbrx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void sthux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sthux", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void sthx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sthx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stswi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stswi", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_UIMM, NB); }
void stswx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stswx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stwbrx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stwbrx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stwcxd_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stwcx.", 4, XML_DSTCRF, 0, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stwux_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stwux", 4, XML_DSTGPR, RA, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void stwx_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "stwx", 3, XML_SRCGPR, RS, XML_SRCGPR, RA, XML_SRCGPR, RB); }
void tlbia_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "tlbia", 0); }
void tlbie_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "tlbie", 1, XML_SRCGPR, RB); }
void tlbsync_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "tlbsync", 0); }
