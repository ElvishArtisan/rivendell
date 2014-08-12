// status_tip.cpp
//
// Custom ToolTip for RDMonitor's Status Bubble
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: status_tip.cpp,v 1.1.2.2 2012/10/22 23:09:39 cvs Exp $
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

#include <qobject.h>

#include <dbversion.h>

#include <status_tip.h>

StatusTip::StatusTip(QWidget *widget,QToolTipGroup *group)
  : QToolTip(widget,group)
{
  setStatus(QRect(),true,0,true);
}


void StatusTip::setStatus(const QRect &rect,bool db_status,int schema,
			  bool snd_status)
{
  tip_rect=rect;
  if(db_status&&(schema==RD_VERSION_DATABASE)&&snd_status) {
    tip_text=QObject::tr("Status: OK");
  }
  else {
    tip_text="";
    if(!db_status) {
      tip_text=QObject::tr("Database: CONNECTION FAILED");
    }
    else {
      if(schema!=RD_VERSION_DATABASE) {
	tip_text=QObject::tr("Database: SCHEMA SKEWED");
      }
    }
    if(!snd_status) {
      if(!tip_text.isEmpty()) {
	tip_text+="\n";
      }
      tip_text+=QObject::tr("Audio Store: FAILED");
    }
  }
}


void StatusTip::maybeTip(const QPoint &pt)
{
  tip(tip_rect,tip_text);
}
