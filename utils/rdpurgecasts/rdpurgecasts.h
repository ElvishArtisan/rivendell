// rdpurgecasts.h
//
// A Utility to Purge Expired Podcasts.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpurgecasts.h,v 1.3 2010/07/29 19:32:40 cvs Exp $
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


#ifndef RDPURGECASTS_H
#define RDPURGECASTS_H

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rdcmd_switch.h>

#define RDPURGECASTS_USAGE "[--help] [--verbose]\n\nPurge expired podcasts.\n"


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void PurgeCast(unsigned id);
  RDConfig *purge_config;
  bool purge_verbose;
  RDCmdSwitch *purge_cmd;
};


#endif  // RDPURGECASTS_H
