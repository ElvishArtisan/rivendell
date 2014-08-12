// status_tip.h
//
// Custom ToolTip for RDMonitor's Status Bubble
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: status_tip.h,v 1.1.2.2 2012/10/22 23:09:39 cvs Exp $
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

#ifndef STATUS_TIP_H
#define STATUS_TIP_H

#include <qtooltip.h>

class StatusTip : public QToolTip
{
 public:
  StatusTip(QWidget *widget,QToolTipGroup *group=0);
  void setStatus(const QRect &rect,bool db_status,int schema,bool snd_status);

 protected:
  void maybeTip(const QPoint &pt);

 private:
  QString tip_text;
  QRect tip_rect;
};

#endif  // STATUS_TIP_H
