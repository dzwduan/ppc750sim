/***************************************************************************
                          Simulator.h  -  description
                             -------------------
    begin                : Thu Mar 28 2002
    copyright            : (C) 2002 CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <PowerPC.h>
#include <time.h>

extern double trace_start;

class Simulator
{
	public:
		static void sigint_handler(int signum);
		static bool simulating;
		static void exit_proc(int code);


		Simulator();
		~Simulator();
		bool LoadProgram(char *filename, int argc, char *argv[], char *envp[]);
		void Emulate(unsigned long long ninstructions);
		void Simulate(unsigned long long ninstructions, double duration, unsigned long long subchunk_size);
		void DumpStats();
		double TimeStamp() { return time_stamp; }
	private:
#ifdef SC2
		sc_signal<bool> clock;
#else
		sc_clock clock;
#endif
		PowerPC *powerpc;
		time_t totalTime;
		double time_stamp;
		time_t start;

		friend double sim_time_stamp();
};

#endif
