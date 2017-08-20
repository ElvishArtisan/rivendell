// mainloop.cpp
//
// Render a Rivendell log.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <sndfile.h>

#include <rdcart.h>
#include <rdcut.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdlog_line.h>

#include "rdrender.h"

int MainObject::MainLoop()
{
  static float pcm[16384];
  QTime current_time=render_start_time;
  //  QDate current_date=render_start_date;

  //
  // Open Endpoints
  //
  RDLog *log=new RDLog(render_logname);
  if(!log->exists()) {
    fprintf(stderr,"rdrender: no such log\n");
    return 1;
  }
  RDLogEvent *log_event=new RDLogEvent(RDLog::tableName(render_logname));
  log_event->load();

  SF_INFO sf_info;
  memset(&sf_info,0,sizeof(sf_info));
  sf_info.samplerate=render_system->sampleRate();
  sf_info.channels=render_channels;
  sf_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16;
  SNDFILE *sf_out=sf_open(render_output_filename,SFM_WRITE,&sf_info);
  if(sf_out==NULL) {
    fprintf(stderr,"rdrender: unable to open output file [%s]\n",
	    sf_strerror(sf_out));
    return 1;
  }

  //
  // Iterate through the log
  //
  for(int i=0;i<log_event->size();i++) {
    RDLogLine *ll=log_event->logLine(i);
    if(ll->type()==RDLogLine::Cart) {
      RDCart *cart=new RDCart(ll->cartNumber());
      if(cart->exists()&&(cart->type()==RDCart::Audio)) {
	QString cutname;
	if(cart->selectCut(&cutname,current_time)) {
	  RDCut *cut=new RDCut(cutname);
	  QString filename;
	  if(GetCutFile(cutname,cut->startPoint(),cut->endPoint(),&filename)) {
	    SNDFILE *sf_in=sf_open(filename,SFM_READ,&sf_info);
	    int n;
	    if(sf_in!=NULL) {
	      DeleteCutFile(filename);
	      while((n=sf_readf_float(sf_in,pcm,8192))>0) {
		sf_writef_float(sf_out,pcm,n);
	      }
	      sf_close(sf_in);
	    }
	  }
	  delete cut;
	}
      }
      delete cart;
    }
  }

  //
  // Clean up
  //
  sf_close(sf_out);
  return 0;
}
