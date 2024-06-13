/***************************************************************************
                systemregisters.c  -  description
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
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.   *
 * *
 ***************************************************************************/

#include <systemregisters.h>
#include <ppcemul.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmlize.h>

void mcrfs_impl(ppc_inst_t inst) { abort(); }

void mffsx_impl(ppc_inst_t inst)
{
	dword_t _f;
	_f= (dword_t)FPSCR;
	FPR_DW(FD) = _f;
}

void mtfsb0x_impl(ppc_inst_t inst)
{
	if(CRBD != 1 && CRBD != 2)
	{
//		FPSCR = FPSCR & (~(1 << (31 - CRBD)));
	}
}

void mtfsb1x_impl(ppc_inst_t inst) { abort(); }
void mtfsfix_impl(ppc_inst_t inst)
{
//	FPSCR = MTFSFI_IMM;
}

void mtfsfx_impl(ppc_inst_t inst)
{
	word_t _m,_f, _fm, _rb,_c;
	int _i;
	_rb = FPR_DW(FB);
	_f=FPSCR;
	_fm = FM;
	
	if(_fm & 0x80)
	{
		_m = 0x60000000;
	}
	else
	{
		_m=0x0;
	}
	_c=0x1;
	for(_i=0;_i<7;_i++)
	{
		if(_c & _fm)
		{
			_m= _m | (0xf<<(_i*4));
		}
		_c=_c<<1;
	}
	_f = _rb & _m;
//	FPSCR = _f;
}

void mcrxr_impl(ppc_inst_t inst) { abort(); }

void mfcr_impl(ppc_inst_t inst)
{
	GPR(RD) = CR;
}

void mfmsr_impl(ppc_inst_t inst) { abort(); }
void mfsr_impl(ppc_inst_t inst) { abort(); }
void mfsrin_impl(ppc_inst_t inst) { abort(); }
void mtmsr_impl(ppc_inst_t inst) { abort(); }
void mtsr_impl(ppc_inst_t inst) { abort(); }
void mtsrin_impl(ppc_inst_t inst) { abort(); }

void mfspr_impl(ppc_inst_t inst)
{
	word_t _s, _d;
	_s = SPR;
	_d = ((_s & 0x1F)<< 5) + (( _s >> 5)&0x1F);
	if (_d ==1)
	{
		GPR(RD) = XER;
	}	
	else if (_d ==8)
	{
		GPR(RD) = LR;
	}
	else if (_d == 9)
	{
		GPR(RD) = CTR;
	}
}

void mftb_impl(ppc_inst_t inst)
{
	word_t _d, _s;
	_s = TBR;
	_d = ((_s & 0x1F) << 5) + (( _s >> 5)&0x1F);
	switch(_d)
	{
		case 268:
			GPR(RD) = TBL;
			break;
			
		case 269:
			GPR(RD) = TBU;
			break;
	}
}

void mtcrf_impl(ppc_inst_t inst)
{
	word_t _m,_crm, _c, _a;
	int _i, _b, _d;
	_crm = CRM;
	_d = GPR(RS);
	for(_i = 0; _i < 8; _i++)
	{
		_b = (_crm >> _i) & 0x1;
		if(_b == 1)
		{
			_c = CR;
			_a = (_d >> (_i*4) ) & 0xf;
			_m = (0xf << ( (_i)*4) );
			CR = (_c & ~_m) | (_a << ((_i)*4));
		}
	}
}

void mtspr_impl(ppc_inst_t inst)
{
	word_t _s, _d;
	_s = SPR;
	_d = ((_s & 0x1f) << 5) + (( _s >> 5)&0x1F);
	if (_d ==1)
	{
		XER = GPR(RD);
	}
	else if (_d ==8)
	{
		LR = GPR(RD);
	}
	else if (_d == 9)
	{
		CTR = GPR(RD);
	}
}

void crand_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (_a & _b)<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void crandc_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (_a & (!_b))<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void creqv_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (!(_a ^ _b))<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void crnand_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (!(_a & _b))<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void crnor_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (!(_a | _b))<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void cror_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (_a | _b)<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void crorc_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (_a | (!_b))<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void crxor_impl(ppc_inst_t inst)
{
	word_t _a, _b, _c, _d, _m;
	_c = CR;
	_a = (0x1)  & (_c >> (31- CRBA));
	_b = (0x1)  & (_c >> (31- CRBB));
	_d = (_a ^ _b)<<(31-CRBD);
	_m = ~(1<<(31-CRBD));
	CR = ((_c & _m) | _d );
}

void mcrf_impl(ppc_inst_t inst)
{
	CR = (CR & (~(0xf << (4 * (7 - CRFD))))) | (((CR >> (4 * (7 - CRFS))) & 0xf) << (4 * (7 - CRFD)));
}

void rfi_impl(ppc_inst_t inst) { abort(); }
void rfid_impl(ppc_inst_t inst) { abort(); }	
void sync_impl(ppc_inst_t inst) { abort(); }
void isync_impl(ppc_inst_t inst) { abort(); }















char *mcrfs_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *mffsx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdword_t _f;\n");
	buf += sprintf(buf, "\t_f= (dword_t)FPSCR;\n");
	buf += sprintf(buf, "\tFPR_DW(%u) = _f;\n", FD);
	return buf;
}

char *mtfsb0x_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
//	if(CRBD != 1 && CRBD != 2)
//	{
//		FPSCR = FPSCR & (~(1 << (31 - CRBD)));
//	}
	strcpy(buf, "");
	return buf;
}

char *mtfsb1x_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mtfsfix_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
//	FPSCR = MTFSFI_IMM;
	strcpy(buf, "");
	return buf;
}

char *mtfsfx_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _m,_f, _fm, _rb,_c;\n");
	buf += sprintf(buf, "\tint _i;\n");
	buf += sprintf(buf, "\t_rb = FPR_DW(%u);\n", FB);
	buf += sprintf(buf, "\t_f=FPSCR;\n");
	buf += sprintf(buf, "\t_fm = %u;\n", FM);
	
	buf += sprintf(buf, "\tif(_fm & 0x80)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\t_m = 0x60000000;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\telse\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\t_m=0x0;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\t_c=0x1;\n");
	buf += sprintf(buf, "\tfor(_i=0;_i<7;_i++)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\tif(_c & _fm)\n");
	buf += sprintf(buf, "\t\t{\n");
	buf += sprintf(buf, "\t\t\t_m= _m | (0xf<<(_i*4));\n");
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t\t_c=_c<<1;\n");
	buf += sprintf(buf, "\t}\n");
	buf += sprintf(buf, "\t_f = _rb & _m;\n");
//	FPSCR = _f;
	return buf;
}

char *mcrxr_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *mfcr_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "GPR(%u) = CR;\n", RD);
	return buf;
}

char *mfmsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mfsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mfsrin_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mtmsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mtsr_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *mtsrin_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

char *mfspr_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	word_t _s, _d;;
	_s = SPR;
	_d = ((_s & 0x1F)<< 5) + (( _s >> 5)&0x1F);
	if (_d ==1)
		buf += sprintf(buf, "\tGPR(%u) = XER;\n", RD);
	else if (_d ==8)
		buf += sprintf(buf, "\tGPR(%u) = LR;\n", RD);
	else if (_d == 9)
		buf += sprintf(buf, "\tGPR(%u) = CTR;\n", RD);
	return buf;
}

char *mftb_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	word_t _d, _s;
	_s = TBR;
	_d = ((_s & 0x1F) << 5) + (( _s >> 5)&0x1F);
	switch(_d)
	{
		case 268:
			buf += sprintf(buf, "\tGPR(%u) = TBL;\n", RD);
			break;
		case 269:
			buf += sprintf(buf, "\tGPR(%u) = TBU;\n", RD);
			break;
	}
	return buf;
}

char *mtcrf_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _m,_crm, _c, _a;\n");
	buf += sprintf(buf, "\tint _i, _b, _d;\n");
	buf += sprintf(buf, "\t_crm = %u;\n", CRM);
	buf += sprintf(buf, "\t_d = GPR(%u);\n", RS);
	buf += sprintf(buf, "\tfor(_i = 0; _i < 8; _i++)\n");
	buf += sprintf(buf, "\t{\n");
	buf += sprintf(buf, "\t\t_b = (_crm >> _i) & 0x1;\n");
	buf += sprintf(buf, "\t\tif(_b == 1)\n");
	buf += sprintf(buf, "\t\t{\n");
	buf += sprintf(buf, "\t\t\t_c = CR;\n");
	buf += sprintf(buf, "\t\t\t_a = (_d >> (_i*4) ) & 0xf;\n");
	buf += sprintf(buf, "\t\t\t_m = (0xf << ( (_i)*4) );\n");
	buf += sprintf(buf, "\t\t\tCR = (_c & ~_m) | (_a << ((_i)*4));\n");
	buf += sprintf(buf, "\t\t}\n");
	buf += sprintf(buf, "\t}\n");
	return buf;
}

char *mtspr_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	word_t _s, _d;
	_s = SPR;
	_d = ((_s & 0x1f) << 5) + (( _s >> 5)&0x1F);
	if (_d ==1)
		buf += sprintf(buf, "\tXER = GPR(%u);\n", RD);
	else if (_d ==8)
		buf += sprintf(buf, "\tLR = GPR(%u);\n", RD);
	else if (_d == 9)
		buf += sprintf(buf, "\tCTR = GPR(%u);\n", RD);
	return buf;
}

char *crand_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (_a & _b)<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *crandc_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (_a & (!_b))<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *creqv_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (!(_a ^ _b))<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *crnand_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (!(_a & _b))<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *crnor_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (!(_a | _b))<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *cror_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (_a | _b)<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *crorc_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (_a | (!_b))<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *crxor_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tword_t _a, _b, _c, _d, _m;\n");
	buf += sprintf(buf, "\t_c = CR;\n");
	buf += sprintf(buf, "\t_a = (0x1)  & (_c >> (31- %u));\n", CRBA);
	buf += sprintf(buf, "\t_b = (0x1)  & (_c >> (31- %u));\n", CRBB);
	buf += sprintf(buf, "\t_d = (_a ^ _b)<<(31-%u);\n", CRBD);
	buf += sprintf(buf, "\t_m = ~(1<<(31-%u));\n", CRBD);
	buf += sprintf(buf, "\tCR = ((_c & _m) | _d );\n");
	return buf;
}

char *mcrf_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tCR = (CR & (~(0xf << (4 * (7 - %u))))) | (((CR >> (4 * (7 - %u))) & 0xf) << (4 * (7 - %u)));\n", CRFD, CRFS, CRFD);
	return buf;
}

char *rfi_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *rfid_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }	
char *sync_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }
char *isync_bin2c(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "\tabort();\n"); return buf; }

















char *mcrfs_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mcrfs cr%u, cr%u", CRFD, CRFS); return buf; }
char *mffsx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mffs");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " f%u", FD);
	return buf;
}

char *mtfsb0x_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mtfsb0");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " crb%u", CRBD);
	return buf;
}

char *mtfsb1x_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mtfsb1");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " crb%u", CRBD);
	return buf;
}

char *mtfsfix_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mtfsfi");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " crf%u, %u", CRFD, MTFSFI_IMM);
	return buf;
}

char *mtfsfx_disasm(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "mtfsf");
	if(Rc) buf += sprintf(buf, ".");
	buf += sprintf(buf, " %u, f%u", FM, FB);
	return buf;
}

char *mcrxr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mcrxr cr%u", CRFD); return buf; }
char *mfcr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mfcr r%u", RD); return buf; }
char *mfmsr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mfmsr r%u", RD); return buf; }
char *mfsr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mfsr r%u, %u", RD, SR); return buf; }
char *mfsrin_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mfsrin r%u, r%u", RD, RB); return buf; }
char *mtmsr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mtmsr r%u", RS); return buf; }
char *mtsr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mtsr %u, r%u", SR, RS); return buf; }
char *mtsrin_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mtsrin r%u, r%u", RS, RB); return buf; }
char *mfspr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mfspr r%u, %u", RD, SPR); return buf; }
char *mftb_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mftb r%u, %u", RD, TBR); return buf; }
char *mtcrf_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mtcrf %u, r%u", CRM, RS); return buf; }
char *mtspr_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mtspr %u, r%u", SPR, RS); return buf; }
char *crand_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crand cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *crandc_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crandc cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *creqv_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "creqv cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *crnand_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crnand cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *crnor_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crnor cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *cror_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "cror cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *crorc_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crorc cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *crxor_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "crxor cr%u, cr%u, cr%u", CRBD, CRBA, CRBB); return buf; }
char *mcrf_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "mcrf cr%u, cr%u", CRFD, CRFS); return buf; }
char *rfi_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "rfi"); return buf; }
char *rfid_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "rfid"); return buf; }	
char *sync_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sync"); return buf; }
char *isync_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "isync"); return buf; }






void mcrfs_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void mffsx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICMoveFromFPSCR;
	di->operation.floatingPoint.ident = Rc ? ID_MFFSD : ID_MFFS;
	di->io.floatingPoint.inFPSCR = TRUE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = -1;
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

void mtfsb0x_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICMoveToFPSCR;
	di->operation.floatingPoint.ident = Rc ? ID_MTFSB0D : ID_MTFSB0;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = -1;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = FALSE;
}

void mtfsb1x_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICMoveToFPSCR;
	di->operation.floatingPoint.ident = Rc ? ID_MTFSB1D : ID_MTFSB1;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = -1;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = FALSE;
}

void mtfsfix_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICMoveToFPSCR;
	di->operation.floatingPoint.ident = Rc ? ID_MTFSFID : ID_MTFSFI;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = -1;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = FALSE;
}

void mtfsfx_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->hasImmed = FALSE;
	di->iclass = ICMoveToFPSCR;
	di->operation.floatingPoint.ident = Rc ? ID_MTFSFD : ID_MTFSF;
	di->io.floatingPoint.inFPSCR = FALSE;
	di->io.floatingPoint.srcReg[0] = -1;
	di->io.floatingPoint.srcReg[1] = FB;
	di->io.floatingPoint.srcReg[2] = -1;
	di->io.floatingPoint.dstReg = -1;
	di->io.floatingPoint.outCR = FALSE;
}

void mcrxr_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void mfcr_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICMoveFromCR;
	di->operation.systemRegister.ident = ID_MFCR;
	di->hasImmed = FALSE;
	di->io.mfcr.dstReg = RD;
}

void mfmsr_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void mfsr_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void mfsrin_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void mtmsr_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void mtsr_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void mtsrin_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }

void mfspr_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	word_t _s, _d;
	_s = SPR;
	_d = ((_s & 0x1F) << 5) + (( _s >> 5)&0x1F);
	di->iclass = ICMoveFromSpr;
	di->hasImmed = FALSE;
	di->operation.systemRegister.ident = ID_MFSPR;
	di->operation.systemRegister.extra.mfspr.spr = _d;
	di->io.mfspr.srcSpr = _d;
	di->io.mfspr.dstReg = RD;
}

void mftb_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	word_t _s, _d;
	_s = TBR;
	_d = ((_s & 0x1F) << 5) + (( _s >> 5)&0x1F);
	di->iclass = ICMoveFromTbr;
	di->hasImmed = FALSE;
	di->operation.systemRegister.ident = ID_MFTB;
	di->operation.systemRegister.extra.mftb.tbr = _d;
	di->io.mftb.srcTbr = _d;
	di->io.mftb.dstReg = RD;
}

void mtcrf_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	word_t _crm;
	_crm = CRM;
	di->iclass = ICMoveToCrf;
	di->operation.systemRegister.ident = ID_MTCRF;
	di->operation.systemRegister.extra.mtcrf.gpr = RS;
	di->operation.systemRegister.extra.mtcrf.crm = _crm;
	di->hasImmed = FALSE;
	di->io.mtcrf.srcReg = RS;
	switch(_crm)
	{
		case 128: di->io.mtcrf.dstCrf = 0; break;
		case 64: di->io.mtcrf.dstCrf = 1; break;
		case 32: di->io.mtcrf.dstCrf = 2; break;
		case 16: di->io.mtcrf.dstCrf = 3; break;
		case 8: di->io.mtcrf.dstCrf = 4; break;
		case 4: di->io.mtcrf.dstCrf = 5; break;
		case 2: di->io.mtcrf.dstCrf = 6; break;
		case 1: di->io.mtcrf.dstCrf = 7; break;
		default: di->io.mtcrf.dstCrf = -1;
	}
}

void mtspr_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	word_t _s, _d;
	_s = SPR;
	_d = ((_s & 0x1f) << 5) + (( _s >> 5)&0x1F);
	di->iclass = ICMoveToSpr;
	di->hasImmed = FALSE;
	di->operation.systemRegister.ident = ID_MTSPR;
	di->operation.systemRegister.extra.mtspr.gpr = RD;
	di->io.mtspr.dstSpr = _d;
	di->io.mtspr.srcReg = RD;
}

void crand_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRAND;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void crandc_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRANDC;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void creqv_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CREQV;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void crnand_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRNAND;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void crnor_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRNOR;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void cror_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CROR;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void crorc_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRORC;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void crxor_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICCRLogical;
	di->operation.systemRegister.ident = ID_CRXOR;
	di->operation.systemRegister.extra.crlogical.srcCrb[0] = CRBA;
	di->operation.systemRegister.extra.crlogical.srcCrb[1] = CRBB;
	di->operation.systemRegister.extra.crlogical.dstCrb = CRBD;
	di->hasImmed = FALSE;
	di->io.crlogical.srcCrf[0] = CRBA/4;
	di->io.crlogical.srcCrf[1] = CRBB/4;
	di->io.crlogical.dstCrf = CRBD/4;
}

void mcrf_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICMoveCrf;
	di->operation.systemRegister.ident = ID_MCRF;
	di->operation.systemRegister.extra.mcrf.srcCrf = CRFS;
	di->hasImmed = FALSE;
	di->io.mcrf.srcCrf = CRFS;
	di->io.mcrf.dstCrf = CRFD;
}

void rfi_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void rfid_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }	
void sync_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }
void isync_translate(ppc_inst_t inst, struct DecodedInstruction *di) { di->iclass = 0; }













void mcrfs_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mcrfs", 2, XML_DSTCRF, CRFD, XML_SRCCRF, CRFS); }
void mffsx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mffs.", 3, XML_DSTFPR, FD, XML_DSTCRF, 1, XML_SRCFPSCR, 0);
	else
		xmlf(buf, pc, "mffs", 2, XML_DSTFPR, FD, XML_SRCFPSCR, 0);
}

void mtfsb0x_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mtfsb0.", 2, XML_DSTCRF, 1, XML_DSTFPSCR, 0);
	else
		xmlf(buf, pc, "mtfsb0", 1, XML_DSTFPSCR, 0);
}

void mtfsb1x_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mtfsb1.", 2, XML_DSTCRF, 1, XML_DSTFPSCR, 0);
	else
		xmlf(buf, pc, "mtfsb1", 1, XML_DSTFPSCR, 0);
}

void mtfsfix_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mtfsfi.", 3, XML_DSTCRF, 1, XML_DSTFPSCR, 0, XML_UIMM, MTFSFI_IMM);
	else
		xmlf(buf, pc, "mtfsfi", 2, XML_DSTFPSCR, 0, XML_UIMM, MTFSFI_IMM);
}

void mtfsfx_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	if(Rc)
		xmlf(buf, pc, "mtfsf.", 3, XML_DSTCRF, 1, XML_DSTFPSCR, 0, XML_SRCFPR, FB);
	else
		xmlf(buf, pc, "mtfsf", 2, XML_DSTFPSCR, 0, XML_SRCFPR, FB);
}

void mcrxr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mcrxr", 2, XML_DSTCRF, CRFD, XML_SRCSPR, 1); }
void mfcr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mfcr", 2, XML_DSTGPR, RD, XML_SRCCRF, -1); }
void mfmsr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mfmsr", 0);}
void mfsr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mfsr", 0); }
void mfsrin_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mfsrin", 0); }
void mtmsr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mtmsr", 0); }
void mtsr_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mtsr", 0); }
void mtsrin_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mtsrin", 0); }

void mfspr_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	word_t _s, _d;
	_s = SPR;
	_d = (_s & 0x1F) + (( _s >> 5)&0x1F);
	xmlf(buf, pc, "mfspr", 2, XML_DSTGPR, RD, XML_SRCSPR, _d);
}

void mftb_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mftb", 0); }
void mtcrf_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "mtcrf", 2, XML_DSTCRF, -1, XML_SRCGPR, RS); }

void mtspr_xml(ppc_inst_t inst, addr_t pc, char *buf)
{
	word_t _s, _d;
	_s = SPR;
	_d = (_s & 0x1F) + (( _s >> 5)&0x1F);
	xmlf(buf, pc, "mtspr", 2, XML_DSTSPR, _d, XML_SRCGPR, RS);
}

void crand_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crand", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void crandc_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crandc", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void creqv_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "creqv", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void crnand_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crnand", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void crnor_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crnor", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void cror_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "cror", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void crorc_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crorc", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void crxor_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crxor", 2, XML_DSTCRF, -1, XML_SRCCRF, -1); }
void mcrf_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "crand", 2, XML_DSTCRF, CRFD, XML_SRCCRF, CRFS); }
void rfi_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "rfi", 0); }
void rfid_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "rfid", 0); }	
void sync_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sync", 0); }
void isync_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "isync", 0); }
