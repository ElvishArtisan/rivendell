// edit_encoder.cpp
//
// Edit a Rivendell Encoder
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_encoder.cpp,v 1.4 2012/02/13 19:26:14 cvs Exp $
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

#include <rdescape_string.h>
#include <rddb.h>

#include <edit_encoder.h>

EditEncoder::EditEncoder(int encoder_id,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  QSqlQuery *q;

  edit_encoder_id=encoder_id;
  setCaption(tr("RDAdmin - Edit Encoder"));

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
  // Name
  //
  QLabel *label=new QLabel(tr("Name:"),this);
  label->setGeometry(10,10,95,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);
  QLabel *name_label=new QLabel(this);
  name_label->setGeometry(110,10,sizeHint().width()-120,20);
  name_label->setFont(font);
  name_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Default Extension
  //
  edit_extension_edit=new QLineEdit(this);
  edit_extension_edit->setGeometry(130,32,80,20);
  label=new QLabel(edit_extension_edit,tr("Default Extension:"),this);
  label->setGeometry(10,32,115,20);
  label->setAlignment(AlignRight|AlignVCenter);
  label->setFont(bold_font);

  //
  // Command Line
  //
  edit_commandline_edit=new QLineEdit(this);
  edit_commandline_edit->setGeometry(130,54,sizeHint().width()-140,20);
  label=new QLabel(edit_commandline_edit,tr("Command Line:"),this);
  label->setGeometry(10,54,115,20);
  label->setAlignment(AlignRight|AlignVCenter);
  label->setFont(bold_font);

  //
  // Channels
  //
  edit_channel_edit=new RDIntegerEdit(tr("Allow Channels"),1,2,this);
  edit_channel_edit->setGeometry(50,82,120,180);

  //
  // Sample Rates
  //
  edit_samprate_edit=new RDIntegerEdit(tr("Allow Sample Rates"),
				       4000,192000,this);
  edit_samprate_edit->setGeometry(190,82,120,180);

  //
  // Bit Rates
  //
  edit_bitrate_edit=new RDIntegerEdit(tr("Allow Bit Rates"),0,1000000,this);
  edit_bitrate_edit->setGeometry(330,82,120,180);

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this,"ok_button");
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
  sql=QString().sprintf("select NAME,DEFAULT_EXTENSION,COMMAND_LINE \
                         from ENCODERS where ID=%d",
			encoder_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    name_label->setText(q->value(0).toString());
    edit_extension_edit->setText(q->value(1).toString());
    edit_commandline_edit->setText(q->value(2).toString());
  }
  delete q;
  LoadList("CHANNELS",edit_channel_edit);
  LoadList("SAMPLERATES",edit_samprate_edit);
  LoadList("BITRATES",edit_bitrate_edit);
}


QSize EditEncoder::sizeHint() const
{
  return QSize(500,332);
}


QSizePolicy EditEncoder::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditEncoder::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().
    sprintf("update ENCODERS set DEFAULT_EXTENSION=\"%s\",COMMAND_LINE=\"%s\"",
	    (const char *)RDEscapeString(edit_extension_edit->text()),
	    (const char *)RDEscapeString(edit_commandline_edit->text()));
  q=new RDSqlQuery(sql);
  delete q;
  SaveList("CHANNELS",edit_channel_edit);
  SaveList("SAMPLERATES",edit_samprate_edit);
  SaveList("BITRATES",edit_bitrate_edit);

  done(0);
}


void EditEncoder::cancelData()
{
  done(1);
}


void EditEncoder::LoadList(const QString &paramname,RDIntegerEdit *edit)
{
  QString sql;
  RDSqlQuery *q;
  std::vector<int> values;

  sql=QString().sprintf("select %s from ENCODER_%s where ENCODER_ID=%d\
                         order by %s",
			(const char *)paramname,
			(const char *)paramname,
			edit_encoder_id,
			(const char *)paramname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    values.push_back(q->value(0).toInt());
  }
  delete q;
  edit->setValues(&values);
}


void EditEncoder::SaveList(const QString &paramname,RDIntegerEdit *edit)
{
  QString sql;
  RDSqlQuery *q;
  std::vector<int> values;

  sql=QString().sprintf("delete from ENCODER_%s where ENCODER_ID=%d",
			(const char *)paramname,edit_encoder_id);
  q=new RDSqlQuery(sql);
  delete q;
  edit->values(&values);
  for(unsigned i=0;i<values.size();i++) {
    sql=QString().sprintf("insert into ENCODER_%s set ENCODER_ID=%d,%s=%d",
			  (const char *)paramname,
			  edit_encoder_id,
			  (const char *)paramname,
			  values[i]);
    q=new RDSqlQuery(sql);
    delete q;
  }
}
