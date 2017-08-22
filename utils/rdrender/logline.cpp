// logline.cpp
//
// Container class for Rivendell Log Line.
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

#include "logline.h"

#include <rd.h>
#include <rdaudioconvert.h>
#include <rdaudioexport.h>
#include <rdconf.h>

LogLine::LogLine(RDLogLine *ll,RDUser *user,RDStation *station,RDSystem *sys,
		 RDConfig *config,unsigned chans)
  : RDLogLine(*ll)
{
  ll_cart=NULL;
  ll_cut=NULL;
  ll_handle=NULL;
  ll_user=user;
  ll_station=station;
  ll_system=sys;
  ll_config=config;
  ll_channels=chans;
  ll_ramp_level=0.0;
  ll_ramp_rate=0.0;
}


RDCart *LogLine::cart() const
{
  return ll_cart;
}


RDCut *LogLine::cut() const
{
  return ll_cut;
}


SNDFILE *LogLine::handle() const
{
  return ll_handle;
}


double LogLine::rampLevel() const
{
  return ll_ramp_level;
}


void LogLine::setRampLevel(double lvl)
{
  ll_ramp_level=lvl;
}


double LogLine::rampRate() const
{
  return ll_ramp_rate;
}


void LogLine::setRampRate(double lvl)
{
  ll_ramp_rate=lvl;
}


void LogLine::setRamp(RDLogLine::TransType next_trans)
{
  if((next_trans==RDLogLine::Segue)&&
     (ll_cut->segueStartPoint()>=0)&&(ll_cut->segueEndPoint()>=0)) {
    ll_ramp_rate=((double)RD_FADE_DEPTH)/((double)FramesFromMsec(ll_cut->segueEndPoint()-ll_cut->segueStartPoint()));
  }
}


bool LogLine::open(const QTime &time)
{
  QString cutname;
  SF_INFO sf_info;

  if(type()==RDLogLine::Cart) {
    ll_cart=new RDCart(cartNumber());
    if(ll_cart->exists()&&(ll_cart->type()==RDCart::Audio)) {
      if(ll_cart->selectCut(&cutname,time)) {
	ll_cut=new RDCut(cutname);
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


void LogLine::close()
{
  sf_close(ll_handle);
  ll_handle=NULL;
}


bool LogLine::GetCutFile(const QString &cutname,int start_pt,int end_pt,
			 QString *dest_filename) const
{
  bool ret=false;
  RDAudioConvert::ErrorCode conv_err;
  RDAudioExport::ErrorCode export_err;
  char tempdir[PATH_MAX];
  
  strncpy(tempdir,RDTempDir()+"/rdrenderXXXXXX",PATH_MAX);
  *dest_filename=QString(mkdtemp(tempdir))+"/"+cutname+".wav";
  RDAudioExport *conv=new RDAudioExport(ll_station,ll_config);
  conv->setDestinationFile(*dest_filename);
  conv->setCartNumber(RDCut::cartNumber(cutname));
  conv->setCutNumber(RDCut::cutNumber(cutname));
  RDSettings s;
  s.setFormat(RDSettings::Pcm16);
  s.setSampleRate(ll_system->sampleRate());
  s.setChannels(ll_channels);
  s.setNormalizationLevel(0);
  conv->setDestinationSettings(&s);
  conv->setRange(start_pt,end_pt);
  conv->setEnableMetadata(false);
  switch(export_err=conv->runExport(ll_user->name(),
				    ll_user->password(),&conv_err)) {
  case RDAudioExport::ErrorOk:
    ret=true;
    break;

  default:
    ret=false;
    printf("export err %d [%s]\n",export_err,
	   (const char *)RDAudioExport::errorText(export_err,conv_err));
    break;
  }

  delete conv;
  return ret;
}


void LogLine::DeleteCutFile(const QString &dest_filename) const
{
  unlink(dest_filename);
  QStringList f0=f0.split("/",dest_filename);
  f0.erase(f0.fromLast());
  rmdir("/"+f0.join("/"));
}


uint64_t LogLine::FramesFromMsec(uint64_t msec)
{
  return msec*ll_system->sampleRate()/1000;
}
