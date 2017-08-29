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

#include <math.h>
#include <stdio.h>

#include <vector>

#include <rdcart.h>
#include <rdcut.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdlog_line.h>

#include "rdrender.h"

int MainObject::MainLoop()
{
  float *pcm=NULL;
  QTime current_time=render_start_time;
  QString warnings="";

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
  // Initialize the log
  //
  std::vector<LogLine *> lls;
  for(int i=0;i<log_event->size();i++) {
    lls.push_back(new LogLine(log_event->logLine(i),render_user,render_station,
			      render_system,render_config,render_channels));
    if((!render_first_time.isNull())&&
       (lls.back()->timeType()==RDLogLine::Hard)&&
       (render_first_line==-1)&&
       (lls.back()->startTime(RDLogLine::Imported)==render_first_time)) {
      render_first_line=i;
    }
    if((!render_last_time.isNull())&&
       (lls.back()->timeType()==RDLogLine::Hard)&&
       (render_last_line==-1)&&
       (lls.back()->startTime(RDLogLine::Imported)==render_last_time)) {
      render_last_line=i;
    }
  }
  QString time_errs="";
  if((!render_first_time.isNull())&&(render_first_line==-1)) {
    time_errs+="--first-time event not found";
  }
  if((!render_last_time.isNull())&&(render_last_line==-1)) {
    if(!time_errs.isEmpty()) {
      time_errs+=", ";
    }
    time_errs+="--last-time event not found";
  }
  if(!time_errs.isEmpty()) {
    fprintf(stderr,"rdrender: %s\n",(const char *)time_errs);
    return 1;
  }
  lls.push_back(new LogLine(new RDLogLine(),render_user,render_station,
			      render_system,render_config,render_channels));
  lls.back()->setTransType(RDLogLine::Play);
  if((!render_first_time.isNull())&&(render_first_line==-1)) {
    render_first_line=log_event->size();
  }

  //
  // Iterate through it
  //
  for(unsigned i=0;i<lls.size();i++) {
    if(((render_first_line==-1)||(render_first_line<=(int)i))&&
       ((render_last_line==-1)||(render_last_line>(int)i))) {
      if(lls.at(i)->transType()==RDLogLine::Stop) {
	Verbose(current_time,i,"STOP ",lls.at(i)->summary());
	warnings+=
	  QString().sprintf("log render halted at line %d due to STOP\n",i);
	break;
      }
      if(lls.at(i)->open(current_time)) {
	Verbose(current_time,i,RDLogLine::transText(lls.at(i)->transType()),
		QString().sprintf(" cart %06u [",lls.at(i)->cartNumber())+
		lls.at(i)->title()+"]");
	sf_count_t frames=0;
	if((lls.at(i+1)->transType()==RDLogLine::Segue)&&
	   (lls.at(i)->cut()->segueStartPoint()>=0)) {
	  frames=FramesFromMsec(lls.at(i)->cut()->segueStartPoint()-
				lls.at(i)->cut()->startPoint());
	  current_time=current_time.addMSecs(lls.at(i)->cut()->segueStartPoint()-
					     lls.at(i)->cut()->startPoint());
	}
	else {
	  frames=FramesFromMsec(lls.at(i)->cut()->endPoint()-
				lls.at(i)->cut()->startPoint());
	  current_time=current_time.addMSecs(lls.at(i)->cut()->endPoint()-
					     lls.at(i)->cut()->startPoint());
	}
	pcm=new float[frames*render_channels];
	memset(pcm,0,frames*render_channels);
	
	for(unsigned j=0;j<i;j++) {
	  Sum(pcm,lls.at(j),frames);
	}
	Sum(pcm,lls.at(i),frames);
	sf_writef_float(sf_out,pcm,frames);
	delete pcm;
	pcm=NULL;
	lls.at(i)->setRamp(lls.at(i+1)->transType());
      }
      else {
	if(i<(lls.size()-1)) {
	  if(lls.at(i)->type()==RDLogLine::Cart) {
	    Verbose(current_time,i,"FAIL",lls.at(i)->summary()+
		    " (NO AUDIO AVAILABLE)");
	    warnings+=
	      lls.at(i)->summary()+QString().
	      sprintf("at line %d failed to play (NO AUDIO AVAILABLE)\n",i);
	  }
	  else {
	    Verbose(current_time,i,"SKIP",lls.at(i)->summary());
	  }
	}
	else {
	  Verbose(current_time,lls.size()-1,"STOP","--- end of log ---");
	}
      }
    }
  }
  fprintf(stderr,"%s",(const char *)warnings);
  fflush(stderr);

  //
  // Clean up
  //
  sf_close(sf_out);
  return 0;
}


void MainObject::Sum(float *pcm_out,LogLine *ll,sf_count_t frames)
{
  if(ll->handle()!=NULL) {
    float *pcm=new float[frames*render_channels];

    memset(pcm,0,frames*render_channels);
    sf_count_t n=sf_readf_float(ll->handle(),pcm,frames);
    for(sf_count_t i=0;i<n;i+=render_channels) {
      double ratio=exp10(((double)i*ll->rampRate()+ll->rampLevel())/2000.0);
      for(sf_count_t j=0;j<render_channels;j++) {
	pcm_out[i*render_channels+j]+=ratio*pcm[i*render_channels+j];
      }
    }
    ll->setRampLevel((double)n*ll->rampRate()+ll->rampLevel());
    if(n<frames) {
      ll->close();
    }
    delete pcm;
  }
}
