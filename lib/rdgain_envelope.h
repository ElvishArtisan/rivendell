// rdgain_envelope.h
//
// A Container Class for Rivendell Event Segue Parameters
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgain_envelope.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDGAIN_ENVELOPE_H
#define RDGAIN_ENVELOPE_H

#include <qstring.h>

class RDGainEnvelope
{
 public:
  RDGainEnvelope();
  int startPoint() const;
  void setStartPoint(int point);
  int fadeupPoint() const;
  void setFadeupPoint(int point);
  int fadeupGain() const;
  void setFadeupGain(int gain);
  int segueStartPoint() const;
  void setSegueStartPoint(int point);
  int segueEndPoint() const;
  void setSegueEndPoint(int point);
  int segueGain() const;
  void setSegueGain(int gain);
  void clear();

 private:
  int env_start_point;
  int env_fadeup_point;
  int env_fadeup_gain;
  int env_segue_start_point;
  int env_segue_end_point;
  int env_segue_gain;
};


#endif  // RDGAIN_ENVELOPE_H
