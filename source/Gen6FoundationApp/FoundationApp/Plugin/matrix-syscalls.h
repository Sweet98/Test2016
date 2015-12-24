//======================================================================================
// matrix-syscalls.h
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/matrix-syscalls.h $
// $Revision: 1.6 $
// $Date: 2010/11/18 15:11:08EST $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the matrix syscall interfaces.
//
//---------------------------------------------------------------------------


/** Matrix system call numbers for the ARM processor. */

#ifndef _LIBGLOSS_ARM_MATRIX_SYSCALL_H
#define _LIBGLOSS_ARM_MATRIX_SYSCALL_H

enum matrix_syscall_id
{
	MATRIX_RESTART = 0,
	MATRIX_EXIT = 1,
	MATRIX_FORK = 2,
	MATRIX_READ = 3,
	MATRIX_WRITE = 4,
	MATRIX_OPEN = 5,
	MATRIX_CLOSE = 6,
	MATRIX_ISATTY = 7,
	MATRIX_RENAME = 8,

	
	MATRIX_LINK = 9,
	MATRIX_UNLINK = 10,
	MATRIX_EXECVE = 11,

	MATRIX_MALLOC = 12,
	MATRIX_CALLOC = 13,
	MATRIX_FREE = 14,
	MATRIX_REALLOC = 15,

	MATRIX_REGAPI = 16,
	MATRIX_IOBEEPLED = 17,
	
	MATRIX_LSEEK = 19,
	MATRIX_GETPID = 20,

	MATRIX_MAYCONTINUE = 21,
	MATRIX_PROCESSRST = 22,
	MATRIX_MSTIMER = 23,
	MATRIX_DIV64 = 24,
	MATRIX_SBRK = 25,
	MATRIX_VER = 26,

	MATRIX_FSTAT = 28,
	MATRIX_KILL = 37,
	MATRIX_TIMES = 43,
	MATRIX_BRK = 45,
	
	MATRIX_IOINIT = 50,
	MATRIX_IOREAD = 51,
	MATRIX_IOWRITE = 52,
	
	MATRIX_STAT = 106,
	MATRIX_WAIT4 = 114

	
};

struct matrix_syscall_param
{
	enum	matrix_syscall_id syscall_id;
	int* 		params[5];
	int		err;
};


#endif /* _LIBGLOSS_ARM_LINUX_SYSCALL_H */

//=================================================================================
//                       File Modification History
//===================================================================================
