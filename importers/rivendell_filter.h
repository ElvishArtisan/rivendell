// rivendell_filter.h
//
// A Library import filter for an external Rivendell system
//
//   (C) Copyright 2002-2005, 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rivendell_filter.h,v 1.3 2010/07/29 19:32:32 cvs Exp $
//      $Date: 2010/07/29 19:32:32 $
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

#ifndef RIVENDELL_FILTER_H
#define RIVENDELL_FILTER_H

#include <stdio.h>

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#define RIVENDELL_FILTER_USAGE " -h <hostname> -u <username> -p <password> -A <audio-dir> -g <default-group> -s <start-cartnum> -e <end-cartnum>"

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);
};


#endif  // RIVENDELL_FILTER_H
