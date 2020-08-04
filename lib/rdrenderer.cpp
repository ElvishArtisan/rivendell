// rdrenderer.cpp
//
// Render a Rivendell log to a single audio object.
//
//   (C) Copyright 2017-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <math.h>

#include "rdapplication.h"
#include "rdaudioconvert.h"
#include "rdaudioexport.h"
#include "rdaudioimport.h"
#include "rdcart.h"
#include "rdconf.h"
#include "rdcut.h"
#include "rdtempdirectory.h"

#include "rdrenderer.h"

__RDRenderLogLine::__RDRenderLogLine(RDLogLine *ll,unsigned chans)
  : RDLogLine(*ll)
{
  ll_cart=NULL;
  ll_cut=NULL;
  ll_handle=NULL;
  ll_channels=chans;
  ll_ramp_level=0.0;
  ll_ramp_rate=0.0;
}


RDCart *__RDRenderLogLine::cart() const
{
  return ll_cart;
}


RDCut *__RDRenderLogLine::cut() const
{
  return ll_cut;
}


SNDFILE *__RDRenderLogLine::handle() const
{
  return ll_handle;
}


double __RDRenderLogLine::rampLevel() const
{
  return ll_ramp_level;
}


void __RDRenderLogLine::setRampLevel(double lvl)
{
  ll_ramp_level=lvl;
}


double __RDRenderLogLine::rampRate() const
{
  return ll_ramp_rate;
}


void __RDRenderLogLine::setRampRate(double lvl)
{
  ll_ramp_rate=lvl;
}


void __RDRenderLogLine::setRamp(RDLogLine::TransType next_trans,int segue_gain)
{
  if((next_trans==RDLogLine::Segue)&&(segueStartPoint()>=0)) {
    ll_ramp_rate=((double)segue_gain)/
      ((double)FramesFromMsec(segueEndPoint()-segueStartPoint()));
    //ll_ramp_rate=((double)RD_FADE_DEPTH)/
    //  ((double)FramesFromMsec(segueEndPoint()-segueStartPoint()));
  }
}


bool __RDRenderLogLine::open(const QTime &time)
{
  QString cutname;
  SF_INFO sf_info;

  if(type()==RDLogLine::Cart) {
    ll_cart=new RDCart(cartNumber());
    if(ll_cart->exists()&&(ll_cart->type()==RDCart::Audio)) {
      if(ll_cart->selectCut(&cutname,time)) {
	ll_cut=new RDCut(cutname);
	setStartPoint(ll_cut->startPoint(),RDLogLine::CartPointer);
	setEndPoint(ll_cut->endPoint(),RDLogLine::CartPointer);
	setSegueStartPoint(ll_cut->segueStartPoint(),RDLogLine::CartPointer);
	setSegueEndPoint(ll_cut->segueEndPoint(),RDLogLine::CartPointer);
	setSegueGain(ll_cut->segueGain());
	QString filename;
	if(GetCutFile(cutname,ll_cut->startPoint(),ll_cut->endPoint(),
		      &filename)) {
	  ll_handle=sf_open(filename,SFM_READ,&sf_info);
	  if(ll_handle!=NULL) {
 	    DeleteCutFile(filename);
	    return true;
	  }
	}
      }
    }
  }
  return false;
}


void __RDRenderLogLine::close()
{
  sf_close(ll_handle);
  ll_handle=NULL;
}


QString __RDRenderLogLine::summary() const
{
  QString ret=QString().sprintf("unknown event [type: %d]",type());
  switch(type()) {
  case RDLogLine::Cart:
    ret=QString().sprintf("cart %06u [",cartNumber())+title()+"]";
    break;
	  
  case RDLogLine::Marker:
    ret="marker ["+markerComment()+"]";
    break;

  case RDLogLine::Macro:
    ret="macro cart ["+title()+"]";
    break;

  case RDLogLine::Chain:
    ret="chain-to ["+markerLabel()+"]";
    break;

  case RDLogLine::Track:
    ret="track marker ["+markerComment()+"]";
    break;

  case RDLogLine::MusicLink:
    ret="music link";
    break;

  case RDLogLine::TrafficLink:
    ret="traffic link";
    break;

  case RDLogLine::OpenBracket:
  case RDLogLine::CloseBracket:
  case RDLogLine::UnknownType:
    break;
  }
  return ret;
}


bool __RDRenderLogLine::GetCutFile(const QString &cutname,int start_pt,
				   int end_pt,QString *dest_filename) const
{
  bool ret=false;
  RDAudioConvert::ErrorCode conv_err;
  RDAudioExport::ErrorCode export_err;
  char tempdir[PATH_MAX];
  
  strncpy(tempdir,RDTempDirectory::basePath()+"/rdrenderXXXXXX",PATH_MAX);
  *dest_filename=QString(mkdtemp(tempdir))+"/"+cutname+".wav";
  RDAudioExport *conv=new RDAudioExport();
  conv->setDestinationFile(*dest_filename);
  conv->setCartNumber(RDCut::cartNumber(cutname));
  conv->setCutNumber(RDCut::cutNumber(cutname));
  RDSettings s;
  s.setFormat(RDSettings::Pcm16);
  s.setSampleRate(rda->system()->sampleRate());
  s.setChannels(ll_channels);
  s.setNormalizationLevel(0);
  conv->setDestinationSettings(&s);
  conv->setRange(start_pt,end_pt);
  conv->setEnableMetadata(false);
  switch(export_err=conv->runExport(rda->user()->name(),
				    rda->user()->password(),&conv_err)) {
  case RDAudioExport::ErrorOk:
    ret=true;
    break;

  default:
    ret=false;
    printf("export err %d [%s]\n",export_err,
	  (const char *)RDAudioExport::errorText(export_err,conv_err).toUtf8());
    break;
  }

  delete conv;
  return ret;
}


void __RDRenderLogLine::DeleteCutFile(const QString &dest_filename) const
{
  unlink(dest_filename);
  QStringList f0=dest_filename.split("/");
  f0.erase(f0.fromLast());
  rmdir("/"+f0.join("/"));
}


uint64_t __RDRenderLogLine::FramesFromMsec(uint64_t msec)
{
  return msec*rda->system()->sampleRate()/1000;
}




RDRenderer::RDRenderer(QObject *parent)
  : QObject(parent)
{
  render_total_passes=0;
}


RDRenderer::~RDRenderer()
{
}


bool RDRenderer::renderToFile(const QString &outfile,RDLogEvent *log,
			      RDSettings *s,const QTime &start_time,
			      bool ignore_stops,QString *err_msg,
			      int first_line,int last_line,
			      const QTime &first_time,const QTime &last_time)
{
  QString temp_output_filename;
  char tempdir[PATH_MAX];
  bool ok=false;
  FILE *f=NULL;
  bool ret;

  //
  // Verify Destination
  //
  if((f=fopen(outfile,"w"))==NULL) {
    *err_msg=tr("unable to open output file")+" ["+QString(strerror(errno))+"]";
    return false;
  }
  fclose(f);

  if(((s->format()!=RDSettings::Pcm16)&&(s->format()!=RDSettings::Pcm24))||
     (s->normalizationLevel()!=0)) {
    ProgressMessage("Pass 1 of 2");
    render_total_passes=2;

    //
    // Get Temporary File
    //
    strncpy(tempdir,RDTempDirectory::basePath()+"/rdrenderXXXXXX",PATH_MAX);
    temp_output_filename=QString(mkdtemp(tempdir))+"/log.wav";
    ProgressMessage(tr("Using temporary file")+" \""+temp_output_filename+"\".");

    //
    // Render It
    //
    if(!Render(temp_output_filename,log,s,start_time,ignore_stops,err_msg,
	       first_line,last_line,first_time,last_time)) {
      return false;
    }

    //
    // Convert It
    //
    ProgressMessage(tr("Pass 2 of 2"));
    ProgressMessage(tr("Writing output file"));
    ok=ConvertAudio(temp_output_filename,outfile,s,err_msg);
    DeleteTempFile(temp_output_filename);
    emit lineStarted(log->size()+1,log->size()+1);
    if(!ok) {
      return false;
    }
  }
  else {
    ProgressMessage(tr("Pass 1 of 1"));
    render_total_passes=1;

    ret=Render(outfile,log,s,start_time,ignore_stops,err_msg,
	       first_line,last_line,first_time,last_time);
    emit lineStarted(log->size(),log->size());
    return ret;
  }
  return true;
}


bool RDRenderer::renderToCart(unsigned cartnum,int cutnum,RDLogEvent *log,
			      RDSettings *s,const QTime &start_time,
			      bool ignore_stops,QString *err_msg,
			      int first_line,int last_line,
			      const QTime &first_time,const QTime &last_time)
{
  QString temp_output_filename;
  char tempdir[PATH_MAX];
  bool ok=false;

  if(first_line<0) {
    first_line=0;
  }
  if(last_line<0) {
    last_line=log->size();
  }

  //
  // Check that we won't overflow the 32 bit BWF structures
  // when we go to import the rendered log back into the audio store
  //
  if((double)log->length(first_line,last_line-1)/1000.0>=
     (1073741824.0/((double)s->channels()*(double)s->sampleRate()))) {
    *err_msg=tr("Rendered log is too long!");
    return false;
  }

  ProgressMessage(tr("Pass 1 of 2"));
  render_total_passes=2;

  //
  // Verify Destination
  //
  if(!RDCart::exists(cartnum)) {
    *err_msg=tr("no such cart");
    return false;
  }
  if(!RDCut::exists(cartnum,cutnum)) {
    *err_msg=tr("no such cut");
    return false;
  }

  //
  // Get Temporary File
  //
  strncpy(tempdir,RDTempDirectory::basePath()+"/rdrenderXXXXXX",PATH_MAX);
  temp_output_filename=QString(mkdtemp(tempdir))+"/log.wav";
  ProgressMessage(tr("Using temporary file")+" \""+temp_output_filename+"\".");

  //
  // Render It
  //
  if(!Render(temp_output_filename,log,s,start_time,ignore_stops,err_msg,
	     first_line,last_line,first_time,last_time)) {
    return false;
  }

  //
  // Convert It
  //
  ProgressMessage(tr("Pass 2 of 2"));
  ProgressMessage(tr("Importing cart"));
  ok=ImportCart(temp_output_filename,cartnum,cutnum,s->channels(),err_msg);
  DeleteTempFile(temp_output_filename);
  emit lineStarted(log->size()+1,log->size()+1);
  if(!ok) {
    return false;
  }

  return true;
}


QStringList RDRenderer::warnings() const
{
  return render_warnings;
}


void RDRenderer::abort()
{
  render_abort=true;
}


bool RDRenderer::Render(const QString &outfile,RDLogEvent *log,RDSettings *s,
			const QTime &start_time,bool ignore_stops,
			QString *err_msg,int first_line,int last_line,
			const QTime &first_time,const QTime &last_time)
{
  float *pcm=NULL;
  QString temp_output_filename;
  QTime current_time;

  render_warnings.clear();
  render_abort=false;

  if(start_time.isNull()) {
    current_time=QTime::currentTime();
  }
  else {
    current_time=start_time;
  }

  //
  // Open Output File
  //
  SF_INFO sf_info;
  SNDFILE *sf_out;

  memset(&sf_info,0,sizeof(sf_info));
  sf_info.samplerate=rda->system()->sampleRate();
  sf_info.channels=s->channels();
  if(s->format()==RDSettings::Pcm16) {
    sf_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_16;
  }
  else {
    sf_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_24;
  }
  sf_out=sf_open(outfile,SFM_WRITE,&sf_info);
  if(sf_out==NULL) {
    fprintf(stderr,"rdrender: unable to open output file [%s]\n",
	    sf_strerror(sf_out));
    return 1;
  }

  //
  // Initialize the log
  //
  std::vector<__RDRenderLogLine *> lls;
  for(int i=0;i<log->size();i++) {
    lls.push_back(new __RDRenderLogLine(log->logLine(i),s->channels()));
    if(ignore_stops&&(lls.back()->transType()==RDLogLine::Stop)) {
      lls.back()->setTransType(RDLogLine::Play);
    }
    if((!first_time.isNull())&&
       (lls.back()->timeType()==RDLogLine::Hard)&&
       (first_line==-1)&&
       (lls.back()->startTime(RDLogLine::Imported)==first_time)) {
      first_line=i;
    }
    if((!last_time.isNull())&&
       (lls.back()->timeType()==RDLogLine::Hard)&&
       (last_line==-1)&&
       (lls.back()->startTime(RDLogLine::Imported)==last_time)) {
      last_line=i;
    }
  }
  if((!first_time.isNull())&&(first_line==-1)) {
    *err_msg+=tr("first-time event not found");
  }
  if((!last_time.isNull())&&(last_line==-1)) {
    if(!err_msg->isEmpty()) {
      *err_msg+=", ";
    }
    *err_msg+=tr("last-time event not found");
  }
  if(!err_msg->isEmpty()) {
    return false;
  }
  lls.push_back(new __RDRenderLogLine(new RDLogLine(),s->channels()));
  lls.back()->setTransType(RDLogLine::Play);
  if((!first_time.isNull())&&(first_line==-1)) {
    first_line=log->size();
  }

  //
  // Iterate through it
  //
  for(unsigned i=0;i<lls.size();i++) {
    if(render_abort) {
      emit lineStarted(log->size()+render_total_passes-1,
		       log->size()+render_total_passes-1);
      *err_msg+="Render aborted.\n";
      sf_close(sf_out);
      return false;
    }
    emit lineStarted(i,log->size()+render_total_passes-1);
    if(((first_line==-1)||(first_line<=(int)i))&&
       ((last_line==-1)||(last_line>=(int)i))) {
      if(lls.at(i)->transType()==RDLogLine::Stop) {
	ProgressMessage(current_time,i,tr("STOP")+" ",lls.at(i)->summary());
	render_warnings.
	  push_back(tr("log render halted at line")+QString().sprintf(" %d ",i)+
		    tr("due to STOP"));
	break;
      }
      if(lls.at(i)->open(current_time)) {
	ProgressMessage(current_time,i,
			RDLogLine::transText(lls.at(i)->transType()),
		      QString().sprintf(" cart %06u [",lls.at(i)->cartNumber())+
			lls.at(i)->title()+"]");
	sf_count_t frames=0;
	if((lls.at(i+1)->transType()==RDLogLine::Segue)&&
	   (lls.at(i)->segueStartPoint()>=0)) {
	  if(lls.at(i)->segueStartPoint()>lls.at(i)->startPoint()) {
	    frames=FramesFromMsec(lls.at(i)->segueStartPoint()-
				  lls.at(i)->startPoint());
	    current_time=
	      current_time.addMSecs(lls.at(i)->segueStartPoint()-
				    lls.at(i)->startPoint());
	  }
	  else {
	    frames=0;
	  }
	}
	else {
	  if(lls.at(i)->endPoint()>lls.at(i)->startPoint()) {
	    frames=FramesFromMsec(lls.at(i)->endPoint()-
				  lls.at(i)->startPoint());
	    current_time=current_time.addMSecs(lls.at(i)->endPoint()-
					       lls.at(i)->startPoint());
	  }
	  else {
	    frames=0;
	  }
	}
	pcm=new float[frames*s->channels()];
	memset(pcm,0,frames*s->channels()*sizeof(float));

	for(unsigned j=0;j<i;j++) {
	  Sum(pcm,lls.at(j),frames,s->channels());
	}
	Sum(pcm,lls.at(i),frames,s->channels());
	sf_writef_float(sf_out,pcm,frames);
	delete pcm;
	pcm=NULL;
	lls.at(i)->setRamp(lls.at(i+1)->transType(),lls.at(i)->segueGain());
      }
      else {
	if(i<(lls.size()-1)) {
	  if(lls.at(i)->type()==RDLogLine::Cart) {
	    ProgressMessage(current_time,i,tr("FAIL"),lls.at(i)->summary()+
			    " ("+tr("NO AUDIO AVAILABLE")+")");
	    render_warnings.
	      push_back(lls.at(i)->summary()+tr("at line")+
			QString().sprintf(" %d ",i)+
			tr("failed to play (NO AUDIO AVAILABLE)"));
	  }
	  else {
	    ProgressMessage(current_time,i,tr("SKIP"),lls.at(i)->summary());
	  }
	}
	else {
	  ProgressMessage(current_time,lls.size()-1,
			  tr("STOP"),tr("--- end of log ---"));
	}
      }
    }
  }
  sf_close(sf_out);

  return true;
}


void RDRenderer::Sum(float *pcm_out,__RDRenderLogLine *ll,sf_count_t frames,
		     unsigned chans)
{
  if(ll->handle()!=NULL) {
    float *pcm=new float[frames*chans];

    memset(pcm,0,frames*chans);
    sf_count_t n=sf_readf_float(ll->handle(),pcm,frames);
    for(sf_count_t i=0;i<n;i+=chans) {
      double ratio=exp10(((double)i*ll->rampRate()+ll->rampLevel())/2000.0);
      for(sf_count_t j=0;j<chans;j++) {
	pcm_out[i*chans+j]+=ratio*pcm[i*chans+j];
      }
    }
    ll->setRampLevel((double)n*ll->rampRate()+ll->rampLevel());
    if(n<frames) {
      ll->close();
    }
    delete pcm;
  }
}


bool RDRenderer::ConvertAudio(const QString &srcfile,const QString &dstfile,
			      RDSettings *s,QString *err_msg)
{
  RDAudioConvert::ErrorCode err_code;

  RDAudioConvert *conv=new RDAudioConvert(this);
  conv->setSourceFile(srcfile);
  conv->setDestinationFile(dstfile);
  conv->setDestinationSettings(s);
  err_code=conv->convert();
  *err_msg=RDAudioConvert::errorText(err_code);
  delete conv;

  return err_code==RDAudioConvert::ErrorOk;
}


bool RDRenderer::ImportCart(const QString &srcfile,unsigned cartnum,int cutnum,
			    unsigned chans,QString *err_msg)
{
  RDAudioImport::ErrorCode err_import_code;
  RDAudioConvert::ErrorCode err_conv_code;
  RDSettings settings;
  
  settings.setChannels(chans);
  settings.setNormalizationLevel(0);

  RDAudioImport *conv=new RDAudioImport(this);
  conv->setCartNumber(cartnum);
  conv->setCutNumber(cutnum);
  conv->setSourceFile(srcfile);
  conv->setUseMetadata(false);
  conv->setDestinationSettings(&settings);
  err_import_code=
    conv->runImport(rda->user()->name(),rda->user()->password(),&err_conv_code);
  *err_msg=RDAudioImport::errorText(err_import_code,err_conv_code);
  delete conv;
  return err_import_code==RDAudioImport::ErrorOk;
}


void RDRenderer::DeleteTempFile(const QString &filename) const
{
  unlink(filename);
  QStringList f0=filename.split("/");
  f0.erase(f0.fromLast());
  rmdir("/"+f0.join("/"));
}


uint64_t RDRenderer::FramesFromMsec(uint64_t msec) const
{
  return msec*rda->system()->sampleRate()/1000;
}


void RDRenderer::ProgressMessage(const QString &msg)
{
  emit progressMessageSent(msg);
}


void RDRenderer::ProgressMessage(const QTime &time,int line,
				 const QString &trans,const QString &msg)
{
  QString str=QString().sprintf("%04d : ",line)+
    time.toString("hh:mm:ss")+" : "+
    QString().sprintf("%-5s",(const char *)trans)+msg;
  emit progressMessageSent(str);
}
