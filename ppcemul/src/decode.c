/***************************************************************************
                          decode.c  -  description
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

#include <ppcemul.h>
#include <integer.h>
#include <loadstore.h>
#include <branch.h>
#include <syscall.h>
#include <floatingpoint.h>
#include <systemregisters.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include <xmlize.h>



void (*impl_table[64])(ppc_inst_t);
void (*impl_table31[1024])(ppc_inst_t);
void (*impl_table63[1024])(ppc_inst_t);
void (*impl_table19[1024])(ppc_inst_t);
void (*impl_table59[32])(ppc_inst_t);

char *(*bin2c_table[64])(ppc_inst_t, addr_t, char *);
char *(*bin2c_table31[1024])(ppc_inst_t, addr_t, char *);
char *(*bin2c_table63[1024])(ppc_inst_t, addr_t, char *);
char *(*bin2c_table19[1024])(ppc_inst_t, addr_t, char *);
char *(*bin2c_table59[32])(ppc_inst_t, addr_t, char *);

char *(*disasm_table[64])(ppc_inst_t, addr_t, char *);
char *(*disasm_table31[1024])(ppc_inst_t, addr_t, char *);
char *(*disasm_table63[1024])(ppc_inst_t, addr_t, char *);
char *(*disasm_table19[1024])(ppc_inst_t, addr_t, char *);
char *(*disasm_table59[32])(ppc_inst_t, addr_t, char *);

void (*translate_table[64])(ppc_inst_t, struct DecodedInstruction *di);
void (*translate_table31[1024])(ppc_inst_t, struct DecodedInstruction *di);
void (*translate_table63[1024])(ppc_inst_t, struct DecodedInstruction *di);
void (*translate_table19[1024])(ppc_inst_t, struct DecodedInstruction *di);
void (*translate_table59[32])(ppc_inst_t, struct DecodedInstruction *di);

void (*xml_table[64])(ppc_inst_t, addr_t, char *);
void (*xml_table31[1024])(ppc_inst_t, addr_t, char *);
void (*xml_table63[1024])(ppc_inst_t, addr_t, char *);
void (*xml_table19[1024])(ppc_inst_t, addr_t, char *);
void (*xml_table59[32])(ppc_inst_t, addr_t, char *);

UInt64 index_table[64];
UInt64 index_table31[1024];
UInt64 index_table63[1024];
UInt64 index_table19[1024];
UInt64 index_table59[32];

int ninstructions;
UInt64 instruction_counter;
char *mnemonic_table[1024];
int counters[1024];

void impl_31(ppc_inst_t inst)
{
	impl_table31[XO_10](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table31[XO_10]]++;
	instruction_counter++;
#endif
}

void impl_63(ppc_inst_t inst)
{
	impl_table63[XO_10](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table63[XO_10]]++;
	instruction_counter++;
#endif
}

void impl_19(ppc_inst_t inst)
{
	impl_table19[XO_10](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table19[XO_10]]++;
	instruction_counter++;
#endif
}

void impl_59(ppc_inst_t inst)
{
	impl_table59[XO_5](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table59[XO_5]]++;
	instruction_counter++;
#endif
}

void impl_default(ppc_inst_t inst)
{
	fprintf(stderr, "Attempted to execute a bogus opcode at 0x%08x (OPCD = %d, XO_10 = %d, XO_9 = %d, XO_5 = %d)\n", PC, OPCD, XO_10, XO_9, XO_5);
	ppc_debug_disasm(stderr, PC);
	abort();
}

void ppc_execute(ppc_inst_t inst)
{
	NPC = PC + sizeof(ppc_inst_t);
	impl_table[OPCD](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table[OPCD]]++;
	instruction_counter++;
#endif
	PC = NPC;
}

void ppc_execute_check_path(ppc_inst_t inst, addr_t pc)
{
	if(pc != PC)
	{
		fprintf(stderr, "Not the good path (pc = 0x%08x instead of 0x%08x) !\n", pc, PC);
		abort();
	}
	if(ppc_fetch(PC) != inst)
	{
		fprintf(stderr, "Emulator does not execute the good instruction !\n");
		abort();
	}
	NPC = PC + sizeof(ppc_inst_t);
	impl_table[OPCD](inst);
#ifdef COUNT_INSTRUCTIONS
	counters[index_table[OPCD]]++;
	instruction_counter++;
#endif
	PC = NPC;
}



char *bin2c_31(ppc_inst_t inst, addr_t pc, char *buf)
{
	return bin2c_table31[XO_10](inst, pc, buf);
}

char *bin2c_63(ppc_inst_t inst, addr_t pc, char *buf)
{
	return bin2c_table63[XO_10](inst, pc, buf);
}

char *bin2c_19(ppc_inst_t inst, addr_t pc, char *buf)
{
	return bin2c_table19[XO_10](inst, pc, buf);
}

char *bin2c_59(ppc_inst_t inst, addr_t pc, char *buf)
{
	return bin2c_table59[XO_5](inst, pc, buf);
}

char *bin2c_default(ppc_inst_t inst, addr_t pc, char *buf)
{
//	buf += sprintf(buf, "\tfprintf(stderr, \"Bogus opcode\\n\");\n");
//	buf += sprintf(buf, "\tabort();\n");
	buf += sprintf(buf, "");
	return buf;
}

char *ppc_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "pc_%08x: /* ", pc);
	buf = ppc_disassemble(inst, pc, buf);
	buf += sprintf(buf, " */\n");
	buf += sprintf(buf, "{\n");
	buf = bin2c_table[OPCD](inst, pc, buf);
	buf += sprintf(buf, "}\n");	
	return buf;
}

void ppc_bin2c_to_stream(ppc_inst_t inst, addr_t pc, FILE *stream)
{
	char buf[4096];
	ppc_bin2c(inst, pc, buf);
	fprintf(stream, "%s", buf);
}


void xml_31(ppc_inst_t inst, addr_t pc, char *buf)
{
	xml_table31[XO_10](inst, pc, buf);
}

void xml_63(ppc_inst_t inst, addr_t pc, char *buf)
{
	xml_table63[XO_10](inst, pc, buf);
}

void xml_19(ppc_inst_t inst, addr_t pc, char *buf)
{
	xml_table19[XO_10](inst, pc, buf);
}

void xml_59(ppc_inst_t inst, addr_t pc, char *buf)
{
	xml_table59[XO_5](inst, pc, buf);
}

void xml_default(ppc_inst_t inst, addr_t pc, char *buf)
{
	xmlf(buf, pc, "?", 0);
}


void ppc_xmlize(ppc_inst_t inst, addr_t pc, char *buf)
{
	xml_table[OPCD](inst, pc, buf);
}

void ppc_xmlize_to_stream(ppc_inst_t inst, addr_t pc, FILE *stream)
{
	char buf[4096];
	ppc_xmlize(inst, pc, buf);
	fprintf(stream, "%s", buf);
}

char *disasm_31(ppc_inst_t inst, addr_t pc, char *buf)
{
	return disasm_table31[XO_10](inst, pc, buf);
}

char *disasm_63(ppc_inst_t inst, addr_t pc, char *buf)
{
	return disasm_table63[XO_10](inst, pc, buf);
}

char *disasm_19(ppc_inst_t inst, addr_t pc, char *buf)
{
	return disasm_table19[XO_10](inst, pc, buf);
}

char *disasm_59(ppc_inst_t inst, addr_t pc, char *buf)
{
	return disasm_table59[XO_5](inst, pc, buf);
}

char *disasm_default(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "????");
	return buf;
}

char *ppc_disassemble(ppc_inst_t inst, addr_t pc, char *buf)
{
	return disasm_table[OPCD](inst, pc, buf);
}

char *ppc_disassemble2(ppc_inst_t inst, addr_t pc)
{
	static char buffer[256];
	ppc_disassemble(inst, pc, buffer);
	return buffer;
}

void ppc_disassemble_to_stream(ppc_inst_t inst, addr_t pc, FILE *stream)
{
	char buf[256];
	ppc_disassemble(inst, pc, buf);
	fprintf(stream, "0x%08x : 0x%08x    %s", pc, inst, buf);
}

void translate_31(ppc_inst_t inst, struct DecodedInstruction *di)
{
	translate_table31[XO_10](inst, di);
}

void translate_63(ppc_inst_t inst, struct DecodedInstruction *di)
{
	translate_table63[XO_10](inst, di);
}

void translate_19(ppc_inst_t inst, struct DecodedInstruction *di)
{
	translate_table19[XO_10](inst, di);
}

void translate_59(ppc_inst_t inst, struct DecodedInstruction *di)
{
	translate_table59[XO_5](inst, di);
}

void translate_default(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = 0;
}

void ppc_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	translate_table[OPCD](inst, di);
}


void init_table(int opcd, void (*impl)(ppc_inst_t), char *(*bin2c)(ppc_inst_t, addr_t, char *), char *(*disasm)(ppc_inst_t, addr_t, char *), void (*translate)(ppc_inst_t, struct DecodedInstruction *), void (*xmlize)(ppc_inst_t, addr_t, char *), char *mnemonic)
{
	impl_table[opcd] = impl;
	bin2c_table[opcd] = bin2c;
	disasm_table[opcd] = disasm;
	translate_table[opcd] = translate;
	xml_table[opcd] = xmlize;
	mnemonic_table[ninstructions] = mnemonic;
	index_table[opcd] = ninstructions++;
}

void init_table59(int xo, void (*impl)(ppc_inst_t), char *(*bin2c)(ppc_inst_t, addr_t, char *), char *(*disasm)(ppc_inst_t, addr_t, char *), void (*translate)(ppc_inst_t, struct DecodedInstruction *), void (*xmlize)(ppc_inst_t, addr_t, char *), char *mnemonic)
{
	impl_table59[xo] = impl;
	bin2c_table59[xo] = bin2c;
	disasm_table59[xo] = disasm;
	translate_table59[xo] = translate;
	xml_table[xo] = xmlize;
	mnemonic_table[ninstructions] = mnemonic;
	index_table59[xo] = ninstructions++;
}

void init_table19(int xo, void (*impl)(ppc_inst_t), char *(*bin2c)(ppc_inst_t, addr_t, char *), char *(*disasm)(ppc_inst_t, addr_t, char *), void (*translate)(ppc_inst_t, struct DecodedInstruction *), void (*xmlize)(ppc_inst_t, addr_t, char *), char *mnemonic)
{
	impl_table19[xo] = impl;
	bin2c_table19[xo] = bin2c;
	disasm_table19[xo] = disasm;
	translate_table19[xo] = translate;
	xml_table[xo] = xmlize;
	mnemonic_table[ninstructions] = mnemonic;
	index_table19[xo] = ninstructions++;
}

void init_table31(int xo, int bitcount, void (*impl)(ppc_inst_t), char *(*bin2c)(ppc_inst_t, addr_t, char *), char *(*disasm)(ppc_inst_t, addr_t, char *), void (*translate)(ppc_inst_t, struct DecodedInstruction *), void (*xmlize)(ppc_inst_t, addr_t, char *), char *mnemonic)
{
	int d = 10 - bitcount;
	int n = 1 << d;
	int i;
	
	for(i = 0; i < n; i++)
	{
		impl_table31[xo | (i << bitcount)] = impl;
		bin2c_table31[xo | (i << bitcount)] = bin2c;
		disasm_table31[xo | (i << bitcount)] = disasm;
		translate_table31[xo | (i << bitcount)] = translate;
		xml_table31[xo | (i << bitcount)] = xmlize;
		index_table31[xo | (i << bitcount)] = ninstructions;
	}
	mnemonic_table[ninstructions] = mnemonic;
	ninstructions++;
}

void init_table63(int xo, int bitcount, void (*impl)(ppc_inst_t), char *(*bin2c)(ppc_inst_t, addr_t, char *), char *(*disasm)(ppc_inst_t, addr_t, char *), void (*translate)(ppc_inst_t, struct DecodedInstruction *), void (*xmlize)(ppc_inst_t, addr_t, char *), char *mnemonic)
{
	int d = 10 - bitcount;
	int n = 1 << d;
	int i;
	
	for(i = 0; i < n; i++)
	{
		impl_table63[xo | (i << bitcount)] = impl;
		bin2c_table31[xo | (i << bitcount)] = bin2c;
		disasm_table63[xo | (i << bitcount)] = disasm;
		translate_table63[xo | (i << bitcount)] = translate;
		xml_table63[xo | (i << bitcount)] = xmlize;
		index_table63[xo | (i << bitcount)] = ninstructions;
	}
	mnemonic_table[ninstructions] = mnemonic;
	ninstructions++;
}

void ppc_clear_instruction_counters()
{
	int i;
	for(i = 0; i < ninstructions; i++)
	{
		counters[i] = 0;
	}
	instruction_counter = 0;
}

void ppc_init_decode()
{
	int i;
	
	ninstructions = 0;
	instruction_counter = 0;
	
	for(i = 0; i < 64; i++) impl_table[i] = impl_default;
	for(i = 0; i < 1024; i++) impl_table31[i] = impl_default;
	for(i = 0; i < 1024; i++) impl_table63[i] = impl_default;
	for(i = 0; i < 1024; i++) impl_table19[i] = impl_default;
	for(i = 0; i < 32; i++) impl_table59[i] = impl_default;
		
	for(i = 0; i < 64; i++) bin2c_table[i] = bin2c_default;
	for(i = 0; i < 1024; i++) bin2c_table31[i] = bin2c_default;
	for(i = 0; i < 1024; i++) bin2c_table63[i] = bin2c_default;
	for(i = 0; i < 1024; i++) bin2c_table19[i] = bin2c_default;
	for(i = 0; i < 32; i++) bin2c_table59[i] = bin2c_default;
	
	for(i = 0; i < 64; i++) disasm_table[i] = disasm_default;
	for(i = 0; i < 1024; i++) disasm_table31[i] = disasm_default;
	for(i = 0; i < 1024; i++) disasm_table63[i] = disasm_default;
	for(i = 0; i < 1024; i++) disasm_table19[i] = disasm_default;
	for(i = 0; i < 32; i++) disasm_table59[i] = disasm_default;

	for(i = 0; i < 64; i++) translate_table[i] = translate_default;
	for(i = 0; i < 1024; i++) translate_table31[i] = translate_default;
	for(i = 0; i < 1024; i++) translate_table63[i] = translate_default;
	for(i = 0; i < 1024; i++) translate_table19[i] = translate_default;
	for(i = 0; i < 32; i++) translate_table59[i] = translate_default;

	for(i = 0; i < 64; i++) xml_table[i] = xml_default;
	for(i = 0; i < 1024; i++) xml_table31[i] = xml_default;
	for(i = 0; i < 1024; i++) xml_table63[i] = xml_default;
	for(i = 0; i < 1024; i++) xml_table19[i] = xml_default;
	for(i = 0; i < 32; i++) xml_table59[i] = xml_default;	
		
	for(i = 0; i < 1024; i++) mnemonic_table[i] = "?";
	
	for(i = 0; i < 64; i++) index_table[i] = 0;
	for(i = 0; i < 1024; i++) index_table31[i] = 0;
	for(i = 0; i < 1024; i++) index_table63[i] = 0;
	for(i = 0; i < 1024; i++) index_table19[i] = 0;
	for(i = 0; i < 32; i++) index_table59[i] = 0;
	
	init_table(14, addi_impl, addi_bin2c, addi_disasm, addi_translate, addi_xml, "addi");
	init_table(12, addic_impl, addic_bin2c, addic_disasm, addic_translate, addic_xml, "addic");
	init_table(13, addicd_impl, addicd_bin2c, addicd_disasm, addicd_translate, addicd_xml, "addic.");
	init_table(15, addis_impl, addis_bin2c, addis_disasm, addis_translate, addis_xml, "addis");
	init_table(28, andid_impl, andid_bin2c, andid_disasm, andid_translate, andid_xml, "andi.");
	init_table(29, andisd_impl, andisd_bin2c, andisd_disasm, andisd_translate, andisd_xml, "andis.");
	init_table(11, cmpi_impl, cmpi_bin2c, cmpi_disasm, cmpi_translate, cmpi_xml, "cmpi");
	init_table(10, cmpli_impl, cmpli_bin2c, cmpli_disasm, cmpli_translate, cmpli_xml, "cmpli");
	init_table(34, lbz_impl, lbz_bin2c, lbz_disasm, lbz_translate, lbz_xml, "lbz");
	init_table(35, lbzu_impl, lbzu_bin2c, lbzu_disasm, lbzu_translate, lbzu_xml, "lbzu");
	init_table(50, lfd_impl, lfd_bin2c, lfd_disasm, lfd_translate, lfd_xml, "lfd");
	init_table(51, lfdu_impl, lfdu_bin2c, lfdu_disasm, lfdu_translate, lfdu_xml, "lfdu");
	init_table(48, lfs_impl, lfs_bin2c, lfs_disasm, lfs_translate, lfs_xml, "lfs");
	init_table(49, lfsu_impl, lfsu_bin2c, lfsu_disasm, lfsu_translate, lfsu_xml, "lfsu");
	init_table(42, lha_impl, lha_bin2c, lha_disasm, lha_translate, lha_xml, "lha");
	init_table(43, lhau_impl, lhau_bin2c, lhau_disasm, lhau_translate, lhau_xml, "lhau");
	init_table(40, lhz_impl, lhz_bin2c, lhz_disasm, lhz_translate, lhz_xml, "lhz");
	init_table(41, lhzu_impl, lhzu_bin2c, lhzu_disasm, lhzu_translate, lhzu_xml, "lhzu");
	init_table(46, lmw_impl, lmw_bin2c, lmw_disasm, lmw_translate, lmw_xml, "lmw");
	init_table(32, lwz_impl, lwz_bin2c, lwz_disasm, lwz_translate, lwz_xml, "lwz");
	init_table(33, lwzu_impl, lwzu_bin2c, lwzu_disasm, lwzu_translate, lwzu_xml, "lwzu");
	init_table(7, mulli_impl, mulli_bin2c, mulli_disasm, mulli_translate, mulli_xml, "mulli");
	init_table(24, ori_impl, ori_bin2c, ori_disasm, ori_translate, ori_xml, "ori");
	init_table(25, oris_impl, oris_bin2c, oris_disasm, oris_translate, oris_xml, "oris");
	init_table(38, stb_impl, stb_bin2c, stb_disasm, stb_translate, stb_xml, "stb");
	init_table(39, stbu_impl, stbu_bin2c, stbu_disasm, stbu_translate, stbu_xml, "stbu");
	init_table(54, stfd_impl, stfd_bin2c, stfd_disasm, stfd_translate, stfd_xml, "stfd");
	init_table(55, stfdu_impl, stfdu_bin2c, stfdu_disasm, stfdu_translate, stfdu_xml, "stfdu");
	init_table(52, stfs_impl, stfs_bin2c, stfs_disasm, stfs_translate, stfs_xml, "stfs");
	init_table(53, stfsu_impl, stfsu_bin2c, stfsu_disasm, stfsu_translate, stfsu_xml, "stfsu");
	init_table(44, sth_impl, sth_bin2c, sth_disasm, sth_translate, sth_xml, "sth");
	init_table(45, sthu_impl, sthu_bin2c, sthu_disasm, sthu_translate, sthu_xml, "sthu");
	init_table(47, stmw_impl, stmw_bin2c, stmw_disasm, stmw_translate, stmw_xml, "stmw");
	init_table(36, stw_impl, stw_bin2c, stw_disasm, stw_translate, stw_xml, "stw");
	init_table(37, stwu_impl, stwu_bin2c, stwu_disasm, stwu_translate, stwu_xml, "stwu");
	init_table(8, subfic_impl, subfic_bin2c, subfic_disasm, subfic_translate, subfic_xml, "subfic");
	init_table(3, twi_impl, twi_bin2c, twi_disasm, twi_translate, twi_xml, "twi");
	init_table(26, xori_impl, xori_bin2c, xori_disasm, xori_translate, xori_xml, "xori");
	init_table(27, xoris_impl, xoris_bin2c, xoris_disasm, xoris_translate, xoris_xml, "xoris");
	init_table(31, impl_31, bin2c_31, disasm_31, translate_31, xml_31, NULL);
	init_table(63, impl_63, bin2c_63, disasm_63, translate_63, xml_63, NULL);
	init_table(19, impl_19, bin2c_19, disasm_19, translate_19, xml_19, NULL);
	init_table(59, impl_59, bin2c_59, disasm_59, translate_59, xml_59, NULL);
	init_table(18, bx_impl, bx_bin2c, bx_disasm, bx_translate, bx_xml, "bx");
	init_table(16, bcx_impl, bcx_bin2c, bcx_disasm, bcx_translate, bcx_xml, "bcx");
	init_table(17, sc_impl, sc_bin2c, sc_disasm, sc_translate, sc_xml, "sc");
	init_table(20, rlwimix_impl, rlwimix_bin2c, rlwimix_disasm, rlwimix_translate, rlwimix_xml, "rlwimix");
	init_table(21, rlwinmx_impl, rlwinmx_bin2c, rlwinmx_disasm, rlwinmx_translate, rlwinmx_xml, "rlwinmx");
	init_table(23, rlwnmx_impl, rlwnmx_bin2c, rlwnmx_disasm, rlwnmx_translate, rlwnmx_xml, "rlwnmx");

	
	init_table59(21, faddsx_impl, faddsx_bin2c, faddsx_disasm, faddsx_translate, faddsx_xml, "faddsx");
	init_table59(18, fdivsx_impl, fdivsx_bin2c, fdivsx_disasm, fdivsx_translate, fdivsx_xml, "fdivsx");
	init_table59(29, fmaddsx_impl, fmaddsx_bin2c, fmaddsx_disasm, fmaddsx_translate, fmaddsx_xml, "fmaddsx");
	init_table59(28, fmsubsx_impl, fmsubsx_bin2c, fmsubsx_disasm, fmsubsx_translate, fmsubsx_xml, "fmsubsx");
	init_table59(25, fmulsx_impl, fmulsx_bin2c, fmulsx_disasm, fmulsx_translate, fmulsx_xml, "fmulsx");
	init_table59(31, fnmaddsx_impl, fnmaddsx_bin2c, fnmaddsx_disasm, fnmaddsx_translate, fnmaddsx_xml, "fnmaddsx");
	init_table59(30, fnmsubsx_impl, fnmsubsx_bin2c, fnmsubsx_disasm, fnmsubsx_translate, fnmsubsx_xml, "fnmsubsx");
/*
	init_table59(24, fresx_impl, fresx_bin2c, fresx_disasm, fresx_translate, fresx_xml, "fresx");
	init_table59(22, fsqrtsx_impl, fsqrtsx_bin2c, fsqrtsx_disasm, fsqrtsx_translate, fsqrtsx_xml, "fsqrtsx");
*/
	init_table59(20, fsubsx_impl, fsubsx_bin2c, fsubsx_disasm, fsubsx_translate, fsubsx_xml, "fsubsx");
	init_table63(264, 10, fabsx_impl, fabsx_bin2c, fabsx_disasm, fabsx_translate, fabsx_xml, "fabsx");
/*
	init_table63(32, 10, fcmpo_impl, fcmpo_bin2c, fcmpo_disasm, fcmpo_translate, fcmpo_xml, "fcmpo");
*/
	init_table63(0, 10, fcmpu_impl, fcmpu_bin2c, fcmpu_disasm, fcmpu_translate, fcmpu_xml, "fcmpu");
/*
	init_table63(14, 10, fctiwx_impl, fctiwx_bin2c, fctiwx_disasm, fctiwx_translate, fctiwx_xml, "fctiwx");
*/
	init_table63(15, 10, fctiwzx_impl, fctiwzx_bin2c, fctiwzx_disasm, fctiwzx_translate, fctiwzx_xml, "fctiwzx");
	init_table63(72, 10, fmrx_impl, fmrx_bin2c, fmrx_disasm, fmrx_translate, fmrx_xml, "fmrx");
	init_table63(136, 10, fnabsx_impl, fnabsx_bin2c, fnabsx_disasm, fnabsx_translate, fnabsx_xml, "fnabsx");
	init_table63(40, 10, fnegx_impl, fnegx_bin2c, fnegx_disasm, fnegx_translate, fnegx_xml, "fnegx");
	init_table63(12, 10, frspx_impl, frspx_bin2c, frspx_disasm, frspx_translate, frspx_xml, "frspx");
/*
	init_table63(64, 10, mcrfs_impl, mcrfs_bin2c, mcrfs_disasm, mcrfs_translate, mcrfs_xml, "mcrfs");
*/
	init_table63(583, 10, mffsx_impl, mffsx_bin2c, mffsx_disasm, mffsx_translate, mffsx_xml, "mffsx");
	init_table63(70, 10, mtfsb0x_impl, mtfsb0x_bin2c, mtfsb0x_disasm, mtfsb0x_translate, mtfsb0x_xml, "mtfsb0x");
/*
	init_table63(38, 10, mtfsb1x_impl, mtfsb1x_bin2c, mtfsb1x_disasm, mtfsb1x_translate, mtfsb1x_xml, "mtfsb1x");
*/
	init_table63(134, 10, mtfsfix_impl, mtfsfix_bin2c, mtfsfix_disasm, mtfsfix_translate, mtfsfix_xml, "mtfsfix");
	init_table63(711, 10, mtfsfx_impl, mtfsfx_bin2c, mtfsfx_disasm, mtfsfx_translate, mtfsfx_xml, "mtfsfx");
	init_table63(21, 5, faddx_impl, faddx_bin2c, faddx_disasm, faddx_translate, faddx_xml, "faddx");
	init_table63(18, 5, fdivx_impl, fdivx_bin2c, fdivx_disasm, fdivx_translate, fdivx_xml, "fdivx");
	init_table63(29, 5, fmaddx_impl, fmaddx_bin2c, fmaddx_disasm, fmaddx_translate, fmaddx_xml, "fmaddx");
	init_table63(28, 5, fmsubx_impl, fmsubx_bin2c, fmsubx_disasm, fmsubx_translate, fmsubx_xml, "fmsubx");
	init_table63(25, 5, fmulx_impl, fmulx_bin2c, fmulx_disasm, fmulx_translate, fmulx_xml, "fmulx");
	init_table63(31, 5, fnmaddx_impl, fnmaddx_bin2c, fnmaddx_disasm, fnmaddx_translate, fnmaddx_xml, "fnmaddx");
	init_table63(30, 5, fnmsubx_impl, fnmsubx_bin2c, fnmsubx_disasm, fnmsubx_translate, fnmsubx_xml, "fnmsubx");
/*
	init_table63(26, 5, frsqrtex_impl, frsqrtex_bin2c, frsqrtex_disasm, frsqrtex_translate, frsqrtex_xml, "frsqrtex");
	init_table63(23, 5, fselx_impl, fselx_bin2c, fselx_disasm, fselx_translate, fselx_xml, "fselx");
	init_table63(22, 5, fsqrtx_impl, fsqrtx_bin2c, fsqrtx_disasm, fsqrtx_translate, fsqrtx_xml, "fsqrtx");
*/	
	init_table63(20, 5, fsubx_impl, fsubx_bin2c, fsubx_disasm, fsubx_translate, fsubx_xml, "fsubx");


	init_table31(28, 10, andx_impl, andx_bin2c, andx_disasm, andx_translate, andx_xml, "andx");
	init_table31(60, 10, andcx_impl, andcx_bin2c, andcx_disasm, andcx_translate, andcx_xml, "andcx");
	init_table31(0, 10, cmp_impl, cmp_bin2c, cmp_disasm, cmp_translate, cmp_xml, "cmp");
	init_table31(32, 10, cmpl_impl, cmpl_bin2c, cmpl_disasm, cmpl_translate, cmpl_xml, "cmpl");
	init_table31(26, 10, cntlzwx_impl, cntlzwx_bin2c, cntlzwx_disasm, cntlzwx_translate, cntlzwx_xml, "cntlzwx");
/*	
	init_table31(758, 10, dcba_impl, dcba_bin2c, dcba_disasm, dcba_translate, dcba_xml, "dcba");
	init_table31(86, 10, dcbf_impl, dcbf_bin2c, dcbf_disasm, dcbf_translate, dcbf_xml, "dcbf");
	init_table31(470, 10, dcbi_impl, dcbi_bin2c, dcbi_disasm, dcbi_translate, dcbi_xml, "dcbi");
	init_table31(54, 10, dcbst_impl, dcbst_bin2c, dcbst_disasm, dcbst_translate, dcbst_xml, "dcbst");
	init_table31(278, 10, dcbt_impl, dcbt_bin2c, dcbt_disasm, dcbt_translate, dcbt_xml, "dcbt");
	init_table31(246, 10, dcbtst_impl, dcbtst_bin2c, dcbtst_disasm, dcbtst_translate, dcbtst_xml, "dcbtst");
*/
	init_table31(1014, 10, dcbz_impl, dcbz_bin2c, dcbz_disasm, dcbz_translate, dcbz_xml, "dcbz");
/*
	init_table31(310, 10, eciwx_impl, eciwx_bin2c, eciwx_disasm, eciwx_translate, eciwx_xml, "eciwx");
	init_table31(438, 10, ecowx_impl, ecowx_bin2c, ecowx_disasm, ecowx_translate, ecowx_xml, "ecowx");
	init_table31(854, 10, eieio_impl, eieio_bin2c, eieio_disasm, eieio_translate, eieio_xml, "eieio");
*/
	init_table31(284, 10, eqvx_impl, eqvx_bin2c, eqvx_disasm, eqvx_translate, eqvx_xml, "eqvx");
	init_table31(954, 10, extsbx_impl, extsbx_bin2c, extsbx_disasm, extsbx_translate, extsbx_xml, "extsbx");
	init_table31(922, 10, extshx_impl, extshx_bin2c, extshx_disasm, extshx_translate, extshx_xml, "extshx");
//	init_table31(982, 10, icbi_impl, icbi_bin2c, icbi_disasm, icbi_translate, icbi_xml, "icbi");
	init_table31(119, 10, lbzux_impl, lbzux_bin2c, lbzux_disasm, lbzux_translate, lbzux_xml, "lbzux");
	init_table31(87, 10, lbzx_impl, lbzx_bin2c, lbzx_disasm, lbzx_translate, lbzx_xml, "lbzx");
	init_table31(631, 10, lfdux_impl, lfdux_bin2c, lfdux_disasm, lfdux_translate, lfdux_xml, "lfdux");
	init_table31(599, 10, lfdx_impl, lfdx_bin2c, lfdx_disasm, lfdx_translate, lfdx_xml, "lfdx");
	init_table31(567, 10, lfsux_impl, lfsux_bin2c, lfsux_disasm, lfsux_translate, lfsux_xml, "lfsux");
	init_table31(535, 10, lfsx_impl, lfsx_bin2c, lfsx_disasm, lfsx_translate, lfsx_xml, "lfsx");
	init_table31(375, 10, lhaux_impl, lhaux_bin2c, lhaux_disasm, lhaux_translate, lhaux_xml, "lhaux");
	init_table31(343, 10, lhax_impl, lhax_bin2c, lhax_disasm, lhax_translate, lhax_xml, "lhax");
	init_table31(790, 10, lhbrx_impl, lhbrx_bin2c, lhbrx_disasm, lhbrx_translate, lhbrx_xml, "lhbrx");
	init_table31(311, 10, lhzux_impl, lhzux_bin2c, lhzux_disasm, lhzux_translate, lhzux_xml, "lhzux");
	init_table31(279, 10, lhzx_impl, lhzx_bin2c, lhzx_disasm, lhzx_translate, lhzx_xml, "lhzx");
/*
	init_table31(597, 10, lswi_impl, lswi_bin2c, lswi_disasm, lswi_translate, lswi_xml, "lswi");
	init_table31(533, 10, lswx_impl, lwsx_bin2c, lswx_disasm, lswx_translate, lswx_xml, "lswx");
	init_table31(20, 10, lwarx_impl, lwarx_bin2c, lwarx_disasm, lwarx_translate, lwarx_xml, "lwarx");
*/
	init_table31(534, 10, lwbrx_impl, lwbrx_bin2c, lwbrx_disasm, lwbrx_translate, lwbrx_xml, "lwbrx");
	init_table31(55, 10, lwzux_impl, lwzux_bin2c, lwzux_disasm, lwzux_translate, lwzux_xml, "lwzux");
	init_table31(23, 10, lwzx_impl, lwzx_bin2c, lwzx_disasm, lwzx_translate, lwzx_xml, "lwzx");
//	init_table31(512, 10, mcrxr_impl, mcrxr_bin2c, mcrxr_disasm, mcrxr_translate, mcrxr_xml, "mcrxr");
	init_table31(19, 10, mfcr_impl, mfcr_bin2c, mfcr_disasm, mfcr_translate, mfcr_xml, "mfcr");
/*
	init_table31(83, 10, mfmsr_impl, mfmsr_bin2c, mfmsr_disasm, mfmsr_translate, mfmsr_xml, "mfmsr");
	init_table31(595, 10, mfsr_impl, mfsr_bin2c, mfsr_disasm, mfsr_translate, mfsr_xml, "mfsr");
	init_table31(659, 10, mfsrin_impl, mfsrin_bin2c, mfsrin_disasm, mfsrin_translate, mfsrin_xml, "mfsrin");
	init_table31(146, 10, mtmsr_impl, mtmsr_bin2c, mtmsr_disasm, mtmsr_translate, mtmsr_xml, "mtmsr");
	init_table31(210, 10, mtsr_impl, mtsr_bin2c, mtsr_disasm, mtsr_translate, mtsr_xml, "mtsr");
	init_table31(242, 10, mtsrin_impl, mtsrin_bin2c, mtsrin_disasm, mtsrin_translate, mtsrin_xml, "mtsrin");
*/
	init_table31(476, 10, nandx_impl, nandx_bin2c, nandx_disasm, nandx_translate, nandx_xml, "nandx");
	init_table31(124, 10, norx_impl, norx_bin2c, norx_disasm, norx_translate, norx_xml, "norx");
	init_table31(444, 10, orx_impl, orx_bin2c, orx_disasm, orx_translate, orx_xml, "orx");
	init_table31(412, 10, orcx_impl, orcx_bin2c, orcx_disasm, orcx_translate, orcx_xml, "orcx");
	init_table31(24, 10, slwx_impl, slwx_bin2c, slwx_disasm, slwx_translate, slwx_xml, "slwx");
	init_table31(792, 10, srawx_impl, srawx_bin2c, srawx_disasm, srawx_translate, srawx_xml, "srawx");
	init_table31(824, 10, srawix_impl, srawix_bin2c, srawix_disasm, srawix_translate, srawix_xml, "srawix");
	init_table31(536, 10, srwx_impl, srwx_bin2c, srwx_disasm, srwx_translate, srawx_xml, "srawx");
	init_table31(247, 10, stbux_impl, stbux_bin2c, stbux_disasm, stbux_translate, stbux_xml, "stbux");
	init_table31(215, 10, stbx_impl, stbx_bin2c, stbx_disasm, stbx_translate, stbx_xml, "stbx");
	init_table31(759, 10, stfdux_impl, stfdux_bin2c, stfdux_disasm, stfdux_translate, stfdux_xml, "stfdux");
	init_table31(727, 10, stfdx_impl, stfdx_bin2c, stfdx_disasm, stfdx_translate, stfdx_xml, "stfdx");
/*
	init_table31(983, 10, stfiwx_impl, stfiwx_bin2c, stfiwx_disasm, stfiwx_translate, stfiwx_xml, "stfiwx");
*/	
	init_table31(695, 10, stfsux_impl, stfsux_bin2c, stfsux_disasm, stfsux_translate, stfsux_xml, "stfsux");
	init_table31(663, 10, stfsx_impl, stfsx_bin2c, stfsx_disasm, stfsx_translate, stfsx_xml, "stfsx");
	init_table31(918, 10, sthbrx_impl, sthbrx_bin2c, sthbrx_disasm, sthbrx_translate, sthbrx_xml, "sthbrx");
	init_table31(439, 10, sthux_impl, sthux_bin2c, sthux_disasm, sthux_translate, sthux_xml, "sthux");
	init_table31(407, 10, sthx_impl, sthx_bin2c, sthx_disasm, sthx_translate, sthx_xml, "sthx");
/*	
	init_table31(725, 10, stswi_impl, stswi_bin2c, stswi_disasm, stswi_translate, stswi_xml, "stswi");
	init_table31(661, 10, stswx_impl, stswx_bin2c, stswx_disasm, stswx_translate, stswx_xml, "stswx");
*/
	init_table31(662, 10, stwbrx_impl, stwbrx_bin2c, stwbrx_disasm, stwbrx_translate, stwbrx_xml, "stwbrx");
//	init_table31(150, 10, stwcxd_impl, stwcxd_bin2c, stwcxd_disasm, stwcxd_xml, stwcxd_translate);
	init_table31(183, 10, stwux_impl, stwux_bin2c, stwux_disasm, stwux_translate, stwux_xml, "stwux");
	init_table31(151, 10, stwx_impl, stwx_bin2c, stwx_disasm, stwx_translate, stwx_xml, "stwx");
/*
	init_table31(598, 10, sync_impl, sync_bin2c, sync_disasm, sync_translate, sync_xml, "sync");
	init_table31(370, 10, tlbia_impl, tlbia_bin2c, tlbia_disasm, tlbia_translate, tlbia_xml, "tlbia");
	init_table31(306, 10, tlbie_impl, tlbie_bin2c, tlbie_disasm, tlbie_translate, tlbie_xml, "tlbie");
	init_table31(566, 10, tlbsync_impl, tlbsync_bin2c, tlbsync_disasm, tlbsync_translate, tlbsync_xml, "tlbsync");
	init_table31(4, 10, tw_impl, tw_bin2c, tw_disasm, tw_translate, tw_xml, "tw");
*/
	init_table31(316, 10, xorx_impl, xorx_bin2c, xorx_disasm, xorx_translate, xorx_xml, "xorx");
	init_table31(339, 10, mfspr_impl, mfspr_bin2c, mfspr_disasm, mfspr_translate, mfspr_xml, "mfspr");
	init_table31(371, 10, mftb_impl, mftb_bin2c, mftb_disasm, mftb_translate, mftb_xml, "mftb");
	init_table31(144, 10, mtcrf_impl, mtcrf_bin2c, mtcrf_disasm, mtcrf_translate, mtcrf_xml, "mtcrf");
	init_table31(467, 10, mtspr_impl, mtspr_bin2c, mtspr_disasm, mtspr_translate, mtspr_xml, "mtspr");
			


	init_table31(266, 9, addx_impl, addx_bin2c, addx_disasm, addx_translate, addx_xml, "addx");
	init_table31(10, 9, addcx_impl, addcx_bin2c, addcx_disasm, addcx_translate, addcx_xml, "addcx");
	init_table31(138, 9, addex_impl, addex_bin2c, addex_disasm, addex_translate, addex_xml, "addex");
	init_table31(234, 9, addmex_impl, addmex_bin2c, addmex_disasm, addmex_translate, addmex_xml, "addmex");
	init_table31(202, 9, addzex_impl, addzex_bin2c, addzex_disasm, addzex_translate, addzex_xml, "addzex");
	init_table31(491, 9, divwx_impl, divwx_bin2c, divwx_disasm, divwx_translate, divwx_xml, "divwx");
	init_table31(459, 9, divwux_impl, divwux_bin2c, divwux_disasm, divwux_translate, divwux_xml, "divwux");
	init_table31(75, 9, mulhwx_impl, mulhwx_bin2c, mulhwx_disasm, mulhwx_translate, mulhwx_xml, "mulhwx");
	init_table31(11, 9, mulhwux_impl, mulhwux_bin2c, mulhwux_disasm, mulhwux_translate, mulhwux_xml, "mulhwux");
	init_table31(235, 9, mullwx_impl, mullwx_bin2c, mullwx_disasm, mullwx_translate, mullwx_xml, "mullwx");
	init_table31(104, 9, negx_impl, negx_bin2c, negx_disasm, negx_translate, negx_xml, "negx");
	init_table31(40, 9, subfx_impl, subfx_bin2c, subfx_disasm, subfx_translate, subfx_xml, "subfx");
	init_table31(8, 9, subfcx_impl, subfcx_bin2c, subfcx_disasm, subfcx_translate, subfcx_xml, "subfcx");
	init_table31(136, 9, subfex_impl, subfex_bin2c, subfex_disasm, subfex_translate, subfex_xml, "subfex");
	init_table31(232, 9, subfmex_impl, subfmex_bin2c, subfmex_disasm, subfmex_translate, subfmex_xml, "subfmex");
	init_table31(200, 9, subfzex_impl, subfzex_bin2c, subfzex_disasm, subfzex_translate, subfzex_xml, "subfzex");
	

	init_table19(528, bcctrx_impl, bcctrx_bin2c, bcctrx_disasm, bcctrx_translate, bcctrx_xml, "bcctrx");
	init_table19(16, bclrx_impl, bclrx_bin2c, bclrx_disasm, bclrx_translate, bclrx_xml, "bclrx");
	init_table19(257, crand_impl, crand_bin2c, crand_disasm, crand_translate, crand_xml, "crand");
	init_table19(129, crandc_impl, crandc_bin2c, crandc_disasm, crandc_translate, crandc_xml, "crandc");
	init_table19(289, creqv_impl, creqv_bin2c, creqv_disasm, creqv_translate, creqv_xml, "creqv");
	init_table19(225, crnand_impl, crnand_bin2c, crnand_disasm, crnand_translate, crnand_xml, "crnand");
	init_table19(33, crnor_impl, crnor_bin2c, crnor_disasm, crnor_translate, crnor_xml, "crnor");
	init_table19(449, cror_impl, cror_bin2c, cror_disasm, cror_translate, cror_xml, "cror");
	init_table19(417, crorc_impl, crorc_bin2c, crorc_disasm, crorc_translate, crorc_xml, "crorc");
	init_table19(193, crxor_impl, crxor_bin2c, crxor_disasm, crxor_translate, crxor_xml, "crxor");
/*	
	init_table19(150, isync_impl, isync_bin2c, isync_disasm, isync_translate, isync_xml, "isync");
*/
	init_table19(0, mcrf_impl, mcrf_bin2c, mcrf_disasm, mcrf_translate, mcrf_xml, "mcrf");
/*
	init_table19(50, rfi_impl, rfi_bin2c, rfi_disasm, rfi_translate, rfi_xml, "rfi");
	init_table19(18, rfid_impl, rfid_bin2c, rfid_disasm, rfid_translate, rfid_xml, "rfid");	
*/
}

void dump_int64(UInt64 value)
{
	char buffer[32];
	char *p = buffer + sizeof(buffer) - 1;
	
	*p = 0;
	if(value > 0)
	{
		do
		{
			*--p = '0' + (value % 10);
			value = value / 10;
		} while(value > 0);
	}
	else
	{
		*--p = '0';
	}
	fputs(p, stdout);
}

void dump_int64_smart(UInt64 value)
{
	if(value > 1000000000)
	{
		dump_int64(value / 1000000000);
		fputs(" G", stdout);
		return;
	}
	if(value > 1000000)
	{
		dump_int64(value / 1000000);
		fputs(" M", stdout);
		return;
	}
	if(value > 1000)
	{
		dump_int64(value / 1000);
		fputs(" K", stdout);
		return;
	}
	dump_int64(value);	
}

void ppc_dump_instruction_counters()
{
	int i;
	
	for(i = 0; i < ninstructions; i++)
	{
		if(counters[i] > 0 && mnemonic_table[i])
		{
			printf("%s : ", mnemonic_table[i]);
			if(counters[i] < 1000)
			{
				dump_int64(counters[i]);
			}
			else
			{
				dump_int64_smart(counters[i]);
				fputs(" (", stdout);
				dump_int64(counters[i]);
				fputs(")", stdout);
			}
			fputs("\n", stdout);
		}
	}
	fputs("Total : ", stdout);
	if(instruction_counter < 1000)
	{
		dump_int64(instruction_counter);
	}
	else
	{
		dump_int64_smart(instruction_counter);
		fputs(" (", stdout);
		dump_int64(instruction_counter);
		fputs(")", stdout);
	}
	fputs("\n", stdout);
}

BOOL ppc_is_a_branch(ppc_inst_t inst)
{
	return OPCD == 16 || OPCD == 18 || (OPCD == 19 && (XO_10 == 528 || XO_10 == 16));
}

BOOL ppc_is_conditional_branch(ppc_inst_t inst)
{
	return OPCD == 16;
}

BOOL ppc_is_unconditional_branch(ppc_inst_t inst)
{
	return OPCD == 18;
}

addr_t ppc_get_unconditional_branch_target_addr(ppc_inst_t inst, addr_t pc)
{
	UInt32 offset = inst & 0x03fffffc;
	if(offset & 0x02000000) offset |= 0xfc000000;

	if(inst & 2)
	{
		return offset;
	}
	else
	{
		return pc + offset;
	}
}

addr_t ppc_get_conditional_branch_target_addr(ppc_inst_t inst, addr_t pc)
{
	UInt32 offset = inst & 0xfffc;
	if(offset & 32768) offset |= 0xffff0000; /* extension de signe sur 32 bits */
	
	if(inst & 2)
	{
		return offset;
	}
	else
	{
		return pc + offset;
	}
}

