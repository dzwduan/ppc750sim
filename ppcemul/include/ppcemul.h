/***************************************************************************
                          ppcemul.h  -  description
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

#ifndef __PPCEMUL_H__
#define __PPCEMUL_H__

#include <branch.h>
#include <floatingpoint.h>
#include <integer.h>
#include <loader.h>
#include <loadstore.h>
#include <memory.h>
#include <misc.h>
#include <registers.h>
#include <syscall.h>
#include <systemregisters.h>
#include <decode.h>
#include <fetch.h>
#include <translate.h>
#include <debug.h>
#include <read_elf.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void ppc_init();
extern void ppc_delete();
extern void ppc_save(char *filename);
extern void ppc_restore(char *filename);
extern void ppc_run();
extern void ppc_run_until(int ninstructions);
#if defined(__cplusplus)
}
#endif

extern dword_t icount;
extern BOOL running;

#endif
