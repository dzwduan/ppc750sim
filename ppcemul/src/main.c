/***************************************************************************
                          main.c  -  description
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[], char *envp[])
{
	int i;
	char *prog_name = NULL;
	char *xml_name = NULL;
	char *bin2c_name = NULL;
	BOOL verbose = FALSE;
	BOOL debuging = FALSE;
	
	//analyze the command line arguments
	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-d") == 0) debuging = TRUE; else
		if(strcmp(argv[i], "-v") == 0) verbose = TRUE; else
		if(strcmp(argv[i], "-xml") == 0)
		{
			i++;
			if(i < argc)
				xml_name = argv[i];
		}
		else if(strcmp(argv[i], "-bin2c") == 0)
		{
			i++;
			if(i < argc)
				bin2c_name = argv[i];
		}
		else if(!prog_name)
		{
			prog_name = argv[i];
			break;
		}	
	}
	
	if(prog_name)
	{
		ppc_init();
		
		if(xml_name)
			xmlize_program(prog_name, xml_name);
			
		if(bin2c_name)
			bin2c_program(prog_name, bin2c_name);
			
		syscall_init(NULL, verbose, TRUE, NULL);
		load_program(prog_name, argc - i, argv + i, envp);
		if(debuging)
		{
			ppc_debug();
			printf("Bye.\n");
		}
		else
		{
			time_t start, stop;
			
			start = time(NULL);
			ppc_run();
			stop = time(NULL);
			fprintf(stderr, "%lld executed instructions\n", icount);
			fprintf(stderr, "%lf instructions/sec\n", (double) icount / (double)(stop - start));
			
		}
		ppc_delete();
	}
	else
	{
		fprintf(stderr, "usage : ppcemul [-d] [-v] <file name>\n  -d : debuging\n  -v : verbose\n  file name : a program to execute\n");
	}
	return 0;
}
