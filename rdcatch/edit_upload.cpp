// edit_upload.cpp
//
// Edit a Rivendell Upload Event
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

#include <QMessageBox>
#include <QUrl>

#include <rdcut_path.h>
#include <rdescape_string.h>
#include <rdexport_settings_dialog.h>
#include <rdtextvalidator.h>

#include "edit_upload.h"
#include "globals.h"

EditUpload::EditUpload(int id,std::vector<int> *adds,QString *filter,
		       QWidget *parent)
  : RDDialog(parent)
{
  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setWindowTitle("RDCatch - "+tr("Edit Upload"));

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);

  //
  // Dialogs
  //
  edit_cut_dialog=new RDCutDialog(edit_filter,&edit_group,&edit_schedcode,
				  false,false,false,"RDCatch",false,this);
  //
  // Event Widget
  //
  edit_event_widget=new EventWidget(EventWidget::OtherEvent,this);

  //
  // Source
  //
  edit_source_edit=new QLineEdit(this);
  edit_source_edit->setReadOnly(true);
  edit_source_label=new QLabel(tr("Source:"),this);
  edit_source_label->setFont(labelFont());
  edit_source_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_source_select_button=new QPushButton(this);
  edit_source_select_button->setFont(subButtonFont());
  edit_source_select_button->setText(tr("Select"));
  connect(edit_source_select_button,SIGNAL(clicked()),
	  this,SLOT(selectCartData()));

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this);
  edit_description_label=new QLabel(tr("Description:"),this);
  edit_description_label->setFont(labelFont());
  edit_description_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // RSS Feed
  //
  edit_feed_box=new QComboBox(this);
  connect(edit_feed_box,SIGNAL(activated(int)),this,SLOT(feedChangedData(int)));
  edit_feed_label=new QLabel(tr("RSS Feed:"),this);
  edit_feed_label->setFont(labelFont());
  edit_feed_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_feed_model=new RDFeedListModel(true,true,this);
  edit_feed_model->setFont(defaultFont());
  edit_feed_box->setModel(edit_feed_model);

  //
  // Url
  //
  edit_url_edit=new QLineEdit(this);
  edit_url_edit->setMaxLength(191);
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
  edit_username_label=new QLabel(tr("Username:"),this);
  edit_username_label->setFont(labelFont());
  edit_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Password
  //
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setMaxLength(64);
  edit_password_label=new QLabel(tr("Password:"),this);
  edit_password_label->setFont(labelFont());
  edit_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Use ssh(1) ID File
  //
  edit_use_id_file_label=
    new QLabel(tr("Authenticate with local identity file"),this);
  edit_use_id_file_label->setFont(labelFont());
  edit_use_id_file_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  edit_use_id_file_check=new QCheckBox(this);
  
  //
  // Audio Format
  //
  edit_format_edit=new QLineEdit(this);
  edit_format_edit->setReadOnly(true);
  edit_format_label=new QLabel(tr("Export Format:"),this);
  edit_format_label->setFont(labelFont());
  edit_format_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_format_set_button=new QPushButton(this);
  edit_format_set_button->setFont(subButtonFont());
  edit_format_set_button->setText(tr("Set"));
  connect(edit_format_set_button,SIGNAL(clicked()),this,SLOT(setFormatData()));

  //
  // Normalize Check Box
  //
  edit_normalize_box=new QCheckBox(this);
  edit_normalize_box->setChecked(true);
  edit_normalize_label=new QLabel(tr("Normalize"),this);
  edit_normalize_label->setFont(labelFont());
  edit_normalize_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  edit_normalize_spin=new QSpinBox(this);
  edit_normalize_spin->setRange(-30,0);
  edit_normalize_level_label=new QLabel(tr("Level:"),this);
  edit_normalize_level_label->setFont(labelFont());
  edit_normalize_level_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_normalize_unit=new QLabel(tr("dBFS"),this);
  edit_normalize_unit->setFont(labelFont());
  edit_normalize_unit->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Export Metadata Box
  //
  edit_metadata_box=new QCheckBox(this);
  edit_metadata_label=new QLabel(tr("Export Library Metadata"),this);
  edit_metadata_label->setFont(labelFont());
  edit_metadata_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Day of the week Selector
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
  edit_eventoffset_unit_label=new QLabel(tr("days"),this);
  edit_eventoffset_unit_label->setFont(labelFont());
  edit_eventoffset_unit_label->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

  //
  //  Save As Button
  //
  event_saveas_button=new QPushButton(this);
  event_saveas_button->setFont(buttonFont());
  event_saveas_button->setText(tr("Save As\nNew"));
  connect(event_saveas_button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    event_saveas_button->hide();
  }

  //
  //  Ok Button
  //
  event_ok_button=new QPushButton(this);
  event_ok_button->setDefault(true);
  event_ok_button->setFont(buttonFont());
  event_ok_button->setText(tr("OK"));
  connect(event_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  event_cancel_button=new QPushButton(this);
  event_cancel_button->setFont(buttonFont());
  event_cancel_button->setText(tr("Cancel"));
  connect(event_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

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
  edit_url_edit->setText(edit_recording->url());
  edit_username_edit->setText(edit_recording->urlUsername());
  edit_password_edit->setText(edit_recording->urlPassword());
  edit_use_id_file_check->setChecked(edit_recording->urlUseIdFile());
  edit_cutname=edit_recording->cutName();
  if(!edit_recording->cutName().isEmpty()) {
    edit_source_edit->setText("Cut "+edit_recording->cutName());
  }
  edit_metadata_box->setChecked(edit_recording->enableMetadata());
  edit_dow_selector->fromRecording(edit_recording->id());
  edit_eventoffset_spin->setValue(edit_recording->eventdateOffset());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  edit_settings.setFormat(edit_recording->format());
  edit_settings.setChannels(edit_recording->channels());
  edit_settings.setSampleRate(edit_recording->sampleRate());
  edit_settings.setBitRate(edit_recording->bitrate());
  edit_settings.setQuality(edit_recording->quality());
  edit_format_edit->setText(edit_settings.description());
  if(edit_recording->normalizationLevel()>0) {
    edit_normalize_box->setChecked(false);
  }
  else {
    edit_normalize_box->setChecked(true);
    edit_normalize_spin->setValue(edit_recording->normalizationLevel()/100);
  }
  normalizeCheckData(edit_normalize_box->isChecked());
  edit_feed_box->setCurrentText(edit_recording->feedKeyName());
  feedChangedData(edit_feed_box->currentIndex());
}


EditUpload::~EditUpload()
{
  delete edit_event_widget;
  delete edit_dow_selector;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditUpload::sizeHint() const
{
  return QSize(edit_event_widget->sizeHint().width(),451);
} 


QSizePolicy EditUpload::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditUpload::stationChangedData(const QString &str)
{
  if(!CheckFormat()) {
    QMessageBox::warning(this,tr("Unsupported Format"),
			 tr("The currently selected export format is unsupported on this host!"));
  }
}


void EditUpload::feedChangedData(int index)
{
  edit_url_label->setEnabled(index==0);
  edit_url_edit->setEnabled(index==0);

  edit_username_label->setEnabled(index==0);
  edit_username_edit->setEnabled(index==0);

  edit_password_label->setEnabled(index==0);
  edit_password_edit->setEnabled(index==0);

  urlChangedData(edit_url_edit->text());
  
  edit_format_label->setEnabled(index==0);
  edit_format_edit->setEnabled(index==0);
  edit_format_set_button->setEnabled(index==0);

  edit_normalize_label->setEnabled(index==0);
  edit_normalize_box->setEnabled(index==0);
  edit_normalize_level_label->
    setEnabled((index==0)&&edit_normalize_box->isChecked());
  edit_normalize_spin->setEnabled((index==0)&&edit_normalize_box->isChecked());
  edit_normalize_unit->setEnabled((index==0)&&edit_normalize_box->isChecked());

  edit_metadata_box->setEnabled(index==0);
  edit_metadata_label->setEnabled(index==0);
}


void EditUpload::urlChangedData(const QString &str)
{
  QUrl url(str);
  QString scheme=url.scheme().toLower();
  if(((scheme=="ftp")||(scheme=="ftps")||(scheme=="file")||
      (scheme=="scp")||(scheme=="sftp"))&&
     edit_feed_box->currentIndex()==0) {
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
  if((scheme=="sftp")&&(!rda->station()->sshIdentityFile().isEmpty())&&
     (edit_feed_box->currentIndex()==0)) {
    edit_use_id_file_check->setEnabled(true);
    edit_use_id_file_label->setEnabled(true);
  }
  else {
    edit_use_id_file_check->setDisabled(true);
    edit_use_id_file_label->setDisabled(true);
  }
}


void EditUpload::selectCartData()
{
  QString str;

  if(edit_cut_dialog->exec(&edit_cutname)) {
    edit_description_edit->setText(RDCutPath(edit_cutname));
    str=QString(tr("Cut"));
    edit_source_edit->setText(tr("Cut")+" "+edit_cutname);
  }
}


void EditUpload::setFormatData()
{
  RDExportSettingsDialog *dialog=new RDExportSettingsDialog("RDCatch",this);
  dialog->exec(&edit_settings);
  delete dialog;
  edit_format_edit->setText(edit_settings.description());
}


void EditUpload::normalizeCheckData(bool state)
{
  edit_normalize_level_label->
    setEnabled(state&&(edit_feed_box->currentIndex()==0));
  edit_normalize_spin->setEnabled(state&&(edit_feed_box->currentIndex()==0));
  edit_normalize_unit->setEnabled(state&&(edit_feed_box->currentIndex()==0));
}


void EditUpload::saveasData()
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


void EditUpload::okData()
{
  if(edit_feed_box->currentIndex()==0) {  // No RSS feed selected
    if(!CheckFormat()) {
      QMessageBox::warning(this,tr("Unsupported Format"),
	   tr("The currently selected export format is unsupported on host ")+
			   edit_event_widget->stationName()+"!");
      return;
    }
    QUrl url(edit_url_edit->text());
    if(url.isRelative()||(edit_url_edit->text().right(1)=="/")) {
      QMessageBox::warning(this,tr("Invalid URL"),tr("The URL is invalid!"));
      return;
    }
    QString scheme=url.scheme();
    if((scheme!="ftp")&&(scheme!="ftps")&&(scheme!="file")&&
       (scheme!="scp")&&(scheme!="sftp")) {
      QMessageBox::warning(this,
			   tr("Invalid URL"),tr("Unsupported URL scheme!"));
      return;
    }
    if((scheme=="file")&&(edit_username_edit->text().isEmpty())) {
      QMessageBox::warning(this,tr("Missing Username"),
			   tr("You must specify a username!"));
      return;
    }
  }
  if(!CheckEvent(false)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
			 tr("An event with these parameters already exists!"));
    return;
  }
  Save();
  done(true);
}


void EditUpload::cancelData()
{
  done(false);
}


void EditUpload::resizeEvent(QResizeEvent *e)
{
  edit_event_widget->setGeometry(10,11,edit_event_widget->sizeHint().width(),
				 edit_event_widget->sizeHint().height());

  edit_source_edit->setGeometry(115,43,size().width()-195,20);
  edit_source_label->setGeometry(10,43,100,19);
  edit_source_select_button->setGeometry(size().width()-70,41,60,24);
  edit_description_edit->setGeometry(115,70,size().width()-125,20);
  edit_description_label->setGeometry(10,70,100,20);

  edit_feed_box->setGeometry(115,97,160,20);
  edit_feed_label->setGeometry(10,97,100,19);
  edit_url_edit->setGeometry(115,124,size().width()-125,20);
  edit_url_label->setGeometry(10,124,100,20);
  edit_username_edit->setGeometry(115,151,150,20);
  edit_username_label->setGeometry(10,151,100,20);
  edit_password_edit->setGeometry(360,151,size().width()-370,20);
  edit_password_label->setGeometry(275,151,80,20);
  edit_use_id_file_check->setGeometry(120,176,15,15);
  edit_use_id_file_label->setGeometry(140,174,size().width()-150,20);

  edit_format_label->setGeometry(5,205,105,20);
  edit_format_edit->setGeometry(115,205,size().width()-195,20);
  edit_format_set_button->setGeometry(size().width()-70,203,60,24);
  edit_normalize_box->setGeometry(115,233,15,15);
  edit_normalize_label->setGeometry(135,231,83,20);
  edit_normalize_spin->setGeometry(265,231,40,20);
  edit_normalize_level_label->setGeometry(215,231,45,20);
  edit_normalize_unit->setGeometry(310,231,40,20);
  edit_metadata_box->setGeometry(115,254,15,15);
  edit_metadata_label->setGeometry(135,252,160,20);

  edit_dow_selector->setGeometry(10,283,edit_dow_selector->sizeHint().width(),
				 edit_dow_selector->sizeHint().height());
  edit_oneshot_box->setGeometry(20,359,15,15);
  edit_oneshot_label->setGeometry(40,357,115,20);
  edit_eventoffset_spin->setGeometry(245,357,45,20);
  edit_eventoffset_label->setGeometry(140,357,100,20);
  edit_eventoffset_unit_label->setGeometry(295,357,40,20);

  event_saveas_button->setGeometry(size().width()-300,size().height()-60,80,50);
  event_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  event_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditUpload::keyPressEvent(QKeyEvent *e)
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


void EditUpload::closeEvent(QCloseEvent *e)
{
  cancelData();
}


bool EditUpload::CheckFormat()
{
  bool res=false;

  RDStation *station=new RDStation(edit_event_widget->stationName());
  switch(edit_settings.format()) {
  case RDSettings::Pcm16:
  case RDSettings::Pcm24:
  case RDSettings::MpegL2:
  case RDSettings::MpegL2Wav:
    res=true;
    break;

  case RDSettings::MpegL1:
    res=false;
    break;

  case RDSettings::MpegL3:
    if(station->haveCapability(RDStation::HaveLame)) {
      res=true;
    }
    break;
	
  case RDSettings::Flac:
    if(station->haveCapability(RDStation::HaveFlac)) {
      res=true;
    }
    break;
	
  case RDSettings::OggVorbis:
    if(station->haveCapability(RDStation::HaveOggenc)) {
      res=true;
    }
    break;
  }
  delete station;

  return res;
}


void EditUpload::Save()
{
  edit_recording->setIsActive(edit_event_widget->isActive());
  edit_recording->setStation(edit_event_widget->stationName());
  edit_recording->setStartTime(edit_event_widget->startTime());
  edit_recording->setType(RDRecording::Upload);
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_source_edit->text().right(10));
  edit_recording->setUrl(edit_url_edit->text());
  edit_recording->setUrlUsername(edit_username_edit->text());
  edit_recording->setUrlPassword(edit_password_edit->text());
  edit_recording->setUrlUseIdFile(edit_use_id_file_check->isChecked());
  edit_recording->setEnableMetadata(edit_metadata_box->isChecked());
  edit_dow_selector->toRecording(edit_recording->id());
  edit_recording->setEventdateOffset(edit_eventoffset_spin->value());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
  edit_recording->setFormat(edit_settings.format());
  edit_recording->setChannels(edit_settings.channels());
  edit_recording->setSampleRate(edit_settings.sampleRate());
  edit_recording->setBitrate(edit_settings.bitRate());
  edit_recording->setQuality(edit_settings.quality());
  if(edit_normalize_box->isChecked()) {
    edit_recording->setNormalizationLevel(edit_normalize_spin->value()*100);
  }
  else {
    edit_recording->setNormalizationLevel(1);
  }

  //
  // FIXME: The indexing schema for feeds is lame and fundamentally broken.
  //
  int feed_id=edit_feed_model->
    feedId(edit_feed_model->index(edit_feed_box->currentIndex(),0));
  if(feed_id==0) {
    edit_recording->setFeedId(-1);
  }
  else {
    edit_recording->setFeedId(feed_id);
  }
}


bool EditUpload::CheckEvent(bool include_myself)
{
  QString sql=QString("select `ID` from `RECORDINGS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(edit_event_widget->stationName())+"')&&"+
    QString::asprintf("(`TYPE`=%d)&&",RDRecording::Upload)+
    "(`START_TIME`='"+
    edit_event_widget->startTime().toString("hh:mm:ss")+"')&&"+
    "(`URL`='"+RDEscapeString(edit_url_edit->text())+"')&&"+
    "(`CUT_NAME`='"+RDEscapeString(edit_source_edit->text().right(10))+
    "')";
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
    sql+=QString::asprintf("&&(`ID`!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=!q->first();
  delete q;

  return res;
}

