// edit_logline.cpp
//
// Edit a Rivendell Log Entry
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

#include "edit_logline.h"

EditLogLine::EditLogLine(RDLogLine *line,QString *filter,QString *group,
			 QString *schedcode,QString svcname,
			 RDGroupList *grplist,RDLogEvent *log,int lineno,
			 QWidget *parent)
  : EditEvent(line,parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDLogEdit - "+tr("Edit Log Entry"));

  edit_filter=filter;
  edit_group=group;
  edit_schedcode=schedcode;
  edit_service=svcname;
  edit_group_list=grplist;
  edit_log_event=log;
  edit_line=lineno;

  //
  // Cart Picker
  //
  edit_cart_dialog=new RDCartDialog(edit_filter,edit_group,edit_schedcode,
				    "RDLogEdit",this);

  //
  // Overlap Box
  //
  edit_overlap_box=new QCheckBox(this);
  edit_overlap_box->setGeometry(30,94,15,15);
  edit_overlap_label=
    new QLabel(edit_overlap_box,tr("No Fade on Segue Out"),this);
  edit_overlap_label->setGeometry(50,90,sizeHint().width()-60,26);
  edit_overlap_label->setFont(labelFont());
  edit_overlap_label->
    setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);
  
  //
  // Cart Number
  //
  edit_cart_edit=new QLineEdit(this);
  edit_cart_edit->setGeometry(10,138,60,18);
  QLabel *label=new QLabel(tr("Cart"),this);
  label->setFont(labelFont());
  label->setGeometry(12,122,60,14);

  //
  // Title 
  //
  edit_title_edit=new QLineEdit(this);
  edit_title_edit->setGeometry(75,138,260,18);
  edit_title_edit->setReadOnly(true);
  label=new QLabel(tr("Title"),this);
  label->setFont(labelFont());
  label->setGeometry(77,122,110,14);

  //
  // Artist 
  //
  edit_artist_edit=new QLineEdit(this);
  edit_artist_edit->setGeometry(340,138,sizeHint().width()-350,18);
  edit_artist_edit->setReadOnly(true);
  label=new QLabel(tr("Artist"),this);
  label->setFont(labelFont());
  label->setGeometry(342,122,110,14);

  //
  // Cart Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(20,166,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Select\nCart"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCartData()));

  //
  // Populate Data
  //
  if(logLine()->segueStartPoint(RDLogLine::LogPointer)<0
     && logLine()->segueEndPoint(RDLogLine::LogPointer)<0
     && logLine()->endPoint(RDLogLine::LogPointer)<0
     && logLine()->fadedownPoint(RDLogLine::LogPointer)<0) {
    edit_overlap_box->setEnabled(true);
    edit_overlap_label->setEnabled(true);
    if(logLine()->segueGain()==0) {
      edit_overlap_box->setChecked(true);
    }
    else {
      edit_overlap_box->setChecked(false);
    }
  }
  else {
    edit_overlap_box->setEnabled(false);
    edit_overlap_label->setEnabled(false);
  }  
  FillCart(logLine()->cartNumber());
}


EditLogLine::~EditLogLine()
{
  delete edit_cart_dialog;
}


QSize EditLogLine::sizeHint() const
{
  return QSize(625,252);
} 


QSizePolicy EditLogLine::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditLogLine::selectCartData()
{
  bool ok;
  int cartnum=edit_cart_edit->text().toInt(&ok);
  if(!ok) {
    cartnum=-1;
  }
  if(edit_cart_dialog->exec(&cartnum,RDCart::All,&edit_service,1,
			   rda->user()->name(),rda->user()->password())==0) {
    FillCart(cartnum);
  }
}


bool EditLogLine::saveData()
{
  if(edit_cart_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Missing Cart"),
			 tr("You must supply a cart number!"));
    return false;
  }
  RDCart *cart=new RDCart(edit_cart_edit->text().toUInt());
  if(cart->exists()) {
    if(!edit_group_list->isGroupValid(cart->groupName())) {
      delete cart;
      QMessageBox::warning(this,tr("Disabled Cart"),
			   tr("This cart belongs to a disabled\ngroup for the specified service!"));
      return false;
    }
  }
  delete cart;
  logLine()->setCartNumber(edit_cart_edit->text().toUInt());
  if(logLine()->segueStartPoint(RDLogLine::LogPointer)<0
     && logLine()->segueEndPoint(RDLogLine::LogPointer)<0
     && logLine()->endPoint(RDLogLine::LogPointer)<0
     && logLine()->fadedownPoint(RDLogLine::LogPointer)<0) {
    if(edit_overlap_box->isChecked()) {
      logLine()->setSegueGain(0);
    }
    else {
      logLine()->setSegueGain(RD_FADE_DEPTH);
    }
  }
  return true;
}


void EditLogLine::FillCart(int cartnum)
{
  RDCart *cart=new RDCart(cartnum);
  if(cartnum!=0) {
    edit_cart_edit->setText(QString().sprintf("%05u",cartnum));
  }
  edit_title_edit->setText(cart->title());
  edit_artist_edit->setText(cart->artist());
  delete cart;
}
