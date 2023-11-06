// add_schedcodes.h
//
// Add scheduler codes dialog
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_SCHEDCODES_H
#define ADD_SCHEDCODES_H

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include <rddialog.h>

class AddSchedCode : public RDDialog
{
  Q_OBJECT
 public:
  AddSchedCode(QWidget *parent=0);
  ~AddSchedCode();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *scode);

 protected:
  void resizeEvent(QResizeEvent *e);

 private slots:
  void codeChangedData(const QString &str);
  void okData();
  void cancelData();

 private:
  QLabel *d_code_label;
  QLineEdit *d_code_edit;
  QPushButton *d_ok_button;
  QPushButton *d_cancel_button;
  QString *d_sched_code;
};

#endif  // ADD_SCHEDCODES_H
