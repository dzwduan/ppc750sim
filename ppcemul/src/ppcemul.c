/***************************************************************************
                          ppcemul.c  -  description
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
#include <signal.h>

dword_t icount;
BOOL running = FALSE;

static void sigint_handler(int signum)
{
	if(running)
	{
		running = FALSE;
	}
}


void ppc_init()
{
	ppc_init_decode();
	ppc_init_registers();
	mem = mem_create();
}

void ppc_delete()
{
	mem_delete(mem);
}

void ppc_save(char *filename)
{
	FILE *f = fopen(filename, "wb");
	regs_save(f);
	mem_save(mem, f);
	fclose(f);
}

void ppc_restore(char *filename)
{
	FILE *f = fopen(filename, "rb");
	regs_restore(f);
	mem_restore(mem, f);
	fclose(f);
}

void ppc_run()
{
	ppc_inst_t inst;
	void (*prev_sigint_handler)(int);
	
	icount = 0;
	running = TRUE;
	
	prev_sigint_handler = signal(SIGINT, sigint_handler);
	
	while(running)
	{
		inst = ppc_fetch(PC);
		ppc_execute(inst);
		icount++;
	}
	
	if(prev_sigint_handler != SIG_ERR) signal(SIGINT, SIG_DFL);
}

void ppc_run_until(int ninstructions)
{
	ppc_inst_t inst;
	BOOL old_emulate_syscall = emulate_syscall;
	syscall_cb_t old_cb;
	
	syscall_getcb(&old_cb);
	syscall_setcb(NULL);
	emulate_syscall = TRUE;
	
	while(ninstructions > 0)
	{
		inst = ppc_fetch(PC);
		ppc_execute(inst);
		ninstructions--;
	}
	emulate_syscall = old_emulate_syscall;
	syscall_setcb(&old_cb);
}

