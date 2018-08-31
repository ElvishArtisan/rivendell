/*audioinfo_test.c
 *
 * Test the audioinfo WEB library.
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>             
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rivwebcapi/rd_audioinfo.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{
  int i;
  struct rd_audioinfo *audioinfo=0;

  char buf[BUFSIZ];
  char *p;
  long int cart=0;
  long int cut=0;
  unsigned numrecs;
  char *host;
  char *user;
  char *passwd;
  char ticket[41]="";
  char user_agent[255]={0};

  /*      Get the Rivendell Host, User and Password if set in env */
  if (getenv("RIVHOST")!=NULL) {
    host = getenv("RIVHOST");
  }
  else {
    host="localhost";
  }

  if (getenv("RIVUSER")!=NULL) {
    user = getenv("RIVUSER");
  }
  else {
    user="USER";
  }

  if (getenv("RIVPASS")!=NULL) {
    passwd = getenv("RIVPASS");
  }
  else {
    passwd = "";
  } 

  printf("Please enter the Cart Number ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cart = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 

  printf("Please enter the Cut Number for the Cart used above ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cut = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 

  // Add the User Agent and Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //

  int result=RD_AudioInfo(&audioinfo,
		host,
		user,
		passwd,
		ticket,
		(unsigned)cart,
		(unsigned)cut,
                user_agent,
		&numrecs);

  if (result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    switch(result) {
      case 400:
         fprintf(stderr," ERROR: Missing Cart/Cut Number or Unknown Format\n");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart/Audio Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d\n",result);
    }
    exit(256);
  }
  //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("              Cart Number: %u\n",audioinfo[i].cart_number);
    printf("               Cut Number: %u\n",audioinfo[i].cut_number);
    printf("                   Format: %d\n",audioinfo[i].format);
    printf("                 Channels: %d\n",audioinfo[i].channels);
    printf("              Sample Rate: %d\n",audioinfo[i].samplerate);
    printf("                   Frames: %u\n",audioinfo[i].frames);
    printf("                   Length: %u\n",audioinfo[i].length);
    printf("\n");

  }

  //
  // Free the audioinfo list when finished with it
  //
  free(audioinfo);

// Add test of create_ticket function
    
    struct rd_ticketinfo *myticket=0;
    numrecs=0;

    result = RD_CreateTicket( &myticket,
            host,
            user,
            passwd,
            user_agent,
            &numrecs);

    if ((result< 200 || result > 299) &&
       (result != 0))
    {
        switch(result) {
            case 403:
            fprintf(stderr," ERROR: Invalid User Information During Create Ticket\n");
            break;
        default:
           fprintf(stderr, "Unknown Error occurred ==> %d\n",result);
        }
    exit(256);
    }

    //   We got a ticket created - use it and do the call again
    //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("          Ticket: %s\n",myticket[i].ticket);
    printf("Ticket Expire year value  = %d\n",myticket->tkt_expiration_datetime.tm_year);
    printf("Ticket Expire month value = %d\n",myticket->tkt_expiration_datetime.tm_mon);
    printf("Ticket Expire day value   = %d\n",myticket->tkt_expiration_datetime.tm_mday);
    printf("Ticket Expire wday value  = %d\n",myticket->tkt_expiration_datetime.tm_wday);
    printf("Ticket Expire hour value  = %d\n",myticket->tkt_expiration_datetime.tm_hour);
    printf("Ticket Expire min value   = %d\n",myticket->tkt_expiration_datetime.tm_min);
    printf("Ticket Expire sec value   = %d\n",myticket->tkt_expiration_datetime.tm_sec);
    printf("Ticket Expire isdst value = %d\n",myticket->tkt_expiration_datetime.tm_isdst);
    printf("\n");

  }

    user="";
    passwd="";
    strcpy( ticket,myticket->ticket);
    fprintf(stderr, "Ticket was copied - = %s\n",ticket);

  //
  // Call the function
  //

  result=RD_AudioInfo(&audioinfo,
		host,
		user,
		passwd,
		ticket,
		(unsigned)cart,
		(unsigned)cut,
                user_agent,
		&numrecs);

  if (result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    switch(result) {
      case 400:
         fprintf(stderr," ERROR: Missing Cart/Cut Number or Unknown Format\n");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart/Audio Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d\n",result);
    }
    exit(256);
  }
  //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("              Cart Number: %u\n",audioinfo[i].cart_number);
    printf("               Cut Number: %u\n",audioinfo[i].cut_number);
    printf("                   Format: %d\n",audioinfo[i].format);
    printf("                 Channels: %d\n",audioinfo[i].channels);
    printf("              Sample Rate: %d\n",audioinfo[i].samplerate);
    printf("                   Frames: %u\n",audioinfo[i].frames);
    printf("                   Length: %u\n",audioinfo[i].length);
    printf("\n");

  }
  //
  // Free the audioinfo list when finished with it
  //
  free(audioinfo);

  exit(0);
}
