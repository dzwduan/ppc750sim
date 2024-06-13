/***************************************************************************
                          fetch.h  -  description
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

#ifndef __PPC_FETCH_H__
#define __PPC_FETCH_H__

#include <misc.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern ppc_inst_t ppc_fetch(addr_t addr);
#if defined(__cplusplus)
}
#endif

#endif
