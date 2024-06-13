/***************************************************************************
                          loader.h  -  description
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

#ifndef __PPC_LOADER_H__
#define __PPC_LOADER_H__

#include <misc.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern void load_program(char *filename, int argc, char *argv[], char *envp[]);
extern void xmlize_program(char *srcFilename, char *dstFilename);
extern void bin2c_program(char *srcFilename, char *dstFilename);
#if defined(__cplusplus)
}
#endif

extern UInt32 text_addr;
extern UInt32 text_size;

#endif
