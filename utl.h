/*
 * utl.h 	: 
 *
 *	copied from c:/aim/source/fep/daq/inc/protocol_all_util.h
 *	            c:/aim/source/fep/daq/pmn/protocol_all_util.c
 *
 *	24 Mar 2010
 */

#ifndef UTL_H
#define UTL_H

#ifndef 	SUCCEED1
// #undef	SUCCEED1
#define	SUCCEED1	1
#endif 

#ifndef 	FAIL0
// #undef 	FAIL0
#define	FAIL0	0
#endif 


#ifndef 	FAIL_1
//#undef 	FAIL_1
#define	FAIL_1 -1	
#endif 

/****************************
#ifndef	byte
#define	byte	unsigned char
#endif
 ****************************/

#ifndef	UCHAR
#define	UCHAR	unsigned char
#endif

#ifndef	uchar
#define	uchar	unsigned char
#endif

#ifndef	USHORT
#define	USHORT	unsigned short
#endif

#ifndef	ushort
#define	ushort	unsigned short
#endif

#ifndef	ULONG
#define	ULONG	unsigned long
#endif

#ifndef	ulong
#define	ulong	unsigned long
#endif

#ifndef	UINT
#define	UINT	unsigned int
#endif

#ifndef	uint
#define	uint	unsigned int
#endif

#ifndef 	STR_EQUAL
#define		STR_EQUAL	0
#endif 

#ifndef 	OUT_OF_INT_VALUE
#define		OUT_OF_INT_VALUE -99999	
#endif 

#ifndef 	OUT_OF_DOUBLE_VALUE
#define		OUT_OF_DOUBLE_VALUE -9999999.0	
#endif 

#ifndef 	NUMBER_INTEGER
#define		NUMBER_INTEGER  1
#endif 

#ifndef 	NUMBER_FLOAT
#define		NUMBER_FLOAT  2
#endif 

#define NIBBLE_SPACE 0x01 //for showBit 
#define LSB_FIRST 	 0x02 //for showBit 

#ifndef	MAX_LIMIT_LINE
#define	MAX_LIMIT_LINE 20
#endif

#ifndef	HITKEY
#define	HITKEY {printf("Hit anykey to continue..."); getchar();}
#endif

enum SHOW_DAT { HEX=0, DEC, ASCII, BIN=4, NOADDR=0x10, UPPER=0x20,NOTIME=0x40, NOCR=0x80 };
enum FG_COLOR { GRAY=30, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE};
enum BG_ATTRI { NORMAL, BOLD, WEAK, UNDERLINE=4, REVERSE = 7};
enum BG_COLOR { BN=40, BR, BG, BY, BB, BP, BC, BD, BW};

enum	eIsFtn { E_ISALNUM, E_ISALPHA, E_ISASCII, E_ISCNTRL, E_ISDIGIT,
	   			 E_ISGRAPH, E_ISLOWER, E_ISPRINT, E_ISPUNCT, E_ISSPACE,
	   			 E_ISUPPER, E_ISXDIGIT };


typedef struct {
	char ch;
	int	 freq;
	int	 dura;
} tBeep;

#ifndef	TTIMER
#define TTIMER
typedef struct TTIMER
{
	long set;
	long count;
	long ct;
	long ot;
	void (*timer_function)();
	void (*reset_function)();
} tTimer;
#endif

typedef struct {
	UCHAR	year;	
	UCHAR	mon;	
	UCHAR	day;	
	UCHAR	hour;	
	UCHAR	min;	
	UCHAR	sec;	
} tDateTime;

void 	clearBuf(UCHAR *pbuf,int sz,UCHAR set_value);
void 	shiftBuf(UCHAR *bf,int len,int leftright);


UCHAR 	atoc(char ch);
UCHAR 	a2toc(char *bf);
UCHAR 	htob(char *bf);
USHORT 	utob(char *bf);
int 	stob(char *sbf, int sz,unsigned char mode);
ULONG	htoul(char *bf);
void    readBinStr(unsigned char *numb,char *str);

void 	ustob(unsigned short us,unsigned char *bf);
USHORT 	btous(unsigned char *bf);

USHORT 	crc16_ccitt(const UCHAR *buf, int len);

ULONG 	getLongMsec();
int 	elapseMsec (ULONG origtime);
int		getMsecStr(char *hmsmsec);
// int 	getMsec(char *hmsmsec);
long 	diffSec(long *os, long *ns);
long 	diffMsec(long *os, long *ns);
long 	getLsec();
int 	getSec();
int 	getMsec(void);
long 	getUsec();
int 	getTimeDate(UCHAR *yr,UCHAR *mon,UCHAR *day,UCHAR *hh,UCHAR *mm,UCHAR *ss);
void 	getTime(UCHAR *hh, UCHAR *mm, UCHAR *ss);
long    makeSec(int yr,int mon,int day,int hh,int mm,int ss);
// void 	displayTime(char dd, char hr,char min, char sec, char msec);
char   *displayTime(char *head,char dd,char hr,char min,char sec,char msec,char *tail);
void 	showNData(const char *title, UCHAR *bf, int sz, int usz, UCHAR smode);

void 	printColor(char fg, char bg, char *str);
int 	countStr(char *sbf, char eoc);

void 	insertVal(char *str, char kch,int val);
int 	extractValue(char *str,char kch,int str_sz);
int 	extractIntVal(char *str,char kch);

void 	swapByte(UCHAR *first,UCHAR *second);
int 	compByte(unsigned char *tg, unsigned char *dt, int sz);
// extern void showBit(char *title, unsigned char dt, UCHAR mode);
void 	showBit(char *title, UCHAR ds, UCHAR mode, char zero, char one);
UCHAR 	setBit(UCHAR src,int loc, int set_val);
UCHAR 	readBit(UCHAR src,int bit_start, int bit_sz);
int 	compBit(unsigned char tg, unsigned char dt, int bt);
void 	showCos8(char *title, UCHAR ps, UCHAR ds, UCHAR mode,char zero,char one,char coszero,char cosone);
void 	showCos16(char *title, ushort ps, ushort ds, UCHAR mode,char zero,char one,char coszero,char cosone);
void 	showBit16(char *title, ushort ds, UCHAR mode,char zero,char one);
int		is_in_range(int tg, int low, int upper);
int		isInRange(int tg, int low, int upper);
int		is_in_range_long(long tg, long low, long upper);
int 	query_which_bit_set(char set_val,UCHAR *src,int len, int *ret);
int 	query_how_many_ch(char *str,char ch);
int 	query_where_ch(char *str,char ch,int nth);
int 	query_where_pattern(UCHAR *org,int osz,UCHAR *pat,int psz,int nth);

int 	query_how_many_numbers(char *str,unsigned char opt);
int 	get_number_str_from_str(char *str, int nth, char *nbuf);
void    refine_str_for_repeat_chars(char *tbf,char *obf,int sz);
int		get_integer_from_str(int *pnum,char *str);
char 	*get_ptr_where_str(char *ostr,char *cstr);

void	replace_str_with_ch(char *str,char *div,char ch);

int		strcmp_back(char *fstr,char *sstr,int boffset);
int 	divide_str(char *org,char ch,char *dest1,char *dest2);

UCHAR	set_bit8(unsigned char src,int loc,int setval);
UCHAR 	write_bit8(unsigned char src,int loc,int sz,int setval);
UCHAR 	read_bit8(unsigned char src,int bit_start,int bit_sz);

void 	show_bit8( char *title,unsigned char  ds,char *strbuf,int blank_pace);
void 	show_bit16(char *title,unsigned short ds,char *strbuf,int blank_pace);
USHORT	set_bit16(unsigned short src,int loc,int setval);
USHORT 	write_bit16(unsigned short src,int loc,int sz,int setval);
USHORT 	read_bit16(unsigned short src,int bit_start,int bit_sz);

int 	is_number(unsigned char ch);
void 	btohs(char *des,unsigned char chval);
char 	*get_str_after_ch(char *str,char ch,int nth, char *buf);
char 	*get_str_ignore_ch(char *str,char ch, char *buf);
void 	show_bit(char *title, UCHAR ds);
UCHAR	set_bit_carray(UCHAR *arr, int loc, int set_val);

int 	write_bit_carray(UCHAR *carr,int bit_start,int bit_sz, int set_val);
int 	read_bit_carray(UCHAR *carr,int bit_start,int bit_sz);
int 	query_pattern(FILE *fptr,unsigned char *patt,int len);
ULONG 	changeStrUlong(unsigned char *tgt, char *num_str);

USHORT 	toUshort(unsigned char upper,unsigned char lower);
void 	shiftBuf(UCHAR *bf,int len,int leftright);
ULONG	changeStrUlong(unsigned char *tgt, char *num_str);
// int 	strncasecmp (const char *s1, const char *s2, int n);
int		getArgs(char *src, int num, char *tar);
int 	getNumArgs(char *src);
int 	getStr(char *sbf,int limit);
int 	getValueFromStr(char *str,char *menu_str,int item_num, char *tbuf);

int 	readValueWithFileName(char *fname,char *menu_str, char *rbuf);
int 	getValueFromFile(FILE *fp,char *menu_str,int item_num, char *tbuf);

int 	getPercent(int act,int min,int max);

void 	beep(int freq,int dura);
void	beepSound(long dura);
void 	doNoop(unsigned long sz);
void 	beepCode(char *cds);


void 	printColor(char fg,char bg,char *str);
void	printTimeMsec(char color);
int		countStr(char *sbf,char eoc);

/**
 * areAll : Are all chars of str digit, alpha....etc? 
 * isAny  : Is there any chars(digit, alpha...) in the str?
 * ex)
 *  	areAll(E_ISALNUM,str,strlen(str));
 *  	isAny(E_ISALNUM,str,strlen(str));
 *
 *	07 Aug 2008  drive source is in "exmp/is"
 **/

int areAll(int eis, char *str,int sz);
int isAny( int eis, char *str,int sz);



// long	pow(int base,int exp);
int 	srandNum();

int		kbhit();
int 	randInRange(int low, int high);

void changeEndian(UCHAR *bf,int sz);
void setTimer(tTimer *tmr,long set_count,void (*timer_ftn_ptr)(),void(*reset_ftn_ptr)());
void resetTimer(tTimer *tmr);
void tickTimer(tTimer *tmr);
void tickMStimer(tTimer *tmr);
void countDown(long tmcnt,long interval,void (*runftn)());

void showProgress(void);
void setPromptColor(int fg, int bg);


#ifndef _GETOPT_H_
#define _GETOPT_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* These are global getopt variables */
extern int   opterr,   /* if error message should be printed */
	     optind,   /* index into parent argv vector */
	     optopt,   /* character checked for validity */
	     optreset; /* reset getopt */
extern char* optarg;   /* argument associated with option */

/* Original getopt */
int getopt (int, char*const*, const char*);

void setRtuSid(char *sid);
void getFileName(char *strbuf);
void logFile(char *str);

#ifdef  __cplusplus
}
#endif
 
#endif /* !_GETOPT_H_ */


#endif // UTL_H
/************** End of winutl.h ********************************/
