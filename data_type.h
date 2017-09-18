/*****************************************************************
 *
 *	File Name : data_type.h 
 *
 ******************************************************************/

#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#ifndef uchar
#define	uchar  unsigned char
#endif

#ifndef UCHAR
#define	UCHAR  unsigned char
#endif

#ifndef byte
#define	byte   unsigned char
#endif

#ifndef ushort
#define	ushort unsigned short
#endif

#ifndef uint
#define	uint   unsigned int
#endif

#ifndef ulong
#define	ulong  unsigned long
#endif

#ifndef FAIL
#define	FAIL	-1
#endif

#ifndef OK
#define	OK		 1	
#endif

#ifndef CMD_INT_VAR
#define	CMD_INT_VAR
typedef struct {
	char	varname[30];
	int		*iptr;
} tCmdIntVar;
#endif

#ifdef	NT
#define	sleep	sys_sleep
#define	msleep	sys_msleep
#define	usleep	sys_usleep
#endif

// #ifndef EFAILOK
// #define	EFAILOK
// enum	eFailOk { E_FAIL=0, E_OK=1};
// #endif

#endif
/**************** End Line of data_type.h ****************************/
