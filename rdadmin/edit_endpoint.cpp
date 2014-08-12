// edit_endpoint.cpp
//
// Edit a Rivendell Endpoint
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_endpoint.cpp,v 1.10 2010/07/29 19:32:34 cvs Exp $
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

#include <rdtextvalidator.h>

#include <edit_endpoint.h>


EditEndpoint::EditEndpoint(RDMatrix::Type type,RDMatrix::Endpoint endpoint,
			   int pointnum,QString *pointname,QString *feedname,
			   RDMatrix::Mode *mode,int *enginenum,int *devicenum,
			   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_type=type;
  edit_endpoint=endpoint;
  edit_pointnum=pointnum;
  edit_pointname=pointname;
  edit_feedname=feedname;
  edit_mode=mode;
  edit_enginenum=enginenum;
  edit_devicenum=devicenum;

  switch(edit_endpoint) {
      case RDMatrix::Input:
	edit_table="INPUTS";
	setCaption(tr("Edit Input"));
	break;

      case RDMatrix::Output:
	edit_table="OUTPUTS";
	setCaption(tr("Edit Output"));
	break;
  }

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
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // Endpoint Name
  //
  edit_endpoint_edit=new QLineEdit(this,"edit_endpoint_edit");
  edit_endpoint_edit->setGeometry(75,10,sizeHint().width()-85,20);
  edit_endpoint_edit->setValidator(validator);
  QLabel *label=
    new QLabel(edit_endpoint_edit,tr("Name: "),this,"edit_endpoint_label");
  label->setGeometry(10,13,60,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight);

  //
  // Unity Feed
  //
  edit_feed_edit=new QLineEdit(this,"edit_feed_edit");
  edit_feed_edit->setGeometry(75,40,40,20);
  edit_feed_edit->setValidator(validator);
  label=new QLabel(edit_feed_edit,tr("Feed: "),this,"edit_feed_label");
  label->setGeometry(10,43,60,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight);
  if((edit_type!=RDMatrix::Unity4000)||(edit_endpoint!=RDMatrix::Input)) {
    edit_feed_edit->hide();
    label->hide();
  }

  //
  // Unity Mode
  //
  edit_mode_box=new QComboBox(this,"edit_mode_box");
  label=new QLabel(edit_mode_box,tr("Mode: "),this,"edit_feed_label");
  if(edit_type==RDMatrix::StarGuideIII) {
    edit_mode_box->setGeometry(135,88,85,24);
    label->setGeometry(10,93,120,20);
  }
  else {
    edit_mode_box->setGeometry(195,40,85,24);
    label->setGeometry(130,43,60,20);
  }
  label->setFont(bold_font);
  label->setAlignment(AlignRight);
  if(((edit_type!=RDMatrix::Unity4000)&&(edit_type!=RDMatrix::StarGuideIII))||
     (edit_endpoint!=RDMatrix::Input)) {
    edit_mode_box->hide();
    label->hide();
  }
  edit_mode_box->insertItem(tr("Stereo"));
  edit_mode_box->insertItem(tr("Left"));
  edit_mode_box->insertItem(tr("Right"));

  //
  // Logitek Engine Number / StarGuide Provider ID
  //
  edit_enginenum_edit=new QLineEdit(this,"edit_enginenum_edit");
  edit_enginenum_edit->setGeometry(135,36,50,20);
  label=new QLabel(edit_enginenum_edit,tr("Engine (Hex): "),this,"edit_enginenum_label");
  if(edit_type==RDMatrix::StarGuideIII) {
    label->setText(tr("Provider ID:"));
  }
  label->setGeometry(10,36,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);
  if((edit_type!=RDMatrix::LogitekVguest)&&
     ((edit_type!=RDMatrix::StarGuideIII)||(edit_endpoint!=RDMatrix::Input))) {
    edit_enginenum_edit->hide();
    label->hide();
  }

  //
  // Logitek Device Number / StarGuide Service ID
  //
  edit_devicenum_edit=new QLineEdit(this,"edit_devicenum_edit");
  edit_devicenum_edit->setGeometry(135,62,50,20);
  label=new QLabel(edit_devicenum_edit,tr("Device (Hex): "),this,"edit_devicenum_label");
  if(edit_type==RDMatrix::StarGuideIII) {
    label->setText(tr("Service ID:"));
  }
  label->setGeometry(10,62,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);
  if((edit_type!=RDMatrix::LogitekVguest)&&
     ((edit_type!=RDMatrix::StarGuideIII)||(edit_endpoint!=RDMatrix::Input))) {
    edit_devicenum_edit->hide();
    label->hide();
  }

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
  edit_endpoint_edit->setText(*edit_pointname);
  edit_feed_edit->setText(*edit_feedname);
  edit_mode_box->setCurrentItem(*edit_mode);
  if(*enginenum>=0) {
    if(edit_type==RDMatrix::LogitekVguest) {
      edit_enginenum_edit->setText(QString().sprintf("%04X",*enginenum));
    }
    else {
      edit_enginenum_edit->setText(QString().sprintf("%d",*enginenum));
    }
  }
  if(*devicenum>=0) {
    if(edit_type==RDMatrix::LogitekVguest) {
      edit_devicenum_edit->setText(QString().sprintf("%04X",*devicenum));
    }
    else {
      edit_devicenum_edit->setText(QString().sprintf("%d",*devicenum));
    }
  }
}


QSize EditEndpoint::sizeHint() const
{
  if((edit_endpoint==RDMatrix::Input)&&(edit_type==RDMatrix::Unity4000)) {
    return QSize(400,130);
  }
  if((edit_endpoint==RDMatrix::Input)&&(edit_type==RDMatrix::StarGuideIII)) {
    return QSize(420,156);
  }
  return QSize(400,100);
}


QSizePolicy EditEndpoint::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditEndpoint::okData()
{
  bool ok;
  int enginenum=-1;
  if(edit_type==RDMatrix::LogitekVguest) {
    enginenum=edit_enginenum_edit->text().toInt(&ok,16);
  }
  else {
    enginenum=edit_enginenum_edit->text().toInt(&ok);
  }
  if(!ok) {
    if(edit_enginenum_edit->text().isEmpty()) {
      enginenum=-1;
    }
    else {
      if(edit_type==RDMatrix::LogitekVguest) {
	QMessageBox::warning(this,tr("Invalid Number"),
			     tr("The Engine Number is Invalid!"));
      }
      else {
	QMessageBox::warning(this,tr("Invalid Number"),
			     tr("The Provider ID is Invalid!"));
      }
      return;
    }
  }
  int devicenum=-1;
  if(edit_type==RDMatrix::LogitekVguest) {
    devicenum=edit_devicenum_edit->text().toInt(&ok,16);
  }
  else {
    devicenum=edit_devicenum_edit->text().toInt(&ok);
  }
  if(!ok) {
    if(edit_devicenum_edit->text().isEmpty()) {
      devicenum=-1;
    }
    else {
      if(edit_type==RDMatrix::LogitekVguest) {
	QMessageBox::warning(this,tr("Invalid Number"),
			     tr("The Device Number is Invalid!"));
      }
      else {
	QMessageBox::warning(this,tr("Invalid Number"),
			     tr("The Service ID is Invalid!"));
      }
      return;
    }
  }
  *edit_pointname=edit_endpoint_edit->text();
  *edit_feedname=edit_feed_edit->text();
  *edit_mode=(RDMatrix::Mode)edit_mode_box->currentItem();
  *edit_enginenum=enginenum;
  *edit_devicenum=devicenum;
  done(0);
}


void EditEndpoint::cancelData()
{
  done(1);
}
