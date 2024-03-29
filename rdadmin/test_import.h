// test_import.h
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <rddateedit.h>
#include <rddialog.h>
#include <rdlogimportmodel.h>
#include <rdsvc.h>
#include <rdtableview.h>

class TestImport : public RDDialog
{
 Q_OBJECT
 public:
  TestImport(QWidget *parent=0);
  ~TestImport();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec(RDSvc *svc,RDSvc::ImportSource src);

 private slots:
  void selectData();
  void importData();
  void dateChangedData(const QDate &date);
  void closeData();

 protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  RDSvc *test_svc;
  RDSvc::ImportSource test_src;
  QLabel *test_date_label;
  RDDateEdit *test_date_edit;
  QPushButton *test_import_button;
  QLabel *test_events_label;
  RDTableView *test_events_view;
  RDLogImportModel *test_events_model;
  QLabel *test_filename_label;
  QLineEdit *test_filename_edit;
  QPushButton *test_close_button;
};


#endif  // TEST_IMPORT_H
