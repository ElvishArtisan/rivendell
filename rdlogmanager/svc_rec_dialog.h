// svc_rec_dialog.h
//
// A Services/Reports Management Dialog.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: svc_rec_dialog.h,v 1.6 2010/07/29 19:32:37 cvs Exp $
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


#ifndef DELETE_SVC_REC_DIALOG_H
#define DELETE_SVC_REC_DIALOG_H

#include <qdialog.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qlabel.h>

#include <svc_rec.h>


class SvcRecDialog : public QDialog
{
 Q_OBJECT
 public:
  SvcRecDialog(const QString &svcname,QWidget *parent=0,
		     const char *name=0);
  ~SvcRecDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void dateSelectedData(const QDate &,bool active);
//  void generateData();
  void deleteData();
  void closeData();

 private:
  SvcRec *date_picker;
  QDate *date_date;
//  QPushButton *date_generate_button;
  QPushButton *date_delete_button;
};


#endif  // SVC_REC_DIALOG_H
