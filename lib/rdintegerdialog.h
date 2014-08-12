// rdintegerdialog.h
//
// A widget to set an integer value.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdintegerdialog.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDINTEGERDIALOG_H
#define RDINTEGERDIALOG_H

#include <qdialog.h>
#include <qspinbox.h>

class RDIntegerDialog : public QDialog
{
 Q_OBJECT
 public:
  RDIntegerDialog(int *value,const QString &lbl,int low,int high,
		  QWidget *parent=0,const char *name=0);
  ~RDIntegerDialog();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  QSpinBox *int_value_box;
  int *int_value;
};


#endif
