// edit_schedrules.h
//
// Edit scheduler rules of a clock
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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

#ifndef EDIT_SCHEDRULES_H
#define EDIT_SCHEDRULES_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>
#include <qvariant.h>

#include <schedruleslist.h>

class QSpinBox;

class EditSchedRules : public QDialog
{
  Q_OBJECT
 public:
  EditSchedRules(QString clock,unsigned *artistsep,SchedRulesList *schedruleslist,bool *rules_modified,QWidget *parent=0,const char *name=0);
  ~EditSchedRules();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void editData();
  void importData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void Load();
  void Close();
  QLabel* artistSepLabel;
  QSpinBox* artistSepSpinBox;
  QListView *list_schedCodes_view;
  QString clockname;
  unsigned* edit_artistsep;
  bool* edit_rules_modified;
  SchedRulesList* sched_rules_list;
  bool edit_modified;
};



#endif

