// edit_endpoint.cpp
//
// Edit a Rivendell Endpoint
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_endpoint.h"

EditEndpoint::EditEndpoint(RDMatrix *matrix,int endpt,RDMatrix::Endpoint type,
			   QWidget *parent)
  : QDialog(parent)
{
  edit_matrix=matrix;
  edit_endpt=endpt;
  edit_endpoint=new RDEndPoint(matrix,endpt,type);
  switch(edit_endpoint->pointType()) {
  case RDMatrix::Input:
    edit_table="INPUTS";
    setWindowTitle("RDAdmin - "+tr("Edit Input"));
    break;

  case RDMatrix::Output:
    edit_table="OUTPUTS";
    setWindowTitle("RDAdmin - "+tr("Edit Output"));
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Endpoint Name
  //
  edit_endpoint_edit=new QLineEdit(this);
  edit_endpoint_label=new QLabel(tr("Name")+":",this);
  edit_endpoint_label->setFont(bold_font);
  edit_endpoint_label->setAlignment(Qt::AlignRight);

  //
  // Unity Feed
  //
  edit_feed_edit=new QLineEdit(this);
  edit_feed_label=new QLabel(tr("Feed")+":",this);
  edit_feed_label->setFont(bold_font);
  edit_feed_label->setAlignment(Qt::AlignRight);
  if((edit_matrix->type()!=RDMatrix::Unity4000)||
     (edit_endpoint->pointType()!=RDMatrix::Input)) {
    edit_feed_edit->hide();
    edit_feed_label->hide();
  }

  //
  // Unity Mode
  //
  edit_mode_box=new QComboBox(this);
  edit_mode_label=new QLabel(edit_mode_box,tr("Mode")+":",this);
  edit_mode_label->setFont(bold_font);
  edit_mode_label->setAlignment(Qt::AlignRight);
  if(((edit_matrix->type()!=RDMatrix::Unity4000)&&
      (edit_matrix->type()!=RDMatrix::StarGuideIII))||
     (edit_endpoint->pointType()!=RDMatrix::Input)) {
    edit_mode_box->hide();
    edit_mode_label->hide();
  }
  edit_mode_box->insertItem(-1,tr("Stereo"),RDMatrix::Stereo);
  edit_mode_box->insertItem(-1,tr("Left"),RDMatrix::Left);
  edit_mode_box->insertItem(-1,tr("Right"),RDMatrix::Right);

  //
  // Logitek Engine Number / StarGuide Provider ID
  //
  edit_enginenum_edit=new QLineEdit(this);
  edit_enginenum_edit->setGeometry(135,36,50,20);
  edit_enginenum_label=new QLabel(edit_enginenum_edit,tr("Engine")+":",this);
  if(edit_matrix->type()==RDMatrix::StarGuideIII) {
    edit_enginenum_label->setText(tr("Provider ID:"));
  }
  edit_enginenum_label->setFont(bold_font);
  edit_enginenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  if((edit_matrix->type()!=RDMatrix::LogitekVguest)&&
     ((edit_matrix->type()!=RDMatrix::StarGuideIII)||
      (edit_endpoint->pointType()!=RDMatrix::Input))) {
    edit_enginenum_edit->hide();
    edit_enginenum_label->hide();
  }

  //
  // Logitek Device Number / StarGuide Service ID
  //
  edit_devicenum_edit=new QLineEdit(this);
  edit_devicenum_edit->setGeometry(135,62,50,20);
  edit_devicenum_label=new QLabel(edit_devicenum_edit,tr("Device (Hex)")+":",this);
  if(edit_matrix->type()==RDMatrix::StarGuideIII) {
    edit_devicenum_label->setText(tr("Service ID:"));
  }
  edit_devicenum_label->setFont(bold_font);
  edit_devicenum_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  if((edit_matrix->type()!=RDMatrix::LogitekVguest)&&
     ((edit_matrix->type()!=RDMatrix::StarGuideIII)||
      (edit_endpoint->pointType()!=RDMatrix::Input))) {
    edit_devicenum_edit->hide();
    edit_devicenum_label->hide();
  }

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
  edit_endpoint_edit->setText(edit_endpoint->name());
  if(edit_endpoint->pointType()==RDMatrix::Input) {
    edit_feed_edit->setText(edit_endpoint->feedName());
    edit_mode_box->setCurrentItem(edit_endpoint->channelMode());
  }
  if(edit_endpoint->engineNumber()>=0) {
    edit_enginenum_edit->
      setText(QString().sprintf("%d",edit_endpoint->engineNumber()));
  }
  if(edit_endpoint->deviceNumber()>=0) {
    if(edit_matrix->type()==RDMatrix::LogitekVguest) {
      edit_devicenum_edit->
	setText(QString().sprintf("%04X",edit_endpoint->deviceNumber()));
    }
    else {
      edit_devicenum_edit->
	setText(QString().sprintf("%d",edit_endpoint->deviceNumber()));
    }
  }
}


QSize EditEndpoint::sizeHint() const
{
  if((edit_endpoint->pointType()==RDMatrix::Input)&&
     (edit_matrix->type()==RDMatrix::Unity4000)) {
    return QSize(400,130);
  }
  if((edit_endpoint->pointType()==RDMatrix::Input)&&
     (edit_matrix->type()==RDMatrix::StarGuideIII)) {
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
  if(edit_matrix->type()==RDMatrix::LogitekVguest) {
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
      if(edit_matrix->type()==RDMatrix::LogitekVguest) {
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
  if(edit_matrix->type()==RDMatrix::LogitekVguest) {
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
      if(edit_matrix->type()==RDMatrix::LogitekVguest) {
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

  edit_endpoint->setName(edit_endpoint_edit->text());
  if(edit_endpoint->pointType()==RDMatrix::Input) {
    edit_endpoint->setFeedName(edit_feed_edit->text());
    edit_endpoint->setChannelMode((RDMatrix::Mode)edit_mode_box->currentItem());
  }
  edit_endpoint->setEngineNumber(enginenum);
  edit_endpoint->setDeviceNumber(devicenum);

  done(0);
}


void EditEndpoint::cancelData()
{
  done(1);
}


void EditEndpoint::resizeEvent(QResizeEvent *e)
{
  edit_endpoint_edit->setGeometry(75,10,size().width()-85,20);
  edit_endpoint_label->setGeometry(10,13,60,20);
  edit_feed_edit->setGeometry(75,40,40,20);
  edit_feed_label->setGeometry(10,43,60,20);
  if(edit_matrix->type()==RDMatrix::StarGuideIII) {
    edit_mode_box->setGeometry(135,88,85,24);
    edit_mode_label->setGeometry(10,93,120,20);
  }
  else {
    edit_mode_box->setGeometry(195,40,85,24);
    edit_mode_label->setGeometry(130,43,60,20);
  }
  edit_enginenum_label->setGeometry(10,36,120,20);
  edit_devicenum_label->setGeometry(10,62,120,20);

  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
