// post_counter.h
//
// The post counter widget for Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: post_counter.h,v 1.13 2010/07/29 19:32:36 cvs Exp $
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

#ifndef POST_COUNTER_H
#define POST_COUNTER_H

#include <qpushbutton.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <rdairplay_conf.h>
//
// Settings
//
#define POST_COUNTER_MARGIN 1000

class PostCounter : public QPushButton
{
 Q_OBJECT
 public:
  PostCounter(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setTimeMode(RDAirPlayConf::TimeMode mode);
  
 public slots:
  void setPostPoint(QTime point,int offset,bool offset_valid,bool running);
  void tickCounter();
  void setEnabled(bool state);
  void setDisabled(bool state);

 protected:
  void keyPressEvent(QKeyEvent *e);

 private:
  void UpdateDisplay();
  QTime post_time;
  int post_offset;
  bool post_offset_valid;
  bool post_running;
  QFont post_large_font;
  QFont post_small_font;
  QPalette post_early_palette;
  QPalette post_ontime_palette;
  QPalette post_late_palette;
  QPalette post_idle_palette;
  QString post_time_format;
  QTime post_set_time;
};

#endif
