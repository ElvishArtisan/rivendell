/* getversion.c
 *
 * Test the getversion library routine.
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

#include <rivwebcapi/rd_getversion.h>


int main(int argc,char *argv[])
{
  char  api_version[20];

  // Call the function
  //
  strcpy(api_version,RD_GetVersion());
  //
  //
  // List the Result
  //
    printf(" The Rivendell API Version Number: %s\n",api_version);
    printf("\n");

  exit(0);
}
