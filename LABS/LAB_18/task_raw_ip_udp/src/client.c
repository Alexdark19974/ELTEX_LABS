#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#define TRUE 1
#define SPORT 8888
#define DPORT 7777
#define BUFFER_SIZE 25
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

void client(void); 

struct ip_header
{
    unsigned char iph_verlen;   // 4-bit IPv4 version of IP + 4 bits of ip header length
    unsigned char iph_tos;      // TOS bits control the priority, the quality of service 
    unsigned short iph_len;     // len: ip_h + udp_h + payload(data)
    unsigned short iph_ident;   // allows to reassemble IP datagrams. IP packets can be fragmented.
    unsigned short iph_offset;

    /* the first three bits are flags :
        0 bit: reserved, always zero;
        1 bit: 0 = may fragments, 1 = don't fragment;
        2 bit: 0 = last fragment, 1 = more fragments;

        offset intentifes the 8 byte offsets of blocks of data; 
        3 bits are fragment flags
        13 bits contains the number of 8-byte packets already sent.
    */

    unsigned char iph_ttl;

    /*
        Time to live: the number of passes before the packet is discarded, and an icmp error is returned;
        the counter decreases until it is 0. The maxiumum is 255; 
    */

    unsigned char iph_protocol; // transport layer protocol
    unsigned short iph_chksum;   // a checksum on the header only, the ip datagram;
    unsigned int iph_source;    // source ip address;
    unsigned int iph_dest;      // destination ip address;

};

struct buf
{
    struct ip_header ip_hdr;
    struct udphdr udp_header;
    char payload[BUFFER_SIZE];
};

struct recv_buf
{
    struct ip_header ip_hdr;
    struct udphdr udp_header;
    char payload[BUFFER_SIZE];
};


int main(void)
{
    client();

    exit(EXIT_SUCCESS);
}

void client(void)
{
    struct sockaddr_in servaddr;
    int client_fd = 0;
    int status = 0;
    ssize_t nbytes = 0;
    socklen_t serv_addr_size = sizeof(struct sockaddr_in);
    uint16_t udp_header_size = 8;
    struct buf udp_buf;
    int option = 1;

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    memset(&udp_buf, 0, sizeof(struct buf));
    /* 
     * Creating raw socket that will allow us to manually fill UDP header 
     * */

    client_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (-1 == client_fd) { handle_error("socket"); }

    status = setsockopt(client_fd, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option));
    if (-1 == status) { handle_error("setsockopt"); }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(DPORT); /* converting host byte order (little-endian) to network byte order (i.e big-endian) */

    strncpy(udp_buf.payload, "Hello", strlen("Hello"));
    udp_buf.payload[strlen("Hello")] = '\0';

    udp_buf.ip_hdr.iph_verlen = (4 << 4) | (sizeof (struct ip_header) / sizeof(unsigned int));

    /*
    * 4 is ipV4 and shifted to the left by 4 bits and divided by size of struct ip_header and unsigned long = 5
     * */

    udp_buf.ip_hdr.iph_len = (sizeof(struct ip_header) + sizeof(struct udphdr) + strlen(udp_buf.payload));
    udp_buf.ip_hdr.iph_tos = 0;
    udp_buf.ip_hdr.iph_ident = 0;
    udp_buf.ip_hdr.iph_offset = 0;
    udp_buf.ip_hdr.iph_ttl = 16;
    udp_buf.ip_hdr.iph_protocol = IPPROTO_UDP;
    udp_buf.ip_hdr.iph_chksum = 0;
    udp_buf.ip_hdr.iph_source = inet_addr("127.0.0.1");
    udp_buf.ip_hdr.iph_dest = inet_addr("127.0.0.1");

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
        unsigned short converted_checksum = ntohs(recv.ip_hdr.iph_chksum);
        char *converted_src_ip = inet_ntoa(servaddr.sin_addr);
     //   char *converted_dest_ip = inet_ntoa(recv.ip_hdr.iph_dest);
        if (SPORT != converted_port) /* if converted_port is not equal to source port of client, display and move on */ 
        {
            printf(" port %hu;\n", converted_port); /* Display any port that client actively recieves */
            sleep(1);
        }
        else
        {
            printf(" checksum: 0x%x;\n", converted_checksum);
            printf(" source ip address: %s\n", converted_src_ip);
          //  printf(" destination ip address: %s\n", converted_dest_ip);
            printf(" port %hu;\n payload: %s;\n", converted_port, recv.payload); /* if converted_port is equal to destination port, display payload */
            sleep(2);
            break;
        }
    }

    close(client_fd);
    if (-1 == client_fd) { handle_error("close"); }
}
