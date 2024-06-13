/***************************************************************************
                          Simulator.cpp  -  description
                             -------------------
    begin                : Thu Mar 28 2002
    copyright            : (C) 2002 CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <Simulator.h>
#include <stdlib.h>
#include <systemc.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ppcemul.h>
#include <signal.h>

#if defined(EMULATE) || defined(DEBUG) || defined(TRACE)
#else
#ifdef sc_stop
#undef sc_stop
#endif
#endif

bool Simulator::simulating = false;

double trace_start = 0.0;

Simulator *cur_sim = NULL;


#ifdef DEBUG
bool Debug(int what)
{
	return time_stamp > trace_start;
}
#endif

#ifdef TRACE
bool Trace(int what)
{
	return time_stamp > trace_start;
}
#endif

#ifdef SC2
Simulator::Simulator() : clock()
#else
Simulator::Simulator() : clock("clock", 1, 0.5, 0, true)
#endif
{
	ppc_init();

	powerpc = new PowerPC("PowerPC");
	powerpc->inClock(clock);
	time_stamp = 0.0;
	powerpc->Reset();
	cur_sim = this;
}

Simulator::~Simulator()
{
}

bool Simulator::LoadProgram(char *filename, int argc, char *argv[], char *envp[])
{
	/* Load the program into the emulator memory first */
	load_program(filename, argc, argv, envp);
	/* Load the program into the simulator memory */
	return powerpc->LoadProgram(filename, argc, argv, envp);
}

void Simulator::Emulate(unsigned long long ninstructions)
{
	cout << "Starting emulation" << endl; cout.flush();
	syscall_init(NULL, /* verbose */FALSE, /*emulate */ TRUE, NULL);
	if(ninstructions > 0)
		ppc_run_until(ninstructions);
	else
		ppc_debug();

		/*
	syscall_cb_t syscall_cb;
	
	syscall_cb.write_gpr = PowerPC::write_gpr;
	syscall_cb.read_gpr = PowerPC::read_gpr;
	syscall_cb.reset_cr0so = PowerPC::reset_cr0so;
	syscall_cb.set_cr0so = PowerPC::set_cr0so;
	syscall_cb.mem_read_byte = PowerPC::mem_read_byte;
	syscall_cb.mem_write_byte = PowerPC::mem_write_byte;
	syscall_cb.mem_read_half_word = PowerPC::mem_read_half_word;
	syscall_cb.mem_write_half_word = PowerPC::mem_write_half_word;
	syscall_cb.mem_read_word = PowerPC::mem_read_word;
	syscall_cb.mem_write_word = PowerPC::mem_write_word;
	syscall_cb.mem_read_dword = PowerPC::mem_read_dword;
	syscall_cb.mem_write_dword = PowerPC::mem_write_dword;
	syscall_cb.mem_set = PowerPC::mem_set;
	syscall_cb.mem_read = PowerPC::mem_read;
	syscall_cb.mem_write = PowerPC::mem_write;
	syscall_cb.instance = NULL;
					
	syscall_init(&syscall_cb, FALSE, FALSE, exit_proc);
		  */
	registers_cb_t regs_cb;
	memory_cb_t mem_cb;
		
	regs_cb.write_gpr = PowerPC::write_gpr;
	regs_cb.read_gpr = PowerPC::read_gpr;
	regs_cb.write_fpr = PowerPC::write_fpr;
	regs_cb.read_fpr = PowerPC::read_fpr;
	regs_cb.write_cr = PowerPC::write_cr;
	regs_cb.read_cr = PowerPC::read_cr;
	regs_cb.write_lr = PowerPC::write_lr;
	regs_cb.read_lr = PowerPC::read_lr;
	regs_cb.write_ctr = PowerPC::write_ctr;
	regs_cb.read_ctr = PowerPC::read_ctr;
	regs_cb.write_xer = PowerPC::write_xer;
	regs_cb.read_xer = PowerPC::read_xer;
	regs_cb.write_fpscr = PowerPC::write_fpscr;
	regs_cb.read_fpscr = PowerPC::read_fpscr;
	regs_cb.write_pc = PowerPC::write_pc;
	regs_cb.read_pc = PowerPC::read_pc;
	regs_cb.instance = NULL;
		
	mem_cb.mem_read_byte = PowerPC::mem_read_byte;
	mem_cb.mem_write_byte = PowerPC::mem_write_byte;
	mem_cb.mem_read_half_word = PowerPC::mem_read_half_word;
	mem_cb.mem_write_half_word = PowerPC::mem_write_half_word;
	mem_cb.mem_read_word = PowerPC::mem_read_word;
	mem_cb.mem_write_word = PowerPC::mem_write_word;
	mem_cb.mem_read_dword = PowerPC::mem_read_dword;
	mem_cb.mem_write_dword = PowerPC::mem_write_dword;
	mem_cb.mem_set = PowerPC::mem_set;
	mem_cb.mem_read = PowerPC::mem_read;
	mem_cb.mem_write = PowerPC::mem_write;
	mem_cb.instance = NULL;


	regs_duplicate(&regs_cb);
	mem_duplicate(mem, &mem_cb);

	powerpc->Reset();
}

void Simulator::Simulate(unsigned long long ninstructions, double duration, unsigned long long subchunk_size)
{
	int refreshPeriod = 1000;
	bool cycleStarted = false;
	UInt32 last1 = 0, last2 = 0;

	syscall_cb_t syscall_cb;

	syscall_cb.write_gpr = PowerPC::write_gpr;
	syscall_cb.read_gpr = PowerPC::read_gpr;
	syscall_cb.reset_cr0so = PowerPC::reset_cr0so;
	syscall_cb.set_cr0so = PowerPC::set_cr0so;
	syscall_cb.mem_read_byte = PowerPC::mem_read_byte;
	syscall_cb.mem_write_byte = PowerPC::mem_write_byte;
	syscall_cb.mem_read_half_word = PowerPC::mem_read_half_word;
	syscall_cb.mem_write_half_word = PowerPC::mem_write_half_word;
	syscall_cb.mem_read_word = PowerPC::mem_read_word;
	syscall_cb.mem_write_word = PowerPC::mem_write_word;
	syscall_cb.mem_read_dword = PowerPC::mem_read_dword;
	syscall_cb.mem_write_dword = PowerPC::mem_write_dword;
	syscall_cb.mem_set = PowerPC::mem_set;
	syscall_cb.mem_read = PowerPC::mem_read;
	syscall_cb.mem_write = PowerPC::mem_write;
	syscall_cb.instance = NULL;

	syscall_init(&syscall_cb, /* verbose */FALSE, /*emulate */ FALSE, exit_proc);

	powerpc->Stop(ninstructions);
	signal(SIGINT, Simulator::sigint_handler);
	cout << "Starting simulation" << endl; cout.flush();
#if defined(DEBUG) || defined(EMULATE) || defined(TRACE)
	sc_initialize();
#endif
	start = time(NULL);
	simulating = true;
	bool simulateForEver = duration < 0.0;

#if defined(DEBUG) || defined(EMULATE) || defined(TRACE)
	double t = 0.0;
	while(simulating && (!simulateForEver || t < duration))
	{
		*clock=1;
#ifdef TRACE
		if(Trace(TraceAll))
		{
			if(!cycleStarted)
			{
				trace_file->Begin("cycle");
				trace_file->Value("date", TimeStamp());
				cycleStarted = true;
			}
		}
#endif
#ifdef DEBUG
		if(Debug(DebugPowerPC))
		{
			cout << "----------------------------------------" << endl;
			cout << "  time stamp "; fprintf(stdout, "%lf", TimeStamp()); cout << endl;
			cout << "----------------------------------------" << endl;
			cout << "dispatchedInstructions = " << powerpc->dispatchedInstructions << endl;
		}
#endif

#ifdef EMULATE
		powerpc->CheckRegisters();
		if(--refreshPeriod == 0)
		{
			if(last1 == powerpc->completionUnit->nRetiredInstructions)
			{
				cerr << "Processor is totally stalled" << endl;
				cerr << "time stamp = " << TimeStamp() << endl;
				StopSimulation();
			}
			last1 = powerpc->completionUnit->nRetiredInstructions;
			refreshPeriod = 1000;
		}
#endif

#ifdef DEBUG
		if(((long) TimeStamp() % 1000) == 0)
		{
			cerr << "time stamp " << TimeStamp() << endl;
			cerr << powerpc->completionUnit->nRetiredInstructions	 << " retired instructions (IPC =" << (double)(powerpc->completionUnit->nRetiredInstructions - last2) / 1000.0 << ")" << endl;
			last2 = powerpc->completionUnit->nRetiredInstructions;
		}
#endif

		wait(0.5);
		*clock=0;
#ifdef DEBUG
		if(Debug(DebugPowerPC))
		{
			cout << "----------------------------------------" << endl;
			cout << "  time stamp "; fprintf(stdout, "%lf", TimeStamp()); cout << endl;
			cout << "----------------------------------------" << endl;
			cout << "dispatchedInstructions = " << powerpc->dispatchedInstructions << endl;
		}
#endif
		wait(0.5);
#ifdef TRACE
		if(Trace(TraceAll))
		{
			if(cycleStarted)
			{
				trace_file->End("cycle");
				cycleStarted = false;
			}
		}
#endif
		t = t + 1.0;
		time_stamp = time_stamp + 1.0;
	}
#ifdef TRACE
	if(cycleStarted)
	{
		trace_file->End("cycle");
		cycleStarted = false;
	}
#endif
#else
	sc_initialize();
	if(simulateForEver)
	{
		if(subchunk_size > 0)
		{
			int next_inst = subchunk_size;
			while(simulating)
			{
				clock = 1;
				wait(0.5);
				clock = 0;
				wait(0.5);
				if(powerpc->completionUnit->nRetiredInstructions >= next_inst)
				{
					DumpStats();
					next_inst += subchunk_size;
				}
				time_stamp = time_stamp + 1.0;
			}
		}
		else
		{
			while(simulating)
			{
				clock = 1;
				wait(0.5);
				clock = 0;
				wait(0.5);
				time_stamp = time_stamp + 1.0;
			}
		}
	}
	else
	{
		double t = 0.0;
		while(simulating && t < duration)
		{
			clock = 1;
			wait(0.5);
			clock = 0;
			wait(0.5);
			t = t + 1.0;
			time_stamp = time_stamp + 1.0;
		}
	}
#endif
	simulating = false;

	time_t stop = time(NULL);
	totalTime = stop - start;
	if(totalTime == 0) totalTime = 1;

//	powerpc->fetchUnit->programCounter = powerpc->completionUnit->programCounter;
}

void Simulator::sigint_handler(int signum)
{
	if(Simulator::simulating)
	{
		StopSimulation();
	}
	else
	{
		exit(0);
	}
}

void StopSimulation()
{
	Simulator::simulating = false;
}

void Simulator::exit_proc(int code)
{
	cerr << "Program exit with code " << code << endl;
	StopSimulation();
}

void Simulator::DumpStats()
{
	time_t stop = time(NULL);
	totalTime = stop - start;
	double duration = ceil(TimeStamp());
//	cerr << "Program : " <<  prog_name << endl;
	cerr << "total_time " << totalTime << " # total time (sec)" << endl;
	cerr << "cycles " << duration << " # cycles" << endl;
	if(totalTime > 0) cerr << "speed " << duration / totalTime << " # speed (cycles/sec)" << endl;
	cerr << "instructions " << powerpc->completionUnit->nRetiredInstructions << " # retired instructions" << endl;
	if(totalTime > 0) cerr << "inst_rate " << powerpc->completionUnit->nRetiredInstructions / totalTime << " # instructions/sec" << endl;
	if(duration > 0) cerr << "IPC " << (double) powerpc->completionUnit->nRetiredInstructions / duration << " # instructions/cycle" << endl;
	cerr << "icache.hits " << powerpc->icache->hits << " # icache hits" << endl;
	cerr << "icache.misses " << powerpc->icache->misses << " # icache misses" << endl;
	cerr << "icache.accesses " << powerpc->icache->hits + powerpc->icache->misses << " # icache accesses" << endl;
	if(powerpc->icache->hits + powerpc->icache->misses > 0)
	{
		cerr << "icache.hit_rate " << (double) powerpc->icache->hits / (double)(powerpc->icache->hits + powerpc->icache->misses) << " # icache hit rate (hits/accesses)" << endl;
		cerr << "icache.miss_rate " << (double) powerpc->icache->misses / (double)(powerpc->icache->hits + powerpc->icache->misses) << " # icache miss rate (hits/accesses)" << endl;
	}
	cerr << "dcache.hits " << powerpc->dcache->hits << " # dcache hits" << endl;
	cerr << "dcache.misses " << powerpc->dcache->misses << " # dcache misses" << endl;
	cerr << "dcache.accesses " << powerpc->dcache->hits + powerpc->dcache->misses << " # cache accesses" << endl;
	if(powerpc->dcache->hits + powerpc->dcache->misses > 0)
	{
		cerr << "dcache.hit_rate " << (double) powerpc->dcache->hits / (double)(powerpc->dcache->hits + powerpc->dcache->misses) << " # dcache hit rate (hits/accesses)"  << endl;
		cerr << "dcache.miss_rate " << (double) powerpc->dcache->misses / (double)(powerpc->dcache->hits + powerpc->dcache->misses) << " # dcache hit rate (hits/accesses)"  << endl;
	}
	cerr << "btic.hits " << powerpc->fetchUnit->bticHits << " # BTIC hits" << endl;
	cerr << "btic.misses " << powerpc->fetchUnit->bticMisses << " # BTIC misses" << endl;
	cerr << "btic.accesses " << powerpc->fetchUnit->bticHits + powerpc->fetchUnit->bticMisses << " # BTIC accesses" << endl;
	if(powerpc->fetchUnit->bticHits + powerpc->fetchUnit->bticMisses > 0)
	{
		cerr << "btic.hit_rate " << (double) powerpc->fetchUnit->bticHits / (double)(powerpc->fetchUnit->bticHits + powerpc->fetchUnit->bticMisses) << " # BTIC hit rate (hits/accesses)"  << endl;
		cerr << "btic.miss_rate " << (double) powerpc->fetchUnit->bticMisses / (double)(powerpc->fetchUnit->bticHits + powerpc->fetchUnit->bticMisses) << " # BTIC hit rate (hits/accesses)"  << endl;
	}
	cerr << "bus.accesses " << powerpc->busInterfaceUnit->busActivity << " # number of bus accesses" << endl;
	if(duration > 0) cerr << "bus.activity " <<  (double) powerpc->busInterfaceUnit->busActivity / duration << " # bus activity (accesses/cycles)" << endl;
	cerr << "branch.well_predicted_branches " << powerpc->branchUnit->wellPredictedBranches << " # well predicted branches" << endl;
	cerr << "branch.mispredicted_branchs " << powerpc->branchUnit->mispredictedBranches << " # mispredicted branches" << endl;
	if(powerpc->branchUnit->mispredictedBranches + powerpc->branchUnit->wellPredictedBranches > 0)
		cerr << "branch.pred_ratio " << (double) powerpc->branchUnit->wellPredictedBranches / (double)(powerpc->branchUnit->mispredictedBranches + powerpc->branchUnit->wellPredictedBranches) << " # good prediction ratio" << endl;
	cerr << "branch.BCs " << powerpc->branchUnit->nBC << " # number of BCs" << endl;
	cerr << "branch.mispred_BCs " << powerpc->branchUnit->nMispredictedBC << " # mispredicted BCs" << endl;
	cerr << "branch.BLCRs " << powerpc->branchUnit->nBCLR << " # number of BCLRs" << endl;
	cerr << "branch.BCCTRs " << powerpc->branchUnit->nBCCTR << " # number of BCCTRs" << endl;
	cerr << "\f\n";
}

double sim_time_stamp()
{
	return cur_sim->time_stamp;
}
