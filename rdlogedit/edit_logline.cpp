// edit_logline.cpp
//
// Edit a Rivendell Log Entry
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

#include <QMessageBox>

#include "edit_logline.h"

EditLogLine::EditLogLine(QString *filter,QString *group,QString *schedcode,
			 QWidget *parent)
  : EditEvent(parent)
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

  //
  // Cart Picker
  //
  edit_cart_dialog=new RDCartDialog(edit_filter,edit_group,edit_schedcode,
				    "RDLogEdit",false,this);

  //
  // Overlap Box
  //
  edit_overlap_box=new QCheckBox(this);
  edit_overlap_box->setGeometry(30,94,15,15);
  edit_overlap_label=new QLabel(tr("No Fade on Segue Out"),this);
  edit_overlap_label->setGeometry(50,90,sizeHint().width()-60,26);
  edit_overlap_label->setFont(labelFont());
  edit_overlap_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  
  //
  // Cart Number
  //
  edit_cart_edit=new QLineEdit(this);
  edit_cart_edit->setMaxLength(6);
  edit_cart_edit->setGeometry(10,138,60,18);
  QLabel *label=new QLabel(tr("Cart"),this);
  label->setFont(labelFont());
  label->setGeometry(12,122,60,14);
  connect(edit_cart_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(cartNumberChangedData(const QString &)));

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


int EditLogLine::exec(const QString &svcname,LogModel *model,
		      RDLogLine *ll,int lineno)
{
  edit_service=svcname;
  edit_log_model=model;
  edit_line=lineno;
  setLogLine(ll);

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
  cartNumberChangedData(edit_cart_edit->text());

  return EditEvent::exec();
}


void EditLogLine::selectCartData()
{
  bool ok;
  int cartnum=edit_cart_edit->text().toInt(&ok);
  if(!ok) {
    cartnum=-1;
  }
  if(edit_cart_dialog->exec(&cartnum,RDCart::All,edit_service,NULL)) {
    FillCart(cartnum);
  }
}


void EditLogLine::cartNumberChangedData(const QString &str)
{
  bool ok=false;
  unsigned cartnum=str.toUInt(&ok);

  setOkEnabled(ok&&(cartnum>0)&&(cartnum<=RD_MAX_CART_NUMBER));
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
    if(!edit_log_model->groupIsValid(cart->groupName())) {
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
  if(cartnum==0) {
    edit_cart_edit->setText("");
    edit_title_edit->setText("");
    edit_artist_edit->setText("");
  }
  else {
    RDCart *cart=new RDCart(cartnum);
    edit_cart_edit->setText(QString::asprintf("%05u",cartnum));
    edit_title_edit->setText(cart->title());
    edit_artist_edit->setText(cart->artist());
    delete cart;
  }
}
