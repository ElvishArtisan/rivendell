// log_traffic.h
//
// Add an entry to the reconciliation table.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: log_traffic.h,v 1.9 2010/07/29 19:32:36 cvs Exp $
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

#ifndef LOG_TRAFFIC_H
#define LOG_TRAFFIC_H

#include <rdlog_line.h>
#include <rdairplay_conf.h>


void LogTraffic(const QString &svcname,const QString &logname,
		RDLogLine *logline,RDLogLine::PlaySource src,
		RDAirPlayConf::TrafficAction action,bool onair_flag);


#endif  // LOG_TRAFFIC_H
