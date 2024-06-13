/***************************************************************************
                          syscall.c  -  description
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

#ifdef linux
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <syscall.h>
#include <ppcemul.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <xmlize.h>

#define __SYSCALL_exit		  1
#define __SYSCALL_fork		  2
#define __SYSCALL_read		  3
#define __SYSCALL_write		  4
#define __SYSCALL_open		  5
#define __SYSCALL_close		  6
#define __SYSCALL_waitpid		  7
#define __SYSCALL_creat		  8
#define __SYSCALL_link		  9
#define __SYSCALL_unlink		 10
#define __SYSCALL_execve		 11
#define __SYSCALL_chdir		 12
#define __SYSCALL_time		 13
#define __SYSCALL_mknod		 14
#define __SYSCALL_chmod		 15
#define __SYSCALL_lchown		 16
#define __SYSCALL_break		 17
#define __SYSCALL_oldstat		 18
#define __SYSCALL_lseek		 19
#define __SYSCALL_getpid		 20
#define __SYSCALL_mount		 21
#define __SYSCALL_umount		 22
#define __SYSCALL_setuid		 23
#define __SYSCALL_getuid		 24
#define __SYSCALL_stime		 25
#define __SYSCALL_ptrace		 26
#define __SYSCALL_alarm		 27
#define __SYSCALL_oldfstat		 28
#define __SYSCALL_pause		 29
#define __SYSCALL_utime		 30
#define __SYSCALL_stty		 31
#define __SYSCALL_gtty		 32
#define __SYSCALL_access		 33
#define __SYSCALL_nice		 34
#define __SYSCALL_ftime		 35
#define __SYSCALL_sync		 36
#define __SYSCALL_kill		 37
#define __SYSCALL_rename		 38
#define __SYSCALL_mkdir		 39
#define __SYSCALL_rmdir		 40
#define __SYSCALL_dup		 41
#define __SYSCALL_pipe		 42
#define __SYSCALL_times		 43
#define __SYSCALL_prof		 44
#define __SYSCALL_brk		 45
#define __SYSCALL_setgid		 46
#define __SYSCALL_getgid		 47
#define __SYSCALL_signal		 48
#define __SYSCALL_geteuid		 49
#define __SYSCALL_getegid		 50
#define __SYSCALL_acct		 51
#define __SYSCALL_umount2		 52
#define __SYSCALL_lock		 53
#define __SYSCALL_ioctl		 54
#define __SYSCALL_fcntl		 55
#define __SYSCALL_mpx		 56
#define __SYSCALL_setpgid		 57
#define __SYSCALL_ulimit		 58
#define __SYSCALL_oldolduname	 59
#define __SYSCALL_umask		 60
#define __SYSCALL_chroot		 61
#define __SYSCALL_ustat		 62
#define __SYSCALL_dup2		 63
#define __SYSCALL_getppid		 64
#define __SYSCALL_getpgrp		 65
#define __SYSCALL_setsid		 66
#define __SYSCALL_sigaction		 67
#define __SYSCALL_sgetmask		 68
#define __SYSCALL_ssetmask		 69
#define __SYSCALL_setreuid		 70
#define __SYSCALL_setregid		 71
#define __SYSCALL_sigsuspend		 72
#define __SYSCALL_sigpending		 73
#define __SYSCALL_sethostname	 74
#define __SYSCALL_setrlimit		 75
#define __SYSCALL_getrlimit		 76
#define __SYSCALL_getrusage		 77
#define __SYSCALL_gettimeofday	 78
#define __SYSCALL_settimeofday	 79
#define __SYSCALL_getgroups		 80
#define __SYSCALL_setgroups		 81
#define __SYSCALL_select		 82
#define __SYSCALL_symlink		 83
#define __SYSCALL_oldlstat		 84
#define __SYSCALL_readlink		 85
#define __SYSCALL_uselib		 86
#define __SYSCALL_swapon		 87
#define __SYSCALL_reboot		 88
#define __SYSCALL_readdir		 89
#define __SYSCALL_mmap		 90
#define __SYSCALL_munmap		 91
#define __SYSCALL_truncate		 92
#define __SYSCALL_ftruncate		 93
#define __SYSCALL_fchmod		 94
#define __SYSCALL_fchown		 95
#define __SYSCALL_getpriority	 96
#define __SYSCALL_setpriority	 97
#define __SYSCALL_profil		 98
#define __SYSCALL_statfs		 99
#define __SYSCALL_fstatfs		100
#define __SYSCALL_ioperm		101
#define __SYSCALL_socketcall		102
#define __SYSCALL_syslog		103
#define __SYSCALL_setitimer		104
#define __SYSCALL_getitimer		105
#define __SYSCALL_stat		106
#define __SYSCALL_lstat		107
#define __SYSCALL_fstat		108
#define __SYSCALL_olduname		109
#define __SYSCALL_iopl		110
#define __SYSCALL_vhangup		111
#define __SYSCALL_idle		112
#define __SYSCALL_vm86old		113
#define __SYSCALL_wait4		114
#define __SYSCALL_swapoff		115
#define __SYSCALL_sysinfo		116
#define __SYSCALL_ipc		117
#define __SYSCALL_fsync		118
#define __SYSCALL_sigreturn		119
#define __SYSCALL_clone		120
#define __SYSCALL_setdomainname	121
#define __SYSCALL_uname		122
#define __SYSCALL_modify_ldt		123
#define __SYSCALL_adjtimex		124
#define __SYSCALL_mprotect		125
#define __SYSCALL_sigprocmask	126
#define __SYSCALL_create_module	127
#define __SYSCALL_init_module	128
#define __SYSCALL_delete_module	129
#define __SYSCALL_get_kernel_syms	130
#define __SYSCALL_quotactl		131
#define __SYSCALL_getpgid		132
#define __SYSCALL_fchdir		133
#define __SYSCALL_bdflush		134
#define __SYSCALL_sysfs		135
#define __SYSCALL_personality	136
#define __SYSCALL_afs_syscall	137
#define __SYSCALL_setfsuid		138
#define __SYSCALL_setfsgid		139
#define __SYSCALL__llseek		140
#define __SYSCALL_getdents		141
#define __SYSCALL__newselect		142
#define __SYSCALL_flock		143
#define __SYSCALL_msync		144
#define __SYSCALL_readv		145
#define __SYSCALL_writev		146
#define __SYSCALL_getsid		147
#define __SYSCALL_fdatasync		148
#define __SYSCALL__sysctl		149
#define __SYSCALL_mlock		150
#define __SYSCALL_munlock		151
#define __SYSCALL_mlockall		152
#define __SYSCALL_munlockall		153
#define __SYSCALL_sched_setparam		154
#define __SYSCALL_sched_getparam		155
#define __SYSCALL_sched_setscheduler		156
#define __SYSCALL_sched_getscheduler		157
#define __SYSCALL_sched_yield		158
#define __SYSCALL_sched_get_priority_max	159
#define __SYSCALL_sched_get_priority_min	160
#define __SYSCALL_sched_rr_get_interval	161
#define __SYSCALL_nanosleep		162
#define __SYSCALL_mremap		163
#define __SYSCALL_setresuid		164
#define __SYSCALL_getresuid		165
#define __SYSCALL_vm86		166
#define __SYSCALL_query_module	167
#define __SYSCALL_poll		168
#define __SYSCALL_nfsservctl		169
#define __SYSCALL_setresgid		170
#define __SYSCALL_getresgid		171
#define __SYSCALL_prctl              172
#define __SYSCALL_rt_sigreturn	173
#define __SYSCALL_rt_sigaction	174
#define __SYSCALL_rt_sigprocmask	175
#define __SYSCALL_rt_sigpending	176
#define __SYSCALL_rt_sigtimedwait	177
#define __SYSCALL_rt_sigqueueinfo	178
#define __SYSCALL_rt_sigsuspend	179
#define __SYSCALL_pread		180
#define __SYSCALL_pwrite		181
#define __SYSCALL_chown		182
#define __SYSCALL_getcwd		183
#define __SYSCALL_capget		184
#define __SYSCALL_capset		185
#define __SYSCALL_sigaltstack	186
#define __SYSCALL_sendfile		187
#define __SYSCALL_getpmsg		188
#define __SYSCALL_putpmsg		189
#define __SYSCALL_vfork		190
#define __SYSCALL_ugetrlimit		191
#define __SYSCALL_mmap2		192
#define __SYSCALL_truncate64		193
#define __SYSCALL_ftruncate64	194
#define __SYSCALL_stat64		195
#define __SYSCALL_lstat64		196
#define __SYSCALL_fstat64		197
#define __SYSCALL_lchown32		198
#define __SYSCALL_getuid32		199
#define __SYSCALL_getgid32		200
#define __SYSCALL_geteuid32		201
#define __SYSCALL_getegid32		202
#define __SYSCALL_setreuid32		203
#define __SYSCALL_setregid32		204
#define __SYSCALL_getgroups32	205
#define __SYSCALL_setgroups32	206
#define __SYSCALL_fchown32		207
#define __SYSCALL_setresuid32	208
#define __SYSCALL_getresuid32	209
#define __SYSCALL_setresgid32	210
#define __SYSCALL_getresgid32	211
#define __SYSCALL_chown32		212
#define __SYSCALL_setuid32		213
#define __SYSCALL_setgid32		214
#define __SYSCALL_setfsuid32		215
#define __SYSCALL_setfsgid32		216
#define __SYSCALL_pivot_root		217
#define __SYSCALL_mincore		218
#define __SYSCALL_madvise		219
#define __SYSCALL_getdents64		220
#define __SYSCALL_fcntl64		221

extern BOOL debuging;

static addr_t brk_point = 0;
static BOOL swap = FALSE;
static BOOL verbose = FALSE;
BOOL emulate_syscall = FALSE;
static syscall_cb_t syscall_cb;
static void (*exit_proc)(int) = NULL;

static void default_exit_proc(int code)
{
	exit(code);
}

static void cb_write_gpr(void *instance, int n, word_t value)
{
	my_regs.gpr[n] = value;
}

static word_t cb_read_gpr(void *instance, int n)
{
	return my_regs.gpr[n];
}

static void cb_set_cr0so(void *instance)
{
	my_regs.cr = my_regs.cr | 0x10000000;
}

static void cb_reset_cr0so(void *instance)
{
	my_regs.cr = my_regs.cr & 0xefffffff;
}

static void cb_mem_write_byte(void *instance, addr_t addr, byte_t value)
{
	mem_write_byte(mem, addr, value);
}

static byte_t cb_mem_read_byte(void *instance, addr_t addr)
{
	return mem_read_byte(mem, addr);
}

static void cb_mem_write_half_word(void *instance, addr_t addr, halfword_t value)
{
	mem_write_half_word(mem, addr, value);
}

static halfword_t cb_mem_read_half_word(void *instance, addr_t addr)
{
	return mem_read_half_word(mem, addr);
}

static void cb_mem_write_word(void *instance, addr_t addr, word_t value)
{
	mem_write_word(mem, addr, value);
}

static word_t cb_mem_read_word(void *instance, addr_t addr)
{
	return mem_read_word(mem, addr);
}

static void cb_mem_write_dword(void *instance, addr_t addr, dword_t value)
{
	mem_write_dword(mem, addr, value);
}

static dword_t cb_mem_read_dword(void *instance, addr_t addr)
{
	return mem_read_dword(mem, addr);
}

static void cb_mem_read(void *instance, void *buf, addr_t addr, int size)
{
	mem_read(mem, buf, addr, size);
}

static void cb_mem_write(void *instance, addr_t addr, void *buf, int size)
{
	mem_write(mem, addr, buf, size);
}

static void cb_mem_set(void *instance, addr_t addr, byte_t value, int size)
{
	mem_set(mem, addr, value, size);
}

static syscall_cb_t cb_default = {
	cb_write_gpr,
	cb_read_gpr,
	cb_reset_cr0so,
	cb_set_cr0so,
	cb_mem_read_byte,
	cb_mem_write_byte,
	cb_mem_read_half_word,
	cb_mem_write_half_word,
	cb_mem_read_word,
	cb_mem_write_word,
	cb_mem_read_dword,
	cb_mem_write_dword,
	cb_mem_set,
	cb_mem_read,
	cb_mem_write
};

void syscall_setcb(syscall_cb_t *cb)
{
	syscall_cb = cb ? *cb : cb_default;
}

void syscall_getcb(syscall_cb_t *cb)
{
	*cb = syscall_cb;
}

void syscall_init(syscall_cb_t *cb, BOOL _verbose, BOOL emulate, void (*_exit_proc)(int))
{
	int a = 0x1234;
	swap = *(unsigned char *) &a == 0x34;
	syscall_cb = cb ? *cb : cb_default;
	verbose = _verbose;
	emulate_syscall = emulate;
	exit_proc = _exit_proc ? _exit_proc : default_exit_proc;
}

void sc_impl(ppc_inst_t inst)
{
	if(emulate_syscall)
		do_syscall();
}

char *sc_bin2c(ppc_inst_t inst, addr_t pc, char *buf)
{
	buf += sprintf(buf, "\tdo_syscall();\n");
	return buf;
}

char *sc_disasm(ppc_inst_t inst, addr_t pc, char *buf) { buf += sprintf(buf, "sc"); return buf; }

void sc_translate(ppc_inst_t inst, struct DecodedInstruction *di)
{
	di->iclass = ICSystemCall;
	di->operation.systemRegister.ident = ID_SC;
	di->hasImmed = FALSE;
}

void sc_xml(ppc_inst_t inst, addr_t pc, char *buf) { xmlf(buf, pc, "sc", 0); }

#undef READ_GPR
#undef WRITE_GPR
#undef RESET_CR0SO
#undef SET_CR0SO
#undef MEM_READ_BYTE
#undef MEM_WRITE_BYTE
#undef MEM_READ_HALF_WORD
#undef MEM_WRITE_HALF_WORD
#undef MEM_READ_WORD
#undef MEM_WRITE_WORD
#undef MEM_READ_DWORD
#undef MEM_WRITE_DWORD
#undef MEM_SET
#undef MEM_READ
#undef MEM_WRITE

#define READ_GPR(n) cb->read_gpr(cb->instance, n)
#define WRITE_GPR(n, v) cb->write_gpr(cb->instance, n, v)
#define RESET_CR0SO cb->reset_cr0so(cb->instance)
#define SET_CR0SO cb->set_cr0so(cb->instance)
#define MEM_READ_BYTE(addr) cb->mem_read_byte(cb->instance, addr)
#define MEM_WRITE_BYTE(addr, value) cb->mem_write_byte(cb->instance, addr, value)
#define MEM_READ_HALF_WORD(addr) cb->mem_read_half_word(cb->instance, addr)
#define MEM_WRITE_HALF_WORD(addr, value) cb->mem_write_half_word(cb->instance, addr, value)
#define MEM_READ_WORD(addr) cb->mem_read_word(cb->instance, addr)
#define MEM_WRITE_WORD(addr, value) cb->mem_write_word(cb->instance, addr, value)
#define MEM_READ_DWORD(addr) cb->mem_read_dword(cb->instance, addr)
#define MEM_WRITE_DWORD(addr, value) cb->mem_write_dword(cb->instance, addr, value)
#define MEM_SET(addr, value, size) cb->mem_set(cb->instance, addr, value, size)
#define MEM_READ(buf, addr, size) cb->mem_read(cb->instance, buf, addr, size)
#define MEM_WRITE(addr, buf, size) cb->mem_write(cb->instance, addr, buf, size)
#define STRLEN(addr) my_strlen(cb, addr)

#define PARM(i) READ_GPR(3+(i))
#define RETURN(x) WRITE_GPR(3, (x))
#define SYSCALL_NUM READ_GPR(0)

static void ppc_swap(void *buf, int count)
{
	if(count > 0)
	{
		char temp[8];
		char *src = (char *) buf + count - 1;
		char *dst = temp;
		
		do
		{
			*dst = *src;
		} while(src--, dst++, --count);
	}
}

static int my_strlen(syscall_cb_t *cb, addr_t addr)
{
	int len = 0;
	char buffer[32];
	
	while(1)
	{
		int size = sizeof(buffer);
		char *p = buffer;
		MEM_READ(buffer, addr, size);
		while(size > 0)
		{
			if(*p == 0) return len;
			len++;
			size--;
			p++;
		}
		addr += sizeof(buffer);
	}
}

BOOL ppc_syscall_exit(syscall_cb_t *cb)
{
	int ret = PARM(0);
	if(verbose)
		fprintf(stderr, "exit(%d)\n", ret);
	if(debuging)
	{
		debuging = FALSE;
	}
	else
	{
		if(running)
			running = FALSE;
		else
			exit_proc(ret);
	}
	return TRUE;
}

BOOL ppc_syscall_fork(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

BOOL ppc_syscall_read(syscall_cb_t *cb)
{
	int fd;
	size_t count;
	addr_t buf_addr;
	void *buf;
	size_t ret;
	
	fd = PARM(0);
	buf_addr = PARM(1);
	count = (size_t) PARM(2);
	buf = malloc(count);
	if(verbose)
		fprintf(stderr, "read(fd=%d, buf=0x%08x, count=%d)\n", fd, buf_addr, count);
		
	if(buf)
	{
		ret = read(fd, buf, count);
		if(ret > 0) MEM_WRITE(buf_addr, buf, ret);
		free(buf);
	}
	else
	{
		ret = -1;
	}
	RETURN(ret);
	return ret != (size_t) -1;
}

BOOL ppc_syscall_write(syscall_cb_t *cb)
{
	int fd;
	size_t count;
	void *buf;
	addr_t buf_addr;
	size_t ret;
	
	fd = PARM(0);
	buf_addr = PARM(1);
	count = (size_t) PARM(2);
	if(verbose)
		fprintf(stderr, "write(fd=%d, buf=0x%08x, count=%d)\n", fd, buf_addr, count);
	buf = malloc(count);
	if(buf)
	{
		MEM_READ(buf, buf_addr, count);
		ret = write(fd, buf, count);
		free(buf);
	}
	else
	{
		ret = -1;
	}
	RETURN(ret);
	return ret != (size_t) -1;
}

BOOL ppc_syscall_open(syscall_cb_t *cb)
{
	addr_t addr;
	int pathnamelen;
	char *pathname;
	int flags;
	mode_t mode;
	int ret;
	
	addr = PARM(0);
	pathnamelen = STRLEN(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	MEM_READ(pathname, addr, pathnamelen + 1);
	flags = PARM(1);
	mode = PARM(2);
	if(verbose)
		fprintf(stderr, "open(pathname=\"%s\", flags=%d, mode=%d)\n", pathname, flags, mode);
	ret = open(pathname, flags, mode);
	free(pathname);
	RETURN(ret);
	return ret != -1;
}
BOOL ppc_syscall_close(syscall_cb_t *cb)
{
	int fd;
	int ret;
	
	fd = PARM(0);
	if(verbose)
		fprintf(stderr, "close(fd=%d)\n", fd);
	ret = close(fd);
	RETURN(ret);
	return ret != -1;
}

BOOL ppc_syscall_waitpid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_creat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_link(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_unlink(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_execve(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_chdir(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_time(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mknod(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_chmod(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_lchown(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_break(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_oldstat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

BOOL ppc_syscall_lseek(syscall_cb_t *cb)
{
	int fildes;
	off_t offset;
	int whence;
	off_t ret;
	
	fildes = PARM(0);
	offset = PARM(1);
	whence = PARM(2);
	if(verbose)
		fprintf(stderr, "lseek(fd=%d, offset=%lu, whence=%d)\n", fildes, offset, whence);
	ret = lseek(fildes, offset, whence);
	RETURN(ret);
	return ret != -1;
}

BOOL ppc_syscall_getpid(syscall_cb_t *cb)
{
	pid_t pid;
	
	if(verbose)
		fprintf(stderr, "getpid()\n");
	pid = getpid();
	RETURN(pid);
	return TRUE;
}

BOOL ppc_syscall_mount(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_umount(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setuid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getuid(syscall_cb_t *cb)
{
	uid_t uid;
	
	if(verbose)
		fprintf(stderr, "getuid()\n");
	uid = getuid();
	RETURN(uid);
	return TRUE;
}

BOOL ppc_syscall_stime(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ptrace(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_alarm(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_oldfstat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_pause(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_utime(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_stty(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_gtty(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_access(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_nice(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ftime(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sync(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_kill(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rename(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mkdir(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rmdir(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_dup(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_pipe(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

static void ppc_swap_tms(struct tms *buf)
{
	ppc_swap(&buf->tms_utime, sizeof(&buf->tms_utime));
	ppc_swap(&buf->tms_stime, sizeof(&buf->tms_stime));
	ppc_swap(&buf->tms_cutime, sizeof(&buf->tms_cutime));
	ppc_swap(&buf->tms_cstime, sizeof(&buf->tms_cstime));
}

BOOL ppc_syscall_times(syscall_cb_t *cb)
{
	addr_t buf_addr;
	struct tms buf;
	clock_t ret;
	
	ret = times(&buf);	
	if(swap) ppc_swap_tms(&buf);
	buf_addr = PARM(0);
	MEM_WRITE(buf_addr, &buf, sizeof(struct tms));
	RETURN(ret);
	return ret != (clock_t) -1;
}

BOOL ppc_syscall_prof(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

BOOL ppc_syscall_brk(syscall_cb_t *cb)
{
	word_t new_brk_point;
	BOOL success;
	
	new_brk_point = PARM(0);
	success = TRUE;//FALSE;
	if(verbose)
		fprintf(stderr, "brk(end=0x%08x)\n", new_brk_point);
	if(new_brk_point > brk_point)
	{
		brk_point = new_brk_point;
		success = TRUE;
	}
	
	RETURN(brk_point);
	return success;
}

BOOL ppc_syscall_setgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getgid(syscall_cb_t *cb)
{
	gid_t gid;
	if(verbose)
		fprintf(stderr, "getgid()\n");
	gid = getgid();
	RETURN(gid);
	return TRUE;
}

BOOL ppc_syscall_signal(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_geteuid(syscall_cb_t *cb)
{
	uid_t uid;
	
	if(verbose)
		fprintf(stderr, "geteuid()\n");
	uid = geteuid();
	RETURN(uid);
	return TRUE;
}

BOOL ppc_syscall_getegid(syscall_cb_t *cb)
{
	gid_t gid;
	
	if(verbose)
		fprintf(stderr, "getegid()\n");
	gid = getegid();
	RETURN(gid);
	return TRUE;
}

BOOL ppc_syscall_acct(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_umount2(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_lock(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ioctl(syscall_cb_t *cb) { 	RETURN(-1); return FALSE; }
BOOL ppc_syscall_fcntl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mpx(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setpgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ulimit(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_oldolduname(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_umask(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_chroot(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ustat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_dup2(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getppid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getpgrp(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setsid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigaction(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sgetmask(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ssetmask(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setreuid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setregid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigsuspend(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigpending(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sethostname(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setrlimit(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getrlimit(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getrusage(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_gettimeofday(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_settimeofday(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getgroups(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setgroups(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_select(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_symlink(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_oldlstat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_readlink(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_uselib(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_swapon(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_reboot(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_readdir(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mmap(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_munmap(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_truncate(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ftruncate(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fchmod(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fchown(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getpriority(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setpriority(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_profil(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_statfs(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fstatfs(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ioperm(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_socketcall(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_syslog(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setitimer(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getitimer(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_stat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_lstat(syscall_cb_t *cb) { RETURN(-1); return FALSE; }


static void ppc_swap_stat(struct stat *buf)
{
	ppc_swap(&buf->st_dev, sizeof(&buf->st_dev));
	ppc_swap(&buf->st_ino, sizeof(&buf->st_ino));
	ppc_swap(&buf->st_mode, sizeof(&buf->st_mode));
	ppc_swap(&buf->st_nlink, sizeof(&buf->st_nlink));
	ppc_swap(&buf->st_uid, sizeof(&buf->st_uid));
	ppc_swap(&buf->st_gid, sizeof(&buf->st_gid));
	ppc_swap(&buf->st_rdev, sizeof(&buf->st_rdev));
	ppc_swap(&buf->st_size, sizeof(&buf->st_size));
	ppc_swap(&buf->st_blksize, sizeof(&buf->st_blksize));
	ppc_swap(&buf->st_blocks, sizeof(&buf->st_blocks));
	ppc_swap(&buf->st_atime, sizeof(&buf->st_atime));
	ppc_swap(&buf->st_mtime, sizeof(&buf->st_mtime));
	ppc_swap(&buf->st_ctime, sizeof(&buf->st_ctime));
	ppc_swap(&buf->st_ino, sizeof(&buf->st_ino));
}

BOOL ppc_syscall_fstat(syscall_cb_t *cb)
{
	int fd;
	struct stat *buf;
	addr_t buf_addr;
	int ret;
	
	fd = PARM(0);
	buf_addr = PARM(1);
	if(verbose)
		fprintf(stderr, "fstat(fd=%d, buf=0x%08x)\n", fd, buf_addr);
	buf = (struct stat *) malloc(sizeof(struct stat));
	if(buf)
	{
		ret = fstat(fd, buf);
		if(ret >= 0)
		{
			if(swap) ppc_swap_stat(buf);
			MEM_WRITE(buf_addr, buf, sizeof(struct stat));
		}
		free(buf);
	}
	else
	{
		ret = -1;
	}
	RETURN(ret);
	return ret != -1;
}

BOOL ppc_syscall_olduname(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_iopl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_vhangup(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_idle(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_vm86old(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_wait4(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_swapoff(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sysinfo(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ipc(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fsync(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigreturn(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_clone(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setdomainname(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_uname(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_modify_ldt(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_adjtimex(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mprotect(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigprocmask(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_create_module(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_init_module(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_delete_module(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_get_kernel_syms(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_quotactl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getpgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fchdir(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_bdflush(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sysfs(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

BOOL ppc_syscall_personality(syscall_cb_t *cb) { return TRUE; }

BOOL ppc_syscall_afs_syscall(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setfsuid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setfsgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

BOOL ppc_syscall__llseek(syscall_cb_t *cb)
{
	int fd;
	UInt32 offset_high;
	UInt32 offset_low;
	addr_t result_addr;
	int whence;
	int ret;
	
	fd = PARM(0);
	offset_high = PARM(1);
	offset_low = PARM(2);
	result_addr = PARM(3);
	whence = PARM(4);
	if(verbose)
		fprintf(stderr, "_lseek(fd=%d, offset_high=%u, offset_low=%u, result=0x%08x, whence=%d)\n", fd, offset_high, offset_low, result_addr, whence);
	if(offset_high == 0)
	{
		off_t lseek_ret = lseek(fd, offset_low, whence);
		if(lseek_ret >= 0)
		{
			MEM_WRITE_DWORD(result_addr, lseek_ret);
			ret = 0;
		}
		else
		{
			ret = -1;
		}
	}
	else
	{
		ret = -1;
	}
	RETURN(ret);
	return ret != -1;
}

BOOL ppc_syscall_getdents(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_newselect(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_flock(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_msync(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_readv(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_writev(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getsid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fdatasync(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sysctl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mlock(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_munlock(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mlockall(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_munlockall(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_setparam(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_getparam(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_setscheduler(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getsheduler(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_yield(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_get_priority_max(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_get_priority_min(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sched_rr_get_interval(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_nanosleep(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mremap(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setresuid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getresuid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_vm86(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_query_module(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_poll(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_nfsservctl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setresgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getresgid(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_prctl(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigreturn(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigaction(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigprocmask(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigpending(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigtimedwait(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigqueueinfo(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_rt_sigsuspend(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_pread(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_pwrite(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_chown(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getcwd(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_capget(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_capset(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sigaltstack(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_sendfile(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getpmsg(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_putpmsg(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_vfork(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ugetrlimit(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mmap2(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_truncate64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_ftruncate64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_stat64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_lstat64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

#ifdef linux
static void ppc_swap_stat64(struct stat64 *buf)
{
	ppc_swap(&buf->st_dev, sizeof(&buf->st_dev));
	// ppc_swap(&buf->__pad1, sizeof(&buf->__pad1));
//	ppc_swap(&buf->__st_ino, sizeof(&buf->__st_ino));
	ppc_swap(&buf->st_mode, sizeof(&buf->st_mode));
	ppc_swap(&buf->st_nlink, sizeof(&buf->st_nlink));
	ppc_swap(&buf->st_uid, sizeof(&buf->st_uid));
	ppc_swap(&buf->st_gid, sizeof(&buf->st_gid));
	ppc_swap(&buf->st_rdev, sizeof(&buf->st_rdev));
	// ppc_swap(&buf->__pad2, sizeof(&buf->__pad2));
	ppc_swap(&buf->st_size, sizeof(&buf->st_size));
	ppc_swap(&buf->st_blksize, sizeof(&buf->st_blksize));
	ppc_swap(&buf->st_blocks, sizeof(&buf->st_blocks));
	ppc_swap(&buf->st_atime, sizeof(&buf->st_atime));
	// ppc_swap(&buf->__unused1, sizeof(&buf->__unused1));
	ppc_swap(&buf->st_mtime, sizeof(&buf->st_mtime));
	// ppc_swap(&buf->__unused2, sizeof(&buf->__unused2));
	ppc_swap(&buf->st_ctime, sizeof(&buf->st_ctime));
	// ppc_swap(&buf->__unused3, sizeof(&buf->__unused3));
	ppc_swap(&buf->st_ino, sizeof(&buf->st_ino));
}
#endif

BOOL ppc_syscall_fstat64(syscall_cb_t *cb)
{
#ifdef linux
	int fd;
	addr_t buf_addr;
	struct stat64 *buf;
	int ret;
	
	fd = PARM(0);
	buf_addr = PARM(1);
	if(verbose)
		fprintf(stderr, "fstat64(fd=%d, buf=0x%08x)\n", fd, buf_addr);
	buf = (struct stat64 *) malloc(sizeof(struct stat64));
	if(buf)
	{
		ret = fstat64(fd, buf);
		if(ret >= 0)
		{
			if(swap) ppc_swap_stat64(buf);
			MEM_WRITE(buf_addr, buf, sizeof(struct stat64));
		}
		free(buf);
	}
	else
	{
		ret = -1;
	}
	RETURN(ret);
	return ret != -1;
#else
	int ret = -1;
	RETURN(ret);
	return FALSE;
#endif
}

BOOL ppc_syscall_lchown32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getgid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_geteuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getegid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setreuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setregid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getgroups32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setgroups32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fchown32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setresuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getresuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setresgid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getresgid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_chown32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setgid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setfsuid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_setfsgid32(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_pivot_root(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_mincore(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_madvise(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_getdents64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }
BOOL ppc_syscall_fnctl64(syscall_cb_t *cb) { RETURN(-1); return FALSE; }

static char *ppc_get_syscall_name(int num)
{
	switch(num)
	{
		case __SYSCALL_exit: return "exit";
		case __SYSCALL_fork: return "fork";
		case __SYSCALL_read: return "read";
		case __SYSCALL_write: return "write";
		case __SYSCALL_open: return "open";
		case __SYSCALL_close: return "close";
		case __SYSCALL_waitpid: return "waitpid";
		case __SYSCALL_creat: return "creat";
		case __SYSCALL_link: return "link";
		case __SYSCALL_unlink: return "unlink";
		case __SYSCALL_execve: return "execve";
		case __SYSCALL_chdir: return "chdir";
		case __SYSCALL_time: return "time";
		case __SYSCALL_mknod: return "mknod";
		case __SYSCALL_chmod: return "chmod";
		case __SYSCALL_lchown: return "lchown";
		case __SYSCALL_break: return "break";
		case __SYSCALL_oldstat: return "oldstat";
		case __SYSCALL_lseek: return "lseek";
		case __SYSCALL_getpid: return "getpid";
		case __SYSCALL_mount: return "mount";
		case __SYSCALL_umount: return "umount";
		case __SYSCALL_setuid: return "setuid";
		case __SYSCALL_getuid: return "getuid";
		case __SYSCALL_stime: return "stime";
		case __SYSCALL_ptrace: return "ptrace";
		case __SYSCALL_alarm: return "alarm";
		case __SYSCALL_oldfstat: return "oldfstat";
		case __SYSCALL_pause: return "pause";
		case __SYSCALL_utime: return "utime";
		case __SYSCALL_stty: return "stty";
		case __SYSCALL_gtty: return "gtty";
		case __SYSCALL_access: return "access";
		case __SYSCALL_nice: return "nice";
		case __SYSCALL_ftime: return "ftime";
		case __SYSCALL_sync: return "sync";
		case __SYSCALL_kill: return "kill";
		case __SYSCALL_rename: return "rename";
		case __SYSCALL_mkdir: return "mkdir";
		case __SYSCALL_rmdir: return "rmdir";
		case __SYSCALL_dup: return "dup";
		case __SYSCALL_pipe: return "pipe";
		case __SYSCALL_times: return "times";
		case __SYSCALL_prof: return "prof";
		case __SYSCALL_brk: return "brk";
		case __SYSCALL_setgid: return "setgid";
		case __SYSCALL_getgid: return "getgid";
		case __SYSCALL_signal: return "signal";
		case __SYSCALL_geteuid: return "geteuid";
		case __SYSCALL_getegid: return "getegid";
		case __SYSCALL_acct: return "acct";
		case __SYSCALL_umount2: return "umount2";
		case __SYSCALL_lock: return "lock";
		case __SYSCALL_ioctl: return "ioctl";
		case __SYSCALL_fcntl: return "fcntl";
		case __SYSCALL_mpx: return "mpx";
		case __SYSCALL_setpgid: return "setpgid";
		case __SYSCALL_ulimit: return "ulimit";
		case __SYSCALL_oldolduname: return "oldolduname";
		case __SYSCALL_umask: return "umask";
		case __SYSCALL_chroot: return "chroot";
		case __SYSCALL_ustat	: return "ustat";
		case __SYSCALL_dup2: return "dup2";
		case __SYSCALL_getppid: return "getppid";
		case __SYSCALL_getpgrp: return "getpgrp";
		case __SYSCALL_setsid: return "setsid";
		case __SYSCALL_sigaction: return "sigaction";
		case __SYSCALL_sgetmask: return "sgetmask";
		case __SYSCALL_ssetmask: return "ssetmask";
		case __SYSCALL_setreuid: return "setreuid";
		case __SYSCALL_setregid: return "setregid";
		case __SYSCALL_sigsuspend: return "sigsuspend";
		case __SYSCALL_sigpending: return "sigpending";
		case __SYSCALL_sethostname: return "sethostname";
		case __SYSCALL_setrlimit: return "setrlimit";
		case __SYSCALL_getrlimit: return "getrlimit";
		case __SYSCALL_getrusage: return "getrusage";
		case __SYSCALL_gettimeofday: return "gettimeofday";
		case __SYSCALL_settimeofday: return "settimeofday";
		case __SYSCALL_getgroups: return "getgroups";
		case __SYSCALL_setgroups: return "setgroups";
		case __SYSCALL_select: return "select";
		case __SYSCALL_symlink: return "symlink";
		case __SYSCALL_oldlstat: return "oldlstat";
		case __SYSCALL_readlink: return "readlink";
		case __SYSCALL_uselib: return "uselib";
		case __SYSCALL_swapon: return "swapon";
		case __SYSCALL_reboot: return "reboot";
		case __SYSCALL_readdir: return "readdir";
		case __SYSCALL_mmap: return "mmap";
		case __SYSCALL_munmap: return "munmap";
		case __SYSCALL_truncate: return "truncate";
		case __SYSCALL_ftruncate: return "ftruncate";
		case __SYSCALL_fchmod: return "fchmod";
		case __SYSCALL_fchown: return "fchown";
		case __SYSCALL_getpriority: return "getpriority";
		case __SYSCALL_setpriority: return "setpriority";
		case __SYSCALL_profil: return "profil";
		case __SYSCALL_statfs: return "statfs";
		case __SYSCALL_fstatfs: return "fstatfs";
		case __SYSCALL_ioperm: return "ioperm";
		case __SYSCALL_socketcall: return "socketcall";
		case __SYSCALL_syslog: return "syslog";
		case __SYSCALL_setitimer: return "setitimer";
		case __SYSCALL_getitimer: return "getitimer";
		case __SYSCALL_stat: return "stat";
		case __SYSCALL_lstat: return "lstat";
		case __SYSCALL_fstat: return "fstat";
		case __SYSCALL_olduname: return "olduname";
		case __SYSCALL_iopl: return "iopl";
		case __SYSCALL_vhangup: return "vhangup";
		case __SYSCALL_idle: return "idle";
		case __SYSCALL_vm86old: return "vm86old";
		case __SYSCALL_wait4: return "wait4";
		case __SYSCALL_swapoff: return "swapoff";
		case __SYSCALL_sysinfo: return "sysinfo";
		case __SYSCALL_ipc: return "ipc";
		case __SYSCALL_fsync: return "fsync";
		case __SYSCALL_sigreturn: return "sigreturn";
		case __SYSCALL_clone: return "clone";
		case __SYSCALL_setdomainname: return "setdomainname";
		case __SYSCALL_uname: return "uname";
		case __SYSCALL_modify_ldt: return "modify_ldt";
		case __SYSCALL_adjtimex: return "adjtimex";
		case __SYSCALL_mprotect: return "mprotect";
		case __SYSCALL_sigprocmask: return "sigprocmask";
		case __SYSCALL_create_module: return "create_module";
		case __SYSCALL_init_module: return "init_module";
		case __SYSCALL_delete_module: return "delete_module";
		case __SYSCALL_get_kernel_syms: return "get_kernel_syms";
		case __SYSCALL_quotactl: return "quotactl";
		case __SYSCALL_getpgid: return "getpgid";
		case __SYSCALL_fchdir: return "fchdir";
		case __SYSCALL_bdflush: return "bdflush";
		case __SYSCALL_sysfs: return "sysfs";
		case __SYSCALL_personality: return "personality";
		case __SYSCALL_afs_syscall: return "afs_syscall";
		case __SYSCALL_setfsuid: return "setfsuid";
		case __SYSCALL_setfsgid: return "setfsgid";
		case __SYSCALL__llseek: return "_llseek";
		case __SYSCALL_getdents: return "getdents";
		case __SYSCALL__newselect: return "newselect";
		case __SYSCALL_flock: return "flock";
		case __SYSCALL_msync: return "msync";
		case __SYSCALL_readv: return "readv";
		case __SYSCALL_writev: return "writev";
		case __SYSCALL_getsid: return "getsid";
		case __SYSCALL_fdatasync: return "fdatasync";
		case __SYSCALL__sysctl: return "sysctl";
		case __SYSCALL_mlock: return "mlock";
		case __SYSCALL_munlock: return "munlock";
		case __SYSCALL_mlockall: return "mlockall";
		case __SYSCALL_munlockall: return "munlockall";
		case __SYSCALL_sched_setparam: return "sched_setparam";
		case __SYSCALL_sched_getparam: return "sched_getparam";
		case __SYSCALL_sched_setscheduler: return "sched_setscheduler";
		case __SYSCALL_sched_getscheduler: return "getsheduler";
		case __SYSCALL_sched_yield: return "sched_yield";
		case __SYSCALL_sched_get_priority_max: return "sched_get_priority_max";
		case __SYSCALL_sched_get_priority_min: return "sched_get_priority_min";
		case __SYSCALL_sched_rr_get_interval: return "sched_rr_get_interval";
		case __SYSCALL_nanosleep: return "nanosleep";
		case __SYSCALL_mremap: return "mremap";
		case __SYSCALL_setresuid	: return "setresuid";
		case __SYSCALL_getresuid: return "getresuid";
		case __SYSCALL_vm86: return "vm86";
		case __SYSCALL_query_module: return "query_module";
		case __SYSCALL_poll: return "poll";
		case __SYSCALL_nfsservctl: return "nfsservctl";
		case __SYSCALL_setresgid	: return "setresgid";
		case __SYSCALL_getresgid: return "getresgid";
		case __SYSCALL_prctl: return "prctl";
		case __SYSCALL_rt_sigreturn: return "rt_sigreturn";
		case __SYSCALL_rt_sigaction: return "rt_sigaction";
		case __SYSCALL_rt_sigprocmask: return "rt_sigprocmask";
		case __SYSCALL_rt_sigpending: return "rt_sigpending";
		case __SYSCALL_rt_sigtimedwait: return "rt_sigtimedwait";
		case __SYSCALL_rt_sigqueueinfo: return "rt_sigqueueinfo";
		case __SYSCALL_rt_sigsuspend: return "rt_sigsuspend";
		case __SYSCALL_pread: return "pread";
		case __SYSCALL_pwrite: return "pwrite";
		case __SYSCALL_chown: return "chown";
		case __SYSCALL_getcwd: return "getcwd";
		case __SYSCALL_capget: return "capget";
		case __SYSCALL_capset: return "capset";
		case __SYSCALL_sigaltstack: return "sigaltstack";
		case __SYSCALL_sendfile: return "sendfile";
		case __SYSCALL_getpmsg: return "getpmsg";
		case __SYSCALL_putpmsg: return "putpmsg";
		case __SYSCALL_vfork: return "vfork";
		case __SYSCALL_ugetrlimit: return "ugetrlimit";
		case __SYSCALL_mmap2: return "mmap2";
		case __SYSCALL_truncate64: return "truncate64";
		case __SYSCALL_ftruncate64: return "ftruncate64";
		case __SYSCALL_stat64: return "stat64";
		case __SYSCALL_lstat64: return "lstat64";
		case __SYSCALL_fstat64: return "fstat64";
		case __SYSCALL_lchown32: return "lchown32";
		case __SYSCALL_getuid32: return "getuid32";
		case __SYSCALL_getgid32: return "getgid32";
		case __SYSCALL_geteuid32: return "geteuid32";
		case __SYSCALL_getegid32: return "getegid32";
		case __SYSCALL_setreuid32: return "setreuid32";
		case __SYSCALL_setregid32: return "setregid32";
		case __SYSCALL_getgroups32: return "getgroups32";
		case __SYSCALL_setgroups32: return "setgroups32";
		case __SYSCALL_fchown32: return "fchown32";
		case __SYSCALL_setresuid32: return "setresuid32";
		case __SYSCALL_getresuid32: return "getresuid32";
		case __SYSCALL_setresgid32: return "setresgid32";
		case __SYSCALL_getresgid32: return "getresgid32";
		case __SYSCALL_chown32: return "chown32";
		case __SYSCALL_setuid32: return "setuid32";
		case __SYSCALL_setgid32: return "setgid32";
		case __SYSCALL_setfsuid32: return "setfsuid32";
		case __SYSCALL_setfsgid32: return "setfsgid32";
		case __SYSCALL_pivot_root: return "pivot_root";
		case __SYSCALL_mincore: return "mincore";
		case __SYSCALL_madvise: return "madvise";
		case __SYSCALL_getdents64: return "getdents64";
		case __SYSCALL_fcntl64: return "fnctl64";
	}
	return "?";
}

void do_syscall()
{
	int syscall_num;
	BOOL ret = FALSE;
	syscall_cb_t *cb = &syscall_cb;
	
	syscall_num	= SYSCALL_NUM;
	if(verbose)
		fprintf(stderr, "got a system call (number : %u, name : %s)\n", syscall_num, ppc_get_syscall_name(syscall_num));
		
	fflush(stderr);
	switch(syscall_num)
	{
		case __SYSCALL_exit: ret = ppc_syscall_exit(cb); break;
		case __SYSCALL_fork: ret = ppc_syscall_fork(cb); break;
		case __SYSCALL_read: ret = ppc_syscall_read(cb); break;
		case __SYSCALL_write: ret = ppc_syscall_write(cb); break;
		case __SYSCALL_open: ret = ppc_syscall_open(cb); break;
		case __SYSCALL_close: ret = ppc_syscall_close(cb); break;
		case __SYSCALL_waitpid: ret = ppc_syscall_waitpid(cb); break;
		case __SYSCALL_creat: ret = ppc_syscall_creat(cb); break;
		case __SYSCALL_link: ret = ppc_syscall_link(cb); break;
		case __SYSCALL_unlink: ret = ppc_syscall_unlink(cb); break;
		case __SYSCALL_execve: ret = ppc_syscall_execve(cb); break;
		case __SYSCALL_chdir: ret = ppc_syscall_chdir(cb); break;
		case __SYSCALL_time: ret = ppc_syscall_time(cb); break;
		case __SYSCALL_mknod: ret = ppc_syscall_mknod(cb); break;
		case __SYSCALL_chmod: ret = ppc_syscall_chmod(cb); break;
		case __SYSCALL_lchown: ret = ppc_syscall_lchown(cb); break;
		case __SYSCALL_break: ret = ppc_syscall_break(cb); break;
		case __SYSCALL_oldstat: ret = ppc_syscall_oldstat(cb); break;
		case __SYSCALL_lseek: ret = ppc_syscall_lseek(cb); break;
		case __SYSCALL_getpid: ret = ppc_syscall_getpid(cb); break;
		case __SYSCALL_mount: ret = ppc_syscall_mount(cb); break;
		case __SYSCALL_umount: ret = ppc_syscall_umount(cb); break;
		case __SYSCALL_setuid: ret = ppc_syscall_setuid(cb); break;
		case __SYSCALL_getuid: ret = ppc_syscall_getuid(cb); break;
		case __SYSCALL_stime: ret = ppc_syscall_stime(cb); break;
		case __SYSCALL_ptrace: ret = ppc_syscall_ptrace(cb); break;
		case __SYSCALL_alarm: ret = ppc_syscall_alarm(cb); break;
		case __SYSCALL_oldfstat: ret = ppc_syscall_oldfstat(cb); break;
		case __SYSCALL_pause: ret = ppc_syscall_pause(cb); break;
		case __SYSCALL_utime: ret = ppc_syscall_utime(cb); break;
		case __SYSCALL_stty: ret = ppc_syscall_stty(cb); break;
		case __SYSCALL_gtty: ret = ppc_syscall_gtty(cb); break;
		case __SYSCALL_access: ret = ppc_syscall_access(cb); break;
		case __SYSCALL_nice: ret = ppc_syscall_nice(cb); break;
		case __SYSCALL_ftime: ret = ppc_syscall_ftime(cb); break;
		case __SYSCALL_sync: ret = ppc_syscall_sync(cb); break;
		case __SYSCALL_kill: ret = ppc_syscall_kill(cb); break;
		case __SYSCALL_rename: ret = ppc_syscall_rename(cb); break;
		case __SYSCALL_mkdir: ret = ppc_syscall_mkdir(cb); break;
		case __SYSCALL_rmdir: ret = ppc_syscall_rmdir(cb); break;
		case __SYSCALL_dup: ret = ppc_syscall_dup(cb); break;
		case __SYSCALL_pipe: ret = ppc_syscall_pipe(cb); break;
		case __SYSCALL_times: ret = ppc_syscall_times(cb); break;
		case __SYSCALL_prof: ret = ppc_syscall_prof(cb); break;
		case __SYSCALL_brk: ret = ppc_syscall_brk(cb); break;
		case __SYSCALL_setgid: ret = ppc_syscall_setgid(cb); break;
		case __SYSCALL_getgid: ret = ppc_syscall_getgid(cb); break;
		case __SYSCALL_signal: ret = ppc_syscall_signal(cb); break;
		case __SYSCALL_geteuid: ret = ppc_syscall_geteuid(cb); break;
		case __SYSCALL_getegid: ret = ppc_syscall_getegid(cb); break;
		case __SYSCALL_acct: ret = ppc_syscall_acct(cb); break;
		case __SYSCALL_umount2: ret = ppc_syscall_umount2(cb); break;
		case __SYSCALL_lock: ret = ppc_syscall_lock(cb); break;
		case __SYSCALL_ioctl: ret = ppc_syscall_ioctl(cb); break;
		case __SYSCALL_fcntl: ret = ppc_syscall_fcntl(cb); break;
		case __SYSCALL_mpx: ret = ppc_syscall_mpx(cb); break;
		case __SYSCALL_setpgid: ret = ppc_syscall_setpgid(cb); break;
		case __SYSCALL_ulimit: ret = ppc_syscall_ulimit(cb); break;
		case __SYSCALL_oldolduname: ret = ppc_syscall_oldolduname(cb); break;
		case __SYSCALL_umask: ret = ppc_syscall_umask(cb); break;
		case __SYSCALL_chroot: ret = ppc_syscall_chroot(cb); break;
		case __SYSCALL_ustat	: ret = ppc_syscall_ustat(cb); break;
		case __SYSCALL_dup2: ret = ppc_syscall_dup2(cb); break;
		case __SYSCALL_getppid: ret = ppc_syscall_getppid(cb); break;
		case __SYSCALL_getpgrp: ret = ppc_syscall_getpgrp(cb); break;
		case __SYSCALL_setsid: ret = ppc_syscall_setsid(cb); break;
		case __SYSCALL_sigaction: ret = ppc_syscall_sigaction(cb); break;
		case __SYSCALL_sgetmask: ret = ppc_syscall_sgetmask(cb); break;
		case __SYSCALL_ssetmask: ret = ppc_syscall_ssetmask(cb); break;
		case __SYSCALL_setreuid: ret = ppc_syscall_setreuid(cb); break;
		case __SYSCALL_setregid: ret = ppc_syscall_setregid(cb); break;
		case __SYSCALL_sigsuspend: ret = ppc_syscall_sigsuspend(cb); break;
		case __SYSCALL_sigpending: ret = ppc_syscall_sigpending(cb); break;
		case __SYSCALL_sethostname: ret = ppc_syscall_sethostname(cb); break;
		case __SYSCALL_setrlimit: ret = ppc_syscall_setrlimit(cb); break;
		case __SYSCALL_getrlimit: ret = ppc_syscall_getrlimit(cb); break;
		case __SYSCALL_getrusage: ret = ppc_syscall_getrusage(cb); break;
		case __SYSCALL_gettimeofday: ret = ppc_syscall_gettimeofday(cb); break;
		case __SYSCALL_settimeofday: ret = ppc_syscall_settimeofday(cb); break;
		case __SYSCALL_getgroups: ret = ppc_syscall_getgroups(cb); break;
		case __SYSCALL_setgroups: ret = ppc_syscall_setgroups(cb); break;
		case __SYSCALL_select: ret = ppc_syscall_select(cb); break;
		case __SYSCALL_symlink: ret = ppc_syscall_symlink(cb); break;
		case __SYSCALL_oldlstat: ret = ppc_syscall_oldlstat(cb); break;
		case __SYSCALL_readlink: ret = ppc_syscall_readlink(cb); break;
		case __SYSCALL_uselib: ret = ppc_syscall_uselib(cb); break;
		case __SYSCALL_swapon: ret = ppc_syscall_swapon(cb); break;
		case __SYSCALL_reboot: ret = ppc_syscall_reboot(cb); break;
		case __SYSCALL_readdir: ret = ppc_syscall_readdir(cb); break;
		case __SYSCALL_mmap: ret = ppc_syscall_mmap(cb); break;
		case __SYSCALL_munmap: ret = ppc_syscall_munmap(cb); break;
		case __SYSCALL_truncate: ret = ppc_syscall_truncate(cb); break;
		case __SYSCALL_ftruncate: ret = ppc_syscall_ftruncate(cb); break;
		case __SYSCALL_fchmod: ret = ppc_syscall_fchmod(cb); break;
		case __SYSCALL_fchown: ret = ppc_syscall_fchown(cb); break;
		case __SYSCALL_getpriority: ret = ppc_syscall_getpriority(cb); break;
		case __SYSCALL_setpriority: ret = ppc_syscall_setpriority(cb); break;
		case __SYSCALL_profil: ret = ppc_syscall_profil(cb); break;
		case __SYSCALL_statfs: ret = ppc_syscall_statfs(cb); break;
		case __SYSCALL_fstatfs: ret = ppc_syscall_fstatfs(cb); break;
		case __SYSCALL_ioperm: ret = ppc_syscall_ioperm(cb); break;
		case __SYSCALL_socketcall: ret = ppc_syscall_socketcall(cb); break;
		case __SYSCALL_syslog: ret = ppc_syscall_syslog(cb); break;
		case __SYSCALL_setitimer: ret = ppc_syscall_setitimer(cb); break;
		case __SYSCALL_getitimer: ret = ppc_syscall_getitimer(cb); break;
		case __SYSCALL_stat: ret = ppc_syscall_stat(cb); break;
		case __SYSCALL_lstat: ret = ppc_syscall_lstat(cb); break;
		case __SYSCALL_fstat: ret = ppc_syscall_fstat(cb); break;
		case __SYSCALL_olduname: ret = ppc_syscall_olduname(cb); break;
		case __SYSCALL_iopl: ret = ppc_syscall_iopl(cb); break;
		case __SYSCALL_vhangup: ret = ppc_syscall_vhangup(cb); break;
		case __SYSCALL_idle: ret = ppc_syscall_idle(cb); break;
		case __SYSCALL_vm86old: ret = ppc_syscall_vm86old(cb); break;
		case __SYSCALL_wait4: ret = ppc_syscall_wait4(cb); break;
		case __SYSCALL_swapoff: ret = ppc_syscall_swapoff(cb); break;
		case __SYSCALL_sysinfo: ret = ppc_syscall_sysinfo(cb); break;
		case __SYSCALL_ipc: ret = ppc_syscall_ipc(cb); break;
		case __SYSCALL_fsync: ret = ppc_syscall_fsync(cb); break;
		case __SYSCALL_sigreturn: ret = ppc_syscall_sigreturn(cb); break;
		case __SYSCALL_clone: ret = ppc_syscall_clone(cb); break;
		case __SYSCALL_setdomainname: ret = ppc_syscall_setdomainname(cb); break;
		case __SYSCALL_uname: ret = ppc_syscall_uname(cb); break;
		case __SYSCALL_modify_ldt: ret = ppc_syscall_modify_ldt(cb); break;
		case __SYSCALL_adjtimex: ret = ppc_syscall_adjtimex(cb); break;
		case __SYSCALL_mprotect: ret = ppc_syscall_mprotect(cb); break;
		case __SYSCALL_sigprocmask: ret = ppc_syscall_sigprocmask(cb); break;
		case __SYSCALL_create_module: ret = ppc_syscall_create_module(cb); break;
		case __SYSCALL_init_module: ret = ppc_syscall_init_module(cb); break;
		case __SYSCALL_delete_module: ret = ppc_syscall_delete_module(cb); break;
		case __SYSCALL_get_kernel_syms: ret = ppc_syscall_get_kernel_syms(cb); break;
		case __SYSCALL_quotactl: ret = ppc_syscall_quotactl(cb); break;
		case __SYSCALL_getpgid: ret = ppc_syscall_getpgid(cb); break;
		case __SYSCALL_fchdir: ret = ppc_syscall_fchdir(cb); break;
		case __SYSCALL_bdflush: ret = ppc_syscall_bdflush(cb); break;
		case __SYSCALL_sysfs: ret = ppc_syscall_sysfs(cb); break;
		case __SYSCALL_personality: ret = ppc_syscall_personality(cb); break;
		case __SYSCALL_afs_syscall: ret = ppc_syscall_afs_syscall(cb); break;
		case __SYSCALL_setfsuid: ret = ppc_syscall_setfsuid(cb); break;
		case __SYSCALL_setfsgid: ret = ppc_syscall_setfsgid(cb); break;
		case __SYSCALL__llseek: ret = ppc_syscall__llseek(cb); break;
		case __SYSCALL_getdents: ret = ppc_syscall_getdents(cb); break;
		case __SYSCALL__newselect: ret = ppc_syscall_newselect(cb); break;
		case __SYSCALL_flock: ret = ppc_syscall_flock(cb); break;
		case __SYSCALL_msync: ret = ppc_syscall_msync(cb); break;
		case __SYSCALL_readv: ret = ppc_syscall_readv(cb); break;
		case __SYSCALL_writev: ret = ppc_syscall_writev(cb); break;
		case __SYSCALL_getsid: ret = ppc_syscall_getsid(cb); break;
		case __SYSCALL_fdatasync: ret = ppc_syscall_fdatasync(cb); break;
		case __SYSCALL__sysctl: ret = ppc_syscall_sysctl(cb); break;
		case __SYSCALL_mlock: ret = ppc_syscall_mlock(cb); break;
		case __SYSCALL_munlock: ret = ppc_syscall_munlock(cb); break;
		case __SYSCALL_mlockall: ret = ppc_syscall_mlockall(cb); break;
		case __SYSCALL_munlockall: ret = ppc_syscall_munlockall(cb); break;
		case __SYSCALL_sched_setparam: ret = ppc_syscall_sched_setparam(cb); break;
		case __SYSCALL_sched_getparam: ret = ppc_syscall_sched_getparam(cb); break;
		case __SYSCALL_sched_setscheduler: ret = ppc_syscall_sched_setscheduler(cb); break;
		case __SYSCALL_sched_getscheduler: ret = ppc_syscall_getsheduler(cb); break;
		case __SYSCALL_sched_yield: ret = ppc_syscall_sched_yield(cb); break;
		case __SYSCALL_sched_get_priority_max: ret = ppc_syscall_sched_get_priority_max(cb); break;
		case __SYSCALL_sched_get_priority_min: ret = ppc_syscall_sched_get_priority_min(cb); break;
		case __SYSCALL_sched_rr_get_interval: ret = ppc_syscall_sched_rr_get_interval(cb); break;
		case __SYSCALL_nanosleep: ret = ppc_syscall_nanosleep(cb); break;
		case __SYSCALL_mremap: ret = ppc_syscall_mremap(cb); break;
		case __SYSCALL_setresuid	: ret = ppc_syscall_setresuid(cb); break;
		case __SYSCALL_getresuid: ret = ppc_syscall_getresuid(cb); break;
		case __SYSCALL_vm86: ret = ppc_syscall_vm86(cb); break;
		case __SYSCALL_query_module: ret = ppc_syscall_query_module(cb); break;
		case __SYSCALL_poll: ret = ppc_syscall_poll(cb); break;
		case __SYSCALL_nfsservctl: ret = ppc_syscall_nfsservctl(cb); break;
		case __SYSCALL_setresgid	: ret = ppc_syscall_setresgid(cb); break;
		case __SYSCALL_getresgid: ret = ppc_syscall_getresgid(cb); break;
		case __SYSCALL_prctl: ret = ppc_syscall_prctl(cb); break;
		case __SYSCALL_rt_sigreturn: ret = ppc_syscall_rt_sigreturn(cb); break;
		case __SYSCALL_rt_sigaction: ret = ppc_syscall_rt_sigaction(cb); break;
		case __SYSCALL_rt_sigprocmask: ret = ppc_syscall_rt_sigprocmask(cb); break;
		case __SYSCALL_rt_sigpending: ret = ppc_syscall_rt_sigpending(cb); break;
		case __SYSCALL_rt_sigtimedwait: ret = ppc_syscall_rt_sigtimedwait(cb); break;
		case __SYSCALL_rt_sigqueueinfo: ret = ppc_syscall_rt_sigqueueinfo(cb); break;
		case __SYSCALL_rt_sigsuspend: ret = ppc_syscall_rt_sigsuspend(cb); break;
		case __SYSCALL_pread: ret = ppc_syscall_pread(cb); break;
		case __SYSCALL_pwrite: ret = ppc_syscall_pwrite(cb); break;
		case __SYSCALL_chown: ret = ppc_syscall_chown(cb); break;
		case __SYSCALL_getcwd: ret = ppc_syscall_getcwd(cb); break;
		case __SYSCALL_capget: ret = ppc_syscall_capget(cb); break;
		case __SYSCALL_capset: ret = ppc_syscall_capset(cb); break;
		case __SYSCALL_sigaltstack: ret = ppc_syscall_sigaltstack(cb); break;
		case __SYSCALL_sendfile: ret = ppc_syscall_sendfile(cb); break;
		case __SYSCALL_getpmsg: ret = ppc_syscall_getpmsg(cb); break;
		case __SYSCALL_putpmsg: ret = ppc_syscall_putpmsg(cb); break;
		case __SYSCALL_vfork: ret = ppc_syscall_vfork(cb); break;
		case __SYSCALL_ugetrlimit: ret = ppc_syscall_ugetrlimit(cb); break;
		case __SYSCALL_mmap2: ret = ppc_syscall_mmap2(cb); break;
		case __SYSCALL_truncate64: ret = ppc_syscall_truncate64(cb); break;
		case __SYSCALL_ftruncate64: ret = ppc_syscall_ftruncate64(cb); break;
		case __SYSCALL_stat64: ret = ppc_syscall_stat64(cb); break;
		case __SYSCALL_lstat64: ret = ppc_syscall_lstat64(cb); break;
		case __SYSCALL_fstat64: ret = ppc_syscall_fstat64(cb); break;
		case __SYSCALL_lchown32: ret = ppc_syscall_lchown32(cb); break;
		case __SYSCALL_getuid32: ret = ppc_syscall_getuid32(cb); break;
		case __SYSCALL_getgid32: ret = ppc_syscall_getgid32(cb); break;
		case __SYSCALL_geteuid32: ret = ppc_syscall_geteuid32(cb); break;
		case __SYSCALL_getegid32: ret = ppc_syscall_getegid32(cb); break;
		case __SYSCALL_setreuid32: ret = ppc_syscall_setreuid32(cb); break;
		case __SYSCALL_setregid32: ret = ppc_syscall_setregid32(cb); break;
		case __SYSCALL_getgroups32: ret = ppc_syscall_getgroups32(cb); break;
		case __SYSCALL_setgroups32: ret = ppc_syscall_setgroups32(cb); break;
		case __SYSCALL_fchown32: ret = ppc_syscall_fchown32(cb); break;
		case __SYSCALL_setresuid32: ret = ppc_syscall_setresuid32(cb); break;
		case __SYSCALL_getresuid32: ret = ppc_syscall_getresuid32(cb); break;
		case __SYSCALL_setresgid32: ret = ppc_syscall_setresgid32(cb); break;
		case __SYSCALL_getresgid32: ret = ppc_syscall_getresgid32(cb); break;
		case __SYSCALL_chown32: ret = ppc_syscall_chown32(cb); break;
		case __SYSCALL_setuid32: ret = ppc_syscall_setuid32(cb); break;
		case __SYSCALL_setgid32: ret = ppc_syscall_setgid32(cb); break;
		case __SYSCALL_setfsuid32: ret = ppc_syscall_setfsuid32(cb); break;
		case __SYSCALL_setfsgid32: ret = ppc_syscall_setfsgid32(cb); break;
		case __SYSCALL_pivot_root: ret = ppc_syscall_pivot_root(cb); break;
		case __SYSCALL_mincore: ret = ppc_syscall_mincore(cb); break;
		case __SYSCALL_madvise: ret = ppc_syscall_madvise(cb); break;
		case __SYSCALL_getdents64: ret = ppc_syscall_getdents64(cb); break;
		case __SYSCALL_fcntl64: ret = ppc_syscall_fnctl64(cb); break;
	}
	
	if(!ret)
	{
		if(verbose)
			fprintf(stderr, "Warning : system call returns an error (number : %u, name : %s)\n", syscall_num, ppc_get_syscall_name(syscall_num));
		SET_CR0SO;
	}
	else
	{
		RESET_CR0SO;
	}
}

void syscall_setbrk(addr_t addr)
{
	brk_point = addr;
}
