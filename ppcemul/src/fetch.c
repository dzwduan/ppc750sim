/***************************************************************************
                          fetch.c  -  description
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

#include <fetch.h>
#include <ppcemul.h>
#include <stdio.h>

ppc_inst_t ppc_fetch(addr_t addr)
{
/*	fprintf(stderr, "Fetch at 0x%08x\n", addr);*/
	return MEM_READ_WORD(addr);
}
