// rdbutton_dialog.cpp
//
// Event Editor for RDAirPlay
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdbutton_dialog.cpp,v 1.23.6.1.2.1 2014/03/21 15:41:44 cvs Exp $
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

RDButtonDialog::RDButtonDialog(QString station_name,
			       const QString &label_template,
			       RDCartDialog *cart_dialog,const QString &svcname,
			       QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Edit Button"));
  edit_station_name=station_name;
  edit_label_template=label_template;
  edit_cart_dialog=cart_dialog;
  edit_svcname=svcname;

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",14,QFont::Bold);
  button_font.setPixelSize(14);
  QFont counter_font=QFont("Helvetica",24,QFont::Bold);
  counter_font.setPixelSize(24);

  //
  // Button Label
  //
  edit_label_edit=new QLineEdit(this,"edit_label_edit");
  edit_label_edit->setGeometry(60,10,300,20);
  edit_label_edit->setFont(font);
  QLabel *label=new QLabel(edit_label_edit,tr("Label:"),
			   this,"edit_label_label");
  label->setGeometry(10,12,45,16);
  label->setFont(label_font);
  label->setAlignment(AlignRight);

  //
  // Button Cart
  //
  edit_cart_edit=new QLineEdit(this,"edit_cart_edit");
  edit_cart_edit->setGeometry(60,34,300,20);
  edit_cart_edit->setFont(font);
  edit_cart_edit->setReadOnly(true);
  label=new QLabel(edit_cart_edit,tr("Cart:"),this,"edit_cart_label");
  label->setGeometry(10,36,45,16);
  label->setFont(label_font);
  label->setAlignment(AlignRight);

  //
  // Set Cart Button
  //
  QPushButton *button=new QPushButton(this,"cart_button");
  button->setGeometry(55,60,80,50);
  button->setFont(label_font);
  button->setText(tr("Set\nCart"));
  connect(button,SIGNAL(clicked()),this,SLOT(setCartData()));
  
  //
  // Clear Button
  //
  button=new QPushButton(this,"cart_button");
  button->setGeometry(145,60,80,50);
  button->setFont(label_font);
  button->setText(tr("Clear"));
  connect(button,SIGNAL(clicked()),this,SLOT(clearCartData()));
  
  //
  // Color Button
  //
  edit_color_button=new QPushButton(this,"edit_color_button");
  edit_color_button->setGeometry(sizeHint().width()-135,60,80,50);
  edit_color_button->setFont(label_font);
  edit_color_button->setText(tr("Set\nColor"));
  connect(edit_color_button,SIGNAL(clicked()),this,SLOT(setColorData()));
  
  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(label_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(label_font);
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
  QPalette p=QPalette(edit_color,backgroundColor());
  p.setColor(QColorGroup::ButtonText,RDGetTextColor(edit_color));
  edit_color_button->setPalette(p);
  edit_label_edit->setText(edit_button->text());
  DisplayCart(edit_cart);
  return QDialog::exec();
}


void RDButtonDialog::setCartData()
{
  if(edit_cart_dialog->exec(&edit_cart,RDCart::All,&edit_svcname,1,
			    edit_user_name,edit_user_password)==0) {
    DisplayCart(edit_cart);
  }
}


void RDButtonDialog::clearCartData()
{
  edit_cart=0;
  edit_color=backgroundColor();
  edit_color_button->setPalette(QPalette(edit_color,backgroundColor()));
  edit_label_edit->setText("");
  edit_cart_edit->setText("");
}


void RDButtonDialog::setColorData()
{
  QColor new_color=QColorDialog::getColor(edit_color,this,"edit_color_dialog");
  if(new_color.isValid()) {
    edit_color=new_color;
    QPalette p=QPalette(edit_color,backgroundColor());
    p.setColor(QColorGroup::ButtonText,RDGetTextColor(edit_color));
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
    /*
    edit_cart_edit->setText(QString().sprintf("%06u - %s",
					      cart->number(),
					      (const char *)cart->title()));
    */
  }
  else {
    edit_cart_edit->setText(QString().sprintf("%06u - [",cart->number())+"] "+
			    tr("NOT FOUND"));
    /*
    edit_cart_edit->
      setText(QString().sprintf("%06u - [%s]",cart->number(),
				(const char *)str));
    */
  }
  delete cart;
}
