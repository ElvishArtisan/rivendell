// edit_logline.cpp
//
// Edit a Rivendell Log Entry
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_logline.cpp,v 1.40.8.1 2012/11/26 20:19:41 cvs Exp $
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
#include <qmessagebox.h>
#include <qradiobutton.h>

#include <rd.h>
#include <rdcart.h>
#include <rdcart_dialog.h>

#include <globals.h>
#include <edit_logline.h>

EditLogLine::EditLogLine(RDLogLine *line,QString *filter,QString *group,
			 QString svcname,RDGroupList *grplist,
			 RDLogEvent *log,int lineno,
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

  setCaption(tr("Edit Log Entry"));

  edit_logline=line;
  edit_filter=filter;
  edit_group=group;
  edit_service=svcname;
  edit_group_list=grplist;
  edit_log_event=log;
  edit_line=lineno;

  // 
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont radio_font=QFont("Helvetica",10,QFont::Normal);
  radio_font.setPixelSize(10);

  //
  // Time Type
  //
  edit_timetype_box=new QCheckBox(this,"edit_timetype_box");
  edit_timetype_box->setGeometry(10,22,15,15);
  edit_timetype_label=new QLabel(edit_timetype_box,tr("Start at:"),
			   this,"edit_timetype_label");
  edit_timetype_label->setGeometry(30,21,80,17);
  edit_timetype_label->setFont(label_font);
  edit_timetype_label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Start Time
  //
  edit_time_edit=new RDTimeEdit(this,"edit_time_edit");
  edit_time_edit->setGeometry(85,19,85,20);
  edit_time_edit->setDisplay(RDTimeEdit::Hours|RDTimeEdit::Minutes|
			     RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  connect(edit_time_edit,SIGNAL(valueChanged(const QTime &)),
	  this,SLOT(timeChangedData(const QTime &)));

  //
  // Grace Time
  //
  edit_grace_group
    =new QButtonGroup(1,Qt::Vertical,
		      tr("Action If Previous Event Still Playing"),
		      this,"edit_grace_group");
  edit_grace_group->setGeometry(175,11,435,50);
  edit_grace_group->setFont(label_font);
  edit_grace_group->setRadioButtonExclusive(true);
  QRadioButton *radio_button=
    new QRadioButton(tr("Start Immediately"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);
  radio_button=new QRadioButton(tr("Make Next"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);
  radio_button=new QRadioButton(tr("Wait up to"),edit_grace_group);
  edit_grace_group->insert(radio_button);
  radio_button->setFont(radio_font);

  edit_grace_edit=new RDTimeEdit(this,"edit_grace_edit");
  edit_grace_edit->setGeometry(538,31,65,20);
  edit_grace_edit->
    setDisplay(RDTimeEdit::Minutes|RDTimeEdit::Seconds|RDTimeEdit::Tenths);
  connect(edit_timetype_box,SIGNAL(toggled(bool)),
	  this,SLOT(timeToggledData(bool)));
  connect(edit_grace_group,SIGNAL(clicked(int)),
	  this,SLOT(graceClickedData(int)));

  //
  // Transition Type
  //
  edit_transtype_box=new QComboBox(this,"edit_transtype_box");
  edit_transtype_box->setGeometry(485,68,110,26);
  edit_transtype_box->insertItem(tr("Play"));
  edit_transtype_box->insertItem(tr("Segue"));
  edit_transtype_box->insertItem(tr("Stop"));  
  edit_time_label=new QLabel(edit_transtype_box,tr("Transition Type:"),
			     this,"edit_transtype_label");
  edit_time_label->setGeometry(190,68,290,26);
  edit_time_label->setFont(label_font);
  edit_time_label->setAlignment(AlignRight|AlignVCenter);

  // Overlap Box
  edit_overlap_box=new QCheckBox(this,"edit_overlap_box");
  edit_overlap_box->setGeometry(30,72,15,15);
  edit_overlap_label=new QLabel(edit_overlap_box,tr("No Fade on Segue Out"),
				this,"edit_overlap_label");
  edit_overlap_label->setGeometry(50,68,130,26);
  edit_overlap_label->setFont(button_font);
  edit_overlap_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  
  //
  // Cart Number
  //
  edit_cart_edit=new QLineEdit(this,"edit_cart_edit");
  edit_cart_edit->setGeometry(10,116,60,18);
  QLabel *label=new QLabel(tr("Cart"),this,"cart_label");
  label->setFont(label_font);
  label->setGeometry(12,100,60,14);

  //
  // Title 
  //
  edit_title_edit=new QLineEdit(this,"edit_title_edit");
  edit_title_edit->setGeometry(75,116,260,18);
  edit_title_edit->setReadOnly(true);
  label=new QLabel(tr("Title"),this,"title_label");
  label->setFont(label_font);
  label->setGeometry(77,100,110,14);

  //
  // Artist 
  //
  edit_artist_edit=new QLineEdit(this,"edit_artist_edit");
  edit_artist_edit->setGeometry(340,116,sizeHint().width()-350,18);
  edit_artist_edit->setReadOnly(true);
  label=new QLabel(tr("Artist"),this,"artist_label");
  label->setFont(label_font);
  label->setGeometry(342,100,110,14);

  //
  // Cart Button
  //
  QPushButton *button=new QPushButton(this,"cart_button");
  button->setGeometry(20,144,80,50);
  button->setFont(button_font);
  button->setText(tr("Select\nCart"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCartData()));

  //
  //  Ok Button
  //
  button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(button_font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  edit_time_edit->setTime(edit_logline->startTime(RDLogLine::Logged));
  if(edit_logline->timeType()==RDLogLine::Hard) {
    edit_timetype_box->setChecked(true);
    timeToggledData(true);
  }
  else {
    timeToggledData(false);
  }
  timeChangedData(edit_time_edit->time());
  switch(edit_logline->graceTime()) {
      case -1:
	edit_grace_group->setButton(1);
	graceClickedData(1);
	break;

      case 0:
	edit_grace_group->setButton(0);
	graceClickedData(0);
	break;

      default:
	edit_grace_group->setButton(2);
	graceClickedData(2);
	edit_grace_edit->setTime(QTime().addMSecs(edit_logline->graceTime()));
	break;
  }
  edit_transtype_box->setCurrentItem(edit_logline->transType());
  if(edit_logline->segueStartPoint(RDLogLine::LogPointer)<0
     && edit_logline->segueEndPoint(RDLogLine::LogPointer)<0
     && edit_logline->endPoint(RDLogLine::LogPointer)<0
     && edit_logline->fadedownPoint(RDLogLine::LogPointer)<0) {
    edit_overlap_box->setEnabled(true);
    edit_overlap_label->setEnabled(true);
    if(edit_logline->segueGain()==0) {
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
  FillCart(edit_logline->cartNumber());
}


EditLogLine::~EditLogLine()
{
}


QSize EditLogLine::sizeHint() const
{
  return QSize(625,230);
} 


QSizePolicy EditLogLine::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditLogLine::selectCartData()
{
  bool ok;
  int cartnum=edit_cart_edit->text().toInt(&ok);
  if(!ok) {
    cartnum=-1;
  }
  if(log_cart_dialog->exec(&cartnum,RDCart::All,&edit_service,1,
			   rduser->name(),rduser->password())==0) {
    FillCart(cartnum);
  }
}


void EditLogLine::timeChangedData(const QTime &time)
{
  QString str;

  if(edit_timetype_box->isChecked()) {
    str=QString(tr("Transition If Previous Cart Ends Before"));
    edit_time_label->
      setText(QString().sprintf("%s %s:",(const char *)str,
				(const char *)edit_time_edit->time().
				toString("hh:mm:ss.zzz").left(10)));
  }
}


void EditLogLine::timeToggledData(bool state)
{
  QString str;

  edit_time_edit->setEnabled(state);
  edit_grace_group->setEnabled(state);
  if(state) {
    graceClickedData(edit_grace_group->selectedId());
    str=QString(tr("Transition If Previous Cart Ends Before"));
    edit_time_label->
      setText(QString().sprintf("%s %s:",(const char *)str,
				(const char *)edit_time_edit->time().
				toString("hh:mm:ss.zzz").left(10)));
  }
  else {
    edit_grace_edit->setDisabled(true);
    edit_time_label->setText(tr("Transition Type:"));
  }
}


void EditLogLine::graceClickedData(int id)
{
  switch(id) {
      case 0:
	edit_grace_edit->setDisabled(true);
	break;

      case 1:
	edit_grace_edit->setDisabled(true);
	break;

      case 2:
	edit_grace_edit->setEnabled(true);
	break;
  }
}


void EditLogLine::okData()
{
  if(edit_cart_edit->text().isEmpty()) {
    QMessageBox::warning(this,tr("Missing Cart"),
			 tr("You must supply a cart number!"));
    return;
  }
  RDCart *cart=new RDCart(edit_cart_edit->text().toUInt());
  if(cart->exists()) {
    if(!edit_group_list->isGroupValid(cart->groupName())) {
      delete cart;
      QMessageBox::warning(this,tr("Disabled Cart"),
			   tr("This cart belongs to a disabled\ngroup for the specified service!"));
      return;
    }
  }
  delete cart;
  if(edit_timetype_box->isChecked()&&
     edit_log_event->exists(edit_time_edit->time(),edit_line)) {
      QMessageBox::warning(this,tr("Duplicate Start Time"),
		  tr("An event is already scheduled with this start time!"));
      return;
  }
  if(edit_timetype_box->isChecked()) {
    edit_logline->setTimeType(RDLogLine::Hard);
    edit_logline->setStartTime(RDLogLine::Logged,edit_time_edit->time());
    switch(edit_grace_group->selectedId()) {
	case 0:
	  edit_logline->setGraceTime(0);
	  break;

	case 1:
	  edit_logline->setGraceTime(-1);
	  break;

	case 2:
	  edit_logline->setGraceTime(QTime().msecsTo(edit_grace_edit->time()));
	  break;
    }
  }
  else {
    edit_logline->setTimeType(RDLogLine::Relative);
    edit_logline->setStartTime(RDLogLine::Logged,QTime());
    edit_logline->setGraceTime(0);
  }
  edit_logline->
    setTransType((RDLogLine::TransType)edit_transtype_box->currentItem());
  edit_logline->setCartNumber(edit_cart_edit->text().toUInt());
  if(edit_logline->segueStartPoint(RDLogLine::LogPointer)<0
     && edit_logline->segueEndPoint(RDLogLine::LogPointer)<0
     && edit_logline->endPoint(RDLogLine::LogPointer)<0
     && edit_logline->fadedownPoint(RDLogLine::LogPointer)<0) {
    if(edit_overlap_box->isChecked()) {
      edit_logline->setSegueGain(0);
    }
    else {
      edit_logline->setSegueGain(RD_FADE_DEPTH);
    }
  }
  done(edit_logline->id());
}


void EditLogLine::cancelData()
{
  done(-1);
}


void EditLogLine::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditLogLine::FillCart(int cartnum)
{
  RDCart *cart=new RDCart(cartnum);
  if(cartnum!=0) {
    edit_cart_edit->setText(QString().sprintf("%05u",cartnum));
  }
  edit_title_edit->setText(cart->title());
  edit_artist_edit->setText(cart->artist());
  delete cart;
}
