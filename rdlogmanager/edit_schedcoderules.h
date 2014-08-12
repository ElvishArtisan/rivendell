// edit_schedcoderules.h
//
// Change rules for scheduler codes dialog
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



#ifndef EDIT_SCHEDCODERULES_H
#define EDIT_SCHEDCODERULES_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qvariant.h>

#include <schedruleslist.h>
#include <clock_listview.h>

class QSpinBox;

class editSchedCodeRules : public QDialog
{
    Q_OBJECT

public:
    editSchedCodeRules(QListViewItem *item, SchedRulesList *sched_rules_list, QWidget* parent = 0, const char* name = 0);
    ~editSchedCodeRules();
    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;

private:
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* label_code_name;
    QLabel* label_max_row;
    QLabel* label_min_wait;
    QLabel* label_not_after;
    QLabel* label_or_after;
    QLabel* label_or_after_II;
    QSpinBox* spinBox_max_row;
    QSpinBox* spinBox_min_wait;
    QLabel* label_code;
    QComboBox* comboBox_not_after;
    QComboBox* comboBox_or_after;
    QComboBox* comboBox_or_after_II;
    QLabel* label_description;
    QListViewItem *item_edit; 
 
 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
};

#endif

