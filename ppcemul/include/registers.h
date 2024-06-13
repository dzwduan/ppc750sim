/***************************************************************************
                          registers.h  -  description
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

#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include <misc.h>
#include <stdio.h>

typedef struct
{
	/* Registers interface */
	void (*write_gpr)(void *, int, word_t);
	word_t (*read_gpr)(void *, int);
	void (*write_fpr)(void *, int, dword_t);
	dword_t (*read_fpr)(void *, int);
	void (*write_cr)(void *, word_t);
	word_t (*read_cr)(void *);
	void (*write_lr)(void *, word_t);
	word_t (*read_lr)(void *);
	void (*write_ctr)(void *, word_t);
	word_t (*read_ctr)(void *);
	void (*write_xer)(void *, word_t);
	word_t (*read_xer)(void *);
	void (*write_fpscr)(void *, word_t);
	word_t (*read_fpscr)(void *);
	void (*write_pc)(void *, word_t);
	word_t (*read_pc)(void *);
	
	void *instance;
} registers_cb_t;


typedef struct
{
	word_t gpr[32];
	dword_t fpr[32];
	word_t cr;
	word_t xer;
	word_t lr;
	word_t ctr;
	word_t fpscr;
	addr_t pc;
	addr_t npc;
	word_t tbl;
	word_t tbu;
} my_regs_t;

extern my_regs_t my_regs;

#if defined(__cplusplus)
extern "C" {
#endif
extern void ppc_init_registers();
extern void regs_save(FILE *f);
extern void regs_restore(FILE *f);
extern void regs_duplicate(registers_cb_t *cb);
#if defined(__cplusplus)
}
#endif

#endif
