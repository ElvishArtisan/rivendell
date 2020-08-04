// render_dialog.h
//
// Log Rendering Dialog for rdcastmanager(1)
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

#ifndef RENDER_DIALOG_H
#define RENDER_DIALOG_H

#include <qdatetimeedit.h>
#include <qprogressdialog.h>

#include <rdcut_dialog.h>
#include <rddialog.h>
#include <rdlog_event.h>

#include "logdialog.h"

//
// Widget Settings
//
#define IMPORT_BAR_INTERVAL 500
#define IMPORT_TEMP_BASENAME "rdlib"

class RenderDialog : public RDDialog
{
 Q_OBJECT
 public:
  RenderDialog(QWidget *parent=0);
  ~RenderDialog();
  QSize sizeHint() const;

 public slots:
  int exec(RDLogEvent *log,QTime *start_time,bool *ignore_stops,
	   int *start_line,int *end_line);

 private slots:
  void startTimeActivatedData(int n);
  void selectData();
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateLogEventsSelection();
  QLabel *d_start_time_label;
  QComboBox *d_start_time_box;
  QTimeEdit *d_start_time_edit;
  QLabel *d_ignorestop_label;
  QComboBox *d_ignorestop_box;
  QLabel *d_select_label_label;
  QLabel *d_select_label;
  QPushButton *d_select_button;
  QPushButton *d_ok_button;
  QPushButton *d_cancel_button;

  LogDialog *d_log_dialog;

  int d_modified_start_line;
  int d_modified_end_line;

  QTime *d_start_time;
  bool *d_ignore_stops;
  int *d_start_line;
  int *d_end_line;
  RDLogEvent *d_log;
};


#endif  // RDRENDER_DIALOG_H
