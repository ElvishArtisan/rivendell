// edit_hostvar.h
//
// Edit a Rivendell Host Variable
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_hostvar.h,v 1.6 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_HOSTVAR_H
#define EDIT_HOSTVAR_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>

#include <rdstation.h>
#include <rdcatch_connect.h>


class EditHostvar : public QDialog
{
  Q_OBJECT
 public:
  EditHostvar(QString station,QString var,QString *varvalue,QString *remark,
	       QWidget *parent=0,const char *name=0);
  ~EditHostvar();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void okData();
  void cancelData();

 private:
  QLineEdit *edit_name_edit;
  QLineEdit *edit_varvalue_edit;
  QLineEdit *edit_remark_edit;
  QString *edit_varvalue;
  QString *edit_remark;
};


#endif

