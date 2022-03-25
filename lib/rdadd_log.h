// rdadd_log.h
//
// Create a Rivendell Log
//
// This class creates a basic dialog requesting from the user a name and
// corresponding service that is later used to create a new log.
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

#ifndef ADD_LOG_H
#define ADD_LOG_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

#include <rddialog.h>
#include <rdlog.h>
#include <rdlogfilter.h>

class RDAddLog : public RDDialog
{
  Q_OBJECT
 public:
  RDAddLog(QString *logname,QString *svcname,RDLogFilter::FilterMode mode,
	   const QString &caption,QWidget *parent=0);
  ~RDAddLog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();
  void nameChangedData(const QString &str);

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QLineEdit *add_name_edit;
  QComboBox *add_service_box;
  QPushButton *add_ok_button;
  QPushButton *add_cancel_button;
  QString *log_name;
  QString *log_svc;
};


#endif  // RDADD_LOG_H

