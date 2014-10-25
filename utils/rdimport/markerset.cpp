// markerset.cpp
//
// Abstract a set of marker parameters.
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdlib.h>

#include "markerset.h"

MarkerSet::MarkerSet()
{
  marker_start_valid=false;
  marker_start_value=0;
  marker_end_valid=false;
  marker_end_value=0;
  marker_fade_valid=false;
  marker_fade_value=0;
  marker_audio_length=0;
}


bool MarkerSet::hasStartValue() const
{
  return marker_start_valid;
}


int MarkerSet::startValue(int lo_limit,int hi_limit) const
{
  return LimitCheck(FrontReference(marker_start_value),lo_limit,hi_limit);
}


bool MarkerSet::hasEndValue() const
{
  return marker_end_valid;
}


int MarkerSet::endValue(int lo_limit,int hi_limit) const
{
  return LimitCheck(FrontReference(marker_end_value),lo_limit,hi_limit);
}


bool MarkerSet::hasFadeValue() const
{
  return marker_fade_valid;
}


int MarkerSet::fadeValue(int lo_limit,int hi_limit) const
{
  return LimitCheck(FrontReference(marker_fade_value),lo_limit,hi_limit);
}


void MarkerSet::loadMarker(RDCmdSwitch *cmd,const QString &marker)
{
  QString start_key="--set-marker-start-"+marker;
  QString end_key="--set-marker-end-"+marker;
  marker_marker=marker;
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)==start_key) {
      marker_start_value=cmd->value(i).toInt(&marker_start_valid);
      if(!marker_start_valid) {
	fprintf(stderr,"rdimport: invalid argment to %s\n",
		(const char *)start_key);
	exit(255);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)==end_key) {
      marker_end_value=cmd->value(i).toInt(&marker_end_valid);
      if(!marker_end_valid) {
	fprintf(stderr,"rdimport: invalid argment to %s\n",
		(const char *)end_key);
	exit(255);
      }
      cmd->setProcessed(i,true);
    }
  }
  if(marker_end_valid&&(!marker_start_valid)) {
    marker_start_value=0;
    marker_start_valid=true;
  }
}


void MarkerSet::loadFade(RDCmdSwitch *cmd,const QString &marker)
{
  QString key="--set-marker-"+marker;
  marker_marker=marker;
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)==key) {
      marker_fade_value=cmd->value(i).toInt(&marker_fade_valid);
      if(!marker_fade_valid) {
	fprintf(stderr,"rdimport: invalid argment to %s\n",
		(const char *)key);
	exit(255);
      }
      cmd->setProcessed(i,true);
    }
  }
}


void MarkerSet::setAudioLength(int msecs)
{
  if(marker_start_valid&&(!marker_end_valid)) {
    marker_end_value=msecs;
    marker_end_valid=true;
  }
  marker_audio_length=msecs;
}


void MarkerSet::dump()
{
  if(marker_start_valid) {
    printf(" Marker Start %s: ",(const char *)marker_marker);
    printf("%d mS\n",marker_start_value);
  }
  if(marker_end_valid) {
  printf(" Marker End %s: ",(const char *)marker_marker);
    printf("%d mS\n",marker_end_value);
  }
}


int MarkerSet::LimitCheck(int value,int lo_limit,int hi_limit) const
{
  if(lo_limit!=-1) {
    if(value<lo_limit) {
      return lo_limit;
    }
  }
  if(hi_limit!=-1) {
    if(value>hi_limit) {
      return hi_limit;
    }
  }
  return value;
}


int MarkerSet::FrontReference(int value) const
{
  if(value>=0) {
    if(value>marker_audio_length) {
      return marker_audio_length;
    }
    return value;
  }
  if((marker_audio_length+value)<0) {
    return 0;
  }
  return marker_audio_length+value;
}
