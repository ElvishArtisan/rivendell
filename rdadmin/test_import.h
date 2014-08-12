// test_import.h
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: test_import.h,v 1.6 2010/07/29 19:32:35 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qdatetimeedit.h>
#include <qlineedit.h>

#include <rdsvc.h>
#include <rdlistview.h>


class TestImport : public QDialog
{
 Q_OBJECT
 public:
  TestImport(RDSvc *svc,RDSvc::ImportSource src,
	     QWidget *parent=0,const char *name=0);
  ~TestImport();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectData();
  void importData();
  void dateChangedData(const QDate &date);
  void closeData();

 protected:
  void paintEvent(QPaintEvent *e);

 private:
  RDSvc *test_svc;
  RDSvc::ImportSource test_src;
  QDateEdit *test_date_edit;
  RDListView *test_events_list;
  QLineEdit *test_filename_edit;
};


#endif

