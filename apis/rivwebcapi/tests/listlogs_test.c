/* listlogs_test.c
 *
 * Test the listlog library.
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

#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_listlogs.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{
  int i;
  struct rd_log *logs=0;
  char buf[BUFSIZ];
  char *p;
  char svcname[11]={0};
  char logname[65]={0};
  long int trackable=0;
  unsigned numrecs;
  char *host;
  char *user;
  char *passwd;
  char ticket[41]="";
  char filter[255];
  long int recent=0;
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

  printf("Please enter the Service Name (default is All) ==> ");
  if(fgets(buf,sizeof(buf),stdin) != NULL)
  {
    strncpy(svcname,buf,10);
  } 
  fflush(stdin);
  printf("Please enter the Log Name (default is All) ==> ");
  if(fgets(buf,sizeof(buf),stdin) != NULL)
  {
    strncpy(logname,buf,64);
  } 
  fflush(stdin);
  printf("Please enter the Filter string (default is none) ==> ");
  if(fgets(buf,sizeof(buf),stdin) != NULL)
  {
    strncpy(filter,buf,255);
  } 
  fflush(stdin);
  printf("Please enter 1 if you want trackable logs ==>");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    trackable = strtol(buf,&p,10);
    if ( (buf[0] != '\n') &&
         ((*p != '\n') && (*p != '\0')))
    {
        fprintf(stderr," Illegal Characters detected! Exiting.\n");
        exit(0);
    }
  }
  printf("Please enter 1 if you want only recent logs ==>");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
    recent = strtol(buf,&p,10);
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
  //
  //
  // Call the function
  //
  int result= RD_ListLogs(&logs,
			  host,
			  user,
			  passwd,
			  ticket,
			  &svcname[0],
			  &logname[0],
			  (int)trackable,
			  filter,
			  (int)recent,
			  user_agent,
			  &numrecs); 

  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
    {
        switch(result) {
            case 403:
            fprintf(stderr," ERROR: Invalid User Information During ListLogs call\n");
            break;
        default:
           fprintf(stderr, "Unknown Error occurred ==> %d\n",result);
        }
    exit(256);
    }

  //
  // List the results
  //

  for(i=0;i<numrecs;i++) {
    printf("                   Log Name: %s\n",logs[i].log_name);
    printf("                Log Service: %s\n",logs[i].log_service);
    printf("            Log Description: %s\n",logs[i].log_description);
    printf("       Log Origin User Name: %s\n",logs[i].log_origin_username);
    printf("        Log Origin DateTime\n");
    printf("Log OriginDate  year value : %d\n",logs[i].log_origin_datetime.tm_year);
    printf("Log OriginDate  mon value  : %d\n",logs[i].log_origin_datetime.tm_mon);
    printf("Log OriginDate mday value  : %d\n",logs[i].log_origin_datetime.tm_mday);
    printf("Log OriginDate   day value : %d\n",logs[i].log_origin_datetime.tm_wday);
    printf("Log OriginDate  hour value : %d\n",logs[i].log_origin_datetime.tm_hour);
    printf("Log OriginDate  min value  : %d\n",logs[i].log_origin_datetime.tm_min);
    printf("Log OriginDate  sec value  : %d\n",logs[i].log_origin_datetime.tm_sec);
    printf("Log OriginDate  isdst      : %d\n",logs[i].log_origin_datetime.tm_isdst);
    printf("             Log Purge Date\n");
    printf("Log PurgeDate   year value : %d\n",logs[i].log_purge_date.tm_year);
    printf("Log PurgeDate   mon value  : %d\n",logs[i].log_purge_date.tm_mon);
    printf("Log PurgeDate  mday value  : %d\n",logs[i].log_purge_date.tm_mday);
    printf("Log PurgeDate    day value : %d\n",logs[i].log_purge_date.tm_wday);
    printf("Log PurgeDate   hour value : %d\n",logs[i].log_purge_date.tm_hour);
    printf("Log PurgeDate   min value  : %d\n",logs[i].log_purge_date.tm_min);
    printf("Log PurgeDate   sec value  : %d\n",logs[i].log_purge_date.tm_sec);
    printf("Log PurgeDate    isdst     : %d\n",logs[i].log_purge_date.tm_isdst);
    printf("         Log Link DateTime\n");
    printf("      Log Link  year value : %d\n",logs[i].log_link_datetime.tm_year);
    printf("      Log Link  mon value  : %d\n",logs[i].log_link_datetime.tm_mon);
    printf("      Log Link mday value  : %d\n",logs[i].log_link_datetime.tm_mday);
    printf("      Log Link   day value : %d\n",logs[i].log_link_datetime.tm_wday);
    printf("      Log Link  hour value : %d\n",logs[i].log_link_datetime.tm_hour);
    printf("      Log Link  min value  : %d\n",logs[i].log_link_datetime.tm_min);
    printf("      Log Link  sec value  : %d\n",logs[i].log_link_datetime.tm_sec);
    printf("      Log Link  isdst      : %d\n",logs[i].log_link_datetime.tm_isdst);
    printf("      Log Modified DateTime\n");
    printf("Log Modified    year value : %d\n",logs[i].log_modified_datetime.tm_year);
    printf("Log Modified    mon value  : %d\n",logs[i].log_modified_datetime.tm_mon);
    printf("Log Modified   mday value  : %d\n",logs[i].log_modified_datetime.tm_mday);
    printf("Log Modified     day value : %d\n",logs[i].log_modified_datetime.tm_wday);
    printf("Log Modified    hour value : %d\n",logs[i].log_modified_datetime.tm_hour);
    printf("Log Modified    min value  : %d\n",logs[i].log_modified_datetime.tm_min);
    printf("Log Modified    sec value  : %d\n",logs[i].log_modified_datetime.tm_sec);
    printf("Log Modified    isdst      : %d\n",logs[i].log_modified_datetime.tm_isdst);
    printf("            Log AutoRefresh: %d\n",logs[i].log_autorefresh);
    printf("             Log Start Date\n");
    printf("Log Start Date   year value: %d\n",logs[i].log_startdate.tm_year);
    printf("Log Start Date    mon value: %d\n",logs[i].log_startdate.tm_mon);
    printf("Log Start Date   mday value: %d\n",logs[i].log_startdate.tm_mday);
    printf("Log Start Date    day value: %d\n",logs[i].log_startdate.tm_wday);
    printf("Log Start Date   hour value: %d\n",logs[i].log_startdate.tm_hour);
    printf("Log Start Date   min value : %d\n",logs[i].log_startdate.tm_min);
    printf("Log Start Date   sec value : %d\n",logs[i].log_startdate.tm_sec);
    printf("Log Start Date   isdst     : %d\n",logs[i].log_startdate.tm_isdst);
    printf("               Log End Date\n");
    printf("Log End Date   year value  : %d\n",logs[i].log_enddate.tm_year);
    printf("Log End Date    mon value  : %d\n",logs[i].log_enddate.tm_mon);
    printf("Log End Date   mday value  : %d\n",logs[i].log_enddate.tm_mday);
    printf("Log End Date     day value : %d\n",logs[i].log_enddate.tm_wday);
    printf("Log End Date    hour value : %d\n",logs[i].log_enddate.tm_hour);
    printf("Log End Date    min value  : %d\n",logs[i].log_enddate.tm_min);
    printf("Log End Date    sec value  : %d\n",logs[i].log_enddate.tm_sec);
    printf("Log End Date    isdst      : %d\n",logs[i].log_enddate.tm_isdst);
    printf("       Log Scheduled Tracks: %d\n",logs[i].log_scheduled_tracks);
    printf("       Log Completed Tracks: %d\n",logs[i].log_completed_tracks);
    printf("            Log Music Links: %d\n",logs[i].log_music_links);
    printf("           Log Music Linked: %d\n",logs[i].log_music_linked);
    printf("          Log Traffic Links: %d\n",logs[i].log_traffic_links);
    printf("         Log Traffic Linked: %d\n",logs[i].log_traffic_linked);
    printf("\n");
  }

  //
  // Free the Log list when finished with it
  //
  free(logs);


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
  //
  //
  // Call the function
  //
  result= RD_ListLogs(&logs,
		      host,
		      user,
		      passwd,
		      ticket,
		      &svcname[0],
		      &logname[0],
		      (int)trackable,
		      filter,
		      recent,
		      user_agent,
		      &numrecs); 

  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  //
  // List the results
  //

  for(i=0;i<numrecs;i++) {
    printf("                   Log Name: %s\n",logs[i].log_name);
    printf("                Log Service: %s\n",logs[i].log_service);
    printf("            Log Description: %s\n",logs[i].log_description);
    printf("       Log Origin User Name: %s\n",logs[i].log_origin_username);
    printf("        Log Origin DateTime\n");
    printf("Log OriginDate  year value : %d\n",logs[i].log_origin_datetime.tm_year);
    printf("Log OriginDate  mon value  : %d\n",logs[i].log_origin_datetime.tm_mon);
    printf("Log OriginDate mday value  : %d\n",logs[i].log_origin_datetime.tm_mday);
    printf("Log OriginDate   day value : %d\n",logs[i].log_origin_datetime.tm_wday);
    printf("Log OriginDate  hour value : %d\n",logs[i].log_origin_datetime.tm_hour);
    printf("Log OriginDate  min value  : %d\n",logs[i].log_origin_datetime.tm_min);
    printf("Log OriginDate  sec value  : %d\n",logs[i].log_origin_datetime.tm_sec);
    printf("Log OriginDate  isdst      : %d\n",logs[i].log_origin_datetime.tm_isdst);
    printf("             Log Purge Date\n");
    printf("Log PurgeDate   year value : %d\n",logs[i].log_purge_date.tm_year);
    printf("Log PurgeDate   mon value  : %d\n",logs[i].log_purge_date.tm_mon);
    printf("Log PurgeDate  mday value  : %d\n",logs[i].log_purge_date.tm_mday);
    printf("Log PurgeDate    day value : %d\n",logs[i].log_purge_date.tm_wday);
    printf("Log PurgeDate   hour value : %d\n",logs[i].log_purge_date.tm_hour);
    printf("Log PurgeDate   min value  : %d\n",logs[i].log_purge_date.tm_min);
    printf("Log PurgeDate   sec value  : %d\n",logs[i].log_purge_date.tm_sec);
    printf("Log PurgeDate    isdst     : %d\n",logs[i].log_purge_date.tm_isdst);
    printf("         Log Link DateTime\n");
    printf("      Log Link  year value : %d\n",logs[i].log_link_datetime.tm_year);
    printf("      Log Link  mon value  : %d\n",logs[i].log_link_datetime.tm_mon);
    printf("      Log Link mday value  : %d\n",logs[i].log_link_datetime.tm_mday);
    printf("      Log Link   day value : %d\n",logs[i].log_link_datetime.tm_wday);
    printf("      Log Link  hour value : %d\n",logs[i].log_link_datetime.tm_hour);
    printf("      Log Link  min value  : %d\n",logs[i].log_link_datetime.tm_min);
    printf("      Log Link  sec value  : %d\n",logs[i].log_link_datetime.tm_sec);
    printf("      Log Link  isdst      : %d\n",logs[i].log_link_datetime.tm_isdst);
    printf("      Log Modified DateTime\n");
    printf("Log Modified    year value : %d\n",logs[i].log_modified_datetime.tm_year);
    printf("Log Modified    mon value  : %d\n",logs[i].log_modified_datetime.tm_mon);
    printf("Log Modified   mday value  : %d\n",logs[i].log_modified_datetime.tm_mday);
    printf("Log Modified     day value : %d\n",logs[i].log_modified_datetime.tm_wday);
    printf("Log Modified    hour value : %d\n",logs[i].log_modified_datetime.tm_hour);
    printf("Log Modified    min value  : %d\n",logs[i].log_modified_datetime.tm_min);
    printf("Log Modified    sec value  : %d\n",logs[i].log_modified_datetime.tm_sec);
    printf("Log Modified    isdst      : %d\n",logs[i].log_modified_datetime.tm_isdst);
    printf("            Log AutoRefresh: %d\n",logs[i].log_autorefresh);
    printf("             Log Start Date\n");
    printf("Log Start Date   year value: %d\n",logs[i].log_startdate.tm_year);
    printf("Log Start Date    mon value: %d\n",logs[i].log_startdate.tm_mon);
    printf("Log Start Date   mday value: %d\n",logs[i].log_startdate.tm_mday);
    printf("Log Start Date    day value: %d\n",logs[i].log_startdate.tm_wday);
    printf("Log Start Date   hour value: %d\n",logs[i].log_startdate.tm_hour);
    printf("Log Start Date   min value : %d\n",logs[i].log_startdate.tm_min);
    printf("Log Start Date   sec value : %d\n",logs[i].log_startdate.tm_sec);
    printf("Log Start Date   isdst     : %d\n",logs[i].log_startdate.tm_isdst);
    printf("               Log End Date\n");
    printf("Log End Date   year value  : %d\n",logs[i].log_enddate.tm_year);
    printf("Log End Date    mon value  : %d\n",logs[i].log_enddate.tm_mon);
    printf("Log End Date   mday value  : %d\n",logs[i].log_enddate.tm_mday);
    printf("Log End Date     day value : %d\n",logs[i].log_enddate.tm_wday);
    printf("Log End Date    hour value : %d\n",logs[i].log_enddate.tm_hour);
    printf("Log End Date    min value  : %d\n",logs[i].log_enddate.tm_min);
    printf("Log End Date    sec value  : %d\n",logs[i].log_enddate.tm_sec);
    printf("Log End Date    isdst      : %d\n",logs[i].log_enddate.tm_isdst);
    printf("       Log Scheduled Tracks: %d\n",logs[i].log_scheduled_tracks);
    printf("       Log Completed Tracks: %d\n",logs[i].log_completed_tracks);
    printf("            Log Music Links: %d\n",logs[i].log_music_links);
    printf("           Log Music Linked: %d\n",logs[i].log_music_linked);
    printf("          Log Traffic Links: %d\n",logs[i].log_traffic_links);
    printf("         Log Traffic Linked: %d\n",logs[i].log_traffic_linked);
    printf("\n");
  }

  //
  // Free the Log list when finished with it
  //
  free(logs);

  exit(0);
}
