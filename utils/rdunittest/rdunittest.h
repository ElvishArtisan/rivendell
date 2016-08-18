// rdunittest.h
//
// Unit testing utility for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDUNITTEST_H
#define RDUNITTEST_H

#include <QTextEdit>
#include <QWidget>

#include <rdunittestdata.h>

#define RDUNITTEST_USAGE "[--db-hostname=<hostname>] [--db-database=<db-name>] [--db-username=<username>] [--db-password=<password>]\n\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
   MainWidget(QWidget *parent=0);
   ~MainWidget();
   QSize sizeHint() const;

 public slots:
  void runTests();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void SanityCheck();
  void Append(RDUnitTestData *data);
  void Append(const QString &str);
  void Append(const char *str);
  QTextEdit *main_edit;
  unsigned main_pass_count;
  unsigned main_fail_count;

  bool LowLevelDbTests(RDUnitTestData *data);
  bool MojibakeTests(RDUnitTestData *data);
};


#endif  // MAIN_WIDGET_H
