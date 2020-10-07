// logdialog.h
//
// Real-only lister dialogs for Rivendell logs
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <qpushbutton.h>
#include <qtableview.h>

#include <rddialog.h>
#include <rdlog_event.h>

#include "logmodel.h"

class LogDialog : public RDDialog
{
  Q_OBJECT
 public:
  LogDialog(QWidget *parent=0);
  ~LogDialog();
  QSize sizeHint() const;
  
 public slots:
  int exec(RDLogEvent *log,int *start_line,int *end_line);

 private slots:
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QTableView *d_log_view;
  QPushButton *d_ok_button;
  QPushButton *d_cancel_button;
  LogModel *d_log_model;
  RDLogEvent *d_log;
  int *d_start_line;
  int *d_end_line;
};


#endif  // LIST_CASTS_H
