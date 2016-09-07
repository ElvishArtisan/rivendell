// edit_gpi.cpp
//
// Edit a Rivendell Gpi
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>

#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_gpi.h"
#include "globals.h"

EditGpi::EditGpi(RDMatrix *matrix,RDMatrix::GpioType type,int gpi,
		 QWidget *parent)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  edit_matrix=matrix;
  edit_type=type;
  edit_gpi=gpi;
  QString table;
  switch(type) {
  case RDMatrix::GpioInput:
    setWindowTitle("RDAdmin - "+tr("Edit GPI")+QString().sprintf(" %d",gpi));
    edit_table="GPIS";
    break;

  case RDMatrix::GpioOutput:
    setWindowTitle("RDAdmin - "+tr("Edit GPO")+QString().sprintf(" %d",gpi));
    edit_table="GPOS";
    break;
  }
    
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

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
  // On Section Label
  //
  edit_onsection_label=new QLabel("ON Transition",this);
  edit_onsection_label->setFont(label_font);
  edit_onsection_label->setAlignment(Qt::AlignCenter);
  edit_onsection_label->setAutoFillBackground(true);

  //
  // On Cart Macro Cart
  //
  edit_onmacro_edit=new QLineEdit(this);
  edit_onmacro_edit->setFont(font);
  edit_onmacro_label=new QLabel(tr("Cart Number: "),this);
  edit_onmacro_label->setFont(bold_font);
  edit_onmacro_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // On Select Button
  //
  edit_onselect_button=new QPushButton(this);
  edit_onselect_button->setFont(font);
  edit_onselect_button->setText(tr("&Select"));
  connect(edit_onselect_button,SIGNAL(clicked()),this,SLOT(selectOnData()));

  //
  // On Clear Button
  //
  edit_onclear_button=new QPushButton(this);
  edit_onclear_button->setFont(font);
  edit_onclear_button->setText(tr("C&lear"));
  connect(edit_onclear_button,SIGNAL(clicked()),this,SLOT(clearOnData()));

  //
  // On Cart Description
  //
  edit_ondescription_edit=new QLineEdit(this);
  edit_ondescription_edit->setGeometry(120,52,sizeHint().width()-140,20);
  edit_ondescription_edit->setFont(font);
  edit_ondescription_edit->setReadOnly(true);
  edit_ondescription_label=new QLabel(tr("Description: "),this);
  edit_ondescription_label->setGeometry(15,52,100,20);
  edit_ondescription_label->setFont(bold_font);
  edit_ondescription_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Off Section Label
  //
  edit_offsection_label=new QLabel("OFF Transition",this);
  edit_offsection_label->setFont(label_font);
  edit_offsection_label->setAlignment(Qt::AlignCenter);
  edit_offsection_label->setAutoFillBackground(true);

  //
  // Off Cart Macro Cart
  //
  edit_offmacro_edit=new QLineEdit(this);
  edit_offmacro_edit->setFont(font);
  edit_offmacro_label=new QLabel(tr("Cart Number: "),this);
  edit_offmacro_label->setFont(bold_font);
  edit_offmacro_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Off Select Button
  //
  edit_offselect_button=new QPushButton(this);
  edit_offselect_button->setFont(font);
  edit_offselect_button->setText(tr("&Select"));
  connect(edit_offselect_button,SIGNAL(clicked()),this,SLOT(selectOffData()));

  //
  // Off Clear Button
  //
  edit_offclear_button=new QPushButton(this);
  edit_offclear_button->setFont(font);
  edit_offclear_button->setText(tr("C&lear"));
  connect(edit_offclear_button,SIGNAL(clicked()),this,SLOT(clearOffData()));

  //
  // Off Cart Description
  //
  edit_offdescription_edit=new QLineEdit(this);
  edit_offdescription_edit->setGeometry(120,132,sizeHint().width()-140,20);
  edit_offdescription_edit->setFont(font);
  edit_offdescription_edit->setReadOnly(true);
  edit_offdescription_label=new QLabel(tr("Description: "),this);
  edit_offdescription_label->setFont(bold_font);
  edit_offdescription_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(bold_font);
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(bold_font);
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  sql=QString("select ")+
    edit_table+".MACRO_CART,"+
    "CART.TITLE "+
    "from "+edit_table+" left join CART "+
    "on "+edit_table+".MACRO_CART=CART.NUMBER where "+
    "(STATION_NAME=\""+RDEscapeString(edit_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",edit_matrix->matrix())+
    "("+edit_table+QString().sprintf(".NUMBER=%d)",gpi);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toInt()!=0) {
      edit_onmacro_edit->
	setText(QString().sprintf("%06u",q->value(0).toUInt()));
      edit_ondescription_edit->setText(q->value(1).toString());
    }
  }
  sql=QString("select ")+
    edit_table+".OFF_MACRO_CART,"+
    "CART.TITLE "+
    "from "+edit_table+" left join CART "+
    "on "+edit_table+".OFF_MACRO_CART=CART.NUMBER where "+
    "(STATION_NAME=\""+RDEscapeString(edit_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",edit_matrix->matrix())+
    "("+edit_table+QString().sprintf(".NUMBER=%d)",gpi);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toInt()!=0) {
      edit_offmacro_edit->
	setText(QString().sprintf("%06u",q->value(0).toUInt()));
      edit_offdescription_edit->setText(q->value(1).toString());
    }
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
			     rda->user()->name(),rda->user()->password())==0) {
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
			     rda->user()->name(),rda->user()->password())==0) {
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
  int oncart=0;
  int offcart=0;
  if(!edit_onmacro_edit->text().isEmpty()) {
    oncart=edit_onmacro_edit->text().toInt();
  }
  if(!edit_offmacro_edit->text().isEmpty()) {
    offcart=edit_offmacro_edit->text().toInt();
  }
  QString sql=QString("update ")+edit_table+" set "+
    QString().sprintf("MACRO_CART=%u,",oncart)+
    QString().sprintf("OFF_MACRO_CART=%u where ",offcart)+
    "(STATION_NAME=\""+RDEscapeString(edit_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",edit_matrix->matrix())+
    "("+edit_table+QString().sprintf(".NUMBER=%d)",edit_gpi);
  RDSqlQuery::run(sql);

  done(0);
}


void EditGpi::cancelData()
{
  done(-1);
}


void EditGpi::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->drawLine(10,20,size().width()-10,20);
  p->drawLine(size().width()-10,20,size().width()-10,82);
  p->drawLine(size().width()-10,82,10,82);
  p->drawLine(10,82,10,20);
  p->drawLine(10,100,size().width()-10,100);
  p->drawLine(size().width()-10,100,size().width()-10,162);
  p->drawLine(size().width()-10,162,10,162);
  p->drawLine(10,162,10,100);

  delete p;
}


void EditGpi::resizeEvent(QResizeEvent *e)
{
  edit_onsection_label->setGeometry(30,10,120,20);
  edit_onmacro_edit->setGeometry(120,30,60,20);
  edit_onmacro_label->setGeometry(15,30,100,20);
  edit_onselect_button->setGeometry(190,30,60,20);
  edit_onclear_button->setGeometry(270,30,60,20);
  edit_offsection_label->setGeometry(30,90,120,20);
  edit_offmacro_edit->setGeometry(120,110,60,20);
  edit_offmacro_label->setGeometry(15,110,100,20);
  edit_offselect_button->setGeometry(190,110,60,20);
  edit_offclear_button->setGeometry(270,110,60,20);
  edit_offdescription_label->setGeometry(15,132,100,20);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}

