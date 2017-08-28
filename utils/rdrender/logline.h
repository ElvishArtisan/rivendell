// logline.h
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

#ifndef LOGLINE_H
#define LOGLINE_H

#include <stdint.h>

#include <sndfile.h>

#include <rdcart.h>
#include <rdconfig.h>
#include <rdcut.h>
#include <rdlog_line.h>
#include <rdstation.h>
#include <rdsystem.h>

class LogLine : public RDLogLine
{
 public:
  LogLine(RDLogLine *ll,RDUser *user,RDStation *station,RDSystem *sys,
	  RDConfig *config,unsigned chans);
  RDCart *cart() const;
  RDCut *cut() const;
  SNDFILE *handle() const;
  double rampLevel() const;
  void setRampLevel(double lvl);
  double rampRate() const;
  void setRampRate(double lvl);
  void setRamp(RDLogLine::TransType next_trans);
  bool open(const QTime &time);
  void close();
  QString summary() const;

 private:
  bool GetCutFile(const QString &cutname,int start_pt,int end_pt,
		  QString *dest_filename) const;
  void DeleteCutFile(const QString &dest_filename) const;
  uint64_t FramesFromMsec(uint64_t msec);
  RDCart *ll_cart;
  RDCut *ll_cut;
  SNDFILE *ll_handle;
  RDLogLine *ll_logline;
  RDUser *ll_user;
  RDStation *ll_station;
  RDSystem *ll_system;
  RDConfig *ll_config;
  unsigned ll_channels;
  double ll_ramp_level;
  double ll_ramp_rate;
};


#endif  // LOGLINE_H
