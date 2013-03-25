/*
 * CS3600, Spring 2013
 * Project 3 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "3600dns.h"

/**
 * This function will print a hex dump of the provided packet to the screen
 * to help facilitate debugging.  In your milestone and final submission, you 
 * MUST call dump_packet() with your packet right before calling sendto().  
 * You're welcome to use it at other times to help debug, but please comment those
 * out in your submissions.
 *
 * DO NOT MODIFY THIS FUNCTION
 *
 * data - The pointer to your packet buffer
 * size - The length of your packet
 */
static void dump_packet(unsigned char *data, int size) {
    unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4x",
               ((unsigned int)p-(unsigned int)data) );
        }
            
        c = *p;
        if (isprint(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) { 
            /* line completed */
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}

// Parses a url to format it for a packet ( 3www6google3com0 )
char* parse_url(char* url){
  int acc = 0;
  
  char* tempp = url;
  char* buf = malloc(sizeof(int));
  char buf2[1];

  char built_string[strlen(url) + 2];
  for(int i = 0; i < strlen(built_string); i++){
    built_string[i] = 0;
  }
 
  int flag = 0;
  while(flag == 0){
    if(*url == '.' || *url == '\0'){

      buf2[0] = acc;
      //sprintf(buf,"'%d'",acc);
      strcat(built_string,buf2);
      strncat(built_string, tempp, acc);

      acc = 0;
      tempp = url;
      tempp++;
      if(*url == '\0'){flag=1;}
    } else {
      acc++;
    }
    url++;
  }

  free(buf);
  buf2[0] = 0;
  strcat(built_string, buf2);
   

  return built_string;
}

int main(int argc, char *argv[]) {
  /**
   * I've included some basic code for opening a socket in C, sending
   * a UDP packet, and then receiving a response (or timeout).  You'll 
   * need to fill in many of the details, but this should be enough to
   * get you started.
   */

  // process the arguments

  // First, find the element of argv that matches the addresses' format. (Starts with an @)
  char* raw_address; // String representing the server's ip address (and optionally, port)
  char* raw_name;

  for(int i = 0; i < argc; i++){
    if(*argv[i] == '@'){
      raw_address = argv[i];
      raw_address++;
      raw_name = argv[i+1];
    }
  }

  // Define the data we will be parsing;
  char* server_ip;
  short server_port;  
  char* server_port_string;
  char* server_name;

  if(strchr(raw_address,':') != NULL){
    server_ip = strtok(raw_address,":");
    server_port_string = strtok(NULL," ");
    server_port = (short) strtol(server_port_string,NULL,0);
  } else {
    server_ip = strtok(raw_address," ");
    server_port = 53;
  }
/*
   printf("---- DEBUG ----\n");
   printf("Server IP: %s\n", server_ip);
   printf("Server Port: %d\n", server_port);
   printf("Parsed Name: %s\n", parse_url(raw_name));
*/
   // We have parsed our arguments. Next, we need to modify server_name
   // Into an acceptable format ( 3www6google3com0 )
   server_name = parse_url(raw_name);

   // construct the DNS request
   header h;
   h.ID = htons(1337);
   h.RD = 1;
   h.TC = 0;
   h.AA = 0;
   h.OPCODE = 0;
   h.QR = 0;
   h.RCODE = 0;
   h.RA = 0;
   h.Z = 0;
   h.QDCOUNT = htons(1);
   h.ANCOUNT = htons(0);
   h.NSCOUNT = htons(0);
   h.ARCOUNT = htons(0);

   question q;
   q.QTYPE = htons(1); // Will change if we do bonus (requesting mail/name servers)
   q.QCLASS = htons(1);

   // Create and zero out a buffer.
   char buffer[65536];
   memset(buffer,0,65536);


   // Add our header to the buffer.
   memcpy(&buffer,&h,sizeof(header));

   // Add our server name to the buffer.
   memcpy(&buffer[sizeof(header)],server_name,strlen(server_name));

   // Add our question to the buffer.
   memcpy(&buffer[sizeof(header)+strlen(server_name)+1],&q,sizeof(question));
  


   // send the DNS request (and call dump_packet with your request)
   dump_packet(buffer,sizeof(header)+strlen(server_name)+1+sizeof(question));
   
   // first, open a UDP socket  
   int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   // next, construct the destination address
   struct sockaddr_in out;
   out.sin_family = AF_INET;
   out.sin_port = htons(server_port);
   out.sin_addr.s_addr = inet_addr(server_ip);
   
   /*
     if (sendto(sock,<<your packet>>,<<packet length>>, 0, &out, sizeof(out)) < 0) {
     // an error occurred
     }
   */
   // wait for the DNS reply (timeout: 5 seconds)
   struct sockaddr_in in;
   socklen_t in_len;
   
   // construct the socket set
   fd_set socks;
   FD_ZERO(&socks);
   FD_SET(sock, &socks);
   
   // construct the timeout
   struct timeval t;
   t.tv_sec  = 5;
   t.tv_usec = 0;
   
   // wait to receive, or for a timeout
   /*
     if (select(sock + 1, &socks, NULL, NULL, &t)) {
     
     if (recvfrom(sock,<<your input buffer>>,<<input len>>, 0, &in, &in_len) < 0) {
     // an error occured
     }
     } else {
     // a timeout occurred
     }
   */
   // print out the result
   
   return 0;
}
