// edit_event.h
//
// Event Editor for RDAirPlay
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_event.h,v 1.25.8.2.2.1 2014/05/20 01:45:17 cvs Exp $
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

#ifndef EDIT_EVENT_H
#define EDIT_EVENT_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qdatetimeedit.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qtimer.h>

#include <rdtransportbutton.h>
#include <rdslider.h>
#include <rdcae.h>
#include <rdplay_deck.h>
#include <rdmarker_edit.h>
#include <rdpushbutton.h>
#include <rdtimeedit.h>
#include <rdcueedit.h>

#include <log_play.h>

class EditEvent : public QDialog
{
  Q_OBJECT
 public:
  EditEvent(LogPlay *log,QWidget *parent=0,const char *name=0);
  ~EditEvent();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(int line);

 private slots:
  void timeChangedData(const QTime &);
  void timeToggledData(bool state);
  void graceClickedData(int id);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  LogPlay *edit_log;
  RDLogLine *edit_logline;
  int edit_line;
  RDTimeEdit *edit_time_edit;
  bool edit_time_changed;
  QCheckBox *edit_timetype_box;
  QLabel *edit_timetype_label;
  QButtonGroup *edit_grace_group;
  RDTimeEdit *edit_grace_edit;
  QLabel *edit_time_label;
  QComboBox *edit_transtype_box;
  QLabel *edit_transtype_label;
  QCheckBox *edit_overlap_box;
  QLabel *edit_overlap_label;
  RDCueEdit *edit_cue_edit;
  QFont normal_font;
  QLabel *edit_cart_notes_label;
  QTextEdit *edit_cart_notes_text;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  QLabel *edit_horizrule_label;
  int edit_height;
};


#endif  // EDIT_EVENT_H
