/* getuseragent.c
 *
 * Test the getuseragent library routine.
 *
 * (C) Copyright 2017 Todd Baker  <bakert@rfa.org>             
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

#include <rivwebcapi/rd_getuseragent.h>


int main(int argc,char *argv[])
{
  char  user_agent_string[255];

  // Call the function
  //
  strcpy(user_agent_string,RD_GetUserAgent());
  //
  //
  // List the Result
  //
    printf(" The User Agent String is: %s\n",user_agent_string);
    printf("\n");

  exit(0);
}
