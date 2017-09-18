/*****************************************************************************
                                KvJ
NAME: winutl.c
-----


PURPOSE:
-------

  This file contains the TTM protocol utility functions.

SYNOPSYS:
--------

****************************************************************************/

/*
 * Table specific API includes.
 */
// #include <rtd_api.h>
// #include <rtu_api.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>

// Only for NT, WINDOWS to use beep
#ifdef NT
#include <windows.h>
#endif

// #include "sys_time.h"

// #include "daq_pmn.h"
#include "./winutl.h"
// #include "daq_pmn_rtu_status.h"
#ifdef AIM
// #include <sys_time.h>
#endif


UCHAR asciihex_to_binary (char ascii_char)
{
    if (ascii_char > '9' ) ascii_char -=7;
    ascii_char -= '0';
    return (ascii_char);
}

/*
** This function will convert a binary integer (number) to an ASCII-hex
** representation of that number.
**
** Parameters:  number      an positive integer
**
** Returns:     An ASCII character that represents the HEX-digit passed in
**
*/
UCHAR binary_to_asciihex (int number)
{
    /* Mask off all but the low order nibble */
    number = number & 0x0F;
    number += '0';
    if (number > '9' ) number +=7;
    return (number);
}

/*************************************************
 * read binary string(str)
 *      and assign the byte number to numb array.
 *
 *          0       7 0       7 0       7
 * ex) (LSB)1000 0001 0100 0001 1100 0001(MSB)
 *          81        82        83
 *************************************************/ 

void readBinStr(unsigned char *numb,char *str)
{
	char    tmp[256] = {0};
	char	ch;
	int	    idx = 0, i, j;

	while(1L)
	{
		ch = *str++;	
		if( 0 == ch )
			break;
		if( '0' == ch || '1' == ch )
		{
			tmp[idx++] = ch;
		}
	}


	for(i=0; i<idx/8; i++)
	{
		numb[i] = 0;
		for(j=0; j<8; j++)
		{
			ch = 0;
			ch = (('0'==tmp[i*8+j])?0:1);	
			numb[i] += (ch << j);	
		}
	}

	printf("(%02X %02X %02X %02X)%s->%d\n", numb[3], numb[2], numb[1], numb[0], tmp, idx);

} //void

void setPromptColor(int fg, int bg)
{
#ifdef	LINUX
	char set_color[20];

	sprintf(set_color, "\x1b[1m\x1b[%d;%dm", fg, bg);
	printf("%s", set_color);
	fflush(stdout);
#endif
	return;
} // void setPromptColor(int mode)

void	printTimeMsec(char color)
{
	unsigned char hh, mm, ss;

	getTime(&hh, &mm, &ss);

	setPromptColor(color,BOLD);
	printf("%02d:%02d:%02d.", hh, mm, ss);
	// setPromptColor(CYAN,NORMAL);
	printf("%03u", (unsigned short)getMsec() );	
	setPromptColor(WHITE,BN);
} //void printTime(...

void showProgress()
{
	char	ch[4] = "-\x5C|/";
	static	int seq = 0;

	printf("%c\r",ch[seq]);
	fflush(stdout);
	seq = ((seq+1)%4);

	return;	
} // void showProgress()


void printColor(char fg, char bg, char *str)
{
	if( str == NULL )
		return;

#ifdef LINUX	
	setPromptColor(fg,bg);
	printf("%s", str); 
	setPromptColor(WHITE,BN);
#else
	printf("%s", str); 
#endif

	return;
} // void printColor(fg,bg,str)

/* 
** This function will calculate the 16 bit CRC.
**
** Parameters:  pt          Pointer to buffer that we want to calculate the CRc for
**              length      Number of bytes in the buffer
**
** Returns:     A 16-bit CRC : 0x8005 (X^16+X^15+X^2+1)
**
*/

/**************** End Of protocol_ams_util.c *****************************/

void clearBuf(UCHAR *pbuf,int sz,UCHAR set_value)
{
	int	i;

	for(i=0; i<sz; i++, pbuf++)
		*pbuf = set_value;

	return;
} //void clearBuf(...

unsigned char atoc(char ch)
{
	if((ch>= '0') && (ch <= '9'))
			return (ch - '0');
	else if((ch>= 'A') && (ch <= 'F'))
			return (ch - 'A'+10);
	else if((ch>= 'a') && (ch <= 'f'))
			return (ch - 'a'+10);
	else
			return (0);
} //unsigned char atoc(char ch..

unsigned char a2toc(char *bf) //2 ascii chars to bin value:'1A'->26(0x1A)
{
    unsigned char ret = 0; 
     
    ret = atoc(*bf);
    ret <<= 4;
    ret = ret + atoc(*(bf+1));

    return (ret); 
} // ulong a2toc(char *bf)

unsigned char htob(char *bf)
{
	unsigned char ret = 0;

	ret = atoc(*bf);
	ret <<= 4;
	ret = ret + atoc(*(bf+1));

	return (ret);
} //unsigned char htob

unsigned short utob(char *bf)
{
	unsigned short ret = 0;

	while(1L)
	{
		if(0 == isxdigit(*bf))
			break;

		ret <<= 4;
		ret += atoc(*bf++);
	}

	return (ret);
}

/* string to binary.
 * convert hex or digit string to binary from sbf array.
 * mode : HEX or anything.
 * similiar to stoix(str), stox(str)
 * 2003. 03.04. JKB
 */
int stob(char *sbf, int sz,unsigned char mode)
{
	int	ret_val = 0;

	if( mode == DEC )
	{
		ret_val = atoi(sbf);
		// printf("bin %x\n\r", ret_val);
	} else {
		while(sz-->0)	
		{
			if( mode == HEX )
			{
				ret_val *= 0x10;
				ret_val += atoc(*sbf);
				// printf("hex %x\n\r", ret_val);
			}
			sbf++;
		} // while(sz-->0)	
	}

	return ret_val;
} // void setFromStr(char *sbf, int sz

/*
 * bf[0~3] hexa character --> returns unsigned long
 *
 *	"123456AB" --> 0x123456AB
 */
unsigned long	htoul(char *bf)
{
	unsigned long	lret = 0;
	int	i;

	for(i=0; i<8; i++, bf++)
	{
		if( isxdigit(*bf) )
		{
			lret <<= 4;
			lret += atoc(*bf);
		} else {
			break;
		}
	}

	return lret;
} // unsigned long htol(char *bf...

// unsigned short to binary buffer
void ustob(unsigned short us,unsigned char *bf)
{
	*bf 	= (unsigned char)(us>>8);
	*(bf+1) = (unsigned char)(us);
}

// binary buffer to unsigned short
unsigned short btous(unsigned char *bf)
{
	return *bf*0x100 + *(bf+1);
}

const unsigned short crc_ccitt_table[256] = {
      0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
      0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
      0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
      0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
      0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
      0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
      0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
      0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
      0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
      0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
      0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
      0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
      0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
      0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
      0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
      0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
      0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
      0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
      0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
      0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
      0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
      0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
      0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
      0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
      0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
      0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
      0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
      0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
      0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
      0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
      0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
      0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

// unsigned short crc16_ccitt(unsigned short init_crc,const UCHAR *buf, int len)
unsigned short crc16_ccitt(const UCHAR *buf, int len)
{
	register int counter;
	register unsigned short crc = 0; // crc = init_crc // 0xFFFF

	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc_ccitt_table[((crc>>8) ^ *(char *)buf++)&0x00FF];

	return crc;
}

/*
 *	Equal = return 1; Not Equal => return -1
 */

int compByte(unsigned char *tg, unsigned char *dt, int sz)
{
	int i;

	for(i=0; i<sz; i++)
	{
		if( tg[i] != dt[i] )
			return -1;
	}

	return 1;
} // int compData(byte*,byte*,int)

/*
 * showCos8 :  Comparing pds and ds,
 *		       cosbit value is displayed in coszero and cosone color.
 *		09 APR 2010 (FRI) by KvJ 
 */

void showCos8(	char *title,uchar ps, uchar ds, uchar mode,
				char zero, char one, char coszero, char cosone)
{
	int 	i, bitval, pitval;

	if( title != NULL )
		printf("%s", title);

	for(i=0; i<8; i++)
	{
		if( mode & NIBBLE_SPACE )
		{
			if((0 == i%4) && (0 != i))
				printf(" ");
			if((0 == i%8) && (0 != i))
				printf(", ");
		}
		
		if( mode & LSB_FIRST )
		{
			bitval = ((0x01<<i)&ds)?1:0;
			pitval = ((0x01<<i)&ps)?1:0;

			if(pitval != bitval)
			{
				if( bitval )
					setPromptColor(cosone,  BOLD);
				else
					setPromptColor(coszero, BOLD);
			} else {
				if( bitval )
					setPromptColor(one,  BOLD);
				else
					setPromptColor(zero, BOLD);
			} //if(pitval != bitval
		} else {
			bitval = ((0x80>>i)&ds)?1:0;
			pitval = ((0x80>>i)&ps)?1:0;

			if(pitval != bitval)
			{
				if( bitval )
					setPromptColor(cosone,  BOLD);
				else
					setPromptColor(coszero, BOLD);
			} else {
				if( bitval )
					setPromptColor(one,  BOLD);
				else
					setPromptColor(zero, BOLD);
			} // if( pitval != bitval
		} // if( mode & LSB_FIRST...
		printf("%d", bitval);
		setPromptColor(WHITE, NORMAL);
	} // for(i=0; i<16; i++)

	return;
} // void showCos8

/*
 * showCos16 :  Comparing pds and ds,
 *		         cosbit value is displayed in coszero and cosone color.
 *		05 APR 2010 (MON) by KvJ 
 */

void showCos16(	char *title,ushort ps, ushort ds, uchar mode,
				char zero, char one, char coszero, char cosone)
{
	int 	i, bitval, pitval;

	if( title != NULL )
		printf("%s", title);

	for(i=0; i<16; i++)
	{
		if( mode & NIBBLE_SPACE )
		{
			if((0 == i%4) && (0 != i))
				printf(" ");
			if((0 == i%8) && (0 != i))
				printf(", ");
		}
		
		if( mode & LSB_FIRST )
		{
			bitval = ((0x0001<<i)&ds)?1:0;
			pitval = ((0x0001<<i)&ps)?1:0;

			if(pitval != bitval)
			{
				if( bitval )
					setPromptColor(cosone,  BOLD);
				else
					setPromptColor(coszero, BOLD);
			} else {
				if( bitval )
					setPromptColor(one,  BOLD);
				else
					setPromptColor(zero, BOLD);
			} //if(pitval != bitval
		} else {
			bitval = ((0x8000>>i)&ds)?1:0;
			pitval = ((0x8000>>i)&ps)?1:0;

			if(pitval != bitval)
			{
				if( bitval )
					setPromptColor(cosone,  BOLD);
				else
					setPromptColor(coszero, BOLD);
			} else {
				if( bitval )
					setPromptColor(one,  BOLD);
				else
					setPromptColor(zero, BOLD);
			} // if( pitval != bitval
		} // if( mode & LSB_FIRST...
		printf("%d", bitval);
		setPromptColor(WHITE, NORMAL);
	} // for(i=0; i<16; i++)

	return;
} // void showCos16

void showBit16(char *title, ushort ds, uchar mode, char zero, char one)
{
	int 	i, bitval;

	// if( NULL == strbuf )
	//	printf("\n\r");

	if( title != NULL )
		printf("%s", title);

	for(i=0; i<16; i++)
	{
		if( mode & NIBBLE_SPACE )
		{
			if((0 == i%4) && (0 != i))
				printf(" ");
			if((0 == i%8) && (0 != i))
				printf(", ");
		}
		
		if( mode & LSB_FIRST )
		{
			bitval = ((0x0001<<i)&ds)?1:0;

			if( bitval )
				setPromptColor(one,  BOLD);
			else
				setPromptColor(zero, BOLD);

			printf("%d", bitval);
			setPromptColor(WHITE, NORMAL);
		} else {
			bitval = ((0x8000>>i)&ds)?1:0;

			if( bitval )
				setPromptColor(one,  BOLD);
			else
				setPromptColor(zero, BOLD);

			printf("%d", ((0x8000>>i)&ds)?1:0);
			setPromptColor(WHITE, NORMAL);
		}
	}
	return;
} // void showBit16(char *title target_data...

void showBit(char *title, unsigned char ds, unsigned char mode, char zero, char one)
{
	int i;
	int	bitval;
/***************************
	if( title == NULL )
		printf(" %02X : ", ds);
	else	
		printf("%s %02X : ", title, ds);
 ****************************/
	if( title != NULL )
		printf("%s", title);

	for(i=0; i<8; i++)
	{
		if( mode & NIBBLE_SPACE )
			if(4 == i)
				printf(" ");

		if( mode & LSB_FIRST )
		{
			bitval = ((0x01<<i)&ds)?1:0;

			if( bitval )
				setPromptColor(one,  BOLD);
			else
				setPromptColor(zero, BOLD);

			printf("%d", bitval);
			setPromptColor(WHITE, NORMAL);
		} else {
			bitval = ((0x80>>i)&ds)?1:0;

			if( bitval )
				setPromptColor(one,  BOLD);
			else
				setPromptColor(zero, BOLD);

			printf("%d", bitval);
			setPromptColor(WHITE, NORMAL);
		}
	} //for(i=0; i<8...

	// printf("\n\r");

	return;
} // void showBit(char *title target_data...

unsigned long getLongMsec()
{
	int	cmsec, csec;
    struct timeb systimeb;

    ftime(&systimeb);

    cmsec = systimeb.millitm;
    csec  = systimeb.time;

	return cmsec+csec*1000;	
}

/*
 *	Calculate the delta time in msec from original time in msec.
 *
 *
 */
int elapseMsec (unsigned long origtime)
{
	unsigned long currtime;
    // struct timeb systimeb;

	currtime = getLongMsec();

    return currtime-origtime;
} // int elapseTime
/******
int getMsec(char *hmsmsec)
{
	 struct timeb tmb;
	 struct tm 	*date;
	 int	msec = -1;
	
	 ftime(&tmb);
	 date = localtime(&tmb.time);

	 if(NULL != date)
	 {
		if(NULL != hmsmsec)
		{
	 		sprintf(hmsmsec, "%02d:%02d:%02d.%03u",
					  		date->tm_hour, date->tm_min,
	 						date->tm_sec,  tmb.millitm );
		} else {
	 		printf("%02d:%02d:%02d.%03u",
					date->tm_hour, date->tm_min,
	 				date->tm_sec,  tmb.millitm );
		}
		msec = (int)tmb.millitm;
	 }

	 return msec; 
} // int getMsec
******************/

int getTimeDate(UCHAR *yr,UCHAR *mon,UCHAR *day,UCHAR *hh,UCHAR *mm,UCHAR *ss)
{
	struct timeb tmb;
	struct tm 	*date;
	int		status = 0;

	ftime(&tmb);
 	date = localtime(&tmb.time);

	if( NULL != date ) 
	{
		*yr  = (unsigned char)(date->tm_year%100);
		*mon = date->tm_mon+1;
		*day = date->tm_mday;
		*hh  = date->tm_hour;
		*mm  = date->tm_min;
		*ss  = date->tm_sec;
	}

	return status;
} //void getTimeDate(yr,mon,day,hh,mm,ss...

void getTime(byte *hh, byte *mm, byte *ss)
{
	time_t tml;
	struct tm *tmp, *localtime();

	// printf("1. getTime()\n\r"); getchar();
	time(&tml);

	// printf("2. getTime()\n\r"); getchar();
	tmp = localtime(&tml);

	// printf("3. getTime()\n\r"); getchar();

	*hh = tmp->tm_hour;
	*mm = tmp->tm_min;
	*ss = tmp->tm_sec;

	// printf("getTime() %02d:%02d:%02d\n\r", *hh, *mm, *ss);
	return;
} // void getTime(..


time_t sys_mktime(const struct tm *tmNow)
{
	struct tm mtime;

	mtime = *tmNow;

	if (mtime.tm_year > 1900)
	{
		mtime.tm_year -= 1900;
	}
	else if ((mtime.tm_year >= 0) && (mtime.tm_year < 90))
	{
		mtime.tm_year += 100;
	}
	
    // if tm_isdst is not set properly, force -1
    if ((mtime.tm_isdst != 0) && (mtime.tm_isdst != 1))
    {
        // set isdst flag so that mktime will calculate local time
        mtime.tm_isdst = -1;
    }

	return mktime(&mtime);
} // already define in sys_time.h 

long makeSec(int yr,int mon,int day,int hh,int mm,int ss)
{
	struct tm now;

	now.tm_year = yr;
	now.tm_mon  = mon;
	now.tm_mday = day;
	now.tm_hour = hh;
	now.tm_min  = mm;
	now.tm_sec  = ss;

	return (long)sys_mktime(&now);
} //makeSec

/***************************
void displayTime(char dd, char hr,char min, char sec, char msec)
{
	 struct timeb tmb;
	 struct tm 	*date;

	ftime(&tmb);
 	date = localtime(&tmb.time);

	if(NULL != date)
	{
		if( dd )
			printf("%02d/%02d/%02d ", date->tm_mon+1, date->tm_mday, date->tm_year%100);

		if( hr )
			printf("%02d:", date->tm_hour);

		if( min )
			printf("%02d:", date->tm_min);

		if( sec )
			printf("%02d", date->tm_sec);

		if( msec )
			printf(".%03u", tmb.millitm);

	} //if(NULL != date...

	return;
} // void displayTime(char dd, char hr,char min, char sec, char msec ***********************/

char *displayTime(char *head, char dd, char hr,char min, char sec, char msec, char *tail)
{
	 struct timeb tmb;
	 struct tm 	*date;
	 static char TmStr[128];
	 char	strbuf[64] = {0};
	 int	headlen =0, taillen=0;

	clearBuf(TmStr, sizeof(TmStr), 0);

	if( NULL != head )
	{
		headlen = strlen(head);
	}

	if( NULL != tail )
	{
		taillen = strlen(tail);
	}


	if( 100 < headlen+taillen )
	{
		printf("Error: the sum of head and tail string length is over 100.(%d,%d=%d)\n",
						headlen, taillen, headlen+taillen );
		return NULL;
				
	}

	ftime(&tmb);
 	date = localtime(&tmb.time);

	if(NULL != date)
	{
		if( NULL != head )
			sprintf(TmStr, "%s", head);

		if( dd )
		{
			sprintf(strbuf,"%02d/%02d/%02d ", date->tm_mon+1, date->tm_mday, date->tm_year%100);
			strcat(TmStr, strbuf);
		}

		if( hr )
		{
			sprintf(strbuf,"%02d:", date->tm_hour);
			strcat(TmStr, strbuf);
		}

		if( min )
		{
			sprintf(strbuf,"%02d:", date->tm_min);
			strcat(TmStr, strbuf);
		}

		if( sec )
		{
			sprintf(strbuf,"%02d", date->tm_sec);
			strcat(TmStr, strbuf);
		}

		if( msec )
		{
			sprintf(strbuf,".%03u", tmb.millitm);
			strcat(TmStr, strbuf);
		}

		if( NULL != tail )
		{
			sprintf(strbuf,"%s", tail);
			strcat(TmStr, strbuf);
		}
	} //if(NULL != date...

	return TmStr;
} // void displayTime(char dd, char hr,char min, char sec, char msec

void showNData(const char *title, UCHAR *bf, int sz, int usz, unsigned char smode)
{
    int i, j;
    int sep = usz/2;
	// char addr_flag;
	char ascii_buf[128];

	if( (sz <= 0) || (usz <= 0) )
		return;

	if( (smode&ASCII) && usz > 128) //"ascii_buf" size is 128.
		return;

	if( smode & ASCII )
			clearBuf((unsigned char *)ascii_buf,128,0);

	//  if(sz >= usz)
	//	printf("\n\r");

	// addr_flag = (smode & 0x10); // Don't display Address.
	if( !(smode & NOTIME) )
	{
	 	struct timeb tmb;
	 	struct tm 	*date;
	
		ftime(&tmb);
	 	date = localtime(&tmb.time);

		if(NULL != date)
	 		printf("%02d:%02d:%02d.%03u", 	
	 					date->tm_hour,
	 					date->tm_min,
	 					date->tm_sec,
	 					tmb.millitm );
	 }
	

     if( title != NULL )
	    printf("%s", title); 


	if( !(smode & NOADDR) )
		printf(" %04d(0x%04X)> ", 0,0);
	else
		printf(" ");

    // smode = smode & 0x0F;

    for(i=0; i<sz; i++)
    {
		if(i!=0 && (i%usz) == 0)
		{
			if( smode & ASCII )
			{
				printf(" %s", ascii_buf);
			}
			if( !(smode & NOADDR) )
			{
				printf("\n\r %04d(0x%04X)> ", i, i);
			} else {
				printf("\n\r ");

				if( !(smode & NOTIME) )
					for(j=0;j<12; j++)
						printf(" ");

				if( NULL != title )
					for(j=0;j < (int)strlen(title); j++)
							printf(" ");

				fflush(stdout);
			}
		}

		if( ((i%sep) == 0) &&  ((i%usz) != 0))
		{
			printf(" *");
		}

		if( smode & ASCII )
			ascii_buf[i%usz]=(isprint(bf[i])?bf[i]:'.');

		/*******
		if(smode & HEX)
		{
			if(smode&UPPER) printf(" %02X", bf[i]);
			else			printf(" %02x", bf[i]);
		} else **/
		if(smode & DEC)
		{
			printf(" %03d", bf[i]);
		} else {
			if(smode&UPPER) printf(" %02X", bf[i]);
			else			printf(" %02x", bf[i]);
		}
		// else // ASCII
		//	printf(" %c", (char)bf[i]);
		
		fflush(stdout);
    } //for(i=0; i<sz..

	// printf("i=%d usz=%d sz=%d\n\r", i, usz, sz);

	if( (smode & ASCII))
	{
		if( i%usz )
		{
			ascii_buf[i%usz] = 0;

			if( ((i%sep) == 0) &&  ((i%usz) != 0))
				printf(" *");

			for(j=0; j<(usz - (sz%usz)); j++)
			{
				if( (i%sep !=0) && (((i+j)%sep) == 0) &&  ((i%usz) != 0))
					printf(" *");
				printf("   ");
			}
		}
		printf(" %s", ascii_buf);
	}

	if(!(smode & NOCR))
    	printf("\n\r"); 

    return;
} //showNData(...

long	diffUsec(long *ot,long *ct)
{
	long	dt;

	*ct = getUsec();

	dt  = *ct - *ot;
	*ot = *ct;

	if( dt < 0 )
			dt += 60000000;
	return dt;
} // long diffUsec(..

long diffMsec(long *ot,long *ct)
{
	long uot, uct, dtms; // original ot, ct is msec.

	uot = *ot * 1000;
	uct = *ct * 1000;

	dtms = 	diffUsec(&uot, &uct)/1000;

	*ot = uot/1000;
	*ct = uct/1000;

	return dtms; 

} // long diffMsec(long *ot

long diffSec(long *os,long *ns)
{
	long	dsec;

	time((time_t *)ns);

	dsec = *ns - *os;
	*os  = *ns;

	if( dsec < 0 )
			dsec += 60;

	return dsec;
} //long diffSec

long	getLsec()
{
	time_t	tm;

	time(&tm);

	return (long)tm;
}

int getSec()
{
	byte hh, mm, ss;

	getTime(&hh, &mm, &ss);

	return (int)ss;
} // int getSec()

int	getMsec(void)
{
#ifdef LINUX
	struct 	timeval tv;
	(void)gettimeofday(&tv, (struct timezone *)NULL);
	return((int)((tv.tv_usec/1000L)%1000));
#endif

#ifdef AIM
	sys_high_resolution_time hr_time;
	(void)sys_get_high_resolution_time( &hr_time);

	return((int)(hr_time.seconds*1000+hr_time.milliseconds));
#endif

} // int getMsec(void)...get milli-seconds

long	getUsec()
{
#ifdef LINUX
	struct timeval  tmv;
	struct timezone tmz;
	long	sec;

	gettimeofday(&tmv, &tmz);
#define MAX_GETUSEC_TIME 1000000000000
	sec = (tmv.tv_sec % (MAX_GETUSEC_TIME/1000000L));

	//return ((long)tmv.tv_sec*1000000L + tmv.tv_usec);
	return ((long)sec*1000000L + tmv.tv_usec);
#endif
#ifdef AIM
	return ((long)getMsec()*1000);
#endif
} // long getUsec(..

int countStr(char *sbf, char eoc)
{
	int	sz = 0;

	if(NULL == sbf)
		return sz;

	while(sbf[sz] != eoc)
		sz++;

	return sz;

} // int countStr


/*****
 * insertVal(str, kch, val)
 *		("A987B65C4", 'B', 12)
 *		--> "A987B12C4"
 *	insert values to fit the digit number with character.
 * 	return void;	
 ******/

void insertVal(char *str, char kch,int val)
{
	int		i, j;
	char	bf[5];

	i = j = 0;


	// printf("insertVal(): %s %c   %04d\n\r", str, kch, val);

	for(i=0; str[i] != 0; i++)
	{
		if(str[i] == kch)
		{
			i++;
			break;
		}
	} //for(i=0; ...

	for(j=0; (isalnum(str[i+j]) != 0); j++)
	{
		if( isalpha(str[i+j]) != 0 )
		{
			break;
		}
	}

	switch( j )	
	{
		case 1:
				sprintf(bf, "%1d", val);
				memcpy(&str[i],bf, 1);
				break;
		case 2:
				sprintf(bf,"%02d", val);
				memcpy(&str[i],bf, 2);
				break;
		case 3:
				sprintf(bf,"%03d", val);
				memcpy(&str[i],bf, 3);
				break;
		case 4:
				sprintf(bf,"%04d", val);
				memcpy(&str[i],bf, 4);
				break;
		default:
				printf("658 utl.c Error: code 0x%02X\n\r", kch);
				printf("659 %s\n\r", str);
				break;
	} //switch( diff....

	// printf("\ti=%d   j=%d\n\r%s", i, j, (char *)&str[0]);
	return;

} //insertVal(...G


/********************************************************************
	Same as extractIntVal( )
	if it can't find 'kch', returns -99999(defined as OUT_OF_INT_VALUE)
*********************************************************************/

int extractValue(char *str,char kch,int str_sz)
{
	int i;
	int	ret_val = OUT_OF_INT_VALUE;

	if( str_sz < 4 )	
		return ret_val;

	for(i=4; i < str_sz; i++) 
	{
		if( isalpha(str[str_sz-i]) ) 
		{
			if( str[str_sz-i] == kch ) 
			{
				ret_val = atoi(&str[str_sz-i+1]);
				return ret_val;
			}
		} // if (isalpha(str...
	} //for(i=0; i< str_sz; i++

	return ret_val;
	
} //int extractValue(...

/*********************************************
	extractIntVal("A123B456C789", A) returns 123
 *********************************************/
int extractIntVal(char *str, char kch)
{
	int		i, j;
	char	bf[5];

	i = j = 0;


	// printf("insertVal(): %s %c   %04d\n\r", str, kch, val);

	for(i=0; str[i] != 0; i++)
	{
		if(str[i] == kch)
		{
			i++;
			break;
		}
	} //for(i=0; ...

	for(j=0; (isalnum(str[i+j]) != 0); j++)
	{
		if( isalpha(str[i+j]) != 0 )
		{
			break;
		}
	}

	strncpy(bf, &str[i], j);
	bf[j] = 0;
	// printf("%c : %s %d\n\r",kch, bf, atoi(bf));

	return atoi(bf);
} // int extractIntVal(char *str, char 

void swapByte(byte *first,byte *second)
{
	byte tmp_val;

	tmp_val = *first;
	*first = *second;
	*second = tmp_val;

	return;
} // void swap(....)

int	is_in_range(int tg, int low, int upper)
{
		if( (tg < low) || (tg > upper) )
				return 0;
		else	return 1;
} //is_in_range(int,int,int)

int	isInRange(int tg, int low, int upper)
{
		if( (tg < low) || (tg > upper) )
				return 0;
		else	return 1;
}

int	is_in_range_long(long tg, long low, long upper)
{
		if( (tg < low) || (tg > upper) )
				return 0;
		else	return 1;
} //is_in_range_long(long,long,long)

int isNth(int cnt,int nth)
{
	if( cnt!=0 && ((cnt%nth)==0) )
	{
		return 1;
	} else {
		return 0;
	}
}

/* query_which_bit_set()
 *	MSB(15) 14 13 12 11 10 9 8  7 6 5 4 3 2 1 0 
 *	-------------------------------------------
 * src   1   0  0  0  0  0 0 1  0 1 1 0 0 0 0 1 = 0x8161
 *		query_which_bit_set(1,src,2(byte),loc)
 *
 * RETURNS	 5 (The number of bit set in 2 bytes src)	
 *			*ret = 0, 5, 6, 8, 15
 *
 *		2009.03.11. by KvJ
 *
 */

int query_which_bit_set(char set_val,UCHAR *src,int len, int *ret)
{
	int i, j, tnum = 0;	

	for(i=0; i<len; i++)
	{
		for(j=0; j<8; j++)
		{
			// printf("[%02d = %02d,%02d] 0x%02X %02X", i*8+j, i, j, src[i], (1<<j));
			if(set_val == ((src[i]>>j) & 1))
			{
				// printf("  OK!");
				tnum++;
				*ret = i*8+j;
				ret++;
			}
			// printf("\n\r");
		} //for(j=0, 8
	} //for(i=0, len in bytes

	return tnum;
} // int query_which_bit_set(char set_val,UCHAR *src,int len, int *ret

int query_how_many_ch(char *str,char ch)
{
	int how_many = 0;
	int idx;
	char	*pstr = str;

	for(pstr=str, idx=0;(0!=*pstr) && (idx < 1024); pstr++, idx++)
		if(ch == *pstr)
			how_many++;

	return how_many;
} //int query_how_many_ch(..

/*********************************************
 *  returns the location of nth of char(ch)
 *
 *	("0.123.45.6789",'.',2) --> 5 
 *	("0.123.45.6789",'.',3) --> 8 
 *	("0.123.45.6789",'.',4) --> -1 
 *********************************************/
int query_where_ch(char *str,char ch,int nth)
{
		int idx = 0;

		if( nth<=0 )
				return -1;

		while(0 != str[idx])
		{
			if(ch == str[idx])
			{
				nth--;

				if(0 == nth)
					break;
			}
			idx++;
		} //while(0 != str[idx

		if( idx >= (int)strlen(str) )
				return -1;
		else
				return idx;
} //int query_where_ch(..

int divide_str(char *org,char ch,char *dest1,char *dest2)
{
	int loc = 0;
	int	sz	= 0;

	sz = (int)strlen(org);

	if(!is_in_range(sz, 2, 256))
		return 0;

	if((loc = query_where_ch(org, ch, 1)) < 0)
		return 0;

	strncpy(dest1, org, loc);
	dest1[loc] = 0;
	sprintf(dest2, "%s", &org[loc]);

	return loc;
} //int divideStr(

unsigned char	set_bit8(unsigned char src,int loc,int setval)
{
	unsigned char setbit = 0x01;

	setbit = (0x01 << loc);

	if(1 == setval)
	{
		src |= setbit;
	} else {
		src &= ~setbit;
	}

	return src;
} //unsigned char set_bit8(

unsigned char write_bit8(unsigned char src,int loc,int sz,int setval)
{
	unsigned char clrbit = 0;
	unsigned char setbit = 1;
	unsigned char retval;
	int				i;

	setval &= ((setbit <<sz)-1);

	for(i=0; i<sz; i++, setbit=1)
	{
		setbit <<= (loc+i);
		clrbit |= setbit;
	}

	src &= ~clrbit;
	setval <<= loc;
	retval = (src | setval);

	return retval;
} //uchar write_bit8

unsigned char read_bit8(unsigned char src,int bit_start,int bit_sz)
{
	unsigned char filter = 0;
	unsigned char retval = 0xFF;
	int				i;

	retval &= (src>>bit_start);

	for(i=0; i<bit_sz; i++)
	{
		filter = set_bit8(filter, i, 1);
	}

	retval &= filter;

	return retval;
} //uchar read_bit8

/**
 *	show_bit8: shows bit(0 or 1) strings of 8bit uchar.
 *		If strbuf is not NULL, the bit string will be saved in "strbuf". 
 *
 **/

void show_bit8(char *title,unsigned char ds,char *strbuf,int blank_pace)
{
	int 	i;
	char	ch;

	if( NULL == strbuf )
	{
		if( title == NULL )
			; //printf(" 0x%04X : ", ds);
		else	
			printf("%s %02X", title, ds);
	}

	for(i=0; i<8; i++)
	{
		ch = ((0x80>>i)&ds)?'1':'0';
		if( NULL == strbuf )
		{
			if((i!=0)&&(i%blank_pace==0))
				printf(" ");
			printf("%c", ch);
		} else {
			strbuf[i] = ch;
		}
	}

	if( NULL == strbuf )
		printf("\n\r");

	return;
} // void show_bit8(char *title target_data...

/**
 *	show_bit16: shows bit(0 or 1) strings of 16bit ushort.
 *		If strbuf is not NULL, the bit string will be saved in "strbuf". 
 *
 **/

void show_bit16(char *title,unsigned short ds,char *strbuf,int blank_pace)
{
	int 	i;
	char	ch;

	if( NULL == strbuf )
	{
		if( title == NULL )
			printf(" 0x%04X : ", ds);
		else	
			printf("%s 0x%04X : ", title, ds);
	}

	for(i=0; i<16; i++)
	{
		ch = ((0x8000>>i)&ds)?'1':'0';
		if( NULL == strbuf )
		{
			if((i!=0)&&(i%blank_pace==0))
				printf(" ");
			printf("%c", ch);
		} else {
			strbuf[i] = ch;
		}
	}

	if( NULL == strbuf )
		printf("\n\r");

	return;
} // void show_bit16(char *title target_data...
/*
 * loc: 0~15 setval: 0/1
 */
unsigned short	set_bit16(unsigned short src,int loc,int setval)
{
	unsigned short setbit = 0x0001;

	setbit = (0x0001 << loc);

	if(1 == setval)
	{
		src |= setbit;
	} else {
		src &= ~setbit;
	}

	return src;
} //unsigned short set_bit16

unsigned short write_bit16(unsigned short src,int loc,int sz,int setval)
{
	unsigned short clrbit = 0;
	unsigned short setbit = 1;
	unsigned short retval;
	int				i;

	setval &= ((setbit <<sz)-1);

	for(i=0; i<sz; i++, setbit=1)
	{
		setbit <<= (loc+i);
		clrbit |= setbit;
	}

	src &= ~clrbit;
	setval <<= loc;
	retval = (src | setval);

	return retval;
} //ushort write_bit16

unsigned short read_bit16(unsigned short src,int bit_start,int bit_sz)
{
	unsigned short filter = 0;
	unsigned short retval = 0xFFFF;
	int				i;

	retval = (src>>bit_start);

	for(i=0; i<bit_sz; i++)
	{
		filter = set_bit16(filter, i, 1);
	}

	retval &= filter;

	return retval;
} //ushort read_bit16

/*
 * is_number : If ch is number(decimal), returns 1.
 */

int is_number(unsigned char ch)
{
	if(is_in_range((int)ch,(int)0x30, (int)0x39))
		return 1;
	else
		return 0;
}


/*******************/

/*
 * btohs : read chval and convert to 2 byte hexa char. 
 *
 *	ex) chval=1A --> des[0]='1', des[1]='A' 
 *
 */
void btohs(char *des,unsigned char chval)
{
	int	high_nib, low_nib;

	high_nib = (chval / 0x10);
	low_nib  = (chval % 0x10);
	des[0] = (high_nib < 10)? high_nib+0x30 : high_nib+0x37;
	des[1] = (low_nib  < 10)?  low_nib+0x30 :  low_nib+0x37;

	return;
} // btohs( bin_to_hexa_string...

/* 
 * get_str_after_ch(src_str, separator, order, des_buf)
 *
 *	ex) "TI/R/123/456",'/',2, &buf[10]
 *
 *	RETURNS: "123/456", buf = "123"
 *
 */
char *get_str_after_ch(char *str,char ch,int nth, char *buf)
{
	int 	max_idx = strlen(str);
	int		cur_idx, nxt_idx;
	char	*pret = NULL;

	cur_idx = query_where_ch(str, ch, nth);
	
	if(cur_idx < 0)
			return NULL;

	pret = (char *)&str[cur_idx];

	while(cur_idx < max_idx)
	{
		cur_idx++;
		if(isalnum((int)*pret))
			break;
		else
			pret++;
	} //while(...

	if(NULL != buf && NULL != pret)
	{
		nxt_idx = query_where_ch(str, ch, nth+1);

		if(nxt_idx < 0)
			nxt_idx = max_idx;

		if(cur_idx > nxt_idx)
			nxt_idx = cur_idx;

		strncpy(buf, pret, nxt_idx-cur_idx+1);
		buf[nxt_idx-cur_idx+1] = 0;
		// strncpy_s(buf, 128, pret, nxt_idx-cur_idx);
	}

	return pret;

} //char *get_str_after_ch

/*
 * "ABCC---1WT" returns "1WT" pointer. 	
 *
 */

char *get_str_ignore_ch(char *str,char ch, char *buf)
{
	int 	max_idx = strlen(str);
	int		how_many_ch = 0;
	char	*pret = NULL;

	how_many_ch = query_how_many_ch(str, ch);

	pret = get_str_after_ch(str, ch, how_many_ch, buf);

	if(NULL != buf && NULL != pret)
	{
		strncpy(buf, pret, strlen(pret));
		buf[strlen(pret)] = 0;
		// strncpy_s(buf, 128, pret, nxt_idx-cur_idx);
	}

	return pret;

} //char *get_str_after_ch


/*
 *	print the bit string of a byte with title.
 *	
 *	if title is NULL, only bit string would be printed.
 *
 */
void show_bit(char *title, UCHAR ds)
{
	int i;

	if( title != NULL )
		printf("%s %02X : ", title, ds);

	for(i=0; i<8; i++)
	{
		printf("%d", ((0x80>>i)&ds)?1:0);
	}

	// printf("\n\r");

	return;
} // void show_bit(char *title target_data...

/*
 * set_bit_carray : set "loc" order bit of (carray) char array.
 *			set_val 0 or 1
 * returns : the byte value which has located in loc. 
 */

UCHAR	set_bit_carray(UCHAR *arr, int loc, int set_val)
{
	UCHAR setbit = 0x01;
	UCHAR *pbyte;

	if( loc < 0 )
		return 0;

	pbyte = (UCHAR *)&arr[loc/8];

	setbit = (0x01 << (loc%8));

	if(1 == set_val)
	{
		*pbyte |= setbit;
	} else {
		*pbyte &= ~setbit;
	}

	return *pbyte;
}

/*
 * write_bit_carray : from "bit_start" by "bit_sz"
 *			set_val   : 0 or 1
 *			bit_start : 0(LSB) ~ 
 * returns : the byte value which has located in loc. 
 * 			
 * ex) carray[3] = 00000000 00000000 00000000 (bit)
 * 	   bit_start = 5, bit_sz = 6, set_val = 1
 * 	   carray[3] = 00000000 00000111 11100000
 */

int write_bit_carray(UCHAR *carr,int bit_start,int bit_sz, int set_val)
{
	int i;

	for(i=bit_start; i<(bit_start+bit_sz); i++)
		(void)set_bit_carray(carr, i, set_val);

	return bit_start+bit_sz;
}
/*
 * read from bit_start by bit_sz in char array.
 *
 *	returns the number(value) read.
 *
 */
int read_bit_carray(UCHAR *carr,int bit_start,int bit_sz)
{
	int i, ret = -1;
	int bit_index = 0;
	UCHAR tmpch;

	for(i=0; i<bit_sz; i++, ret <<= 1)
	{
		tmpch = carr[(bit_start+i)/8];
		ret += read_bit8(tmpch, (bit_start+i)%8, 1);
	}

	return ret;	
}

/*
 * strcmp case insensitive. same as strncasecmp in Linux
 *
 *	28 Apr 2010 by KvJ
 */
/***************
int strncasecmp (const char *s1, const char *s2, int n)
{
   char c1, c2;

   if (s1 == NULL) return s2 == NULL ? 0 : -(*s2);
   if (s2 == NULL) return *s1;

   while ((c1 = tolower (*s1)) == (c2 = tolower (*s2)) && (0 < --n))
   {
     if (*s1 == '\0') break;
     ++s1; ++s2;
   }

   return c1 - c2;
} // int strncasecmp ****************/

/*
 * catch the string and copy to target of Nth arguement among src. 
 * returns the string length of that argument.
 *	ex) "ABC, 123 456" (,2,) --> "123"
 *
 *	28 Apr 2010 by KvJ
 */

int	getArgs(char *src, int num, char *tar)
{
	int 	i, retlen = 0;
	char 	sprt[] = ", ";
	char 	tmpbuf[256] = {0};
	char	*chptr;
	char	*nulptr = NULL;

	strncpy(tmpbuf, src, strlen(src));

	for(i=0; i<num; i++)  
	{
		if(0 == i)
			chptr = strtok(tmpbuf, sprt);
		else
			chptr = strtok(NULL, sprt);

		if( NULL != chptr )
		{
			retlen = strlen(chptr);

			if( (num-1) == i )
			{
				strncpy(tar, chptr, retlen);
			}

			printf("%d (%d): %s \n\r", i, retlen, chptr);
		} else {
			retlen = 0;
			break;
		}
	}

	// printf("FINAL %d[%d] %s \n\r", i, retlen, chptr);
	return	retlen; 
} // char *getArgs

/*
 * return number of items by separator
 * ex) "ABC, 123 456 789 DEF"--> 6
 *
 *	28 Apr 2010 by KvJ
 */
int getNumArgs(char *src)	//fundec
{
	int 	i, retnum = 0;
	char 	sprt[] = ", ";
	char 	tmpbuf[256] = {0};
	char	*chptr;

	strncpy(tmpbuf, src, strlen(src));

	for(i=0; i<100; i++)  
	{
		if(0 == i)
			chptr = strtok(tmpbuf, sprt);
		else
			chptr = strtok(NULL, sprt);

		if( NULL != chptr )
		{
			continue;
		} else {
			break;
		}
	} //for(i=0; ...

	return	i; 
} //int getNumArgs
/*
 * getStr(char *rcvbuf,int max_recv_char)
 * 		returns num_of_received_char
 *	ESC, cancel all input string.
 *  \n , put NULL to the last char and return input string.
 *
 */
int getStr(char *sbf,int limit)
{
	int ch, index = 0;	

	while(1L)
	{
		ch = getc(stdin);
		if('\n' == ch)
		{
		    sbf[index] = 0;
			// printf("%s\n\r", sbf);
			break;
		} else if('\x1b' == ch) {
			sbf[0] = 0;
			// printf("ESC: 0x%02x cancel string\n\r", ch);
			break;
		} else {
			sbf[index++] = ch;
		}
		if(index == limit)
		{
			sbf[index] = 0;
			break;
		}
	} //while(1L..

	return index;
}//int getStr(char *sbf.

int getValueFromStr(char *str,char *menu_str,int item_num, char *tbuf)
{
	char sprt[] = ", []=/\t\n\r;*:";
	char *menu_item, *menu_arg;
	char tmpbuf[512] = {0};
	char	*newptr;
	int		i;

	*tbuf = 0;

	newptr = get_ptr_where_str(str, menu_str);

	if( NULL == newptr )
	{
		return OUT_OF_INT_VALUE;
	}

	// clearBuf(tmpbuf, 256, 0);
	// strncpy(tmpbuf, str, strlen(str));
	strncpy(tmpbuf, newptr, strlen(newptr));

	menu_item = strtok(tmpbuf, sprt);

	if( menu_item != NULL )
	{
		if(strcmp(menu_item, menu_str) == STR_EQUAL)
		{
			if(item_num == 0)
			{
				fprintf(stderr,"ERR: incorrect item_num utl.c\n\r");
				return OUT_OF_INT_VALUE;
			}

			for(i=0; i<item_num; i++)
			{
				if((menu_arg = strtok(NULL, sprt))==NULL)
					return OUT_OF_INT_VALUE;
			} // for(i=0; i<item_num

			if(isdigit(menu_arg[0]))
			{
				if((menu_arg[1]=='x')&&(menu_arg[0]=='0'))
				{
					strcpy(tbuf, &menu_arg[2]);
					return (int)a2toc(tbuf);
				} else {
					strcpy(tbuf, &menu_arg[0]);
					return (int)atoi(tbuf);
				} //if( menu_arg == 'x'
			} else {//if( isdigit(menu_arg
				strcpy(tbuf, &menu_arg[0]);
				return strlen(tbuf);
			} //if( isdigit(menu_arg
		} // if(strcmp( ... == STR_EQUAL
	} else { //if( men NULL
		menu_arg = NULL;
	} // if( menu_item != NULL
	return OUT_OF_INT_VALUE;
} // int getValueFromFile(....

int readValueWithFileName(char *fname,char *menu_str, char *rbuf)
{
	FILE	*fptr;
	int		ret;

	if((fptr=fopen(fname,"r")) == NULL) 
		return -1;

	ret = getValueFromFile(fptr,menu_str,1,rbuf);

	fclose( fptr );

	return ret;
} // int readValueWithFileName

/****************************************************************
search *menu_str in fp pointing File.
move pointer to nth item of item_num after '='.
copy the string of nth item by item_num to tbuf. 
NULL : when meet end of line, returns -1
When the string starts at "0x" or "number", returns binary value. 
Comment Line starts with '#' or "//"
*******************************************************************/
int getValueFromFile(FILE *fp,char *menu_str,int item_num, char *tbuf)
{
	int	 ret;
	char tmpbuf[256];

	if(strcmp(tbuf, "DoNotRewind") != STR_EQUAL)
	{
		rewind(fp);
	}
	*tbuf = 0;

	while( fgets(tmpbuf, 256, fp) != NULL)
	{
		if((0x23==tmpbuf[0])||(0x2F==tmpbuf[0] && 0x2F==tmpbuf[1])) // '#' or "//"
				continue;
		if((ret=getValueFromStr(tmpbuf, menu_str, item_num, tbuf)) >= 0)
		{
			return ret; 
		}
	} // while( fgets(..
	return -1;
} // int getValueFromFile(....

/*
 * changeStrUlong(byte *target,char *src_num_str)
 *
 * "12345678" --> return unsigned long   12345678
 * "x1234567" --> return unsigned long 0x1234567
 *				  if(NULL != tgt)	
 *				  	tgt[0~3]
 */
unsigned long	changeStrUlong(unsigned char *tgt, char *num_str)
{
	unsigned long lret = -999999999; // Not a number

	if('x' == num_str[0])
	{
		lret = htoul(&num_str[1]);
	} else if(isdigit(num_str[1])) {
		lret = atol(&num_str[1]);
	} else {
			;
	}

	if(-999999999 != lret && NULL != tgt) // Make Big Endian
	{
		tgt[0] = (unsigned char)(lret/0x1000000);
		tgt[1] = (unsigned char)(lret/0x10000);
		tgt[2] = (unsigned char)(lret/0x100);
		tgt[3] = (unsigned char)(lret%0x100);
		// printf("====== %lX %02X %02X %02X %02X\n\r",
		//			 lret, tgt[0], tgt[1], tgt[2], tgt[3]);
	}


	return lret;
} // ulong changeStrUlong

unsigned short toUshort(unsigned char upper,unsigned char lower)
{
	return (unsigned short)upper*0x100+lower ;
}

/*
 * shiftBuf : shift the buf to the right(+) or left(-).
 * 	buf = "0123456789" --> shiftBuf(buf,3,-2)
 *  Result: buf = "2003456789"
 */
void shiftBuf(UCHAR *bf,int len,int leftright)
{
	int i;

	if(0 < leftright)
	{
		memcpy(bf+leftright, bf, len); 

		for(i=0; i<leftright; i++)
			*bf++ = 0;
	} else {
		leftright *= -1;
		memcpy(bf, bf+leftright, len); 
	}
	return;
} //void shiftBuf

int query_pattern(FILE *fptr,unsigned char *patt,int len)
{
	// FILE	*fptr;
	int 	idx = -1;
	int 	uch;
	int		ilen=0;

	// fptr = fopen(filename,"rb");

	if(NULL != fptr)
	{
		idx = 0;

		while(EOF != (uch=getc(fptr)))
		{
			idx++;
			if((unsigned char)uch == *patt)
			{
				patt++;
				ilen++;

				if(len == ilen)
				{
					idx -= len;
					break;
				}
			} else {
				ilen = 0;
			}
		} //while(NULL != (uch=getc
	} //if( NULL != fptr...

	// printf("check_pattern(index=%d): %d %d\n\r", idx, ilen, len);

	// if(NULL != fptr)
	//	fclose(fptr);

	if(0 == ilen)
		idx = -1;

	return idx;
} //int check_pattern

int query_where_pattern(byte *org,int osz,byte *pat,int psz,int nth)
{
	int	idx, chk_iter;
	int	pos = -1;

	if( osz < psz )
		return pos;

	if(0 >= nth)
		return pos;

	chk_iter = osz - psz + 1; 	


	idx = 0;

	while(idx < chk_iter)
	{
		if(0 == memcmp(org, pat, psz))
		{
			nth--;

			if(0 >= nth)
			{
				pos = idx;
				break;
			}
		}
		idx++;
		org++;
	} //while(0 < nth

	return pos;
} // int query_where_pattern(byte *org,int osz,byte *pat,int psz,int nth..

/*
 * query_how_many_numbers(): counts number string group in str.
 *	 opt = NUMBER_INTEGER | NUMBER_FLOAT
 *
 */
int query_how_many_numbers(char *str,unsigned char opt)
{
	char 	numstr[32] = {0};
	char	pch, cch;
	char	*ptrorg = str;
	char	*tmptr;
	int		numtype;
	int		idx, numstart;
	int		nth = 0;
	int		nthint, nthfloat;

	numstart = 0;
	nthint = nthfloat = 0;
	idx = pch = cch = 0;

	while( 0 != (cch = *str++) )
	{
		switch(cch)
		{
			case '-':
			case '+':
			case '.':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if( 0 == idx )
				{
					memset(numstr, 0, sizeof(numstr));
					numstart = 1;
				}

				if( numstart )
					numstr[idx++] = cch; 

				break;
			case ' ':
			case ',':
				if( '0'<=pch && '9'>=pch || '.'==pch)
				{
					// printf("--->%d(%s)\n", (nthint+nthfloat), numstr);

					tmptr = &numstr[0];

					numtype = NUMBER_INTEGER;	
					while(0 != (cch = *tmptr++))
						if( '.' == cch )
							numtype = NUMBER_FLOAT;	

					if( NUMBER_INTEGER == numtype )
						nthint++;
					else if(NUMBER_FLOAT == numtype )
						nthfloat++;
					else	;

					numstart = 0;
					idx = cch = 0;
				}
				break;
			default:
				idx = 0;
				break;
		}

		pch = cch;
	} // while( 0 != cch

	if( '0'<=pch && '9'>=pch || '.'==pch)
	{
			// printf("--->%d(%s)\n", (nthint+nthfloat), numstr);

			tmptr = &numstr[0];

			numtype = NUMBER_INTEGER;	
			while(0 != (cch = *tmptr++))
				if( '.' == cch )
					numtype = NUMBER_FLOAT;	

			if( NUMBER_INTEGER == numtype )
				nthint++;
			else if(NUMBER_FLOAT == numtype )
				nthfloat++;
			else	;

			idx = cch = 0;
	}

	if( opt & NUMBER_INTEGER )
		nth += nthint;

	if( opt & NUMBER_FLOAT )
		nth += nthfloat;

	return nth;

} // int query_how_many_numbers(char *str...

void refine_str_for_repeat_chars(char *tbf,char *obf,int sz)
{
	char pch, cch;
	char *ptbf;
	int	 i = 0;

	ptbf = (char *)calloc(sz+1, 1);

	if( NULL == ptbf )
		return;

	pch = cch = 0;

	while( 0 != (cch = *obf++) )
	{
		if( ' '==cch || ','==cch)
		{
			if( ' '==pch || ','==pch)
				continue;
		}

		pch 	 = cch;
		ptbf[i++]= cch;
	} //while( 0 != (cch =

	sprintf(tbf, "%s", (char *)&ptbf[0] );

	if( NULL != ptbf )
		free(ptbf);

	return;
} // void refine_str_for_repeat_chars(char *tbf,char *obf,int sz...

/*
 * get_number_str_from_str(): look for nth number group in str.
 * 					   and save the string in nbuf.
 * 	returns number type. (NUMBER_INTEGER or NUMBER_FLOAT )
 *			0 - no nth number group in str.
 *
 *	Caution: number string should not be exceed 64.
 *
 */
int get_number_str_from_str(char *str, int nth, char *nbuf)
{
	char 	numstr[64] = {0};
	char	pch, cch;
	char	*ptrorg = str;
	int		idx;
	int		numtype = 0;

	idx = pch = cch = 0;
	nth--;

	while( 0 != (cch = *str++) )
	{
		switch(cch)
		{
			case '-':
			case '+':
			case '.':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// printf("=%c(%d,%d)", cch, idx, pch);
				if( 0 == idx && 0 == pch )
				{
					// printf("\t\tnth is %d", nth);
					nth--;
					// printf(" --> %d.\n", nth);
				}

				if( 0 == nth )
				{
					numstr[idx++]=cch;
					// printf("[%d]<%s>\n", idx, numstr);
				}
				break;
			case ' ':
			case ',':
				if( '0'<=pch && '9'>=pch || '.'==pch)
				{
					idx = cch = 0;
				}
				break;
			default:
				idx = 0;
				break;
		}

		if( 0  > nth )
			break;

		pch = cch;
	} // while( 0 != cch

	if( 0 < (idx = strlen(numstr)) )
	{
		// printf("NUM(%s)", numstr); 
		if( NULL != nbuf )
		{
			strncpy(nbuf, numstr, strlen(numstr));
			*(nbuf+strlen(numstr)) = 0;

			numtype = NUMBER_INTEGER;

			while(0 != (cch = *nbuf++))
				if( '.' == cch )
					numtype = NUMBER_FLOAT;
				
		}
	}

	// printf("Hit anykey."); getchar();
	return numtype;
} // int get_number_str_from_str(char *str, int nth, char *nbuf...

int get_integer_from_str(int *pnum,char *str)
{
	int i = 0, j;
	int	startnum, endnum;

	if(NULL == str || NULL == pnum)
		return 0;

	i = 0;

	if(isdigit(*str))
		pnum[i++] = atoi(str);

	while(0 != *str++)
	{
		if( ',' == *str )
		{
			// printf("\t\t%s\n", str+1);
			if( isdigit(*(str+1)) )
				pnum[i++] = atoi(str+1);
		} else if( '-' == *str )
		{
			startnum=pnum[i-1];
			endnum = atoi(str+1);
			for(j=startnum+1; j<=endnum; j++)
			{
				pnum[i++] = j;
			}
		}
	} //while(0 != *str++

	return i;	
} //int get_integer_from_str
/*
 * char *getptr_wherestr(char *ostr,char *cstr)
 * 			ostr = " 12345/ABCD"
 * 			cstr = "345/"
 *
 * 		returns ptr="ABCD"
 *
 */
char *get_ptr_where_str(char *ostr,char *cstr)
{
	int		retpos  = 0;
	char	*strptr;
	// int 	cmpsize = strlen(cstr);
	int 	cmpsize, varsize;

	if(NULL == ostr || NULL == cstr)
		return NULL;

	cmpsize = strlen(cstr);

	if(NULL != ostr && 0 >= cmpsize)
		return NULL;

	// printf("Hit anykey...\n\r"); getchar();

	strptr 	= ostr;

	while(1L)
	{
		varsize = strlen(strptr);

		if(cmpsize > varsize)
			break;

		if(0 == strncmp(strptr, cstr, cmpsize))
		{
			// printf("retstr = %s vs %s Hit anykey...\n\r", strptr, ostr); getchar();
			return strptr;
		} else {
			strptr++;
			retpos++;
		}
		// printf("%s vs %s Hit anykey...\n\r", strptr, cstr); getchar();
	} //while(1L...


	return NULL;
} //char *get_ptr_where_str*(...

void replace_str_with_ch(char *str,char *div,char ch)
{
	char	*tmpstr, *pstr;
	char	*pdiv;

	if( NULL==div || NULL==str || 0==ch ) 
		return;

	tmpstr = calloc(strlen(str), 1);

	if(NULL == tmpstr)
		return;

	strncpy(tmpstr, str, strlen(str));

	pstr = tmpstr;

	// printf("\n--------------------\n");
	while(0 != *pstr++)
	{
		for(pdiv=div; 0!=*pdiv; pdiv++)
		{
			if(*pstr == *pdiv)
			{
				// printf("%c-%c(%c)\n", *pstr, *pdiv, ch);
				*pstr = ch;
				break;
			}
		}
	}

	// printf("\n--------------------\n");
	strncpy(str, tmpstr, strlen(tmpstr));

	if(NULL != tmpstr)	
		free(tmpstr);

	return;
} //void replace_str_with_ch


/*
 * strcmp_back(char *fstr,char *sstr,int offset) 
 *
 * 	compare two strings from the end of first string(fstr) with offset.
 *
 * 	used for "CLEC-----1TKDI" compare with "TK" offset=2
 * 		to decide the point type.
 *
 */

int	strcmp_back(char *fstr,char *sstr,int boffset)
{
	int fsz, ssz;
	int status = 1;

	if(NULL == fstr || NULL == sstr || 0 > boffset)
	{
		printf("strcmp_back() Invalid arguemnt assigned.\n\r");
		return status;
	}

	fsz = strlen(fstr);
	ssz = strlen(sstr);

	if( 0 == fsz || 0 == ssz )
	{
		// printf("Invalid string size %d, %d.\n\r", fsz, ssz);
		return status;
	}

	if( ssz+boffset > fsz )
	{
		// printf("strcmp_back Size Mismatch = %d, %d\n\r", fsz, ssz+boffset);
		return status;
	}

	status = strncmp(&fstr[fsz-ssz-boffset], sstr, ssz);

	return status;
} //int strcmp_back

void setTimer(tTimer *tmr,long set_count,void (*timer_ftn_ptr)(),void (*reset_ftn_ptr)())
{
	tmr->set = set_count;
	tmr->count = set_count;
	tmr->timer_function = timer_ftn_ptr; 
	tmr->reset_function = reset_ftn_ptr; 
} // void setTimer(tTimer *tmr,int set_count,void (*func_ptr)()

void resetTimer(tTimer *tmr)
{
	long dt; // for diffMStimer(...)

	tmr->count = tmr->set;
	dt = diffMsec(&tmr->ot, &tmr->ct); //refresh ot, ct.

	if( NULL != tmr->reset_function )
			tmr->reset_function();
} //void resetTimer(tTimer *tmr...

void tickTimer(tTimer *tmr)
{
	if(tmr->set == 0)
		return;

	// printf("293 utl %ld --> %ld\n\r", tmr->set, tmr->count);
	if( --(tmr->count) <= 0 )
	{
		//printf("296 utl %ld --> %ld\n\r", tmr->set, tmr->count);
		tmr->count = tmr->set;
		tmr->timer_function();
	}

	return;
} // void tickTimer(tTimer *tmr)
/*
 *  tickMStimer(tTimer *tmr) : timeout millisecond
 *					tmr->count and tmr->set are regarded as msec.
 *		15Apr2009  KvJ 
 */
void tickMStimer(tTimer *tmr)
{
	long	dt;

	if(tmr->set == 0)
		return;

	dt = diffMsec(&tmr->ot, &tmr->ct);

	// printf("340 utl tickMStimer %ld ,  %ld (ot %ld ct %ld dt %ld)\n\r",
	//			tmr->set/1000, tmr->count/1000, tmr->ot, tmr->ct, dt);

	tmr->count = tmr->count-dt;

	if( tmr->count <= 0 )
	{
		//printf("296 utl %d --> %d\n\r", tmr->set, tmr->count);
		tmr->count = tmr->set;
		tmr->timer_function();
	}
	return;
} // void tickMStimer(tTimer *tmr)



/*
 * getPercent(actual, min, max)
 *	 RETURN : % value
 */
int getPercent(int act,int min,int max)
{
	long	tmp = 0;

	if( max == min )
		return 0;

	tmp = (long)((act-min)*100)/(max-min);

	return (int)tmp;
} //int getPercent

/*********************************
#ifndef
#define	tBeep
typedef struct {
	char ch;
	int	 freq;
	int	 dura;
} tBeep;
#endif
***********************************/

tBeep BeepStr[] = {
	{'0', 100, 20}, {'1', 200, 20}, {'2', 300, 20}, {'3', 400, 20}, {'4', 500, 20},
	{'5', 700, 20}, {'6', 800, 20}, {'7', 900, 20}, {'8',1000, 20}, {'9',1100, 20},
	{'a',1200, 30}, {'A',1250, 35}, {'b',1300, 30}, {'B',1350, 35},
	{'c',1400, 30}, {'C',1450, 30}, {'d',1500, 30}, {'D',1550, 35},
	{'e',1600, 35}, {'E',1650, 37}, {'f',1700, 38}, {'F',1750, 39},
	{'g',1800, 40}, {'G',1850, 41}, {'h',1900, 42}, {'H',1950, 43},
	{'i',2000, 44}, {'I',2100, 45}, {'j',2200, 46}, {'J',2300, 47},
	{'k',2400, 48}, {'K',2500, 49}, {'l',2600, 50}, {'L',2700, 51},
	{'m',2800, 52}, {'M',2900, 53}, {'n',3000, 54}, {'N',3100, 55},
	{'o',3200, 56}, {'O',3300, 57}, {'p',3400, 58}, {'P',3500, 59},
	{'q',3600, 60}, {'Q',3700, 61}, {'r',3800, 62}, {'R',3900, 63},
	{'s',4000, 64}, {'S',4200, 65}, {'t',4400, 66}, {'T',4600, 67},
	{'u',4800, 68}, {'U',5000, 69}, {'v',5300, 70}, {'V',5600, 71},
	{'w',5900, 72}, {'W',6300, 73}, {'x',6700, 74}, {'X',7100, 75},
	{'y',7600, 76}, {'Y',8100, 77}, {'z',8500, 78}, {'Z',9000, 79}, {0,0,0}
};

/*
 *	beepCode
 *		to make speaker beep sound	
 *		ex) beepCode("a0kk9zyYsS");
 *
 * 	2009.03.11. by KvJ
 */

void beep(int freq,int dura)
{
	Beep(freq, dura);
}

void beepCode(char *cds)
{
	tBeep *pbc;

	while(isalnum(*cds))
	{
		for(pbc=&BeepStr[0]; 0!=pbc->ch; pbc++)
		{
			if(pbc->ch == *cds)
			{
				beep(pbc->freq, pbc->dura);
				break;
			}
		} //for(pbc...
		cds++;
	} //while(
} //void beepCode(...


/**
 * areAll : Are all chars of str digit, alpha....etc? 
 * isAny  : Is there any chars(digit, alpha...) in the str?
 * ex)
 *  	areAll(E_ISALNUM,str,strlen(str));
 *  	isAny(E_ISALNUM,str,strlen(str));
 *
 *	07 Aug 2008  drive source is in "exmp/is"
 **/

int (*areFtn[13])(int) = {
       isalnum, isalpha, isascii, iscntrl, isdigit,
	   isgraph, islower, isprint, ispunct, isspace,
	   isupper, isxdigit
};
/***** defined in utl.h
enum	eIsFtn { E_ISALNUM, E_ISALPHA, E_ISASCII, E_ISCNTRL, E_ISDIGIT,
	   			 E_ISGRAPH, E_ISLOWER, E_ISPRINT, E_ISPUNCT, E_ISSPACE,
	   			 E_ISUPPER, E_ISXDIGIT };
 *****/
int areAll(int eis, char *str,int sz)
{
	int ret = 0;

	while(sz-->0)
		// if(0==isdigit(*str++))
		if(0== areFtn[eis](*str++))
			return 0;
		else 
			ret++;

	return ret;
} //int areAll(char *, int sz..

int isAny(int eis, char *str,int sz)
{
	int ret = 0;

	while(sz-->0)
		// if(0==isdigit(*str++))
		if(areFtn[eis](*str++))
			ret++;

	return ret;
} //int isAny(char *, int sz..

void doNoop(unsigned long sz)
{
	if( sz <= 0 )
		return;

	while(sz-- > 0) ;

	return;
} // void doNoop(int sz) : No operation

/*****************************
long	pow(int base,int exp)
{
	long ret = 1;

	if(0 == base)
		ret = 0;
	else
		ret = 1;

	while(0 < exp--)
		ret *= base;

	return ret;
} *****************************/

int srandNum()
{
	struct timeb	tmb;
	long 	sec, msec;
	// int 	i;


	ftime(&tmb);
	sec  = tmb.time;
	msec = tmb.millitm;

	srand(sec+(msec+1)*rand());
	srand(rand());

	return rand();
} //


/*
 * return value [low ~ high]
 */
int randInRange(int low, int high)
{
	int rnum, cen;

	rnum = srandNum();
	cen  = (high - low + 1);
	rnum %= cen;

	return rnum+low;
} //int randInRange


int	opterr = 1,		/* if error message should be printed */
	optind = 1,		/* index into parent argv vector */
	optopt,			/* character checked for validity */
	optreset;		/* reset getopt */
char	*optarg;		/* argument associated with option */

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
int getopt(int nargc, char* const* nargv, const char* ostr)
{
	static char *place = EMSG;		/* option letter processing */
	char *oli;				/* option letter list index */

	if (optreset || !*place) {		/* update scanning pointer */
		optreset = 0;
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = EMSG;
			return (-1);
		}
		if (place[1] && *++place == '-') {	/* found "--" */
			++optind;
			place = EMSG;
			return (-1);
		}
	}					/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' ||
	    !(oli = strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means -1.
		 */
		if (optopt == (int)'-')
			return (-1);
		if (!*place)
			++optind;
		if (opterr && *ostr != ':' && optopt != BADCH)
			(void)fprintf(stderr, "%s: illegal option -- %c\n",
			    "progname", optopt);
		return (BADCH);
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {					/* need an argument */
		if (*place)			/* no white space */
			optarg = place;
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    "progname", optopt);
			return (BADCH);
		}
	 	else				/* white space */
			optarg = nargv[optind];
		place = EMSG;
		++optind;
	}
	return (optopt);			/* dump back option letter */
} // int getopt(int nargc, char* const* nargv, const char* ostr...

char RtuSid[4];
static int DisplayOnScreen;

void setRtuSid(char *sid)
{
	if( 0 == *sid )
		return;

	strncpy(RtuSid, sid, 3);

	return;
} //void setSid(char *sid...

void getFileName(char *strbuf)
{
	char	tmpstr[128] = {0};
	byte 	year, mon, day, hh, mm, ss;

	if( 0 != *RtuSid )
		sprintf(strbuf, "log/pmn%s", RtuSid);
	else
		sprintf(strbuf, "log/pmn");

	getTimeDate(&year, &mon, &day, &hh,   &mm,  &ss);

	sprintf(tmpstr, "%04d%02d%02d-%02d%02d%02d.txt", year+2000, mon, day, hh, mm, ss);

	strcat(strbuf, tmpstr);

	return;
} // void getFileName

void setDisplayOnScreen(int flagset)
{
	DisplayOnScreen = flagset;
}

void logFile(char *str)
{
	static FILE *fptr;
	static char logname[100] = {0};
	char 		tmdate[32] = {0};
	static int  index, repeatidx;

	if(0 == index)
	{
		getFileName(logname);
		// sprintf(logname, "c:/aim/user/log/pmn%s.log", tmdate);

		fptr = fopen(logname, "w");

		// printf(" NEW OPEN : %s\n\r", logname);
		if( NULL == fptr )
		{
			printf("FATAL ERROR: Can't log %s.\n\r", logname);
			return;
		} else {
			; // printf("Create log %s.\n\r", logname);
		}
	} else { 
		fptr = fopen(logname, "a");
		if( NULL == fptr )
		{
			printf("FATAL ERROR: Can't open to append: %s.\n\r", logname);
			return;
		}
	}

	if( 2 > strlen(str) )
	{
		if( 0 == repeatidx )
			fprintf(fptr, "\n");

		if( 128 <= repeatidx++ )
		{
			repeatidx = 0;
		} else {
			if( 0 != index )
				index--;
		}
	} else {
		repeatidx = 0;
	}

	fprintf(fptr, "%s", str);

	if( DisplayOnScreen )
		printf("%s", str);


	if( 2000 < index++ )
	{
		index = 0;
		fclose(fptr);
		// printf("INDEX %d %s \n\r", index, (NULL==fptr)? "NULL":"STILL");
		return;
	}


	fclose(fptr);

	return;
} //void logFile

#ifdef	UTLTEST
int main(int argc,char *argv[])
{
	int		i;
	char	strbuf[512] = {0};

	for(i=1; i<argc; i++)
	{
		printf(" [%d] %s --> result=%d\n", i, argv[i], areAll(E_ISDIGIT, argv[i], strlen(argv[i])));
	}

	return 0;
} //int main(int argc,char *argv
#endif
/**************** End Of winutl.c *****************************/
