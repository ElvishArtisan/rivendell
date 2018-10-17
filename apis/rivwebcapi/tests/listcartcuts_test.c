/* listcartcuts_test.c
 *
 * Test the listcartcuts library.
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>             
 * (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rivwebcapi/rd_listcartcuts.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

void dump_datetime(const char *label,struct tm *tm)
{
  printf(" %s year value: %d\n",label,tm->tm_year);
  printf("  %s mon value: %d\n",label,tm->tm_mon);
  printf("  %s day value: %d\n",label,tm->tm_mday);
  printf(" %s wday value: %d\n",label,tm->tm_wday);
  printf(" %s hour value: %d\n",label,tm->tm_hour);
  printf("  %s min value: %d\n",label,tm->tm_min);
  printf("  %s sec value: %d\n",label,tm->tm_sec);
  printf("      %s isdst: %d\n",label,tm->tm_isdst);
}


void dump_cut(struct rd_cut *cut)
{
  printf("Cut %04u:\n",cut->cut_cut_number);
  printf("                   Cut Name: %s\n",cut->cut_name);
  printf("                  Evergreen: %d\n",cut->cut_evergreen);
  printf("                Description: %s\n",cut->cut_description);
  printf("                     Outcue: %s\n",cut->cut_outcue);
  printf("                       ISRC: %s\n",cut->cut_isrc);
  printf("                       ISCI: %s\n",cut->cut_isci);
  printf("                     Length: %u\n",cut->cut_length);
  dump_datetime("Origin Datetime",&cut->cut_origin_datetime);
  dump_datetime("Origin Datetime",&cut->cut_origin_datetime);
  dump_datetime(" Start Datetime",&cut->cut_start_datetime);
  dump_datetime("   End Datetime",&cut->cut_end_datetime);
  printf("                     Sunday: %d\n",cut->cut_sun);
  printf("                     Monday: %d\n",cut->cut_mon);
  printf("                    Tuesday: %d\n",cut->cut_tue);
  printf("                  Wednesday: %d\n",cut->cut_wed);
  printf("                   Thursday: %d\n",cut->cut_thu);
  printf("                     Friday: %d\n",cut->cut_fri);
  printf("                   Saturday: %d\n",cut->cut_sat);
  printf("              Start Daypart: %s\n",cut->cut_start_daypart);
  printf("                End Daypart: %s\n",cut->cut_end_daypart);
  printf("                Origin Name: %s\n",cut->cut_origin_name);
  printf("          Origin Login Name: %s\n",cut->cut_origin_login_name);
  printf("            Source Hostname: %s\n",cut->cut_source_hostname);
  printf("                     Weight: %u\n",cut->cut_weight);
  dump_datetime("Last Play Datetime",&cut->cut_last_play_datetime);
  printf("               Play Counter: %u\n",cut->cut_play_counter);
  printf("              Coding Format: %u\n",cut->cut_coding_format);
  printf("                Sample Rate: %u\n",cut->cut_sample_rate);
  printf("                   Bit Rate: %u\n",cut->cut_bit_rate);
  printf("                   Channels: %u\n",cut->cut_channels);
  printf("                  Play Gain: %d\n",cut->cut_play_gain);
  printf("                Start Point: %d\n",cut->cut_start_point);
  printf("                  End Point: %d\n",cut->cut_end_point);
  printf("              Fade Up Point: %d\n",cut->cut_start_point);
  printf("            Fade Down Point: %d\n",cut->cut_end_point);
  printf("          Segue Start Point: %d\n",cut->cut_segue_start_point);
  printf("            Segue End Point: %d\n",cut->cut_segue_end_point);
  printf("                 Segue Gain: %d\n",cut->cut_segue_gain);
  printf("           Hook Start Point: %d\n",cut->cut_hook_start_point);
  printf("             Hook End Point: %d\n",cut->cut_hook_end_point);
  printf("           Talk Start Point: %d\n",cut->cut_talk_start_point);
  printf("             Talk End Point: %d\n",cut->cut_talk_end_point);
}


void dump_cart(struct rd_cart *cart)
{
  printf("              Cart Number: %d\n",cart->cart_number);
  printf("                Cart Type: %d\n",cart->cart_type);
  printf("               Group Name: %s\n",cart->cart_grp_name);
  printf("               Cart Title: %s\n",cart->cart_title);
  printf("              Cart Artist: %s\n",cart->cart_artist);
  printf("               Cart Album: %s\n",cart->cart_album);
  printf("                Cart Year: %d\n",cart->cart_year);
  printf("               Cart Label: %s\n",cart->cart_label);
  printf("              Cart Client: %s\n",cart->cart_client);
  printf("              Cart Agency: %s\n",cart->cart_agency);
  printf("           Cart Publisher: %s\n",cart->cart_publisher);
  printf("            Cart Composer: %s\n",cart->cart_composer);
  printf("           Cart Conductor: %s\n",cart->cart_conductor);
  printf("        Cart User Defined: %s\n",cart->cart_user_defined);
  printf("          Cart Usage Code: %d\n",cart->cart_usage_code);
  printf("       Cart Forced Length: %d\n",cart->cart_forced_length);
  printf("      Cart Average Length: %d\n",cart->cart_average_length);
  printf("    Cart Length Deviation: %d\n",cart->cart_length_deviation);
  printf("Cart Average Segue Length: %d\n",cart->cart_average_segue_length);
  printf(" Cart Average Hook Length: %d\n",cart->cart_average_hook_length);
  printf("        Cart Cut Quantity: %u\n",cart->cart_cut_quantity);
  printf("     Cart Last Cut Played: %03u\n",cart->cart_last_cut_played);
  printf("            Cart Validity: %u\n",cart->cart_validity);
  printf("      Cart Enforce Length: %d\n",cart->cart_enforce_length);
  printf("         Cart Asyncronous: %d\n",cart->cart_asyncronous);
  printf("               Cart Owner: %s\n",cart->cart_owner);
  dump_datetime("Cart Metadata",&cart->cart_metadata_datetime);
}


int main(int argc,char *argv[])
{
  int i;
  int j;
  struct rd_cart *carts=0;
  char buf[BUFSIZ];
  char *p;
  long int cart=0;
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

  // Add the User Agent and Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //
  int result= RD_ListCartCuts(&carts,
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
      fprintf(stderr,"ERROR:  Missing Cart Number! \n");
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
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    dump_cart(&carts[i]);
    printf("\n");
    for(j=0;j<carts[i].cart_cut_quantity;j++) {
      dump_cut(RD_ListCartCuts_GetCut(&carts[i],j));
      printf("\n");
    }
  }

  //
  // Free the cart list when finished with it
  //
  RD_ListCartCuts_Free(carts);


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
    dump_datetime("Ticket Expire",&myticket->tkt_expiration_datetime);
    printf("\n");
  }

    user="";
    passwd="";
    strcpy( ticket,myticket->ticket);
    fprintf(stderr, "Ticket was copied - = %s\n",ticket);
  //
  // Call the function
  //
  result= RD_ListCartCuts(&carts,
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
      fprintf(stderr,"ERROR:  Missing Cart Number! \n");
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
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    dump_cart(&carts[i]);
    printf("\n");
    for(j=0;j<carts[i].cart_cut_quantity;j++) {
      dump_cut(RD_ListCartCuts_GetCut(&carts[i],j));
      printf("\n");
    }
  }

  //
  // Free the cart list when finished with it
  //
  RD_ListCartCuts_Free(carts);

  exit(0);
}
