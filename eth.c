/*****
 *
 *  eth.c : Manage all data and send/recv through UDP or TCP.
 *	
 *   Kevin Jang		
 *   - 13 Feb 2007 at AGNC  (UDP module)
 *   - 13 Apr 2009 at ARINC (add TCP module)	
 *
 *****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>

#ifdef LINUX
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "./data_type.h"
#include "./eth.h"
#include "./ser.h"
#include "./winutl.h"

#define	MAX_RCV_SZ	1024

char StrTcpUdp[][10] = { "TCP", "UDP", "\0"}; 
char StrSerCli[][10] = { "SERVER", "CLIENT", "\0"}; 

void showEth(tEth *ethptr)
{
	printf("       IP : %s\n\r", ethptr->ip);	
	printf("Client IP : %s\n\r", ethptr->cip);	
	printf("Port Numb : %d\n\r", ethptr->port_num);	
	printf("Port Attr : %s_%s\n\r",
				StrTcpUdp[ethptr->tusc/0x10], StrSerCli[ethptr->tusc&0x01]);	
	printf("Seq       : %d\n\r", ethptr->seq);
	printf("Comm Start: %d\n\r", ethptr->comm_start);
	printf("No Comm   : %d\n\r", ethptr->no_comm_cnt);
	printf("Timeout   : %d\n\r", ethptr->timeout_cnt);
	printf("Retry     : %d\n\r", ethptr->retry_count);
} //void showEth

int	initUdpClient(tEth *udptr) //Client Side 
{
	struct 	hostent *he;
	int		flags = 1;

	if(NULL == (he=gethostbyname(udptr->ip)))
	{
#ifdef LINUX
		herror("gethostbyname");
#else
		printf("gethostbyname");
#endif
		return 0;
	}
	if(0 >= (udptr->sfd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		perror("socket");
		errno = 0;
		return 0;
	}

	udptr->their_addr.sin_family = AF_INET;
	udptr->their_addr.sin_port   = htons((short)udptr->port_num);
	udptr->their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
	// bzero(&(udptr->their_addr.sin_zero), 8);
	memset(&(udptr->their_addr.sin_zero), 0, 8);

#ifdef LINUX
	flags = fcntl(udptr->sfd, F_GETFL, 0);
	fcntl(udptr->sfd, F_SETFL, flags|O_NONBLOCK);
#else	
	ioctlsocket(udptr->sfd, FIONBIO, &flags);
#endif

	printf("Not supported UDP (Client) for AIM.\n");	
	return 1;
} //initUdpClient(...

int initUdpServer(tEth *udptr)
{
	int flags = 1;

// printf("1. initUdpServer: port_num: %d\n\r", udptr->port_num);

	if(-1==(udptr->sfd = socket(AF_INET, SOCK_DGRAM, 0)))
	{
#ifdef LINUX
		perror("socket");
		errno = 0;
#else
		printf("socket");
#endif
		return 0;
	}
// printf("2. UdpServer, %d\n\r", udptr->sfd);
	udptr->my_addr.sin_family = AF_INET;
	udptr->my_addr.sin_port   = htons((short)udptr->port_num);

	if('\0' != udptr->cip[0])
		udptr->my_addr.sin_addr.s_addr = inet_addr(udptr->cip);
	else
		udptr->my_addr.sin_addr.s_addr = INADDR_ANY;
		
	// bzero(&(udptr->my_addr.sin_zero), 8);
	memset(&(udptr->my_addr.sin_zero), 0, 8);

	setsockopt(udptr->sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&flags, sizeof(flags));
	setsockopt(udptr->sfd,SOL_SOCKET,SO_KEEPALIVE,(char *)&flags, sizeof(flags));

// printf("3. UdpServer, %d cip[0]:%02X\n\r", udptr->sfd, udptr->cip[0]);
	if(-1 == bind(udptr->sfd,(struct sockaddr *)&(udptr->my_addr), sizeof(struct sockaddr)))
	{
#ifdef LINUX
		perror("bind");
		errno = 0;
		close(udptr->sfd);
#else
		printf("bind");
		closesocket(udptr->sfd);
#endif
		return 0;
	}

// printf("4. UdpServer, bind\n\r");
#ifdef LINUX
	flags = fcntl(udptr->sfd, F_SETFL, 0);
	fcntl(udptr->sfd, F_SETFL, flags|O_NONBLOCK);
#else
	ioctlsocket(udptr->sfd, FIONBIO, &flags);
#endif

#ifdef AIM	
	//printf("Not supported UDP (Server) for AIM.\n");	
#endif
	return 1;
} // initUdpServer(tUdp....

int	initTcpClient(tEth *tcptr) //Client Side 
{
#ifdef LINUX
	struct 	hostent *he;
	int		retry, flags, connflag;

	if((he=gethostbyname(tcptr->ip))==NULL)
	{
		herror("gethostbyname");
		return 0;
	}

printf("gethostbyname(%s) port_num: %d\n\r", tcptr->ip, tcptr->port_num);

	if((tcptr->sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		errno = 0;
		return 0;
	}

printf("socket tcptr->sfd %d\n\r", tcptr->sfd);

	tcptr->their_addr.sin_family = AF_INET;
	tcptr->their_addr.sin_port   = htons(tcptr->port_num);
	tcptr->their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
	// bzero(&(tcptr->their_addr.sin_zero), 8);
	memset(&(tcptr->their_addr.sin_zero), 0, 8);

//	flags = fcntl(tcptr->sfd, F_GETFL, 0);
//	fcntl(tcptr->sfd, F_SETFL, flags|O_NONBLOCK);
	
	// retry = tcptr->retry_count;
	retry = 1000;

	while(0 < retry--)
	{
		if(0<=(connflag=connect(	tcptr->sfd,
									(struct sockaddr *)&tcptr->their_addr,
									sizeof(struct sockaddr))))
		{
			printf("connect");
			// close(tcptr->sfd);
			break;
		}
		usleep(10000L);
	} //while(0 < retry--
printf("connected connflag = %d retry=%d\n\r", connflag, retry);
	if(0 > connflag)
	{
		// printf("connflag is %d\n\r", connflag);
#ifdef LINUX
		perror("connect");
		errno = 0;
		close(tcptr->sfd);
#else
		closesocket(tcptr->sfd);
#endif
		tcptr->sfd = -1;
		return 0;
	} // if( tcptr->sfd...

	if(0 <= connflag && 0 <= tcptr->sfd)
	{
		flags = fcntl(tcptr->sfd, F_GETFL, 0);
		fcntl(tcptr->sfd, F_SETFL, flags|O_NONBLOCK);
	}
#endif


#ifdef AIM	
     SYS_SOCKET socket_id;
	 char	service[10] = {0};

	 sys_tcpez_non_blocking_mode();

	 sprintf(service, "%d", tcptr->port_num );
     socket_id = sys_tcpez_connect (tcptr->ip, &service[0]);

     if (ISBADSOCKET(socket_id))
	 {
		printf("BAD socket %d\n\r", socket_id);
		exit (1);
	 }

	 tcptr->sfd = socket_id;
#endif
	return 1;
} // void	initTcpClient(tEth *tcptr) : Client Side 

int connectHost(tEth *tcptr,int ssz, int sinsz)
{
#ifdef LINUX	
	int tmp_retry_count;
	char *host_ip;
	char buf[20];

printf("bind..... %d\n\r", tcptr->tmp_fd);
	if(-1==bind(tcptr->tmp_fd,(struct sockaddr *)&tcptr->my_addr,ssz))
	{
		// perror("bind");
		printf("Error: bind. errno:%d\n\r", errno);
		close(tcptr->tmp_fd);
		// closesocket(tcptr->tmp_fd);
		return 0;
	}
printf("bind OK..listen.....\n\r");
	if(-1 == listen(tcptr->tmp_fd, 1))
	{
		perror("listen");
		printf("connectHost: Error...\n\r");
		return 0; // return FAIL_1;
	}

printf("listen OK..accept.....\n\r");
	// fc_flag = fcntl(tcptr->tmp_fd, F_GETFL, 0);
	// fcntl(tcptr->tmp_fd, F_SETFL, fc_flag|O_NONBLOCK);

	tmp_retry_count = 100;
	while(0 < tmp_retry_count--)
	{
		if(-1!=(tcptr->sfd=accept(tcptr->tmp_fd,(struct sockaddr *)&tcptr->their_addr,&sinsz)))
		{
			host_ip = (char *)inet_ntop(AF_INET, &tcptr->their_addr.sin_addr, buf, sizeof(buf));
			sprintf(tcptr->connip, "%s", host_ip);
			printf("---> CLIENT IP is %s\n\r", host_ip);
			break;
			// perror("accept");
			// close(tcptr->tmp_fd);
			// return 0;
		}
		usleep(1000L);
	} //while( tmp_retry_count...

	close(tcptr->tmp_fd);
	// closesocket(tcptr->tmp_fd);
	tcptr->tmp_fd = -1;

	if(-1 == tcptr->sfd)
	{
		return 0; 
	} else {
		if(0 != tcptr->cip[0])
		{
			printf("host_addr=%x, host_ip=%x\n\r",
					tcptr->their_addr.sin_addr.s_addr, inet_addr(tcptr->cip));
			host_ip = (char *)inet_ntop(AF_INET, &tcptr->their_addr.sin_addr, buf, sizeof(buf));

			if(tcptr->their_addr.sin_addr.s_addr != inet_addr(tcptr->ip))
			{
				printf("Req Host(%s):my_ip %s, host_ip %s ---> connection FAIL!!\n\r",
						host_ip, tcptr->ip, tcptr->cip);
				close(tcptr->sfd);
				// closesocket(tcptr->sfd);
				tcptr->sfd = -1;
				return 0;
			}
			printf("Req Host(%s):my_ip %s, host_ip %s ---> connection OK!!\n\r",
					host_ip, tcptr->ip, tcptr->cip);
			errno = 0;
		} // if(0!=tcptr->cip...
	} // if(-1 == tcptr->sfd
printf("connectHost OK.......%d\n\r", tcptr->sfd);
#endif
	return 1;
} // int connectHost(tEth *tcptr,int ssz, int sinsz

int 	initTcpServer(tEth *tcptr)
{
#ifdef LINUX	
	int	sin_sz, s_sz;
	int io_flags = 1;

	sin_sz 	= sizeof(struct sockaddr_in);
	s_sz 	= sizeof(struct sockaddr);

	printf("initTcpServer: %d\n\r", tcptr->port_num);

	if(0 < tcptr->sfd)
	{
		printf("sock disconnected.. fd:%d\n\r", tcptr->sfd);
		close(tcptr->sfd);
		// closesocket(tcptr->sfd);
		tcptr->sfd = -1;
	}
	// if(0 > (tcptr->tmp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	if(-1 == (tcptr->tmp_fd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("socket");
		return 0; // return FAIL_1;
	}

	tcptr->my_addr.sin_family = AF_INET;
	tcptr->my_addr.sin_port   = htons(tcptr->port_num);

	if( tcptr->cip[0] != '\0' )
		tcptr->my_addr.sin_addr.s_addr = inet_addr(tcptr->cip);
	else
		tcptr->my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		
	// bzero(&(tcptr->my_addr.sin_zero), 8);
	memset(&(tcptr->my_addr.sin_zero), 0, 8);
	setsockopt(tcptr->tmp_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&io_flags, sizeof(io_flags));
	setsockopt(tcptr->tmp_fd,SOL_SOCKET,SO_KEEPALIVE,(char *)&io_flags, sizeof(io_flags));

	if(-1 == connectHost(tcptr, s_sz, sin_sz))	
	{
		printf("sock: no connection..\n\r");
		return 0;
	} else {
		io_flags = fcntl(tcptr->sfd, F_GETFL, 0);
		fcntl(tcptr->sfd, F_SETFL, io_flags|O_NONBLOCK);
	}
#endif

#ifdef AIM
char		name[10] = {0}; 
SYS_SOCKET 	service;
SYS_SOCKET 	connection;

	sys_tcpez_non_blocking_mode();

	sprintf(name,"%d", tcptr->port_num);
    service = sys_tcpez_open_service (name);

    if (ISBADSOCKET(service))
	{
		printf("BAD socket service %s\n\r", name);
		exit (1);
	}

    connection = sys_tcpez_accept (service);

    if (ISBADSOCKET(connection))
	{
		printf("BAD socket connection %d\n\r", connection);
		exit (1);
	}

	tcptr->sfd = connection;
#endif

	return 1;
} // int 	initTcpServer(tEth *tcptr)

int initEth(tEth *ethptr)
{
	int	etsz, ret = 0;
	unsigned char sbuf[5] = {0,0,0,0,0};

	// if(0 < ethptr->sfd)
	// {
	//	close(ethptr->sfd);
	//	usleep(250000L); //250ms
	// }

	switch(ethptr->tusc)
	{
		case TCP_SERVER:	//0x00
			printf("initTcpServer\n\r");
			ret = initTcpServer(ethptr);
			break;
		case TCP_CLIENT:	//0x01
			printf("initTcpClient\n\r");
			ret = initTcpClient(ethptr);
			break;
		case UDP_SERVER:	//0x10
			printf("initUdpServer\n\r");
			ret = initUdpServer(ethptr);
			break;
		case UDP_CLIENT:	//0x11
			printf("initUdpClient\n\r");
			ret = initUdpClient(ethptr);
			break;
		default:
			printf("Error in TCP/UDP SERVER/CLIENT.\n\r");
			break;
			return ret;
	} //switch(peth->tusc

	if(0 < ethptr->sfd)
	{
		ethptr->seq 		= 0;
		ethptr->comm_start 	= 0;
		ethptr->no_comm_cnt = 0;	

		if(UDP_CLIENT == ethptr->tusc || TCP_CLIENT == ethptr->tusc)
		{
			sprintf(sbuf, "xy");
			etsz = writeEth(ethptr, sbuf, strlen(sbuf)); 

			if(0 < etsz)
				printf("Init Writing OK.. %d: sfd=%d sendto %s 0x%02X, %d\n\r",
						etsz, ethptr->sfd, sbuf, ethptr->tusc, ethptr->port_num);
			else
				printf("Init Writing FAIL %d: sfd=%d sendto %s 0x%02X, %d\n\r",
						etsz, ethptr->sfd, sbuf, ethptr->tusc, ethptr->port_num);

			usleep(500000L);
		} // if( client...
	} // if(0 < ethptr->sfd)

	return ret;
} // void initEth

int readUdp(tEth *udptr,byte *bf,int rdsz)
{
#ifdef LINUX
	int 	rcv_sz = FAIL_1;
	int		addr_len = 0;

	if(0 >= udptr->sfd) 
		return rcv_sz; 

	addr_len = sizeof(struct sockaddr);
	
	if( (rcv_sz = recvfrom( udptr->sfd, bf, rdsz, 0,
	 			(struct sockaddr *)&udptr->their_addr,
				&addr_len)) > 0)
	{
		udptr->comm_start  = 1; //set flag
		udptr->no_comm_cnt = 0; //reset counter

		// printf("readUdp() rcv_sz:%d %c%c%c %d\n\r",
		//		rcv_sz, bf[0], bf[1], bf[2], (int)bf[3]);
		// bf[rcv_sz] = 0;
		// showNData("RCV<- ", bf, rcv_sz, 20, HEX);
		// printf("RCV(%s): %s\n\r", 
		//	inet_ntoa(udptr->their_addr.sin_addr), bf);
	} else {
		if( udptr->comm_start )
			udptr->no_comm_cnt = (udptr->no_comm_cnt+1)%(udptr->timeout_cnt+1);
	}

	return rcv_sz;
#else
	return 0;
#endif
} //int readUdp(...

int readTcp(tEth *tcptr,byte *bf,int rdsz)
{
	int 	rcv_sz = FAIL_1;
	int		addr_len = 0;

	if(0 >= tcptr->sfd)
		return rcv_sz;

	addr_len = sizeof(struct sockaddr);
	
	if(0 < (rcv_sz = recv(tcptr->sfd, bf, rdsz, 0)))
	{
		tcptr->comm_start  = 1; //set flag
		tcptr->no_comm_cnt = 0; //reset counter

		// printf("readTcp() rcv_sz:%d %c%c%c %d\n\r",
		//		rcv_sz, bf[0], bf[1], bf[2], (int)bf[3]);
		bf[rcv_sz] = 0;
		// showNData("RCV<- ", bf, rcv_sz, 20, HEX);
		// printf("RCV(%s): %s\n\r", 
		//	inet_ntoa(udptr->their_addr.sin_addr), bf);
	} else {

#ifdef LINUX	
		if(EAGAIN == errno)
			errno = 0;
#endif

		bf[0] = 0;

		if( tcptr->comm_start )
			tcptr->no_comm_cnt = (tcptr->no_comm_cnt+1)%(tcptr->timeout_cnt+1);
	}

	return rcv_sz;
} // int readTcp

int writeUdp(tEth *udptr,byte *bf,int wsz)
{
	int 	snd_sz = FAIL_1;
	int		addr_len = 0;

// printf("writeUdp %d wsz %d\n\r", udptr->sfd, wsz);

	if(0 >= udptr->sfd) 
		return snd_sz; 

	addr_len = sizeof(struct sockaddr);

	if(0 < (snd_sz = sendto(udptr->sfd, bf, wsz, 0,
		 (struct sockaddr *)&udptr->their_addr,
		  addr_len)))
	{
		bf[wsz] = '\0';
	  	printf("UDP SND: %s\n\r", bf);
	 	// showNData("SND-> ", bf, snd_sz,20, HEX);
	} else {
#ifdef LINUX
		if(EAGAIN == errno)
			errno = 0;
#endif
	}

	return snd_sz;
} // int writeUdp(tUdp &udptr,byte *bf,int wszU

int writeTcp(tEth *tcptr,byte *bf,int wsz)
{
	int 	snd_sz = FAIL_1;

	if(0 >= tcptr->sfd) 
		return snd_sz; 

	if(0 < (snd_sz = send(tcptr->sfd, bf, wsz, 0)))
	{
		bf[wsz] = '\0';
	 	// showNData("SND-> ", bf, snd_sz,20, HEX);
	} else {
#ifdef LINUX
		if(EAGAIN == errno)
			errno = 0;
#endif

#ifdef AIM
        int error = sys_socket_error();

        if ((error != SYS_EWOULDBLOCK) && (error != SYS_EINPROGRESS))
        {
            sys_printlog(SYS_LOG_ERROR, "send() failed: error %d", error);
        }	
#endif
	}

	return snd_sz;
} // int writeTcp(tEth &tcptr,byte *bf,int wszU

int readEth(tEth *ethptr, byte *bf, int rsz)
{
	int retrsz = -1;

	if((0 == ethptr->comm_start)&&(UDP_CLIENT==ethptr->tusc || TCP_CLIENT==ethptr->tusc))
	{
		printf("1.readEth comm_start %d, %s (Client cannot read wo connection)\n\r",
					ethptr->comm_start, (ethptr->tusc & 0x01) ? "CLIENT":"SERVER");
		return retrsz;
	}
								// 0x00, 0x01, 0x10, 0x11
	if(0==(ethptr->tusc&0x10)) // TCP_SERVER,TCP_CLIENT,UDP_SERVER,UDP_CLIENT 
		retrsz = readTcp(ethptr,bf,rsz);
	else if(0x10==(ethptr->tusc&0x10))
		retrsz = readUdp(ethptr,bf,rsz);
	else
		;

	if((0 < retrsz) && (0 == (ethptr->tusc & 0x01)))
	{
		// printf("2. Connected readEth retrsz %d, %s\n\r", retrsz, bf);
		ethptr->comm_start = 1;
	}

	return retrsz;
} // int readEth

int writeEth(tEth *ethptr, byte *bf, int wsz)
{
	int retwsz = -1;

	if(0 == (ethptr->tusc & 0x01) && (0==ethptr->comm_start))	//Server
	{
		printf("writeEth.......returns FAIL because Server comm_start is %d\n\r",
																	ethptr->comm_start);	
		return retwsz;
	}

	if(0==(ethptr->tusc & 0x10)) // TCP_SERVER,TCP_CLIENT,UDP_SERVER,UDP_CLIENT 
	{
		retwsz = writeTcp(ethptr,bf,wsz);
	} else if(0x10 == (ethptr->tusc & 0x10))
	{
		retwsz = writeUdp(ethptr,bf,wsz);
	} else {
		printf("Out of Range in writeEth\n\r");
	}

	// printf("writeEth: Client? %02X comm_start %d\n\r", ethptr->tusc & 0x01, ethptr->comm_start); 

	if((0 < retwsz) && (1== (ethptr->tusc & 0x01))) // If Client, set comm_start to 1
		ethptr->comm_start = 1;

	return retwsz;
} // int readEth

int closeEth(tEth *peth)
{
#ifdef LINUX
	close(peth->sfd);
#endif
#ifdef NT
	closesocket(peth->sfd);
#endif
	peth->sfd = -1;

	return -1;
} // int closeEth(tEth *peth
/************* End line of eth.c ************/ 
