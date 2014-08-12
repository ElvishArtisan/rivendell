// rdgain_envelope.cpp
//
// A Container Class for Rivendell Event Segue Parameters
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgain_envelope.cpp,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#include <rd.h>
#include <rdgain_envelope.h>


RDGainEnvelope::RDGainEnvelope()
{
  clear();
}


int RDGainEnvelope::startPoint() const
{
  return env_start_point;
}


void RDGainEnvelope::setStartPoint(int point)
{
  env_start_point=point;
}


int RDGainEnvelope::fadeupPoint() const
{
  return env_fadeup_point;
}


void RDGainEnvelope::setFadeupPoint(int point)
{
  env_fadeup_point=point;
}


int RDGainEnvelope::fadeupGain() const
{
  return env_fadeup_gain;
}


void RDGainEnvelope::setFadeupGain(int gain)
{
  env_fadeup_gain=gain;
}


int RDGainEnvelope::segueStartPoint() const
{
  return env_segue_start_point;
}


void RDGainEnvelope::setSegueStartPoint(int point)
{
  env_segue_start_point=point;
}


int RDGainEnvelope::segueEndPoint() const
{
  return env_segue_end_point;
}


void RDGainEnvelope::setSegueEndPoint(int point)
{
  env_segue_end_point=point;
}


int RDGainEnvelope::segueGain() const
{
  return env_segue_gain;
}


void RDGainEnvelope::setSegueGain(int gain)
{
  env_segue_gain=gain;
}


void RDGainEnvelope::clear()
{
  env_start_point=-1;
  env_fadeup_point=-1;
  env_fadeup_gain=RD_MUTE_DEPTH;
  env_segue_start_point=-1;
  env_segue_end_point=-1;
  env_segue_gain=RD_MUTE_DEPTH;
}
