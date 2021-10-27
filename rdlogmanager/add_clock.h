// add_clock.h
//
// Add a Rivendell Log Manager Clock
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef ADD_CLOCK_H
#define ADD_CLOCK_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdclock.h>
#include <rddialog.h>

class AddClock : public RDDialog
{
  Q_OBJECT
 public:
  AddClock(QWidget *parent=0);
  ~AddClock();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(QString *clk_name,QString *clk_code);

 private slots:
  void clockNameChangedData(const QString &str);
  void textChangedData(const QString &str);
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *clock_name_label;
  QLineEdit *clock_name_edit;
  QLabel *clock_code_label;
  QLineEdit *clock_code_edit;
  QString *clock_name;
  QString *clock_code;
  QPushButton *clock_ok_button;
  QPushButton *clock_cancel_button;
};


#endif  // ADD_CLOCK_H
