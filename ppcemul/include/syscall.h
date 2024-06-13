/***************************************************************************
                          syscall.h  -  description
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

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <misc.h>
#include <translate.h>

typedef struct
{
	/* Registers interface */
	void (*write_gpr)(void *, int, word_t);
	word_t (*read_gpr)(void *, int);
	void (*reset_cr0so)(void *);
	void (*set_cr0so)(void *);
	
	/* Memory interface */
	byte_t (*mem_read_byte)(void *, addr_t);
	void (*mem_write_byte)(void *, addr_t, byte_t);
	halfword_t (*mem_read_half_word)(void *, addr_t);
	void (*mem_write_half_word)(void *, addr_t, halfword_t);
	word_t (*mem_read_word)(void *, addr_t);
	void (*mem_write_word)(void *, addr_t, word_t);
	dword_t (*mem_read_dword)(void *, addr_t);
	void (*mem_write_dword)(void *, addr_t, dword_t);
	void (*mem_set)(void *, addr_t, byte_t, int);
	void (*mem_read)(void *, void *, addr_t, int);
	void (*mem_write)(void *, addr_t, void *, int);
	
	
	void *instance;
} syscall_cb_t;

#if defined(__cplusplus)
extern "C" {
#endif
extern void syscall_init(syscall_cb_t *cb, BOOL verbose, BOOL emulate, void (*exit_proc)(int));
extern void syscall_setcb(syscall_cb_t *cb);
extern void syscall_getcb(syscall_cb_t *cb);
extern void syscall_setbrk(addr_t addr);
extern void sc_impl(ppc_inst_t inst);
extern char *sc_bin2c(ppc_inst_t inst, addr_t pc, char *buf);
extern char *sc_disasm(ppc_inst_t inst, addr_t pc, char *buf);
extern void sc_xml(ppc_inst_t inst, addr_t pc, char *buf);
extern void sc_translate(ppc_inst_t inst, struct DecodedInstruction *di);
extern void do_syscall();
#if defined(__cplusplus)
}
#endif

extern BOOL emulate_syscall;

#endif
