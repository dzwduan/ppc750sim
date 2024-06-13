/***************************************************************************
                          floatingpoint.c  -  description
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

#include <floatingpoint.h>
#include <ppcemul.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <xmlize.h>

#define COMPUTE_CR1 (CR = (CR & 0xf0ffffff) | ((FPSCR >> 4) & 0x0f000000))
#define SET_FPSCR_FPCC(c) (FPSCR = (FPSCR & 0xffff0fff) | ((c) << 12))
#define SET_FPSCR_VSXNAN (FPSCR = FPSCR | 0x01000000)
#define IS_NAN(x) (((x) & 0x7ff0000000000000) == 0x7ff0000000000000 && ((x) & 0x000fffffffffffff) != 0)
#define IS_SNAN(x) (((x) & 0x7ff0000000000000) == 0x7ff0000000000000 && ((x) & 0x000fffffffffffff) != 0 && ((x) & 0x0008000000000000) == 0)

BOOL IsNan(UInt64 x)
{
	return IS_NAN(x);
}

BOOL IsSNan(UInt64 x)
{
	return IS_SNAN(x);
}


void faddsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = (float)(a + b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;	
}

void fdivsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = (float)(a / b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fmaddsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (float)((a * c) + b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fmsubsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (float)((a * c) - b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fmulsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double c = FPR_DBL(FC);
	double result = (float)(a * c);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;	
}

void fnmaddsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (float)(-((a * c) + b));
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fnmsubsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (float)(-((a * c) - b));
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fresx_impl(ppc_inst_t inst) { abort(); }
void fsqrtsx_impl(ppc_inst_t inst) { abort(); }

void fsubsx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = (float)(a - b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fabsx_impl(ppc_inst_t inst)
{
	double b = FPR_DBL(FB);
	double result = fabs(b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fcmpo_impl(ppc_inst_t inst) { abort(); }

void fcmpu_impl(ppc_inst_t inst)
{
	double fa = FPR_DBL(FA);
	double fb = FPR_DBL(FB);
	dword_t a = FPR_DW(FA);
	dword_t b = FPR_DW(FB);
	word_t c;
	
	if(IS_NAN(a) || IS_NAN(b)) c = 1; else
	if(fa < fb) c = 8; else
	if(fa > fb) c = 4; else c =2;
	
//	SET_FPSCR_FPCC(c);
	CR = (CR & (~(0xf << ((7 - CRFD) * 4)))) | (c << ((7 - CRFD) * 4));
	if(IS_SNAN(a) || IS_SNAN(b))
	{
//		SET_FPSCR_VSXNAN;
	}
}

void fctiwx_impl(ppc_inst_t inst) { abort(); }

void fctiwzx_impl(ppc_inst_t inst)
{
	double b = FPR_DBL(FB);
	sword_t res = (sword_t) b;
	FPR_DW(FD) = (dword_t)(word_t) res;
}

void fmrx_impl(ppc_inst_t inst)
{
	FPR_DW(FD) = FPR_DW(FB);
	if(Rc) COMPUTE_CR1;
}

void fnabsx_impl(ppc_inst_t inst)
{
	double b = FPR_DBL(FB);
	double result = -fabs(b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fnegx_impl(ppc_inst_t inst)
{
	double b = FPR_DBL(FB);
	double result = -b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}


void frspx_impl(ppc_inst_t inst)
{
	FPR_DBL(FD) = (float) FPR_DBL(FB);
	if(Rc) COMPUTE_CR1;
}

void faddx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = a + b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fdivx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = a / b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fmaddx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (a * c) + b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void fmsubx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = (a * c) - b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;	
}

void fmulx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double c = FPR_DBL(FC);
	double result = a * c;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;	
}

void fnmaddx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = -((a * c) + b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}


void fnmsubx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double c = FPR_DBL(FC);
	double result = -((a * c) - b);
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}

void frsqrtex_impl(ppc_inst_t inst) { abort(); }
void fselx_impl(ppc_inst_t inst) { abort(); }
void fsqrtx_impl(ppc_inst_t inst) { abort(); }

void fsubx_impl(ppc_inst_t inst)
{
	double a = FPR_DBL(FA);
	double b = FPR_DBL(FB);
	double result = a - b;
	FPR_DBL(FD) = result;
	if(Rc) COMPUTE_CR1;
}











char *faddsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = (float)(a + b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fdivsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = (float)(a / b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fmaddsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (float)((a * c) + b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fmsubsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (float)((a * c) - b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fmulsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (float)(a * c);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");	
	return buf;
}

char *fnmaddsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (float)(-((a * c) + b));\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fnmsubsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (float)(-((a * c) - b));\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fresx_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *fsqrtsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *fsubsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = (float)(a - b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fabsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = fabs(b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fcmpo_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *fcmpu_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble fa = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble fb = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdword_t a = FPR_DW(%u);\n", FA);
	buf += sprintf(buf, "\tdword_t b = FPR_DW(%u);\n", FB);
	buf += sprintf(buf, "\tword_t c\n");
	
	buf += sprintf(buf, "\tif(IS_NAN(a) || IS_NAN(b)) c = 1; else\n");
	buf += sprintf(buf, "\tif(fa < fb) c = 8; else\n");
	buf += sprintf(buf, "\tif(fa > fb) c = 4; else c =2\n");
	
//	SET_FPSCR_FPCC(c);\n");
	buf += sprintf(buf, "\tCR = (CR & (~(0xf << ((7 - %u) * 4)))) | (c << ((7 - %u) * 4));\n", CRFD, CRFD);
//if(IS_SNAN(a) || IS_SNAN(b))
//	{
//		SET_FPSCR_VSXNAN\n");
//	}
	return buf;
}

char *fctiwx_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *fctiwzx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tsword_t res = (sword_t) b;\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = (dword_t)(word_t) res;\n", FD);
	return buf;
}

char *fmrx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tFPR_DW(%u) = FPR_DW(%u);\n", FD, FB);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fnabsx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = -fabs(b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fnegx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = -b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}


char *frspx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tFPR_DBL(%u) = (float) FPR_DBL(%u);\n", FD, FB);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *faddx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = a + b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fdivx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = a / b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fmaddx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (a * c) + b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *fmsubx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = (a * c) - b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");	
	return buf;
}

char *fmulx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = a * c;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");	
	return buf;
}

char *fnmaddx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = -((a * c) + b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}


char *fnmsubx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble c = FPR_DBL(%u);\n", FC);
	buf += sprintf(buf, "\tdouble result = -((a * c) - b);\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}

char *frsqrtex_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *fselx_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *fsqrtx_bin2c(ppc_inst_t inst,addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *fsubx_bin2c(ppc_inst_t inst,addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdouble a = FPR_DBL(%u);\n", FA);
	buf += sprintf(buf, "\tdouble b = FPR_DBL(%u);\n", FB);
	buf += sprintf(buf, "\tdouble result = a - b;\n");
	buf += sprintf(buf, "\tFPR_DBL(%u) = result;\n", FD);
	if(Rc) buf += sprintf(buf, "\tCOMPUTE_CR1;\n");
	return buf;
}




















char *faddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fadds");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}

char *fdivsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fdivs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}

char *fmaddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmadds");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fmsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmsubs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fmulsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmuls");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FC);
	return buf;
}

char *fnmaddsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fnmadds");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fnmsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fnmsubs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fresx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fres");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fsqrtsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fsqrts");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fsubsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fsubs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}

char *fabsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fabs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fcmpo_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "fcmpo cr%u, f%u, f%u", CRFD, FA, FB); return buf; }
char *fcmpu_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "fcmpu cr%u, f%u, f%u", CRFD, FA, FB); return buf; }

char *fctiwx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fctiw");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fctiwzx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fctiwz");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fmrx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmr");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fnabsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fnabs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fnegx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fneg");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *frspx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "frsp");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *faddx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fadd");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}

char *fdivx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fdiv");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}

char *fmaddx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmadd");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fmsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmsub");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fmulx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fmul");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FC);
	return buf;
}

char *fnmaddx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fnmadd");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fnmsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fnmsub");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *frsqrtex_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "frsqrte");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fselx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fsel");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u, f%u", FD, FA, FC, FB);
	return buf;
}

char *fsqrtx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fsqrt");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u", FD, FB);
	return buf;
}

char *fsubx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "fsub");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u, f%u, f%u", FD, FA, FB);
	return buf;
}









void faddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FADDSD : ID_FADDS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fdivsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatDiv;
	di->operation.floatingPoint.ident = ID_FDIVS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmaddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FMADDSD : ID_FMADDS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FMSUBSD : ID_FMSUBS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmulsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMul;
	di->operation.floatingPoint.ident = Rc ? ID_FMULSD : ID_FMULS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = -1;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnmaddsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FNMADDSD : ID_FNMADDS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnmsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FNMSUBSD : ID_FNMSUBS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fresx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatConversion;
	di->operation.floatingPoint.ident = Rc ? ID_FRESD : ID_FRES;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fsqrtsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FSQRTSD : ID_FSQRTS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fsubsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FSUBSD : ID_FSUBS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fabsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FABSD : ID_FABS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fcmpo_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = ID_FCMPO;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = TRUE;
	di->io.floatingPoint.crf = CRFD;
}

void fcmpu_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = ID_FCMPU;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = TRUE;
	di->io.floatingPoint.crf = CRFD;
}

void fctiwx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatConversion;
	di->operation.floatingPoint.ident = Rc ? ID_FCTIWD : ID_FCTIW;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fctiwzx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatConversion;
	di->operation.floatingPoint.ident = Rc ? ID_FCTIWZD : ID_FCTIWZ;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	di->io.floatingPoint.outCR = FALSE;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmrx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMove;
	di->operation.floatingPoint.ident = Rc ? ID_FMRD : ID_FMR;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnabsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FNABSD : ID_FNABS;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnegx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatDiv;
	di->operation.floatingPoint.ident = Rc ? ID_FNEG : ID_FNEGD;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void frspx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatConversion;
	di->operation.floatingPoint.ident = Rc ? ID_FRSPD : ID_FRSP;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void faddx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FADDD : ID_FADD;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fdivx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatDiv;
	di->operation.floatingPoint.ident = Rc ? ID_FDIVD : ID_FDIV;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmaddx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FMADDD : ID_FMADD;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FMSUBD : ID_FMSUB;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	di->io.floatingPoint.outCR = FALSE;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fmulx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMul;
	di->operation.floatingPoint.ident = Rc ? ID_FMULD : ID_FMUL;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = -1;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnmaddx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FNMADDD : ID_FNMADD;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fnmsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatMulAdd;
	di->operation.floatingPoint.ident = Rc ? ID_FNMSUBD : ID_FNMSUB;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void frsqrtex_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FRSQRTED : ID_FRSQRTE;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fselx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FSELD : ID_FSEL;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = FC;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fsqrtx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FSQRTD : ID_FSQRT;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	di->io.floatingPoint.outCR = FALSE;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}

void fsubx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICFloatArithmetic;
	di->operation.floatingPoint.ident = Rc ? ID_FSUBD : ID_FSUB;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = FA;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = FD;
	if(Rc)
	{
		di->io.floatingPoint.outCR = TRUE;
		di->io.floatingPoint.crf = 1;
	}
	else
	{
		di->io.floatingPoint.outCR = FALSE;
	}
}










void faddsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fadds.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fadds", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}

void fdivsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fdivs.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fdivs", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}

void fmaddsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmadds.", 5, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fmadds", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fmsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmsubs.", 5, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FA);
	else
		xmlf(buf, pc, "fmsubs", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FA);
}
	
void fmulsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmuls.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FC);
	else
		xmlf(buf, pc, "fmuls", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC);
}

void fnmaddsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fnmadds.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fnmadds", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}
	
void fnmsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fnmsubs.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fnmsubs", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fresx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fres.", 3, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fres", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}	

void fsqrtsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fsqrt.", 3, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fsqrt", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fsubsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fsubs.", 4, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fsubs", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}

void fabsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fabs.", 3, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fabs", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fcmpo_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "fcmpo", 3, XML_DSTCRF, CRFD, XML_SRCFPR, FA, XML_SRCFPR, FB); }
void fcmpu_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "fcmpu", 3, XML_DSTCRF, CRFD, XML_SRCFPR, FA, XML_SRCFPR, FB); }

void fctiwx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fctiw.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fctiw", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fctiwzx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fctiwz.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fctiwz", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fmrx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmr.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fmr", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fnabsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fnabs.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fnabs", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fnegx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fneg.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fneg", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void frspx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "frsp.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "frsp", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void faddx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fadd.", 4, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fadd", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}

void fdivx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fdiv.", 4, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fdiv", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}

void fmaddx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmadd.", 5, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fmadd", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fmsubx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmsub.", 5, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fmsub", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fmulx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fmul.", 4, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC);
	else
		xmlf(buf, pc, "fmul", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC);
}

void fnmaddx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fnmadd.", 5, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fnmadd", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fnmsubx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fnmsub.", 5, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fnmsub", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void frsqrtex_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "frsqrte.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "frsqrte", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fselx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fselx.", 5, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fselx", 4, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FC, XML_SRCFPR, FB);
}

void fsqrtx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "frsqrt.", 3, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "frsqrt", 2, XML_DSTFPR, FD, XML_SRCFPR, FB);
}

void fsubx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "fsub.", 4, XML_DSTCRF, 1, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "fsub", 3, XML_DSTFPR, FD, XML_SRCFPR, FA, XML_SRCFPR, FB);
}
