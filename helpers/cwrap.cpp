// cwrap.cpp
//
// A utility for wrapping arbitrary file data in C-compaibile source
// statements.
//
// (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cwrap.cpp,v 1.3 2007/02/14 21:48:41 fredg Exp $
//      $Date: 2007/02/14 21:48:41 $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <cwrap.h>

int main(int argc,char *argv[])
{
  int input_fd;
  FILE *output_desc;
  char *input_name;
  char output_name[256];
  char var_name[256];
  bool found;
  struct stat stat;
  char line[LINE_LENGTH];
  int n;
  int count=0;

  //
  // Build Defaults
  //
  sprintf(output_name,"%s.c",argv[argc-1]);

  //
  // Process Arguments
  //
  for(int i=1;i<argc;i++) {
    found=false;
    if((!strcmp(argv[i],"-o"))||(!strcmp(argv[i],"--output-file"))) {
      found=true;
      if(i<(argc-2)) {
	strcpy(output_name,argv[++i]);
      }
      else {
	printf(USAGE);
	exit(1);
      }
    }
    if(i==(argc-1)) {
      input_name=argv[i];
      found=true;
    }
    if(!found) {
      printf(USAGE);
      exit(1);
    }
  }

  //
  // Set Variable Name
  //
  for(unsigned j=0;j<strlen(output_name);j++) {
    if(output_name[j]!='.') {
      var_name[j]=output_name[j];
    }
    else {
      var_name[j]=0;
      j=strlen(output_name);
    }
  }
  
  //
  // Open Files
  //
  if((input_fd=open(input_name,O_RDONLY))<0) {
    perror("wrapdat");
    exit(1);
  }
  if((output_desc=fopen(output_name,"w"))==NULL) {
    perror("wrapdat");
    exit(1);
  }

  //
  // Get Size of Source File and Write Header
  //
  memset(&stat,0,sizeof(struct stat));
  if(fstat(input_fd,&stat)) {
    perror("wrapdat");
    exit(1);
  }
  fprintf(output_desc,"const unsigned char %s[%ld] = {\n",
	  var_name,stat.st_size+1);

  //
  // Write Body
  //
  for(int i=0;i<(stat.st_size/LINE_LENGTH+1);i++) {
    n=read(input_fd,line,LINE_LENGTH);
    for(int j=0;j<n;j++) {
      fprintf(output_desc,"%d,",line[j]);
      count++;
    }
    if(n>0) {
      fprintf(output_desc,"\n");
    }
  }
  fprintf(output_desc,"0};\n");

  //
  // Finish Up
  //
  close(input_fd);
  fclose(output_desc);
  exit(0);
}
