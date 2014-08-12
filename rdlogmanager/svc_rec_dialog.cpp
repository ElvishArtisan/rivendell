// svc_rec_dialog.cpp
//
// A Services/Reports Management Dialog.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: svc_rec_dialog.cpp,v 1.10 2010/07/29 19:32:37 cvs Exp $
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


#include <qapplication.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qsignalmapper.h>
#include <qfile.h>

#include <rdreport.h>
#include <rddatedecode.h>

#include <globals.h>
#include <svc_rec_dialog.h>

//
// Global Classes
//
SvcRecDialog::SvcRecDialog(const QString &svcname,
			   QWidget *parent,const char *name)
  :QDialog(parent,name,true)
{
  QFont font;

  font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  setCaption(QString().sprintf("%s %s",(const char *)svcname,
			       (const char *)tr("Report Data")));

  //
  // Datepicker
  //
  date_picker=new SvcRec(svcname,this,"date_picker");
  date_picker->setGeometry(10,10,
			   date_picker->sizeHint().width(),
			   date_picker->sizeHint().height());
  connect(date_picker,SIGNAL(dateSelected(const QDate &,bool)),
	  this,SLOT(dateSelectedData(const QDate &,bool)));

  //
  // Delete Button
  //
  date_delete_button=new QPushButton(this,"date_delete_button");
  date_delete_button->
    setGeometry(10,sizeHint().height()-60,80,50);
  date_delete_button->setFont(font);
  date_delete_button->setText(tr("&Purge\nData"));
  connect(date_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
#ifndef WIN32
  date_delete_button->setEnabled(rduser->deleteRec()&&
    date_picker->dayActive(date_picker->date().day()));
#endif  // WIN32

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(font);\
  button->setText(tr("&Close"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


SvcRecDialog::~SvcRecDialog()
{
}


QSize SvcRecDialog::sizeHint() const
{
  return QSize(date_picker->sizeHint().width()+20,
	       date_picker->sizeHint().height()+85);
}


QSizePolicy SvcRecDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void SvcRecDialog::dateSelectedData(const QDate &,bool active)
{
#ifdef WIN32
  date_delete_button->
    setEnabled(date_picker->dayActive(date_picker->date().day()));
#else
  date_delete_button->setEnabled(rduser->deleteRec()&&
    date_picker->dayActive(date_picker->date().day()));
#endif  // WIN32
}


void SvcRecDialog::deleteData()
{
  if(QMessageBox::question(this,tr("Delete Report Data"),
			   QString().sprintf("%s %s?",
         (const char *)tr("Are you sure you want to delete report data for"),
	 (const char *)date_picker->date().toString("MM/dd/yyyy")),
			QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  date_picker->deleteDay();
  date_delete_button->setDisabled(true);
}


void SvcRecDialog::closeData()
{
  done(-1);
}
