/* import_test.c
 *
 * Test the Save RSS item audio API Library
 *
 * (C) Copyright 2015 Todd Baker <bakert@rfa.org>             
 * (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>
#include <rivwebcapi/rd_savepodcast.h>

int main(int argc,char *argv[])
{

  int i;
  char buf[BUFSIZ];
  char *p;
  long int cast_id=0;
  char filename[BUFSIZ];
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

  printf("Please enter the Podcast ID Number that you want to save to  ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cast_id = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the File Name that you want to save ==> ");
  if (fgets(filename,sizeof(filename),stdin) != NULL)
  {
    if((0xFF&filename[strlen(filename)-1])<32) {
      filename[strlen(filename)-1]=0;
    }
  } 

  // Add the Rivendell-C-API Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //
  int result= RD_SavePodcast(host,
			     user,
			     passwd,
			     ticket,
			     (unsigned)cast_id,
			     filename,
			     user_agent);
  if(result<0) {
    fprintf(stderr,"Something went wrong! Result Code = %d\n",result);
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
    case 404:
      fprintf(stderr,"ERROR: no such podcast item \n");
      break;

    default:
      fprintf(stderr, "Unknown Error occurred ==> %d",result);
      break;
    }
    exit(256);
  }

  //
  // List the results
  //
  printf(" Podcast item: %ld was successfully saved!\n",cast_id);


// Add test of create_ticket function 
    
    struct rd_ticketinfo *myticket=0;
    unsigned numrecs=0;

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

  result=RD_SavePodcast(host,
			user,
			passwd,
			ticket,
			(unsigned)cast_id,
			filename,
			user_agent);

  if(result<0) {
    fprintf(stderr,"Something went wrong! Result Code = %d\n",result);
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
    case 404:
      fprintf(stderr,"ERROR: no such podcast item \n");
      break;

    default:
      fprintf(stderr, "Unknown Error occurred ==> %d",result);
      break;
    }
    exit(256);
  }

  //
  // List the results
  //
  printf(" Podcast item: %ld was successfully saved!\n",cast_id);

  exit(0);
}
