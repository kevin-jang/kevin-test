/*****
 *
 *  eth.h : 
 *	
 *   Kevin Jang		
 *   13 Feb 2007	
 *
 *****/
#ifndef	ETH_H	
#define	ETH_H	
#ifdef LINUX
#include <netinet/in.h>
#endif

#ifdef AIM
#include <sys_tcpez.h>
#endif

#include "./data_type.h"

//#include <netdb.h>

#define	TCP_SERVER 0x00
#define	TCP_CLIENT 0x01
#define	UDP_SERVER 0x10
#define	UDP_CLIENT 0x11 

typedef	struct {
	char 	ip[16];
	char 	cip[16]; //client ip if needed.
	char	connip[20]; // connected IP
	int		port_num;
	struct  sockaddr_in my_addr;
	struct  sockaddr_in their_addr;
	int		sfd;
	int		tmp_fd;
	int		tusc; //TcpUdpServerClient tcp_server=0,tcp_client,udp_server,udp_client
	UCHAR	seq;
	int	comm_start; //comm_start flag
	int	no_comm_cnt;
	int	timeout_cnt;
	int	retry_count;
	// int	timeout_sec;
}tEth;


extern void showEth(tEth *ethptr);

extern char StrTcpUdp[][10];
extern char StrSerCli[][10];

extern int initUdpClient(tEth *udptr);
extern int initUdpServer(tEth *udptr);

extern int initTcpClient(tEth *tcptr);
extern int initTcpServer(tEth *tcptr);

extern int initEth(tEth *ethptr);

extern int  readUdp(tEth *udptr,unsigned char *bf,int len);
extern int  writeUdp(tEth *udptr,unsigned char *bf,int len);

extern int  readTcp(tEth *tcptr,unsigned char *bf,int len);
extern int  writeTcp(tEth *tcptr,unsigned char *bf,int len);

extern int  readEth(tEth *ethptr,unsigned char *bf,int len);
extern int  writeEth(tEth *ethptr,unsigned char *bf,int len);

extern int  closeEth(tEth *peth);
#endif
/********** End of eth.h **************/
