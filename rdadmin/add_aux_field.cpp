// add_aux_field.cpp
//
// Add an Auxiliary Field for an RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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
//Added by qt3to4:
#include <QLabel>
#include <rdescape_string.h>
#include <rddb.h>
#include <add_aux_field.h>

AddAuxField::AddAuxField(unsigned feed_id,unsigned *field_id,QWidget *parent)
  : QDialog(parent,"",true)
{
  add_feed_id=feed_id;
  add_field_id=field_id;
  setCaption(tr("Add Aux Field"));


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
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Variable Name
  //
  add_varname_edit=new QLineEdit(this);
  add_varname_edit->setGeometry(165,10,130,20);
  add_varname_edit->setMaxLength(11);
  QLabel *label=
    new QLabel(add_varname_edit,tr("Variable Name: "),this);
  label->setGeometry(10,13,105,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight);
  label=new QLabel(add_varname_edit,tr("%AUX_"),this);
  label->setGeometry(120,13,45,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight);
  label=new QLabel(add_varname_edit,tr("%"),this);
  label->setGeometry(295,13,30,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignLeft);


  //
  // Caption
  //
  add_caption_edit=new QLineEdit(this);
  add_caption_edit->setGeometry(120,37,sizeHint().width()-130,20);
  add_caption_edit->setMaxLength(64);
  label=new QLabel(add_caption_edit,tr("Caption: "),this);
  label->setGeometry(10,37,105,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(bold_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize AddAuxField::sizeHint() const
{
  return QSize(400,127);
}


QSizePolicy AddAuxField::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddAuxField::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ID from AUX_METADATA where ")+
    QString().sprintf("(FEED_ID=%u)&&",add_feed_id)+
    "(VAR_NAME=\"%%AUX_"+RDEscapeString(add_varname_edit->text())+"%%\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    delete q;
    QMessageBox::warning(this,tr("Name Exists"),
			 tr("That variable name already exists!"));
    return;
  }
  delete q;
  sql=QString("insert into AUX_METADATA set ")+
    "VAR_NAME=\"%%AUX_"+RDEscapeString(add_varname_edit->text())+"%%\","+
    "CAPTION=\""+RDEscapeString(add_caption_edit->text())+"\","+
    QString().sprintf("FEED_ID=%u",add_feed_id);
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select ID from AUX_METADATA where ")+
    QString().sprintf("(FEED_ID=%u)&&",add_feed_id)+
    "(VAR_NAME=\"%%AUX_"+RDEscapeString(add_varname_edit->text())+"%%\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *add_field_id=q->value(0).toUInt();
  }
  delete q;

  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%u",add_feed_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    QString keyname=q->value(0).toString();
    delete q;
    keyname.replace(" ","_");
    sql=QString("alter table `")+keyname+"_FIELDS` "+
      "add column AUX_"+RDEscapeString(add_varname_edit->text())+" char(255)";
    q=new RDSqlQuery(sql);
  }
  delete q;  

  done(0);
}


void AddAuxField::cancelData()
{
  done(-1);
}
