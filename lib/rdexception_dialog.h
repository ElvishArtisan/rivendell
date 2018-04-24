// rdexception_dialog.h
//
// A dialog for displaying exception reports.
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEXCEPTION_DIALOG_H
#define RDEXCEPTION_DIALOG_H

#include <qdialog.h>
#include <q3textview.h>

class RDExceptionDialog : public QDialog
{
 Q_OBJECT
 public:
  RDExceptionDialog(QString report,QWidget *parent=0);
  ~RDExceptionDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void saveData();
  void closeData();

 private:
  Q3TextView *report_view;
};


#endif
