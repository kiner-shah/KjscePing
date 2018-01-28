/*
 * Author: Kiner Shah
 * This program will simulate ping command only for two options i.e. -c and -t.
 * Ping syntax assumed: kjsceping <option> <value> <Destination address>
 * References: http://ws.edu.isoc.org/materials/src/ping.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <unistd.h>
#include <time.h> 
#include <signal.h>

char srca[16];
char dsta[16];

unsigned short in_cksum(unsigned short *addr, int len)  //taken from http://ws.edu.isoc.org/materials/src/ping.c
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);       /* add high 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}

char* getip() {
    char buffer[256];
    struct hostent* h;
     
    gethostname(buffer, 256); //store the hostname in the defined buffer
    h = gethostbyname(buffer); //store the name of the host as host entity
     
    return inet_ntoa(*(struct in_addr *)h->h_addr); //return the actual ASCII string specifying the 8 bit IP address.
     
}

void finish(int sent, int received, float tottime) {
	int i;
	if(tottime == 0) { //in case of destination can't be reached.
		sent = 1;
		received = 0;
		tottime = 0;
	}
	printf("================================================================\nPing statistics\n");
	printf("Transmitted: %d, Received: %d, Packet loss: %d%, Time: %fms\n",sent,received,((sent - received)*100)/sent,tottime);
}
int main(int argc, char **argv) {
    //srca = (char*)calloc(15,sizeof(char));
    //dsta = (char*)calloc(15,sizeof(char));
    struct iphdr* ip; //IP packet
    struct iphdr* ip_reply; //IP reply packet
    struct icmphdr* icmp; //ICMP header packet
    struct sockaddr_in connection; //Holds information about Socket address, port, etc.
    char* packet; //Packet
    char* buffer; //Buffer for temporary storage
    int sockfd; //Socket File Descriptor
    int optval; //Optional Value
    int addrlen; //Address Length
    int c = 0,t = 255,lim,cflag = 0;
    if (getuid() != 0) { //If user ID of calling process is not 0.
        fprintf(stderr, "%s: root privelidges needed\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //printf("HELLO\n");
    //Parse the command line arguments
    if(argc == 1) {
        fprintf(stderr, "\nUsage: pinger [destination] <-s [source]>\n");
        fprintf(stderr, "Destination must be provided\n");
        exit(EXIT_FAILURE);
    }
    else if(argc == 4 && strcmp(argv[1],"-c") == 0) { //if no. of packets to be sent are specified.
        lim = atoi(argv[2]); //Store the value of count
        cflag = 1;
    }
    else if(argc == 4 && strcmp(argv[1],"-t") == 0) { //if TTL field of IP header is specified.
        t = atoi(argv[2]); //Store the value of TTL
    }
    else if(argc > 4 && strcmp(argv[1],"-t") == 0) { //if of type <option1> <value1> <option2> <value2>
        t = atoi(argv[2]);
        lim = atoi(argv[4]);
        cflag = 1;   
    }
    else if(argc > 4 && strcmp(argv[1],"-c") == 0) { //if of type <option1> <value1> <option2> <value2>
        t = atoi(argv[4]);
        lim = atoi(argv[2]);
        cflag = 1;   
    }
    strcpy(dsta,argv[argc - 1]);
    //srca = getip();
    printf("Pinging\n");
    //printf("Source Address: %s\tDestination address: %s\n",getip(),dsta);
    int sent = 0,received = 0;
    float tottime = 0.0;
    for(;;) {
        // Allocate memory
        ip = malloc(sizeof(struct iphdr));
        ip_reply = malloc(sizeof(struct iphdr));
        icmp = malloc(sizeof(struct icmphdr));
        packet = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
        buffer = malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));

        ip = (struct iphdr*) packet;
        icmp = (struct icmphdr*) (packet + sizeof(struct iphdr));
        // Fill IP header
        ip->ihl = 5; 
        ip->version = 4;
        ip->tos = 0; //for Best Effort service
        ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
        ip->id = htons(random()); //The htons() function converts the unsigned short integer hostshort from host byte order to network byte order
        ip->ttl = t;
        ip->protocol = IPPROTO_ICMP; //Upper layer protocol used.
        ip->saddr = inet_addr(srca);
        ip->daddr = inet_addr(dsta);
        ip->check = in_cksum((unsigned short *)ip, sizeof(struct iphdr));
        sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //define the socket file descriptor with address family as INET, and upper layer protocol as ICMP
        if(sockfd < 0) { //if socket can't be defined, throw an error
            perror("ping: socket");
            exit(5);
        }
        setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int)); //set the option with the value specified by optval at IP level, with the IP header included
        // Fill ICMP header
        icmp->type = ICMP_ECHO; //Set the type of ICMP packet as ICMP_ECHO
        icmp->code = 0; //Since the type is ICMP_ECHO, code is 0
        icmp->un.echo.id = 0;
        icmp->un.echo.sequence = 0;
        icmp->checksum = in_cksum((unsigned short *)icmp, sizeof(struct icmphdr));
	      sleep(1);
        connection.sin_family = AF_INET; //Address format is host and port no.
        connection.sin_addr.s_addr = inet_addr(dsta);
        int isconnected = connect(sockfd,(struct sockaddr*)&connection, sizeof(connection)); //try connecting to the destination
        if(isconnected < 0) { //if coudn't connect
            printf("\n Error : Connect Failed \n");
            return 1;
        }
        int sentbytes = sendto(sockfd,packet,ip->tot_len,0,(struct sockaddr*)&connection, sizeof(struct sockaddr)); //send the packet and store the length of packet sent.
        sent++;
        clock_t start = clock(), diff; //start the timer
        printf("Sent %d bytes packet to %s, TTL = %d\n", sentbytes,dsta,ip->ttl);
	      //sleep(1);
        addrlen = sizeof(connection);
	      signal(SIGALRM,finish); //define a signal
        alarm(10); //after 10 seconds, if nothing happens, call the finish procedure.
        int reclength = recvfrom(sockfd,buffer, sizeof(struct iphdr) + sizeof(struct icmphdr),0,(struct sockaddr *)&connection, &addrlen);
        diff = clock() - start; //stop the timer
        float ms = (float)(diff * 1000) / (1000000.0F);
        if(reclength < 0) {
            perror("recv");
            if(cflag) c++;
            continue;
        }
        printf("Received %d bytes reply from %s,",reclength,dsta);
	      received++;
        ip_reply = (struct iphdr*)buffer;
        printf("TTL = %d, Time = %fms\n",ip_reply->ttl,ms);
	      tottime += ms;
        if(cflag && c == lim - 1) break; //In case of -c as option
        else if(cflag) c++;
    }
    finish(sent,received,tottime);
    fflush(stdout);
    //printf("================================================================\nPing statistics\n");
    //printf("Transmitted: %d, Received: %d, Packet loss: %d%, Time: %fs\n",sent,received,((sent - received)*100)/sent,tottime);
    close(sockfd);
    return 0;
}
