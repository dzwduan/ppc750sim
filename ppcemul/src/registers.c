/***************************************************************************
                          registers.c  -  description
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

#include <registers.h>
#include <string.h>
#include <stdio.h>

my_regs_t my_regs;

void ppc_init_registers()
{
	memset(my_regs.gpr, 0, sizeof(my_regs.gpr));
	memset(my_regs.fpr, 0, sizeof(my_regs.fpr));
	my_regs.cr = 0;
	my_regs.lr = 0;
	my_regs.ctr = 0;
	my_regs.xer = 0;
	my_regs.fpscr = 0;
	my_regs.pc = 0;
	my_regs.npc = 0;
}

void regs_save(FILE *f)
{
	fwrite(&my_regs, sizeof(my_regs), 1, f);
}

void regs_restore(FILE *f)
{
	fread(&my_regs, sizeof(my_regs), 1, f);
}

void regs_duplicate(registers_cb_t *cb)
{
	int i;
	for(i = 0; i < 32; i++) cb->write_gpr(cb->instance, i, my_regs.gpr[i]);
	for(i = 0; i < 32; i++) cb->write_fpr(cb->instance, i, my_regs.fpr[i]);
	cb->write_cr(cb->instance, my_regs.cr);
	cb->write_lr(cb->instance, my_regs.lr);
	cb->write_ctr(cb->instance, my_regs.ctr);
	cb->write_xer(cb->instance, my_regs.xer);
	cb->write_fpscr(cb->instance, my_regs.fpscr);
	cb->write_pc(cb->instance, my_regs.pc);
}
