/***********
 *
 *	ser.h
 *
 *	09 APR 2007 	
 *
 *
 *********/

#ifndef SER_H
#define	SER_H

#ifdef LINUX
#include <termios.h>
#endif

#ifdef AIM
#include <sys_serial.h>
#endif

#include "./data_type.h"

#define MAX_BR_CODE_NUM		11

#define MAX_SER_PORT_NUM	2
#define MAX_USB_PORT_NUM	4

#define TTYS0		0
#define TTYS1		1

#define	TTYUSB0		2
#define	TTYUSB1		3
#define	TTYUSB2		4
#define	TTYUSB3		5

//	char	flowctrl;	// NO_FLOW_CTRL, HW_FLOW_CTRL(CRTSCTS), SW_FLOW_CTRL(IXON|OFF|ANY)
#define	NO_FLOW_CTRL	0
#define	HW_FLOW_CTRL	1
#define	SW_FLOW_CTRL	2

#define	RS232 	0
#define	RS485W2 1
#define	RS422 	2
#define	RS485W4 3

#ifdef NT
#define B600	   600
#define B1200	  1200
#define B2400	  2400
#define B4800	  4800
#define B9600	  9600
#define B19200	 19200
#define B38400	 38400
#define B57600	 57600
#define B115200	115200
#endif

typedef	struct TTIME {
	char	yy;
	char	mon;
	char	day;
	char	hh;
	char	mm;
	char	ss;
	int		msec;
} tTime;

typedef struct TBAUDRATE_MAP {
	UCHAR	br_code;   // 
	uint	baudrate;  // uint 115200
	char	br_str[7]; // Max "115200\0"
} tBaudrateMap;

typedef struct TSER {
	char 	port_name[20];	// "/dev/ttyS0, /dev/ttyUSB0"
	uint	baudrate;
	int		port_num;
	int		sfd;
	tTime	stime;
	tTime 	ftime;
	ulong	tcom;
	ulong	tintv;
	int		tout;	// 300msec
	int		tcnt;
	int 	terr;
	int		mode;	//RS232=0, RS485=1
	char	flowctrl;	// NO_FLOW_CTRL, HW_FLOW_CTRL(CRTSCTS), SW_FLOW_CTRL(IXON|OFF|ANY)
	int		databit;	// 5, 6, 7, 8
	char	parity; 	// 'N', 'O'(ODD), 'E'(EVEN), 'S'(SPACE), 'M'(MARK) 
	int		stopbit;	//  1, 2
} tSer;

extern	tBaudrateMap	BaudrateMap[MAX_BR_CODE_NUM];
extern  char 			StrFlowCtrl[][5];
extern  char 			StrSerCommType[][10];

extern int  readSer(tSer *ser, UCHAR *bf,int sz);
extern int  writeSer(tSer *ser, UCHAR *bf,int sz);
extern int  emtySer(tSer *serptr);
extern int  openSer(tSer *ser);
extern int  openUart(char *portname,uint baudrate,int databits,int stopbits,int parity,int flow);
extern int  changeBaudrate(tSer *ser,uint br);
extern char *getBrstr(uint baudrate);
extern uint getBaudrate(char *br);
extern void showSer(tSer *ser);
extern int  readSerEos(tSer *ser, UCHAR *bf,char eos);
#endif
/********** End line of ser.h **********/
