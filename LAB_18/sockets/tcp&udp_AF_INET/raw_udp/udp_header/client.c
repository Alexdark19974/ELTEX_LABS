#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#define TRUE 1
#define SPORT 8888
#define DPORT 7777
#define BUFFER_SIZE 25
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

//struct udphdr 
//{
//	u_short	uh_sport;		/* source port */
//	u_short	uh_dport;		/* destination port */
//	short	uh_ulen;		/* udp length */
//	u_short	uh_sum;			/* udp checksum */
//};

void client(void); 

struct buf
{
    struct udphdr udp_header;
    char payload[10];
};

struct recv_buf
{
    char id_data[20];
    struct udphdr udp_header;
    char payload[10];
};


int main(void)
{
    client();

    exit(EXIT_SUCCESS);
}

void client(void)
{
    struct sockaddr_in servaddr;
    struct udphdr udp_header;
    int client_fd = 0;
    int status = 0;
    ssize_t nbytes = 0;
    socklen_t serv_addr_size = sizeof(struct sockaddr_in);
    char datagram[BUFFER_SIZE] = {0};
    char buffer[BUFFER_SIZE] = {0};
    uint16_t udp_header_size = 8;
    struct buf udp_buf;

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    memset(&udp_header, 0, sizeof(struct udphdr));

    /* 
     * Creating raw socket that will allow us to manually fill UDP header 
     * */

    client_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (-1 == client_fd) { handle_error("socket"); }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(DPORT); /* converting host byte order (little-endian) to network byte order (i.e big-endian) */

    strncpy(udp_buf.payload, "Hello", strlen("Hello"));
    udp_buf.payload[strlen("Hello")] = '\0';

    udp_buf.udp_header.uh_sport = htons(SPORT); /* SPORT -> source port (i.e. client) */
    udp_buf.udp_header.uh_dport = htons(DPORT); /* DPORT -> destination port (i.e server) */
    udp_buf.udp_header.uh_ulen = htons(udp_header_size + strlen(udp_buf.payload)); 

    /* 
     * 8 bytes is the length of UDP header (and of udphdr struct, too)
     * strlen(udp_buf.payload) is the length of the payload
     * Togther, they form the total length of UDP header
     * Calculating the length of the payload is NECESSARY
     * Otherwise, Surfshark will not consider the payload.
     * */

    udp_buf.udp_header.uh_sum = 0; /* For now, checksum is of no need */

    /* 
     * Sending UDP header( udp_buf struct, that is)
     * So that the counterpart can analyze it properly on transport layer via UDP protocol.
     * */

    nbytes = sendto(client_fd, &udp_buf, sizeof(udp_buf), MSG_CONFIRM, (struct sockaddr *) &servaddr, serv_addr_size);
    if (-1 == nbytes) { handle_error("sendto"); }

    while (TRUE)
    {
        /*
         * Creating an additional recv_buf struct so that it can incluse both UDP header and Network header (ip)
         * By default, the size of ip header is 20 bytes.
         * recvfrom() will capture not only UPD header, but also Network header
         * So an additional buffer for 20 bytes is necessary to distinguish between those 2 headers.
         * */

        struct recv_buf recv;

        nbytes = recvfrom(client_fd, &recv, sizeof(recv), MSG_WAITALL, (struct sockaddr *) &servaddr, &serv_addr_size);
        if (-1 == nbytes) { handle_error("sendto"); }

        unsigned short converted_port = ntohs(recv.udp_header.uh_dport); /* Convert network byte order to host byte order */

        if (SPORT != converted_port) /* if converted_port is not equal to source port of client, display and move on */ 
        {
            printf(" port %hu;\n", converted_port); /* Display any port that client actively recieves */
            sleep(1);
        }
        else
        {
            printf("port %hu and %s;\n", converted_port, recv.payload); /* if converted_port is equal to destination port, display payload */
            sleep(2);
            break;
        }
    }

    close(client_fd);
    if (-1 == client_fd) { handle_error("close"); }
}
