/**************************************************
 *	ser.c : serial basic modules
 *
 *	08 Feb 2007		KB
 *
 * 03 Apr 2008 KvJ	emtySer(tSer *serptr)
 ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef LINUX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#endif

#ifdef AIM
#include <sys_lib_def.h>
#endif

#include "./ser.h"
#include "./winutl.h"

#define	SER_MAX_BUF_SZ		4096
#define	SER_UNIT_COMM_SZ	 256
#define	DEFAULT_TIMEOUT   	  3

tBaudrateMap	BaudrateMap[MAX_BR_CODE_NUM] = {
	// Baudrate_Code	baudrate	baudrate_string
	{0x00,	(uint)B2400,	"2400"},
	{0x01,	(uint)B4800,	"4800"},
	{0x02,	(uint)B9600,	"9600"},
	{0x03,	(uint)0,	"0"},
	{0x04,	(uint)B19200,	"19200"},
	{0x05,	(uint)0,	"0"},
	{0x06,	(uint)0,	"0"},
	{0x07,	(uint)B38400,	"38400"},
	{0x08,	(uint)0,	"0"},
	{0x09,	(uint)B57600,	"57600"},
	{0x0A,	(uint)B115200,	"115200"}
};

char StrFlowCtrl[][5] = { "NO", "HW", "SW", "\0"}; 
char StrSerCommType[][10] = { "RS232", "RS422", "RS485W2", "RS485W4", "\0"}; 

void showSer(tSer *ser);

char *getBrstr(uint baudrate)
{
	int i;
	for(i=0; i<MAX_BR_CODE_NUM; i++)
	{
		if( baudrate == BaudrateMap[i].baudrate )
			return BaudrateMap[i].br_str;
	}
	return  BaudrateMap[3].br_str;
}

uint getBaudrate(char *br)
{
	int i;
	for(i=0; i<MAX_BR_CODE_NUM; i++)
	{
		if( strncmp(br,BaudrateMap[i].br_str,strlen(br)) == STR_EQUAL)
			return BaudrateMap[i].baudrate;
	}
	return  BaudrateMap[3].baudrate;
}

int writeSer(tSer *s, byte *buf, int w_sz)
{
	int  sz, retry_cnt, tmpwsz;
	char tmp_buf[SER_MAX_BUF_SZ];

	memcpy(tmp_buf, (char *)buf, w_sz);
	retry_cnt = s->tcnt;

	sz = 0;

	while(retry_cnt-->0)
	{
		while(sz < w_sz)
		{
#ifdef LINUX	
				tmpwsz = write(s->sfd, &tmp_buf[sz], w_sz-sz);
#endif
#ifdef AIM	
				tmpwsz = sys_serial_write((SYS_SERIAL_HANDLE)s->sfd, &tmp_buf[sz], w_sz-sz);
#endif
				if(0 < tmpwsz)
				{
					sz += tmpwsz;
					// printf("wsz:%d sz:%d %d\n\r", w_sz, sz, w_sz-sz);
				}
				doNoop((long)s->tout);
				// usleep((long)s->tout);
		}

		if((sz < w_sz) || (sz<=0))
		{
			// showNData("[WRT err]", tmp_buf, sz, 20, HEX);
		} else {
			// showNData("[WRT SER]", tmp_buf, sz, 20, HEX);
			return sz;
		} //if((sz!=w_sz
		doNoop((long)s->tout);
		// usleep((long)s->tout);
	}//while(retry_cnt--

	return -1; //FAIL;
}//int writeSer(tSer *s..

int readSer(tSer *s, byte *buf, int r_sz)
{
	int sz;
	int retry_cnt;
	byte tmp_buf[SER_UNIT_COMM_SZ]; // 256

	retry_cnt = s->tcnt;

	while(retry_cnt-->0)
	{
#ifdef LINUX	
		sz = read(s->sfd, tmp_buf, r_sz);
#endif

#ifdef AIM	
		sz = sys_serial_read((SYS_SERIAL_HANDLE)s->sfd, tmp_buf, r_sz);
#endif

		if(sz <=0 )
		{
			// show("[RD err]", tmp_buf, sz);
		} else {
			// show("[RD SER]", tmp_buf, sz);
			memcpy(buf, tmp_buf, sz);
			return sz;
		} //if((sz!=w_sz
		doNoop((long)s->tout);
		// usleep(s->tout);
	}//while(retry_cnt--

	return -1; //FAIL;
}//int readSer(tSer *s..

int emtySer(tSer *serptr)
{
    int 	sz, rcv_cnt = 0;
    byte	tmpbuf[12];

	// while((sz = read(serptr->sfd, tmpbuf, 10)) > 0)
	while((sz = readSer(serptr, tmpbuf, 10)) > 0)
	{
			rcv_cnt += sz;
			if( rcv_cnt > 10000 )
			{
				fprintf(stderr, "emtySerial port has too long msg.\n\r");
				rcv_cnt = 0;
			}
	}

	return rcv_cnt;
} //int emtySer(qid, type..

int openSer(tSer *ser)
{
#ifdef LINUX
	struct termios options;
	int	fd;
	int	retry_cnt;
	int retval = FAIL_1;

	retry_cnt = 3;
	while(retry_cnt-->0)
	{
		if((fd=open(ser->port_name, O_RDWR|O_NDELAY)) > 0)
			break;
		doNoop(1000L);
		// usleep(1000L);
	} //while(retry_cnt--

	if(fd <= 0)
	{
		printf("open port(%s) error !! errno = %d\n\r",
						ser->port_name, errno);
		return retval; //FAIL_1 = -1
	} //if(fd <= 0

	ser->sfd = fd;	

	if( RS232 != ser->mode ) // only for MOXA
	{
		if(0 != ioctl(ser->sfd, (0x400+66), &ser->mode))
		{
			printf("serial port type %d is set.\n\r", ser->mode);
		}
	}

	fcntl(ser->sfd, F_SETFL, 0); // reset the status flag

	tcgetattr(ser->sfd, &options);

	// printf("c_iflag:%X c_oflag:%X c_lflag:%X\n\r",
	//	options.c_iflag, options.c_oflag, options.c_lflag );

	//if( ser->baudrate < B9600 )
	//	ser->baudrate = B9600;

	tcflush(ser->sfd, TCIOFLUSH);
	cfsetispeed(&options, ser->baudrate);
	cfsetospeed(&options, ser->baudrate);
	if( tcsetattr(ser->sfd, TCSANOW, &options) != 0 )
	{
		printf("Err: tcsetattr ser->sfd\n\r");
		return retval; //FAIL_1 = -1
	}
	tcflush(ser->sfd, TCIOFLUSH);
// end of setting speed

	tcgetattr(ser->sfd, &options);

	// set databits
	options.c_cflag &= ~CSIZE;	
	options.c_cflag |= CS8;	

	// set parity
	if('N' == ser->parity) //No Parity
	{
		options.c_cflag &= ~PARENB;  // clear Parity Enable
		options.c_iflag &= ~INPCK;   // Enable Parity
	} else if('O' == ser->parity) // Odd
	{
		options.c_cflag |= PARENB;
		options.c_cflag |= PARODD; 
		options.c_iflag |= ~INPCK;   // Enable Parity
	} else if('E' == ser->parity) // Even
	{
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD; 
	} else if('S' == ser->parity) // Space
	{
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
	} else if('E' == ser->parity) // Mark
	{
		options.c_cflag |= CSTOPB;
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
	} else { // Out of scope
		printf("Err: ser->parity %d is out of range.\n\r", ser->parity);
		return retval; //FAIL_1 = -1
	} // if( ser->parity...


	if(5 == ser->databit)
	{
		options.c_cflag |= CS5;
	} else if(6 == ser->databit)
	{
		options.c_cflag |= CS6;
	} else if(7 == ser->databit)
	{
		options.c_cflag |= CS7;
	} else if(8 == ser->databit)
	{
		options.c_cflag |= CS8;
	} else {
		printf("Err: ser->databit %d is out of range.\n\r", ser->databit);
		return retval; //FAIL_1 = -1
	}

	// set stopbit
	if(1 == ser->stopbit)
	{
		options.c_cflag &= ~CSTOPB;
	} else if(2 == ser->stopbit)
	{
		options.c_cflag |= CSTOPB;
	} else {
		printf("Err: ser->stopbit %d is out of range.\n\r", ser->stopbit);
		return retval; //FAIL_1 = -1
	}

	// hang up if close fd
	options.c_cflag |= (HUPCL | CREAD);

	tcflush(ser->sfd, TCIFLUSH);

	options.c_iflag = (IGNBRK | IGNPAR);
	options.c_oflag = 0;	
	options.c_lflag = 0;	
	// options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG); //non-canonical	
	options.c_cc[VMIN] = 1;	
	options.c_cc[VTIME] = 0; //DEFAULT_TIMEOUT;	

	
	fcntl(ser->sfd, F_SETFL, O_RDWR|O_NOCTTY|O_NDELAY);	
	tcsetattr(ser->sfd, TCSANOW, &options);
	tcflush(ser->sfd, TCIOFLUSH);
#endif


#ifdef AIM	
	SYS_SERIAL_HANDLE handle;
	int		timeout = 10000;

	// handle = sys_serial_open(const char *port,int baud,short data_bits,short stop_bits,char parity,int timeout
	// default timeout = 1000
	handle = sys_serial_open(ser->port_name,(int)ser->baudrate,(short)ser->databit,(short)ser->stopbit,(char)ser->parity, timeout);

	if( NULL != handle )
		ser->sfd = (int)handle;
	else
	{
		exit(0);
	}

#endif
	return SUCCEED1; //SUCCESS
} //int openSer(tSer *ser

/*****************************************************************
 ser_ptr->sfd = openUart(  B9600,7,1,'E', 0 ) E:Even O:Odd N:No 
 ser_ptr->sfd = openUart(B115200,8,1,'N','H') H:HW S:SW 0:NoCtrl 
 ****************************************************************/
int openUart(char *portname, uint baudrate, int databits,int stopbits,int parity, int flow)
{
#ifdef LINUX
	struct termios options;
	int	   fd = -1;
	int	   retry_cnt = 3;
	int	   status;

	while(retry_cnt-->0)
	{
		if((fd=open(portname, O_RDWR|O_NDELAY)) > 0)
			break;
		doNoop(1000L);
	} //while(retry_cnt--

	if(fd <= 0)
	{
		printf("open port(%s) error !! errno = %d\n\r", portname, errno);
		return FAIL; //FAIL
	} //if(fd <= 0

	/*! Get uart attribute*/
	if( tcgetattr(fd,&options)  !=  0)
	{
		perror("openUart attribute error!!\n");
		return(FAIL);
	}/*if*/

    /*!set databits*/
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
       	case 7: options.c_cflag |= CS7; break;
       	case 8: options.c_cflag |= CS8; break;
       	default:
       		fprintf(stderr,"Unsupported data size\n"); return (FAIL);
	}/*switch*/

	/*!set parity*/
	switch (parity)
	{
       	case 'n': case 'N':
       		options.c_cflag &= ~PARENB;   /* Clear parity enable */
       		options.c_iflag &= ~INPCK;     /* Enable parity checking */
       		break;
       	case 'o': case 'O':
       		options.c_cflag |= (PARODD | PARENB); /* set parity*/
       		options.c_iflag |= INPCK;             /* Disnable parity checking */
       		break;
       	case 'e': case 'E':
       		options.c_cflag |= PARENB;     /* Enable parity */
       		options.c_cflag &= ~PARODD;   /* change parity to even*/
       		options.c_iflag |= INPCK;       /* Disnable parity checking */
       		break;
       	case 'S': case 's':  /*as no parity*/
         	options.c_cflag &= ~PARENB;
       		options.c_cflag &= ~CSTOPB;break;
       	default:
       		fprintf(stderr,"Unsupported parity\n");
       		return (FAIL);
	}/*switch*/

	/*!set stop bits*/
	switch (stopbits)
	{
       	case 1: options.c_cflag &= ~CSTOPB; break;
       	case 2: options.c_cflag |= CSTOPB;  break;
       	default:
       		 fprintf(stderr,"Unsupported stop bits\n");
       		 return (FAIL);
	} /*switch*/

	/*!Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;

	/*!flow control*/
	switch(flow)
	{
	    case 'H':	/*Hardware flow control*/
	       options.c_cflag |= CRTSCTS;
		   break;
		default:
	       options.c_cflag &= ~CRTSCTS;
		   break;
	}
	/*!hang up if close fd*/
	options.c_cflag |= HUPCL | CREAD;

	tcflush(fd,TCIOFLUSH);

	options.c_iflag      = IGNBRK | IGNPAR;
    options.c_oflag      = 0;
    options.c_lflag      = 0;
    options.c_cc[VMIN]   = 1;/* Update the options and do it NOW */
    options.c_cc[VTIME]  = 0;/* set time over 15 seconds*/
	
	cfsetispeed(&options, baudrate);
	cfsetospeed(&options, baudrate);

	status = tcsetattr(fd, TCSANOW, &options);
	if(status != 0) //! check if setting is ok
	{
		perror("tcsetattr fd in openUart()");
		return (FAIL);
	}  /*if*/

	return (fd);
#else
	return 0;
#endif

}/*openUart*/

int changeBaudrate(tSer *seri,unsigned int br)
{
#ifdef LINUX	
	int	   sts;
	struct termios opt;

	tcgetattr(seri->sfd, &opt);

	tcflush(seri->sfd, TCIOFLUSH);
	cfsetispeed(&opt, br);
	cfsetospeed(&opt, br);

	sts = tcsetattr(seri->sfd, TCSANOW, &opt);
	if( sts != 0 )
	{
		return -1;
	} else {
		tcflush(seri->sfd, TCIOFLUSH);
		return 1;
	}
#else
		return -1;
#endif
} // int changeBaudrate

void clearSeriBuf(tSer *seri)
{
#ifdef LINUX	
	int clr_cnt = 0;
	byte	tmp_buf[100];

	while( readSer(seri, (byte *)&tmp_buf[0], 50) > 0 )
	{
		clr_cnt++;
		if( clr_cnt > 100 )
		{
			printf("Err: clear Serial Buffer Error: %d\n\r",
								clr_cnt);
			break;
		}
	} //while( readSer(..
#endif
	return;	
} //void clearSeriBuf(tSer..


void showSer(tSer *ser)
{
	int br_idx = -1;
	int	i;


	for(i=0; i<MAX_BR_CODE_NUM; i++)
	{
		if(BaudrateMap[i].baudrate == ser->baudrate)
			break;
	}

	if( i < MAX_BR_CODE_NUM)
		br_idx = i;
	else	printf("Can't find the Baudrate Index Code\n\r");


	printf("PORT NAME : %s\n\r", ser->port_name);
	printf("BAUDRATE  : %u %s\n\r", ser->baudrate,
				BaudrateMap[br_idx].br_str);
	printf("PORT NUMB : %d\n\r", ser->port_num);
	printf("SFD       : %d\n\r", ser->sfd);
	printf("TIME OUT  : %d\n\r", ser->tout);
	printf("TIME CNT  : %d\n\r", ser->tcnt);
	printf("ERR  CNT  : %d\n\r", ser->terr);
	printf("FLOW CTRL : %s\n\r", StrFlowCtrl[(int)ser->flowctrl]);
	printf("DATA BIT  : %d\n\r", ser->databit);
	printf("PARITY    : %c\n\r", ser->parity);
	printf("STOP BIT  : %d\n\r", ser->stopbit);

	return;
	
} //showSer

int calChar(char *bf)
{
	int cnt = 0;

	while(*bf++ != 0)
		cnt++;

	return cnt;
}


int readSerEos(tSer *ser,byte *bf, char eos)
{
	int 	retry_cnt = 20;
	long 	interval = 10;
	int		rcv_cnt = 0;
	char	rdbuf[256];

	clearBuf(rdbuf,256,0);

	if(readSer(ser, &rdbuf[rcv_cnt], 1) > 0) 
	{
			rcv_cnt++;
			while(1L)
			{
				if(readSer(ser, &rdbuf[rcv_cnt], 1) > 0) 
				{
					if(eos == rdbuf[rcv_cnt])
					{
							rcv_cnt++;
							break;
					}
					rcv_cnt++;
					if( rcv_cnt >= 250 )
					{
							printf("70: Out of Buf Limit %d.\n\r", rcv_cnt); 
							break;
					}
				} else {
					if(retry_cnt-- < 0)
						break;
					else
						usleep(interval);
				}
			} //while ..1L..
	} //if( rsz .. > 0

	if( rcv_cnt > 0 )
	{
			memcpy((byte *)bf, (byte *)&rdbuf[0], rcv_cnt);
	}
	return rcv_cnt;
} //int readSerEos(tSer *ser...

#ifdef	SERIAL_TEST
void testSerial(void)
{
	tSerial test_seri;
	//char 	port_name[20];
	char 	buf[50];
	byte	rbf[100];
	int	i;
	int	ret=0, tmpcnt=0;
	byte	snd_str[] = {" abcdefgh\n\r\0"};
	int	cnt = 0;
	long 	t;
	struct tm *tp, *localtime();

	time(&t);
	tp = localtime(&t);

	clearBuf(buf, 50, 0);
	clearBuf(rbf,100, 0);

	test_seri.baudrate = BaudrateMap[10].baudrate; //9600--- 4:19200
	sprintf((char *)&test_seri.port_name[0], "/dev/ttyUSB1");

	ret = openSer(&test_seri);

	strncpy((char *)&buf[0], snd_str, 20);

	test_seri.port_num = 0;
	test_seri.tout = 10000;
	test_seri.tcnt = 10;
	test_seri.terr = 0;

	showSer(&test_seri);

	//printf("fflush(): %d\n\r", fflush(test_seri.sfd));

	while(1L) //for(i=0; i<100; i++)
	{
		time(&t);
		tp = localtime(&t);

		sprintf(buf, "%02d:%02d:%02d %03d \n\r",
				tp->tm_hour, tp->tm_min, tp->tm_sec, getMsec());

		ret = writeSer(&test_seri,buf, strlen(buf));

		//usleep(100000L);

		ret = 0;
		ret = readSer(&test_seri,rbf,100);
	} // while(1L)

} //void test Ser(void..

int main(int argc,char *argv[])
{
	printf("Starting Serial Sending\n\r");
	testSerial();
	printf("Terminating Serial Sending\n\r");
	return 0;
} //int main(int argc, char *argv[]
#endif
