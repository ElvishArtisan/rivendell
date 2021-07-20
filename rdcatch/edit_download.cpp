// edit_download.cpp
//
// Edit a Rivendell Download Event
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
#include <QUrl>

#include <rdcut_path.h>
#include <rdescape_string.h>
#include <rdtextvalidator.h>

#include "edit_download.h"
#include "globals.h"

EditDownload::EditDownload(int record_id,std::vector<int> *adds,QString *filter,
			   QWidget *parent)
  : RDDialog(parent)
{
  QString temp;

  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setWindowTitle("RDCatch - "+tr("Edit Download"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(record_id);

  //
  // Dialogs
  //
  edit_cut_dialog=new RDCutDialog(edit_filter,&edit_group,&edit_schedcode,
				  false,true,true,"RDCatch",false,this);

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
  // Url
  //
  edit_url_edit=new QLineEdit(this);
  edit_url_edit->setMaxLength(255);
  edit_url_edit->setValidator(validator);
  connect(edit_url_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(urlChangedData(const QString &)));
  edit_url_label=new QLabel(tr("Url:"),this);
  edit_url_label->setFont(labelFont());
  edit_url_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Username
  //
  edit_username_edit=new QLineEdit(this);
  edit_username_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_username_label=new QLabel(tr("Username:"),this);
  edit_username_label->setFont(labelFont());
  edit_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Password
  //
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_password_label=new QLabel(tr("Password:"),this);
  edit_password_label->setFont(labelFont());
  edit_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this);
  edit_destination_edit->setReadOnly(true);
  edit_destination_label=new QLabel(tr("Destination:"),this);
  edit_destination_label->setFont(labelFont());
  edit_destination_label->setAlignment(Qt::AlignRight);
  edit_destination_button=new QPushButton(this);
  edit_destination_button->setFont(subLabelFont());
  edit_destination_button->setText(tr("Select"));
  connect(edit_destination_button,SIGNAL(clicked()),
	  this,SLOT(selectCartData()));

  //
  // Channels
  //
  edit_channels_box=new QComboBox(this);
  edit_channels_box->insertItem(0,"1");
  edit_channels_box->insertItem(1,"2");
  edit_channels_label=new QLabel(tr("Channels:"),this);
  edit_channels_label->setFont(labelFont());
  edit_channels_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Autotrim Controls
  //
  edit_autotrim_box=new QCheckBox(this);
  connect(edit_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));
  edit_autotrim_label_label=new QLabel(tr("Autotrim"),this);
  edit_autotrim_label_label->setFont(labelFont());
  edit_autotrim_label_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit_autotrim_spin=new QSpinBox(this);
  edit_autotrim_spin->setRange(-99,-1);
  edit_autotrim_label=new QLabel(tr("Level:"),this);
  edit_autotrim_label->setFont(labelFont());
  edit_autotrim_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_autotrim_unit=new QLabel(tr("dBFS"),this);
  edit_autotrim_unit->setFont(labelFont());
  edit_autotrim_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Normalize Controls
  //
  edit_normalize_box=new QCheckBox(this);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeToggledData(bool)));
  edit_normalize_label_label=new QLabel(tr("Normalize"),this);
  edit_normalize_label_label->setFont(labelFont());
  edit_normalize_label_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
  edit_normalize_spin=new QSpinBox(this);
  edit_normalize_spin->setRange(-99,-1);
  edit_normalize_label=new QLabel(tr("Level:"),this);
  edit_normalize_label->setFont(labelFont());
  edit_normalize_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_normalize_unit=new QLabel(tr("dBFS"),this);
  edit_normalize_unit->setFont(labelFont());
  edit_normalize_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  // Export Metadata Box
  //
  edit_metadata_box=new QCheckBox(this);
  edit_metadata_label=new QLabel(tr("Update Library Metadata"),this);
  edit_metadata_label->setFont(labelFont());
  edit_metadata_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

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
  // Event Offset
  //
  edit_eventoffset_spin=new QSpinBox(this);
  edit_eventoffset_spin->setRange(-30,30);
  edit_eventoffset_label=new QLabel(tr("Event Offset:"),this);
  edit_eventoffset_label->setFont(labelFont());
  edit_eventoffset_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  edit_eventoffset_unit=new QLabel(tr("days"),this);
  edit_eventoffset_unit->setFont(labelFont());
  edit_eventoffset_unit->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

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
  // Populate Data
  //
  edit_event_widget->fromRecording(edit_recording->id());
  edit_description_edit->setText(edit_recording->description());
  edit_url_edit->setText(edit_recording->url());
  edit_username_edit->setText(edit_recording->urlUsername());
  edit_password_edit->setText(edit_recording->urlPassword());
  edit_cutname=edit_recording->cutName();
  edit_destination_edit->setText("Cut "+edit_cutname);
  edit_channels_box->setCurrentIndex(edit_recording->channels()-1);
  if(edit_recording->trimThreshold()>0) {
    edit_autotrim_box->setChecked(true);
    edit_autotrim_spin->setValue(-(edit_recording->trimThreshold()/100));
  }
  else {
    edit_autotrim_box->setChecked(false);
    edit_autotrim_spin->setValue(rda->libraryConf()->trimThreshold()/100);
  }
  autotrimToggledData(edit_autotrim_box->isChecked());
  if(edit_recording->normalizationLevel()<0) {
    edit_normalize_box->setChecked(true);
    edit_normalize_spin->setValue(edit_recording->normalizationLevel()/100);
  }
  else {
    edit_normalize_box->setChecked(false);
    edit_normalize_spin->setValue(rda->libraryConf()->ripperLevel()/100);
  }
  normalizeToggledData(edit_normalize_box->isChecked());
  edit_metadata_box->setChecked(edit_recording->enableMetadata());
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_eventoffset_spin->setValue(edit_recording->eventdateOffset());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
}


EditDownload::~EditDownload()
{
  delete edit_event_widget;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditDownload::sizeHint() const
{
  return QSize(520,432);
} 


QSizePolicy EditDownload::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditDownload::urlChangedData(const QString &str)
{
  QUrl url(str);
  QString scheme=url.scheme();
  if((scheme=="ftp")||(scheme=="ftps")||(scheme=="http")||(scheme=="file")||
     (scheme=="scp")||(scheme=="sftp")) {
    edit_username_label->setEnabled(true);
    edit_username_edit->setEnabled(true);
    edit_password_label->setEnabled(true);
    edit_password_edit->setEnabled(true);
  }
  else {
    edit_username_label->setDisabled(true);
    edit_username_edit->setDisabled(true);
    edit_password_label->setDisabled(true);
    edit_password_edit->setDisabled(true);
  }
}


void EditDownload::selectCartData()
{
  if(edit_cut_dialog->exec(&edit_cutname)) {
    edit_description_edit->setText(RDCutPath(edit_cutname));
    edit_destination_edit->setText(tr("Cut")+" "+edit_cutname);
  }
}


void EditDownload::autotrimToggledData(bool state)
{
  edit_autotrim_label->setEnabled(state);
  edit_autotrim_spin->setEnabled(state);
  edit_autotrim_unit->setEnabled(state);
}


void EditDownload::normalizeToggledData(bool state)
{
  edit_normalize_label->setEnabled(state);
  edit_normalize_spin->setEnabled(state);
  edit_normalize_unit->setEnabled(state);
}


void EditDownload::saveasData()
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


void EditDownload::okData()
{
  QUrl url(edit_url_edit->text());
  if(url.isRelative()||(edit_url_edit->text().right(1)=="/")) {
    QMessageBox::warning(this,tr("Invalid URL"),tr("The URL is invalid!"));
    return;
  }
  QString scheme=url.scheme();
  if((scheme!="ftp")&&(scheme!="ftps")&&(scheme!="http")&&(scheme!="https")&&
     (scheme!="file")&&(scheme!="scp")&&(scheme!="sftp")) {
    QMessageBox::warning(this,
			 tr("Invalid URL"),tr("Unsupported URL scheme!"));
    return;
  }
  if((scheme=="file")&&(edit_username_edit->text().isEmpty())) {
    QMessageBox::warning(this,tr("Missing Username"),
			 tr("You must specify a username!"));
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


void EditDownload::cancelData()
{
  done(false);
}


void EditDownload::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  edit_description_edit->setGeometry(115,43,size().width()-125,20);
  edit_description_label->setGeometry(10,43,100,20);

  edit_url_edit->setGeometry(115,70,size().width()-125,20);
  edit_url_label->setGeometry(10,70,100,20);

  edit_username_edit->setGeometry(115,97,150,20);
  edit_username_label->setGeometry(10,97,100,20);

  edit_password_edit->setGeometry(360,97,size().width()-370,20);
  edit_password_label->setGeometry(275,97,80,20);

  edit_destination_edit->setGeometry(115,124,size().width()-195,20);
  edit_destination_button->setGeometry(size().width()-70,122,60,24);
  edit_destination_label->setGeometry(10,127,100,19);

  edit_channels_box->setGeometry(190,149,40,20);
  edit_channels_label->setGeometry(120,149,70,20);

  edit_autotrim_box->setGeometry(120,175,15,15);
  edit_autotrim_label_label->setGeometry(140,173,80,20);
  edit_autotrim_spin->setGeometry(265,173,40,20);
  edit_autotrim_label->setGeometry(220,173,40,20);
  edit_autotrim_unit->setGeometry(310,173,40,20);

  edit_normalize_label_label->setGeometry(140,197,80,20);
  edit_normalize_box->setGeometry(120,199,15,15);
  edit_normalize_spin->setGeometry(265,197,40,20);
  edit_normalize_label->setGeometry(220,197,40,20);
  edit_normalize_unit->setGeometry(310,197,40,20);

  edit_metadata_box->setGeometry(120,222,15,15);
  edit_metadata_label->setGeometry(140,222,160,20);

  edit_dow_selector->setGeometry(10,257,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());

  edit_oneshot_box->setGeometry(20,335,15,15);
  edit_oneshot_label->setGeometry(40,333,115,20);

  edit_eventoffset_spin->setGeometry(245,333,45,20);
  edit_eventoffset_label->setGeometry(140,333,100,20);
  edit_eventoffset_unit->setGeometry(295,333,40,20);

  edit_saveas_button->setGeometry(size().width()-300,size().height()-60,80,50);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditDownload::keyPressEvent(QKeyEvent *e)
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


void EditDownload::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditDownload::Save()
{
  edit_event_widget->toRecording(edit_recording->id());
  edit_recording->setType(RDRecording::Download);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setChannels(edit_channels_box->currentIndex()+1);
  if(edit_autotrim_box->isChecked()) {
    edit_recording->setTrimThreshold(-100*edit_autotrim_spin->value());
  }
  else {
    edit_recording->setTrimThreshold(0);
  }
  if(edit_normalize_box->isChecked()) {
    edit_recording->setNormalizationLevel(100*edit_normalize_spin->value());
  }
  else {
    edit_recording->setNormalizationLevel(0);
  }
  edit_recording->setCutName(edit_cutname);
  edit_recording->setUrl(edit_url_edit->text());
  edit_recording->setUrlUsername(edit_username_edit->text());
  edit_recording->setUrlPassword(edit_password_edit->text());
  edit_recording->setEnableMetadata(edit_metadata_box->isChecked());
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setEventdateOffset(edit_eventoffset_spin->value());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


bool EditDownload::CheckEvent(bool include_myself)
{
  QString sql=QString("select `ID` from `RECORDINGS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString().sprintf("(`TYPE`=%d)&&",RDRecording::Download)+
    "(`START_TIME`='"+RDEscapeString(edit_event_widget->startTime().
				    toString("hh:mm:ss"))+"')&&"+
    "(`URL`='"+RDEscapeString(edit_url_edit->text())+"')&&"+
    "(`CUT_NAME`='"+RDEscapeString(edit_destination_edit->text().right(10))+"')";
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

