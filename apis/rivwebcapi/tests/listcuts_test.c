/* listcuts_test.c
 *
 * Test the listcuts library.
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

#include <rivwebcapi/rd_listcuts.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{
  int i;
  struct rd_cut *cuts=0;

  char buf[BUFSIZ];
  char *p;
  long int cart=0;
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

  printf("Please enter the Cart Number that you want to List CUTS ==> ");
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
  //

  // Add the User Agent and Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  // Call the function
  //
  int result=RD_ListCuts(&cuts,
		host,
		user,
		passwd,
		ticket,
		(unsigned)cart,
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
      case 400:
         fprintf(stderr," ERROR: Missing Cart Number");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }
  //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("                 Cut Name: %s\n",cuts[i].cut_name);
    printf("          Cut Cart Number: %u\n",cuts[i].cut_cart_number);
    printf("               Cut Number: %u\n",cuts[i].cut_cut_number);
    printf("                Evergreen: %d\n",cuts[i].cut_evergreen);
    printf("          Cut Description: %s\n",cuts[i].cut_description);
    printf("               Cut Outcue: %s\n",cuts[i].cut_outcue);
    printf("                 Cut ISRC: %s\n",cuts[i].cut_isrc);
    printf("                 Cut ISCI: %s\n",cuts[i].cut_isci);
    printf("               Cut Length: %u\n",cuts[i].cut_length);
    printf("     Cut Origin Date Time\n");
    printf("Cut Origin year value    : %d\n",cuts[i].cut_origin_datetime.tm_year);
    printf("Cut Origin month value   : %d\n",cuts[i].cut_origin_datetime.tm_mon);
    printf("Cut Origin day value     : %d\n",cuts[i].cut_origin_datetime.tm_mday);
    printf("Cut Origin wday value    : %d\n",cuts[i].cut_origin_datetime.tm_wday);
    printf("Cut Origin hour value    : %d\n",cuts[i].cut_origin_datetime.tm_hour);
    printf("Cut Origin min value     : %d\n",cuts[i].cut_origin_datetime.tm_min);
    printf("Cut Origin sec value     : %d\n",cuts[i].cut_origin_datetime.tm_sec);
    printf("Cut Origin isdst value   : %d\n",cuts[i].cut_origin_datetime.tm_isdst);
    printf("      Cut Start Date Time\n");
    printf("Cut Start year value     : %d\n",cuts[i].cut_start_datetime.tm_year);
    printf("Cut Start month value    : %d\n",cuts[i].cut_start_datetime.tm_mon);
    printf("Cut Start day value      : %d\n",cuts[i].cut_start_datetime.tm_mday);
    printf("Cut Start wday value     : %d\n",cuts[i].cut_start_datetime.tm_wday);
    printf("Cut Start hour value     : %d\n",cuts[i].cut_start_datetime.tm_hour);
    printf("Cut Start min value      : %d\n",cuts[i].cut_start_datetime.tm_min);
    printf("Cut Start sec value      : %d\n",cuts[i].cut_start_datetime.tm_sec);
    printf("Cut Start isdst value    : %d\n",cuts[i].cut_start_datetime.tm_isdst);
    printf("        Cut End Date Time\n");
    printf("Cut End year value       : %d\n",cuts[i].cut_end_datetime.tm_year);
    printf("Cut End month value      : %d\n",cuts[i].cut_end_datetime.tm_mon);
    printf("Cut End day value        : %d\n",cuts[i].cut_end_datetime.tm_mday);
    printf("Cut End wday value       : %d\n",cuts[i].cut_end_datetime.tm_wday);
    printf("Cut End hour value       : %d\n",cuts[i].cut_end_datetime.tm_hour);
    printf("Cut End min value        : %d\n",cuts[i].cut_end_datetime.tm_min);
    printf("Cut End sec value        : %d\n",cuts[i].cut_end_datetime.tm_sec);
    printf("Cut End isdst value      : %d\n",cuts[i].cut_end_datetime.tm_isdst);
    printf("                  Cut Sun: %d\n",cuts[i].cut_sun);
    printf("                  Cut Mon: %d\n",cuts[i].cut_mon);
    printf("                  Cut Tue: %d\n",cuts[i].cut_tue);
    printf("                  Cut Wed: %d\n",cuts[i].cut_wed);
    printf("                  Cut Thu: %d\n",cuts[i].cut_thu);
    printf("                  Cut Fri: %d\n",cuts[i].cut_fri);
    printf("                  Cut Sat: %d\n",cuts[i].cut_sat);
    printf("        Cut Start Daypart: %s\n",cuts[i].cut_start_daypart);
    printf("          Cut End Daypart: %s\n",cuts[i].cut_end_daypart);
    printf("          Cut Origin Name: %s\n",cuts[i].cut_origin_name);
    printf("    Cut Origin Login Name: %s\n",cuts[i].cut_origin_login_name);
    printf("          Source Hostname: %s\n",cuts[i].cut_source_hostname);
    printf("               Cut Weight: %u\n",cuts[i].cut_weight);
    printf("  Cut Last Play Date Time\n");
    printf("Cut LastPlay year value  : %d\n",cuts[i].cut_last_play_datetime.tm_year);
    printf("Cut LastPlay month value : %d\n",cuts[i].cut_last_play_datetime.tm_mon);
    printf("Cut LastPlay day value   : %d\n",cuts[i].cut_last_play_datetime.tm_mday);
    printf("Cut LastPlay wday value  : %d\n",cuts[i].cut_last_play_datetime.tm_wday);
    printf("Cut LastPlay hour value  : %d\n",cuts[i].cut_last_play_datetime.tm_hour);
    printf("Cut LastPlay min value   : %d\n",cuts[i].cut_last_play_datetime.tm_min);
    printf("Cut LastPlay sec value   : %d\n",cuts[i].cut_last_play_datetime.tm_sec);
    printf("Cut LastPlay isdst value : %d\n",cuts[i].cut_last_play_datetime.tm_isdst);
    printf("         Cut Play Counter: %u\n",cuts[i].cut_play_counter);
    printf("        Cut Local Counter: %u\n",cuts[i].cut_local_counter);
    printf("             Cut Validity: %u\n",cuts[i].cut_validity);
    printf("        Cut Coding Format: %u\n",cuts[i].cut_coding_format);
    printf("          Cut Sample Rate: %u\n",cuts[i].cut_sample_rate);
    printf("             Cut Bit Rate: %u\n",cuts[i].cut_bit_rate);
    printf("             Cut Channels: %u\n",cuts[i].cut_channels);
    printf("            Cut Play Gain: %d\n",cuts[i].cut_play_gain);
    printf("          Cut Start Point: %d\n",cuts[i].cut_start_point);
    printf("            Cut End Point: %d\n",cuts[i].cut_end_point);
    printf("        Cut Fade Up Point: %d\n",cuts[i].cut_fadeup_point);
    printf("      Cut Fade Down Point: %d\n",cuts[i].cut_fadedown_point);
    printf("    Cut Segue Start Point: %d\n",cuts[i].cut_segue_start_point);
    printf("      Cut Segue End Point: %d\n",cuts[i].cut_segue_end_point);
    printf("           Cut Segue Gain: %d\n",cuts[i].cut_segue_gain);
    printf("     Cut Hook Start Point: %d\n",cuts[i].cut_hook_start_point);
    printf("       Cut Hook End Point: %d\n",cuts[i].cut_hook_end_point);
    printf("     Cut Talk Start Point: %d\n",cuts[i].cut_talk_start_point);
    printf("       Cut Talk End Point: %d\n",cuts[i].cut_talk_end_point);
    printf("\n");

  }
  //
  // Free the cut list when finished with it
  //
  free(cuts);

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
  result=RD_ListCuts(&cuts,
		host,
		user,
		passwd,
		ticket,
		(unsigned)cart,
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
      case 400:
         fprintf(stderr," ERROR: Missing Cart Number");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }
  //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("                 Cut Name: %s\n",cuts[i].cut_name);
    printf("          Cut Cart Number: %u\n",cuts[i].cut_cart_number);
    printf("               Cut Number: %u\n",cuts[i].cut_cut_number);
    printf("                Evergreen: %d\n",cuts[i].cut_evergreen);
    printf("          Cut Description: %s\n",cuts[i].cut_description);
    printf("               Cut Outcue: %s\n",cuts[i].cut_outcue);
    printf("                 Cut ISRC: %s\n",cuts[i].cut_isrc);
    printf("                 Cut ISCI: %s\n",cuts[i].cut_isci);
    printf("               Cut Length: %u\n",cuts[i].cut_length);
    printf("     Cut Origin Date Time\n");
    printf("Cut Origin year value    : %d\n",cuts[i].cut_origin_datetime.tm_year);
    printf("Cut Origin month value   : %d\n",cuts[i].cut_origin_datetime.tm_mon);
    printf("Cut Origin day value     : %d\n",cuts[i].cut_origin_datetime.tm_mday);
    printf("Cut Origin wday value    : %d\n",cuts[i].cut_origin_datetime.tm_wday);
    printf("Cut Origin hour value    : %d\n",cuts[i].cut_origin_datetime.tm_hour);
    printf("Cut Origin min value     : %d\n",cuts[i].cut_origin_datetime.tm_min);
    printf("Cut Origin sec value     : %d\n",cuts[i].cut_origin_datetime.tm_sec);
    printf("Cut Origin isdst value   : %d\n",cuts[i].cut_origin_datetime.tm_isdst);
    printf("      Cut Start Date Time\n");
    printf("Cut Start year value     : %d\n",cuts[i].cut_start_datetime.tm_year);
    printf("Cut Start month value    : %d\n",cuts[i].cut_start_datetime.tm_mon);
    printf("Cut Start day value      : %d\n",cuts[i].cut_start_datetime.tm_mday);
    printf("Cut Start wday value     : %d\n",cuts[i].cut_start_datetime.tm_wday);
    printf("Cut Start hour value     : %d\n",cuts[i].cut_start_datetime.tm_hour);
    printf("Cut Start min value      : %d\n",cuts[i].cut_start_datetime.tm_min);
    printf("Cut Start sec value      : %d\n",cuts[i].cut_start_datetime.tm_sec);
    printf("Cut Start isdst value    : %d\n",cuts[i].cut_start_datetime.tm_isdst);
    printf("        Cut End Date Time\n");
    printf("Cut End year value       : %d\n",cuts[i].cut_end_datetime.tm_year);
    printf("Cut End month value      : %d\n",cuts[i].cut_end_datetime.tm_mon);
    printf("Cut End day value        : %d\n",cuts[i].cut_end_datetime.tm_mday);
    printf("Cut End wday value       : %d\n",cuts[i].cut_end_datetime.tm_wday);
    printf("Cut End hour value       : %d\n",cuts[i].cut_end_datetime.tm_hour);
    printf("Cut End min value        : %d\n",cuts[i].cut_end_datetime.tm_min);
    printf("Cut End sec value        : %d\n",cuts[i].cut_end_datetime.tm_sec);
    printf("Cut End isdst value      : %d\n",cuts[i].cut_end_datetime.tm_isdst);
    printf("                  Cut Sun: %d\n",cuts[i].cut_sun);
    printf("                  Cut Mon: %d\n",cuts[i].cut_mon);
    printf("                  Cut Tue: %d\n",cuts[i].cut_tue);
    printf("                  Cut Wed: %d\n",cuts[i].cut_wed);
    printf("                  Cut Thu: %d\n",cuts[i].cut_thu);
    printf("                  Cut Fri: %d\n",cuts[i].cut_fri);
    printf("                  Cut Sat: %d\n",cuts[i].cut_sat);
    printf("        Cut Start Daypart: %s\n",cuts[i].cut_start_daypart);
    printf("          Cut End Daypart: %s\n",cuts[i].cut_end_daypart);
    printf("          Cut Origin Name: %s\n",cuts[i].cut_origin_name);
    printf("    Cut Origin Login Name: %s\n",cuts[i].cut_origin_login_name);
    printf("          Source Hostname: %s\n",cuts[i].cut_source_hostname);
    printf("               Cut Weight: %u\n",cuts[i].cut_weight);
    printf("  Cut Last Play Date Time\n");
    printf("Cut LastPlay year value  : %d\n",cuts[i].cut_last_play_datetime.tm_year);
    printf("Cut LastPlay month value : %d\n",cuts[i].cut_last_play_datetime.tm_mon);
    printf("Cut LastPlay day value   : %d\n",cuts[i].cut_last_play_datetime.tm_mday);
    printf("Cut LastPlay wday value  : %d\n",cuts[i].cut_last_play_datetime.tm_wday);
    printf("Cut LastPlay hour value  : %d\n",cuts[i].cut_last_play_datetime.tm_hour);
    printf("Cut LastPlay min value   : %d\n",cuts[i].cut_last_play_datetime.tm_min);
    printf("Cut LastPlay sec value   : %d\n",cuts[i].cut_last_play_datetime.tm_sec);
    printf("Cut LastPlay isdst value : %d\n",cuts[i].cut_last_play_datetime.tm_isdst);
    printf("         Cut Play Counter: %u\n",cuts[i].cut_play_counter);
    printf("        Cut Local Counter: %u\n",cuts[i].cut_local_counter);
    printf("             Cut Validity: %u\n",cuts[i].cut_validity);
    printf("        Cut Coding Format: %u\n",cuts[i].cut_coding_format);
    printf("          Cut Sample Rate: %u\n",cuts[i].cut_sample_rate);
    printf("             Cut Bit Rate: %u\n",cuts[i].cut_bit_rate);
    printf("             Cut Channels: %u\n",cuts[i].cut_channels);
    printf("            Cut Play Gain: %d\n",cuts[i].cut_play_gain);
    printf("          Cut Start Point: %d\n",cuts[i].cut_start_point);
    printf("            Cut End Point: %d\n",cuts[i].cut_end_point);
    printf("        Cut Fade Up Point: %d\n",cuts[i].cut_fadeup_point);
    printf("      Cut Fade Down Point: %d\n",cuts[i].cut_fadedown_point);
    printf("    Cut Segue Start Point: %d\n",cuts[i].cut_segue_start_point);
    printf("      Cut Segue End Point: %d\n",cuts[i].cut_segue_end_point);
    printf("           Cut Segue Gain: %d\n",cuts[i].cut_segue_gain);
    printf("     Cut Hook Start Point: %d\n",cuts[i].cut_hook_start_point);
    printf("       Cut Hook End Point: %d\n",cuts[i].cut_hook_end_point);
    printf("     Cut Talk Start Point: %d\n",cuts[i].cut_talk_start_point);
    printf("       Cut Talk End Point: %d\n",cuts[i].cut_talk_end_point);
    printf("\n");

  }
  //
  // Free the cut list when finished with it
  //
  free(cuts);

  exit(0);
}
