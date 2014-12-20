// markerset.h
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

#ifndef MARKERSET_H
#define MARKERSET_H

#include <rdcmd_switch.h>

class MarkerSet
{
 public:
  MarkerSet();
  bool hasStartValue() const;
  int startValue(int lo_limit=-1,int hi_limit=-1) const;
  bool hasEndValue() const;
  int endValue(int lo_limit=-1,int hi_limit=-1) const;
  bool hasFadeValue() const;
  int fadeValue(int lo_limit=-1,int hi_limit=-1) const;
  void loadMarker(RDCmdSwitch *cmd,const QString &marker);
  void loadFade(RDCmdSwitch *cmd,const QString &marker);
  void setAudioLength(int msecs);
  void dump();

 private:
  int LimitCheck(int value,int lo_limit,int hi_limit) const;
  int FrontReference(int value) const;
  QString marker_marker;
  bool marker_start_valid;
  int marker_start_value;
  bool marker_end_valid;
  int marker_end_value;
  bool marker_fade_valid;
  int marker_fade_value;
  int marker_audio_length;
};


#endif  // MARKERSET_H
