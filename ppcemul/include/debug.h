/***************************************************************************
                          debug.h  -  description
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

#ifndef __PPC_DEBUG_H__
#define __PPC_DEBUG_H__

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void ppc_debug();
extern void ppc_debug_dump_registers(FILE *stream);
extern void ppc_debug_disasm(FILE *stream, addr_t addr);
extern void ppc_debug_dump(FILE *stream, addr_t addr);
#if defined(__cplusplus)
}
#endif

#endif
