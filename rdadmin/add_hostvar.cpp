// add_hostvar.cpp
//
// Add a Rivendell Workstation Configuration
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: add_hostvar.cpp,v 1.9 2012/02/13 19:26:13 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <rdcatch_connect.h>

#include <add_hostvar.h>
#include <edit_rdlibrary.h>
#include <edit_rdairplay.h>
#include <edit_decks.h>
#include <edit_audios.h>
#include <edit_ttys.h>
#include <list_matrices.h>
#include <list_hostvars.h>


AddHostvar::AddHostvar(QString station,QString *var,QString *varvalue,
			 QString *remark,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  add_name=var;
  add_varvalue=varvalue;
  add_remark=remark;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setCaption(tr("Add Host Variable"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Variable Name
  //
  add_name_edit=new QLineEdit(this,"add_name_edit");
  add_name_edit->setGeometry(125,11,120,19);
  add_name_edit->setMaxLength(32);
  QLabel *label=new QLabel(add_name_edit,tr("Variable Name:"),this,"add_name_label");
  label->setGeometry(10,11,110,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Variable Value
  //
  add_varvalue_edit=new QLineEdit(this,"add_varvalue_edit");
  add_varvalue_edit->setGeometry(125,33,sizeHint().width()-135,19);
  add_varvalue_edit->setMaxLength(255);
  label=new QLabel(add_varvalue_edit,tr("Variable Value:"),
		   this,"add_varvalue_label");
  label->setGeometry(10,33,110,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Remark
  //
  add_remark_edit=new QLineEdit(this,"add_remark_edit");
  add_remark_edit->setGeometry(125,55,sizeHint().width()-135,19);
  add_remark_edit->setMaxLength(255);
  label=new QLabel(add_remark_edit,tr("Remark:"),this,"add_remark_label");
  label->setGeometry(10,55,110,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddHostvar::~AddHostvar()
{
}


QSize AddHostvar::sizeHint() const
{
  return QSize(385,150);
} 


QSizePolicy AddHostvar::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddHostvar::okData()
{
  if((add_name_edit->text().left(1)!=QString("%"))||
     (add_name_edit->text().right(1)!=QString("%"))||
     (add_name_edit->text().length()<3)) {
    QMessageBox::warning(this,tr("Invalid Name"),
			 tr("The variable name is invalid."));
    return;
  }
  *add_name=add_name_edit->text();
  *add_varvalue=add_varvalue_edit->text();
  *add_remark=add_remark_edit->text();
  done(0);
}


void AddHostvar::cancelData()
{
  done(-1);
}
