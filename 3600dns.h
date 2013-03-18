/*
 * CS3600, Spring 2013
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__

typedef header_s{
 int ID:16;
 int QR:1;
 int OPCODE:4;
 int AA:1;
 int TC:1;
 int RD:1;
 int RA:1;
 int Z:3;
 int RCODE:4;
 unsigned int QDCOUNT:16;
 unsigned int ANCOUNT:16;
 unsigned int NSCOUNT:16;
 unsigned int ARCOUNT:16;
} header;

typedef lname_s{
  int length:8;
  char* name;  
} lname;

typedef question_s{
  lname[] QNAME;
  int QTYPE:16;
  int QCLASS:16;
} question;

typedef answer_s{
  lname[] NAME;
  int TYPE:16;
  int CLASS:16;
  int TTL:16;
  int RDLENGTH:16;
  // char* RDATA:16; FIXME: Might be 2D Array? Dunno.
} 

#endif

