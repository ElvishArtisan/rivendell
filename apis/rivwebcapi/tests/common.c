/* common.c
 *
 * Common routines used by test harnesses for the Rivendell C API
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

void TrimWhitespace(char *str)
{
  int i;
  int j;

  for(i=0;i<strlen(str);i++) {
    if(!isspace(str[i])) {
      memmove(str,str+i,strlen(str)+1);
      for(j=i+1;j<strlen(str);j++) {
	if(isspace(str[j])) {
	  str[j]=0;
	  return;
	}
      }
      return;
    }
  }
}


int PromptForString(const char *prompt,char str[],int len)
{
  printf("%s",prompt);
  fflush(stdout);
  if(fgets(str,len,stdin)==NULL) {
    return -1;
  }
  TrimWhitespace(str);

  return strlen(str);
}
