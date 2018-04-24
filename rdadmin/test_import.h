// test_import.h
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef TEST_IMPORT_H
#define TEST_IMPORT_H

#include <QDateTimeEdit>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rdsvc.h>
#include <rdtablewidget.h>

class TestImport : public QDialog
{
 Q_OBJECT
 public:
  TestImport(RDSvc *svc,RDSvc::ImportSource src,QWidget *parent=0);
  ~TestImport();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectData();
  void importData();
  void dateChangedData(const QDateTime &dt);
  void closeData();

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void AddLine(int line);
  RDSvc *test_svc;
  RDSvc::ImportSource test_src;
  QLabel *test_date_label;
  QDateTimeEdit *test_date_edit;
  QPushButton *test_import_button;
  QLabel *test_events_label;
  RDTableWidget *test_events_widget;
  QLabel *test_filename_label;
  QLineEdit *test_filename_edit;
  QPushButton *test_close_button;
};


#endif

