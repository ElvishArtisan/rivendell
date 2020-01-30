// readcd_test.cpp
//
// Test the Rivendell CD reader routines
//
//   (C) Copyright 2013-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>

#include <qapplication.h>

#include <discid/discid.h>

#include <rdcmd_switch.h>
#include <rddisclookup.h>

#include "readcd_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString device="";
  bool extended=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"readcd_test",
		    READCD_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--device") {
      device=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--extended") {
      extended=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"readcd_test: unknown option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(1);
    }
  }
  if(device.isEmpty()) {
    fprintf(stderr,"readcd_test: you must specify a CD device\n");
    exit(1);
  }

  //
  // Read the disc
  //
  DiscId *disc=discid_new();
  if(extended) {
    if(discid_read(disc,device.toUtf8())==0) {
      fprintf(stderr,"readcd_test: discid error [%s]\n",
	      discid_get_error_msg(disc));
      exit(1);
    }
  }
  else {
    if(discid_read_sparse(disc,device.toUtf8(),0)==0) {
      fprintf(stderr,"readcd_test: discid error [%s]\n",
	      discid_get_error_msg(disc));
      exit(1);
    }
  }

  //
  // Print Results
  //
  printf("             FreeDB DiscID: %s\n",discid_get_freedb_id(disc));
  printf("        MusicBrainz DiscID: %s\n",discid_get_id(disc));
  printf("MusicBrainz Submission URL: %s\n",discid_get_submission_url(disc));
  if(extended) {
    printf("Media Catalog Number (MCN): %s\n",discid_get_mcn(disc));

    printf("ISRCs:\n");
    int first=discid_get_first_track_num(disc);
    int last=discid_get_last_track_num(disc);
    for(int i=first;i<=last;i++) {
      QString isrc=discid_get_track_isrc(disc,i);
      printf("%02d ",i);
      printf("%s  ",(const char *)isrc.toUtf8());
      if(RDDiscLookup::isrcIsValid(isrc)) {
	printf("ok: yes  ");
      }
      else {
	printf("ok: no   ");
      }
      printf("formatted: %s  ",
	     (const char *)RDDiscLookup::formattedIsrc(isrc).toUtf8());
      printf("normalized: %s  ",
	     (const char *)RDDiscLookup::normalizedIsrc(isrc).toUtf8());
      printf("\n");
    }
  }

  //
  // Cleanup
  //
  discid_free(disc);

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
