// edit_track.h
//
// Edit a Rivendell Voice Track Log Entry
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_track.h,v 1.8 2010/07/29 19:32:37 cvs Exp $
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

#ifndef EDIT_TRACK_H
#define EDIT_TRACK_H

#include <qdialog.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <rdlog_line.h>
#include <rdtimeedit.h>

class EditTrack : public QDialog
{
  Q_OBJECT
 public:
  EditTrack(RDLogLine *,QWidget *parent=0,const char *name=0);
  ~EditTrack();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void timeChangedData(const QTime &);
  void timeToggledData(bool state);
  void graceClickedData(int id);
  void selectTimeData(int);
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  RDLogLine *edit_logline;
  RDTimeEdit *edit_time_edit;
  QLabel *edit_time_label;
  QCheckBox *edit_timetype_box;
  QLabel *edit_timetype_label;
  QCheckBox *edit_time_box;
  QComboBox *edit_transtype_box;
  QCheckBox *edit_overlap_box;
  QLabel *edit_overlap_label;
  QLineEdit *edit_comment_edit;
  QButtonGroup *edit_grace_group;
  QFont normal_font;
  QTimeEdit *edit_grace_box;
  QLabel *edit_grace_label;
};


#endif

