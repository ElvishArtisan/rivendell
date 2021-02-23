// rdbutton_dialog.cpp
//
// Button Editor for SoundPanel
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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qcolordialog.h>
#include <rddb.h>
#include <rdcart.h>
#include <rdcart_dialog.h>
#include <rd.h>
#include <rdbutton_dialog.h>
#include <rdconf.h>

RDButtonDialog::RDButtonDialog(QString station_name,const QString &caption,
			       const QString &label_template,
			       RDCartDialog *cart_dialog,const QString &svcname,
			       QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle(caption+" - "+tr("Edit Button"));
  edit_station_name=station_name;
  edit_label_template=label_template;
  edit_cart_dialog=cart_dialog;
  edit_svcname=svcname;

  //
  // Button Label
  //
  edit_label_edit=new QLineEdit(this);
  edit_label_edit->setGeometry(60,10,300,20);
  QLabel *label=new QLabel(tr("Label:"),this);
  label->setGeometry(10,12,45,16);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);

  //
  // Button Cart
  //
  edit_cart_edit=new QLineEdit(this);
  edit_cart_edit->setGeometry(60,34,300,20);
  edit_cart_edit->setReadOnly(true);
  label=new QLabel(tr("Cart:"),this);
  label->setGeometry(10,36,45,16);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);

  //
  // Set Cart Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(55,60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Set\nCart"));
  connect(button,SIGNAL(clicked()),this,SLOT(setCartData()));
  
  //
  // Clear Button
  //
  button=new QPushButton(this);
  button->setGeometry(145,60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Clear"));
  connect(button,SIGNAL(clicked()),this,SLOT(clearCartData()));
  
  //
  // Color Button
  //
  edit_color_button=new QPushButton(this);
  edit_color_button->setGeometry(sizeHint().width()-135,60,80,50);
  edit_color_button->setFont(buttonFont());
  edit_color_button->setText(tr("Set\nColor"));
  connect(edit_color_button,SIGNAL(clicked()),this,SLOT(setColorData()));
  
  //
  //  Ok Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDButtonDialog::~RDButtonDialog()
{
}


QSize RDButtonDialog::sizeHint() const
{
  return QSize(370,190);
} 


QSizePolicy RDButtonDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDButtonDialog::exec(RDPanelButton *button,bool hookmode,
			 const QString &username,const QString &passwd)
{
  edit_button=button;
  edit_hookmode=hookmode;
  edit_user_name=username;
  edit_user_password=passwd;
  edit_cart=edit_button->cart();
  edit_color=edit_button->defaultColor();
  QPalette p=QPalette(edit_color,palette().color(QPalette::Background));
  p.setColor(QPalette::ButtonText,RDGetTextColor(edit_color));
  edit_color_button->setPalette(p);
  edit_label_edit->setText(edit_button->text());
  DisplayCart(edit_cart);
  return QDialog::exec();
}


void RDButtonDialog::setCartData()
{
  if(edit_cart_dialog->exec(&edit_cart,RDCart::All,edit_svcname,NULL)==0) {
    DisplayCart(edit_cart);
  }
}


void RDButtonDialog::clearCartData()
{
  edit_cart=0;
  edit_color=Qt::lightGray;
  edit_color_button->setPalette(QPalette(edit_color,palette().color(QPalette::Background)));
  edit_label_edit->setText("");
  edit_cart_edit->setText("");
}


void RDButtonDialog::setColorData()
{
  QColor new_color=QColorDialog::getColor(edit_color,this,"edit_color_dialog");
  if(new_color.isValid()) {
    edit_color=new_color;
    QPalette p=QPalette(edit_color,palette().color(QPalette::Background));
    p.setColor(QPalette::ButtonText,RDGetTextColor(edit_color));
    edit_color_button->setPalette(p);
  }
}


void RDButtonDialog::okData()
{
  RDCart *cart=new RDCart((unsigned)edit_cart);
  edit_button->setCart(edit_cart);
  edit_button->setColor(edit_color);
  edit_button->setDefaultColor(edit_color);
  if((edit_cart>0)&&edit_label_edit->text().isEmpty()) {
    edit_button->
      setText(RDLogLine::resolveWildcards(edit_cart,edit_label_template));
  }
  else {
    edit_button->setText(edit_label_edit->text());
  }
  edit_button->setLength(false,cart->forcedLength());
  if(cart->averageHookLength()>0) {
    edit_button->setLength(true,cart->averageHookLength());
  }
  else {
    edit_button->setLength(true,cart->forcedLength());
  }
  edit_button->setActiveLength(edit_button->length(edit_hookmode));
  edit_button->setHookMode(edit_hookmode);
  delete cart;
  done(0);
}


void RDButtonDialog::cancelData()
{
  done(-1);
}


void RDButtonDialog::DisplayCart(int cartnum)
{
  if(cartnum==0) {
    edit_cart_edit->setText("");
    return;
  }
  RDCart *cart=new RDCart((unsigned)cartnum);
  if(cart->exists()) {
    edit_cart_edit->
      setText(QString().sprintf("%06u - ",cart->number())+cart->title());
  }
  else {
    edit_cart_edit->setText(QString().sprintf("%06u - [",cart->number())+"] "+
			    tr("NOT FOUND"));
  }
  delete cart;
}
