// edit_clock.h
//
// Edit A Rivendell Log Clock
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_clock.h,v 1.14 2010/07/29 19:32:37 cvs Exp $
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

#ifndef EDIT_CLOCK_H
#define EDIT_CLOCK_H

#include <vector>

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qvariant.h>
#include <qtextedit.h>

#include <rdclock.h>

#include <clock_listview.h>
#include <schedruleslist.h>

//
// Layout
//
#define CENTER_LINE 400
#define PIE_X_MARGIN 100
#define PIE_Y_MARGIN 125

class EditClock : public QDialog
{
 Q_OBJECT
 public:
 EditClock(QString clockname,bool new_clock,std::vector<QString> *new_clocks,
	    QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectionChangedData(QListViewItem *);
  void addData();
  void editData();
  void deleteData();
  void svcData();
//
  void schedRules();
//
  void saveData();
  void saveAsData();
  void doubleClickedData(QListViewItem *,const QPoint &,int);
  void colorData();
  void editEventData(int line);
  void okData();
  void cancelData();

 protected:
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void Save();
  void RefreshList(int select_line=-1);
  void RefreshNames();
  void UpdateClock(int line=-1);
  void CopyClockPerms(QString old_name,QString new_name);
  void AbandonClock(QString name);
  bool ValidateCode();
  ClockListView *edit_clocks_list;
  RDClock *edit_clock;
  QPushButton *edit_color_button;
  QLabel *edit_clockname_label;
  QLineEdit *edit_shortname_edit;
  QLabel *edit_clock_label;
  QFont *edit_title_font;
  QFontMetrics *edit_title_metrics;
  bool edit_modified;
  QString edit_name;
  bool edit_new_clock;
  std::vector<QString> *edit_new_clocks;
  SchedRulesList* sched_rules_list;
  QTextEdit *edit_remarks_edit;
};


#endif

