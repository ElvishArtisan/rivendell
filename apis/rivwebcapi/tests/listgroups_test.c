/* listgroups_test.c
 *
 * Test the listgroups library.
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

#include <rivwebcapi/rd_listgroups.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{
  int i;
  struct rd_group *grps=0;
  unsigned numrecs;
  char *host;
  char *user;
  char *passwd;
  char ticket[41]="";
  char user_agent[255];

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


  // Add the User Agent and Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //
  int result= RD_ListGroups(&grps,
		host,
		user,
		passwd,
		ticket,
                user_agent,
		&numrecs);
  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    fprintf(stderr, "Unknown Error occurred ==> %d",result);
    exit(256);
  }
  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("%s:\n",grps[i].grp_name);
    printf("        Description: %s\n",grps[i].grp_desc);
    printf("  Default cart type: %u\n",grps[i].grp_default_cart_type);
    printf("           Low Cart: %06u\n",grps[i].grp_lo_limit);
    printf("          High Cart: %06u\n",grps[i].grp_hi_limit);
    printf("         Shelf Life: %d\n",grps[i].grp_shelf_life);
    printf("      Default Title: %s\n",grps[i].grp_default_title);
    printf(" Enforce Cart Range: %u\n",grps[i].grp_enforce_range);
    printf("   Incl. in Traffic: %u\n",grps[i].grp_report_tfc);
    printf("     Incl. in Music: %u\n",grps[i].grp_report_mus);
    printf("    Send Now & Next: %u\n",grps[i].grp_now_next);
    printf("              Color: %s\n",grps[i].grp_color);
    printf("\n");
  }

  //
  // Free the group list when finished with it
  //
  free(grps);


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
  result= RD_ListGroups(&grps,
		host,
		user,
		passwd,
		ticket,
                user_agent,
		&numrecs);
  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    fprintf(stderr, "Unknown Error occurred ==> %d",result);
    exit(256);
  }
  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("%s:\n",grps[i].grp_name);
    printf("        Description: %s\n",grps[i].grp_desc);
    printf("  Default cart type: %u\n",grps[i].grp_default_cart_type);
    printf("           Low Cart: %06u\n",grps[i].grp_lo_limit);
    printf("          High Cart: %06u\n",grps[i].grp_hi_limit);
    printf("         Shelf Life: %d\n",grps[i].grp_shelf_life);
    printf("      Default Title: %s\n",grps[i].grp_default_title);
    printf(" Enforce Cart Range: %u\n",grps[i].grp_enforce_range);
    printf("   Incl. in Traffic: %u\n",grps[i].grp_report_tfc);
    printf("     Incl. in Music: %u\n",grps[i].grp_report_mus);
    printf("    Send Now & Next: %u\n",grps[i].grp_now_next);
    printf("              Color: %s\n",grps[i].grp_color);
    printf("\n");
  }

  //
  // Free the group list when finished with it
  //
  free(grps);

  exit(0);
}
