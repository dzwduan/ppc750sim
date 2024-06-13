/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Mar 28 2002
    copyright            : (C) 2002 CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <Simulator.h>

extern "C" {
extern char **environ;
}

int sc_main(int argc, char *argv[])
{
	unsigned long long ninst_to_fastforward = 0;
	unsigned long long ninst_to_emulate = 0;
	unsigned long long ninst_to_simulate = 0;
	unsigned long long subchunk_size = 0;
	char *prog_name = NULL;
	double duration = -1;
	bool fastforward = false;
	bool emulate = false;
	int i;
	char *trace_file_name = NULL;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-d") == 0)
		{
			i++;
			if(i < argc)
				trace_start = atof(argv[i]);

			cerr << "Trace Start at time stamp = " << trace_start << endl;
		}
		else
		{
			if(strcmp(argv[i], "-t") == 0)
			{
				i++;
				if(i < argc)
					duration = atof(argv[i]);
			}
			else
			{
				if(strcmp(argv[i], "-fastfwd") == 0)
				{
					fastforward = true;
					i++;
					if(i < argc)
						ninst_to_fastforward = atoll(argv[i]);
				}
				else
				{
					if(strcmp(argv[i], "-emulsize") == 0)
					{
						emulate = true;
						i++;
						if(i < argc)
							ninst_to_emulate = atoll(argv[i]);
					}
					else
					{
						if(strcmp(argv[i], "-trace") == 0)
						{
							i++;
							if(i < argc)
								trace_file_name = argv[i];
						}
						else
						{
							if(strcmp(argv[i], "-max:inst") == 0)
							{
								i++;
								if(i < argc)
									ninst_to_simulate = atoll(argv[i]);
							}
							else
							{
								if(strcmp(argv[i], "-subchunksize") == 0)
								{
									i++;
									if(i < argc)
										subchunk_size = atoll(argv[i]);
								}
								else
								{
									prog_name = argv[i];
									break;
								}
							}
						}
					}
				}
			}
		}
	}

#ifdef TRACE
	trace_file = new TraceFile(trace_file_name ? trace_file_name : "trace.xml");
	trace_file->Begin("cycle");
	trace_file->Value("date", my_sim_time_stamp());
	cycleStarted = true;
#endif

	if(prog_name == NULL)
	{
		cerr << "usage : powerpc [<options>] <file name>" << endl;
#ifdef DEBUG
		cerr << "        -d <time stamp> : dump debuging infos beginning at <time stamp>" << endl;
#endif
#ifdef TRACE
		cerr << "        -trace <trace file name> : output a trace to file (XML format)" << endl;
#endif
		cerr << "        -t <duration> : simulate during <duration>" << endl;
		cerr << "        -max:inst <n> : simulate n instructions" << endl;
		cerr << "        -fastfwd <count> : emulate some peaces of code before running the simulation itself" << endl;
		cerr << "        -emulsize <count> : emulate some peaces of code before running the simulation itself" << endl;
		cerr << "        -subchunksize <subchunk_size> : for use with DiST" << endl;
		cerr << "        <file name> : a program to execute" << endl;
		return 0;
	}

	Simulator sim;
	sim.LoadProgram(prog_name, argc - i, argv + i, environ);
	cerr << "\v\n" << endl; // before fastforwarding
	if(fastforward && ninst_to_fastforward > 0)
	{
		sim.Emulate(ninst_to_fastforward);
	}
	/*
	cerr << "\v\n" << endl;  // after fastforwarding
	if(emulate && ninst_to_emulate > 0)
	{
		sim.Emulate(ninst_to_emulate);
	}
	*/
	cerr << "\v\n" << endl; // before simulation
	sim.Simulate(ninst_to_simulate, duration, subchunk_size);
	sim.DumpStats();
	return 0;
}
