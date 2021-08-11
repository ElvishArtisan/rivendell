// rdlogeventdialog.h
//
// Base class for rdlogedit(1) event editor dialogs
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGEVENTDIALOG_H
#define RDLOGEVENTDIALOG_H

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

#include <rddialog.h>
#include <rdlog_line.h>
#include <rdtimeedit.h>

class RDLogEventDialog : public RDDialog
{
  Q_OBJECT
 protected:
  RDLogEventDialog(RDLogLine *,QWidget *parent=0);

 private slots:
  void timeChangedData(const QTime &);
  void timeToggledData(bool state);
  void graceClickedData(int id);
  void selectTimeData(int);
  void okData();
  void cancelData();

 protected:
  RDLogLine *logLine();
  virtual bool saveData()=0;
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  RDLogLine *edit_logline;
  RDTimeEdit *edit_time_edit;
  QCheckBox *edit_timetype_box;
  QLabel *edit_timetype_label;
  QCheckBox *edit_time_box;
  QLabel *edit_transtype_label;
  QComboBox *edit_transtype_box;
  QButtonGroup *edit_grace_bgroup;
  QGroupBox *edit_grace_group;
  QTimeEdit *edit_grace_timeedit;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // RDLOGEVENTDIALOG_H
