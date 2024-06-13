/***************************************************************************
                          debug.c  -  description
                             -------------------
    begin                : Sun Sep 30 2001
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

BOOL debuging = FALSE;

static void sigint_handler(int signum)
{
	if(debuging)
	{
		debuging = FALSE;
	}
	else
	{
		exit(0);
	}
}

void ppc_debug_disasm(FILE *stream, addr_t addr)
{
	int i;
	ppc_inst_t inst;
	char disasm[256];
	
	
	for(i = 0; i < 10; i++, addr += 4)
	{
		inst = ppc_fetch(addr);
		ppc_disassemble(inst, addr, disasm);
		fprintf(stream, "0x%08x : 0x%08x    %s\n", addr, inst, disasm);
	}
}

void ppc_debug_dump(FILE *stream, addr_t addr)
{
	int i, j;
	
	fprintf(stream, "  address    0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	for(i = 0; i < 16; i++)
	{
		fprintf(stream, "0x%08x: ", addr);
		for(j = 0; j < 16; j++, addr++)
		{
			fprintf(stream, "%02x", MEM_READ_BYTE(addr));
			if(j < 15) fprintf(stream, " ");
		}
		fprintf(stream, "\n");
	}
}

void ppc_debug_go_to(addr_t addr)
{
	ppc_inst_t inst;
	void (*prev_sigint_handler)(int);
	char disasm[256];
	
	debuging = TRUE;
	
	prev_sigint_handler = signal(SIGINT, sigint_handler);
	
	do
	{
		inst = ppc_fetch(PC);
		ppc_execute(inst);
	} while(debuging && PC != addr);
	
	if(prev_sigint_handler != SIG_ERR) signal(SIGINT, SIG_DFL);
	
	inst = ppc_fetch(PC);
	ppc_disassemble(inst, PC, disasm);
	printf("0x%08x : 0x%08x    %s\n", PC, inst, disasm);
}

void ppc_debug_trace(int count)
{
	ppc_inst_t inst;
	char disasm[256];
	
	while(count > 0)
	{
		inst = ppc_fetch(PC);
		ppc_execute(inst);
		count--;
	}
	
	inst = ppc_fetch(PC);
	ppc_disassemble(inst, PC, disasm);
	printf("0x%08x : 0x%08x    %s\n", PC, inst, disasm);
}

void ppc_debug_dump_registers(FILE *stream)
{
	int i, j, n;
	char s[256];
	
	for(n = 0, i = 0; i < 8;  i++)
	{
		for(j = 0; j < 4; j++, n++)
		{
			sprintf(s, "r%u = 0x%08x", n, GPR(n));
			fprintf(stream, "%17s", s);
			if(j < 3) fprintf(stream, "  ");
		}
		fprintf(stream, "\n");
	}
	for(n = 0, i = 0; i < 16;  i++)
	{
		for(j = 0; j < 2; j++, n++)
		{
			sprintf(s, "f%u = 0x%08x%08x", n, (word_t)(FPR_DW(n) >> 32), (word_t) FPR_DW(n));
			fprintf(stream, "%25s", s);
			if(j < 1) fprintf(stream, "  ");
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "XER[SO] = %d\n", XER_SO);
	fprintf(stream, "XER[OV] = %d\n", XER_OV);
	fprintf(stream, "XER[CA] = %d\n", XER_CA);
}

BOOL get_addr(char *s, addr_t *addr)
{
	return sscanf(s, "%x", addr) == 1 || sscanf(s, "0x%x", addr) == 1;
}

char *my_gets(char *p, int size)
{
	char *ret = fgets(p, size, stdin);
	if(ret)
	{
		int l = strlen(p);
		if(l > 0) p[l - 1] = 0;
	}
	return ret;
}

void ppc_debug()
{
	addr_t disasm_addr = PC;
	addr_t dump_addr = PC;
	addr_t addr;
	unsigned int reg_num;
	int nparms;
	int count;
	char line[256];
	char last_line[256];
	char parm[8][32];
	
	BOOL old_emulate_syscall = emulate_syscall;
	syscall_cb_t old_cb;
	
	syscall_getcb(&old_cb);
	syscall_setcb(NULL);
	emulate_syscall = TRUE;
	
	strcpy(last_line, "");
	strcpy(line, "");
	
	do
	{	
	 	if(strcmp(line, "") == 0)
	 	{
	 		strcpy(line, last_line);
	 	}
		
		nparms = sscanf(line, "%s %s %s %s %s %s %s %s",
		                parm[0], parm[1], parm[2], parm[3], parm[4], parm[5], parm[6], parm[7]);
		
		switch(nparms)
		{
			case 1:
				if(strcmp(parm[0], "u") == 0)
				{
					ppc_debug_disasm(stdout, disasm_addr);
					disasm_addr += 10 * 4;
					break;
				}
				
				if(strcmp(parm[0], "d") == 0)
				{
					ppc_debug_dump(stdout, dump_addr);
					dump_addr += 16 * 16;
					break;
				}
				
				if(strcmp(parm[0], "g") == 0)
				{
					ppc_debug_go_to(0);
					break;
				}
				
				if(strcmp(parm[0], "pc") == 0)
				{
					printf("PC = 0x%08x\n", PC);
					break;
				}
				
				if(strcmp(parm[0], "cr") == 0)
				{
					printf("CR = 0x%08x\n", CR);
					break;
				}
				
				if(strcmp(parm[0], "lr") == 0)
				{
					printf("LR = 0x%08x\n", LR);
					break;
				}
				
				if(strcmp(parm[0], "ctr") == 0)
				{
					printf("CTR = 0x%08x\n", CTR);
					break;
				}
				
				if(strcmp(parm[0], "xer") == 0)
				{
					printf("XER = 0x%08x\n", XER);
				}
				
				if(strcmp(parm[0], "r") == 0)
				{
					ppc_debug_dump_registers(stdout);
					break;
				}
				
				if(sscanf(parm[0], "r%u", &reg_num) == 1)
				{
					printf("r%u = 0x%08x\n", reg_num, GPR(reg_num));
					break;
				}
				
				if(strcmp(parm[0], "t") == 0)
				{
					ppc_debug_trace(1);
					break;
				}
				
				if(strcmp(parm[0], "q") == 0)
				{
					return;
				}
				
				if(strcmp(parm[0], "c") == 0)
				{
					ppc_dump_instruction_counters(stdout);
					break;
				}
				
				if(strcmp(parm[0], "cc") == 0)
				{
					ppc_clear_instruction_counters();
					break;
				}
				break;
				
			case 2:
				if(strcmp(parm[0], "u") == 0 && get_addr(parm[1], &disasm_addr))
				{
					ppc_debug_disasm(stdout, disasm_addr);
					disasm_addr += 10 * 4;
					break;
				}
				
				if(strcmp(parm[0], "d") == 0 && get_addr(parm[1], &dump_addr))
				{
					ppc_debug_dump(stdout, dump_addr);
					dump_addr += 16 * 16;
					break;
				}
				
				if(strcmp(parm[0], "u") == 0 && strcmp(parm[1], "pc") == 0)
				{
					disasm_addr = PC;
					ppc_debug_disasm(stdout, disasm_addr);
					disasm_addr += 10 * 4;
					break;
				}
				
				if(strcmp(parm[0], "g") == 0 && get_addr(parm[1], &addr))
				{
					ppc_debug_go_to(addr);
					break;
				}
				
				if(strcmp(parm[0], "t") == 0 && sscanf(parm[1], "%d", &count) == 1)
				{
					ppc_debug_trace(count);
					break;
				}
				
				if(strcmp(parm[0], "s") == 0)
				{
					ppc_save(parm[1]);
					break;
				}
				
				if(strcmp(parm[0], "l") == 0)
				{
					ppc_restore(parm[1]);
					break;
				}
				break;
		}
	
		strcpy(last_line, line);
		
		printf("> ");
	} while(my_gets(line, sizeof(line)));

	emulate_syscall = old_emulate_syscall;
	syscall_setcb(&old_cb);
}

