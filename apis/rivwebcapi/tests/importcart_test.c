/* import_test.c
 *
 * Test the Impourt Cart/Cut API Library
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

#include <rivwebcapi/rd_import.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{

  int i;
  char buf[BUFSIZ];
  char *p;
  long int cartnum=0;
  long int cutnum=0;
  int create_flag=0;
  char filename[BUFSIZ];
  char group_name[BUFSIZ]="";
  char *host;
  char *user;
  char *passwd;
  char ticket[41]="";
  char title[BUFSIZ]="";
  struct rd_cartimport *cartimport=0;
  unsigned numrecs;
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

  printf("Please enter the Cart Number that you want to Import To  ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cartnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the Cut Number that you want to import To ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    cutnum = strtol(buf, &p,10);

    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
	exit(0);
    }
  } 
  printf("Please enter the File Name that you want to Import ==> ");
  if (fgets(filename,sizeof(filename),stdin) != NULL)
  {
  } 
  
  printf("Do you want to Create Cart If Doesn't Exist - (Yes or No) ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    if (( buf[0] != '\n') && (buf[0] =='Y'))
    {
      create_flag = 1;
      printf("Please enter the Group Name ==> ");
      if (fgets(group_name,sizeof(group_name),stdin) != NULL)
      {
      }
      printf("Optional Title Field (Checks Duplicate titles)===> ");
      if (fgets(title,sizeof(title),stdin) != NULL)
      {
      }
    }
  }

  // Add the Rivendell-C-API Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //
  
  int result= RD_ImportCart(&cartimport,
                host,
		user,
		passwd,
		ticket,
		(unsigned)cartnum,
		(unsigned)cutnum,
		(unsigned)2,
		0,
		0,
		0,
		create_flag,
		group_name,
		title,
                filename,
                user_agent,
		&numrecs);

  if(result<0) {
    fprintf(stderr,"Something went wrong! Result Code = %d\n",result);
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
      case 404:
        fprintf(stderr,"ERROR:  %s \n",cartimport->error_string);
        break;
      case  401:
        fprintf(stderr, "ERROR:  Unauthorized Or Cart out of Range! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }

  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("Cart Number: %u\n",cartimport[i].cart_number);
    printf("Cut Number:  %u\n",cartimport[i].cut_number);
    printf("\n");
    printf(" Cart: %u  -  Cut: %u  - Filename: %s was successfully imported!\n",cartimport[i].cart_number,
                                cartimport[i].cut_number,
                                filename);
    printf("\n");
  }

  //
  // Free the cart import info when finished with it
  //
  free(cartimport);



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

  result= RD_ImportCart(&cartimport,
                host,
		user,
		passwd,
		ticket,
		(unsigned)cartnum,
		(unsigned)cutnum,
		(unsigned)2,
		0,
		0,
		0,
		create_flag,
		group_name,
		title,
                filename,
                user_agent,
		&numrecs);

  if(result<0) {
    fprintf(stderr,"Something went wrong! Result Code = %d\n",result);
    exit(256);
  }

  if ((result< 200 || result > 299) && 
       (result != 0))
  {
    switch(result) {
      case 404:
        fprintf(stderr,"ERROR:  %s \n",cartimport->error_string);
        //fprintf(stderr,"ERROR:  No Such Cart/Cut Exists! \n");
        break;
      case  401:
        fprintf(stderr, "ERROR:  Unauthorized Or Cart out of Range! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }

  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("Cart Number: %u\n",cartimport[i].cart_number);
    printf("Cut Number:  %u\n",cartimport[i].cut_number);
    printf("\n");
    printf(" Cart: %u  -  Cut: %u  - Filename: %s was successfully imported!\n",cartimport[i].cart_number,
                                cartimport[i].cut_number,
                                filename);
    printf("\n");
  }

  //
  // Free the cart import info when finished with it
  //
  free(cartimport);

  exit(0);
}
