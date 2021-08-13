// edit_cartevent.cpp
//
// Edit a Rivendell Macro Cart Event
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QGroupBox>
#include <QMessageBox>

#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "rdapplication.h"
#include "edit_cartevent.h"
#include "globals.h"

EditCartEvent::EditCartEvent(int id,std::vector<int> *adds,QWidget *parent)
  : RDDialog(parent)
{
  QString temp;
  int cartnum;

  edit_deck=NULL;
  edit_added_events=adds;

  setWindowTitle("RDCatch - "+tr("Edit Cart Event"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);
  if((cartnum=edit_recording->macroCart())>=0) {
    edit_cart=new RDCart(cartnum);
  }
  else {
    edit_cart=NULL;
  }

  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::OtherEvent,this);

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_edit->setValidator(validator);
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Number
  //
  edit_destination_edit=new QLineEdit(this);
  edit_destination_edit->setReadOnly(false);
  edit_destination_label=new QLabel(tr("Cart Number:"),this);
  edit_destination_label->setFont(labelFont());
  edit_destination_label->setAlignment(Qt::AlignRight);
  edit_destination_button=new QPushButton(this);
  edit_destination_button->setFont(subLabelFont());
  edit_destination_button->setText(tr("Select"));
  connect(edit_destination_button,SIGNAL(clicked()),this,SLOT(selectCartData()));

  //
  // DOW Selector
  //
  edit_dow_selector=new DowSelector(this);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this);
  edit_oneshot_label=new QLabel(tr("Make OneShot"),this);
  edit_oneshot_label->setFont(labelFont());
  edit_oneshot_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  //  Save As Button
  //
  edit_saveas_button=new QPushButton(this);
  edit_saveas_button->setFont(buttonFont());
  edit_saveas_button->setText(tr("Save As\nNew"));
  connect(edit_saveas_button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    edit_saveas_button->hide();
  }

  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Populate Data
  //
  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  if(edit_cart!=NULL) {
    edit_destination_edit->
      setText(QString().sprintf("%06d",edit_cart->number()));
  }
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
}


EditCartEvent::~EditCartEvent()
{
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditCartEvent::sizeHint() const
{
  return QSize(edit_event_widget->sizeHint().width(),255);
} 


QSizePolicy EditCartEvent::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditCartEvent::selectCartData()
{
  int cartnum=0;
  if(edit_cart!=NULL) {
    cartnum=edit_cart->number();
  }
  if(catch_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)) {
    if(edit_cart!=NULL) {
      delete edit_cart;
    }
    edit_cart=new RDCart(cartnum);
    edit_destination_edit->setText(QString().sprintf("%d",cartnum));
    edit_description_edit->setText(edit_cart->title());
  }
}


void EditCartEvent::saveasData()
{
  if(!CheckEvent(true)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
		     tr("An event with these parameters already exists!"));
    return;
  }
  delete edit_recording;
  edit_recording=new RDRecording(-1,true);
  edit_added_events->push_back(edit_recording->id());
  Save();
}


void EditCartEvent::okData()
{
  if(edit_cart!=NULL) {
    delete edit_cart;
  }
  edit_cart=new RDCart(edit_destination_edit->text().toUInt());
  if(!edit_cart->exists()) {
    QMessageBox::
      information(this,tr("Invalid Cart"),tr("That cart doesn't exist!"));
    return;
  }
  if(!CheckEvent(false)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
		     tr("An event with these parameters already exists!"));
    return;
  }
  Save();
  done(true);
}


void EditCartEvent::cancelData()
{
  done(false);
}


void EditCartEvent::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  edit_description_edit->setGeometry(115,43,size().width()-125,20);
  edit_description_label->setGeometry(10,43,100,20);

  edit_destination_edit->setGeometry(115,70,60,20);
  edit_destination_label->setGeometry(10,73,100,19);
  edit_destination_button->setGeometry(185,68,60,24);

  edit_dow_selector->setGeometry(10,104,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  edit_oneshot_box->setGeometry(20,180,15,15);
  edit_oneshot_label->setGeometry(40,178,115,20);

  edit_saveas_button->setGeometry(size().width()-300,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditCartEvent::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
  case Qt::Key_Escape:
    e->accept();
    cancelData();
    break;

  default:
    QDialog::keyPressEvent(e);
    break;
  }
}


void EditCartEvent::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditCartEvent::Save()
{
  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setType(RDRecording::MacroEvent);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setMacroCart(edit_cart->number());
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


bool EditCartEvent::CheckEvent(bool include_myself)
{
  QString sql=QString("select `ID` from `RECORDINGS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString().sprintf("(`TYPE`=%d)&&",RDRecording::MacroEvent)+
    "(`START_TIME`='"+RDEscapeString(edit_event_widget->startTime().
				   toString("hh:mm:ss"))+"')&&"+
    QString().sprintf("(`MACRO_CART`=%u)",
		      edit_destination_edit->text().toUInt());
  if(edit_dow_selector->dayOfWeekEnabled(7)) {
    sql+="&&(`SUN`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(1)) {
    sql+="&&(`MON`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(2)) {
    sql+="&&(`TUE`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(3)) {
    sql+="&&(`WED`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(4)) {
    sql+="&&(`THU`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(5)) {
    sql+="&&(`FRI`='Y')";
  }
  if(edit_dow_selector->dayOfWeekEnabled(6)) {
    sql+="&&(`SAT`='Y')";
  }
  if(!include_myself) {
    sql+=QString().sprintf("&&(`ID`!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=!q->first();
  delete q;

  return res;
}
