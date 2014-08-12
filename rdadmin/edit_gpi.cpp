// edit_gpi.cpp
//
// Edit a Rivendell Gpi
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_gpi.cpp,v 1.17.8.1 2012/11/26 20:19:38 cvs Exp $
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
#include <qpainter.h>

#include <rdcart_dialog.h>
#include <rdtextvalidator.h>

#include <globals.h>
#include <edit_gpi.h>


EditGpi::EditGpi(int gpi,int *oncart,QString *ondesc,
		 int *offcart,QString *offdesc,
		 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

  edit_gpi=gpi;
  edit_oncart=oncart;
  edit_offcart=offcart;
  edit_ondescription=ondesc;
  edit_offdescription=offdesc;
  str=QString(tr("Edit GPI"));
  setCaption(QString().sprintf("%s %d",(const char *)str,gpi));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont label_font=QFont("Helvetica",14,QFont::Bold);
  label_font.setPixelSize(14);
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // On Section Label
  //
  QLabel *label=new QLabel("ON Transition",this);
  label->setGeometry(30,10,120,20);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);

  //
  // On Cart Macro Cart
  //
  edit_onmacro_edit=new QLineEdit(this,"edit_onmacro_edit");
  edit_onmacro_edit->setGeometry(120,30,60,20);
  edit_onmacro_edit->setFont(font);
  edit_onmacro_edit->setValidator(validator);
  label=new QLabel(tr("Cart Number: "),this,"edit_macro_label");
  label->setGeometry(15,30,100,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // On Select Button
  //
  QPushButton *button=new QPushButton(this,"select_button");
  button->setGeometry(190,30,60,20);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectOnData()));

  //
  // On Clear Button
  //
  button=new QPushButton(this,"select_button");
  button->setGeometry(270,30,60,20);
  button->setFont(font);
  button->setText(tr("C&lear"));
  connect(button,SIGNAL(clicked()),this,SLOT(clearOnData()));

  //
  // On Cart Description
  //
  edit_ondescription_edit=new QLineEdit(this,"edit_ondescription_edit");
  edit_ondescription_edit->setGeometry(120,52,sizeHint().width()-140,20);
  edit_ondescription_edit->setFont(font);
  edit_ondescription_edit->setReadOnly(true);
  label=new QLabel(tr("Description: "),this,"edit_ondescription_label");
  label->setGeometry(15,52,100,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Off Section Label
  //
  label=new QLabel("OFF Transition",this);
  label->setGeometry(30,90,120,20);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);

  //
  // Off Cart Macro Cart
  //
  edit_offmacro_edit=new QLineEdit(this,"edit_offmacro_edit");
  edit_offmacro_edit->setGeometry(120,110,60,20);
  edit_offmacro_edit->setFont(font);
  edit_offmacro_edit->setValidator(validator);
  label=new QLabel(tr("Cart Number: "),this,"edit_macro_label");
  label->setGeometry(15,110,100,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Off Select Button
  //
  button=new QPushButton(this,"select_button");
  button->setGeometry(190,110,60,20);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectOffData()));

  //
  // Off Clear Button
  //
  button=new QPushButton(this,"select_button");
  button->setGeometry(270,110,60,20);
  button->setFont(font);
  button->setText(tr("C&lear"));
  connect(button,SIGNAL(clicked()),this,SLOT(clearOffData()));

  //
  // Off Cart Description
  //
  edit_offdescription_edit=new QLineEdit(this,"edit_offdescription_edit");
  edit_offdescription_edit->setGeometry(120,132,sizeHint().width()-140,20);
  edit_offdescription_edit->setFont(font);
  edit_offdescription_edit->setReadOnly(true);
  label=new QLabel(tr("Description: "),this,"edit_offdescription_label");
  label->setGeometry(15,132,100,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  if(*edit_oncart>0) {
    RDCart *rdcart=new RDCart(*oncart);
    edit_onmacro_edit->setText(QString().sprintf("%06d",*oncart));
    edit_ondescription_edit->setText(rdcart->title());
    delete rdcart;
  }
  if(*edit_offcart>0) {
    RDCart *rdcart=new RDCart(*offcart);
    edit_offmacro_edit->setText(QString().sprintf("%06d",*offcart));
    edit_offdescription_edit->setText(rdcart->title());
    delete rdcart;
  }
}


QSize EditGpi::sizeHint() const
{
  return QSize(420,230);
}


QSizePolicy EditGpi::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditGpi::selectOnData()
{
  int oncart=edit_onmacro_edit->text().toInt();
  if(admin_cart_dialog->exec(&oncart,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    if(oncart>0) {
      RDCart *rdcart=new RDCart(oncart);
      edit_onmacro_edit->setText(QString().sprintf("%06d",oncart));
      edit_ondescription_edit->setText(rdcart->title());
      delete rdcart;
    }
    else {
      edit_onmacro_edit->setText("");
      edit_ondescription_edit->clear();
    }
  }
}


void EditGpi::clearOnData()
{
  edit_ondescription_edit->clear();
  edit_onmacro_edit->clear();
}


void EditGpi::selectOffData()
{
  int offcart=edit_offmacro_edit->text().toInt();
  if(admin_cart_dialog->exec(&offcart,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    if(offcart>0) {
      RDCart *rdcart=new RDCart(offcart);
      edit_offmacro_edit->setText(QString().sprintf("%06d",offcart));
      edit_offdescription_edit->setText(rdcart->title());
      delete rdcart;
    }
    else {
      edit_offmacro_edit->setText("");
      edit_offdescription_edit->clear();
    }
  }
}


void EditGpi::clearOffData()
{
  edit_offdescription_edit->clear();
  edit_offmacro_edit->clear();
}


void EditGpi::okData()
{
  bool ok;
  if(!edit_onmacro_edit->text().isEmpty()) {
    int oncart=edit_onmacro_edit->text().toInt(&ok);
    if(ok) {
      *edit_oncart=oncart;
      RDCart *rdcart=new RDCart(oncart);
      *edit_ondescription=rdcart->title();
      delete rdcart;
      done(0);
    }
    else {
      QMessageBox::warning(this,tr("Invalid Cart"),tr("Invalid Cart Number!"));
    }
  }
  else {
    *edit_oncart=-1;
    *edit_ondescription="";
  }
  if(!edit_offmacro_edit->text().isEmpty()) {
    int offcart=edit_offmacro_edit->text().toInt(&ok);
    if(ok) {
      *edit_offcart=offcart;
      RDCart *rdcart=new RDCart(offcart);
      *edit_offdescription=rdcart->title();
      delete rdcart;
      done(0);
    }
    else {
      QMessageBox::warning(this,tr("Invalid Cart"),tr("Invalid Cart Number!"));
    }
  }
  else {
    *edit_offcart=-1;
    *edit_offdescription="";
  }
  done(0);
}


void EditGpi::cancelData()
{
  done(-1);
}


void EditGpi::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->moveTo(10,20);
  p->lineTo(sizeHint().width()-10,20);
  p->lineTo(sizeHint().width()-10,82);
  p->lineTo(10,82);
  p->lineTo(10,20);

  p->moveTo(10,100);
  p->lineTo(sizeHint().width()-10,100);
  p->lineTo(sizeHint().width()-10,162);
  p->lineTo(10,162);
  p->lineTo(10,100);

  delete p;
}
