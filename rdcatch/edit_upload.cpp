// edit_upload.cpp
//
// Edit a Rivendell Upload Event
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_upload.cpp,v 1.22 2011/06/21 18:31:33 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>

#include <rddb.h>
#include <rdurl.h>
#include <rd.h>
#include <rdcut_dialog.h>
#include <rdcut_path.h>
#include <rdtextvalidator.h>
#include <rdexport_settings_dialog.h>
#include <rdescape_string.h>

#include <edit_upload.h>
#include <globals.h>


EditUpload::EditUpload(int id,std::vector<int> *adds,QString *filter,
		       QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QString temp;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont day_font=QFont("Helvetica",10,QFont::Normal);
  day_font.setPixelSize(10);

  edit_deck=NULL;
  edit_added_events=adds;
  edit_filter=filter;

  setCaption(tr("Edit Upload"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

  //
  // The Recording Record
  //
  edit_recording=new RDRecording(id);

  //
  // Active Button
  //
  edit_active_button=new QCheckBox(this,"edit_active_button");
  edit_active_button->setGeometry(10,11,20,20);
  QLabel *label=new QLabel(edit_active_button,
		   tr("Event Active"),this,"edit_active_label");
  label->setGeometry(30,11,125,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Station
  //
  edit_station_box=new QComboBox(this,"edit_station_box");
  edit_station_box->setGeometry(200,10,140,23);
  connect(edit_station_box,SIGNAL(textChanged(const QString &)),
	  this,SLOT(stationChangedData(const QString &)));
  label=new QLabel(edit_station_box,tr("Location:"),this,
				       "edit_station_label");
  label->setGeometry(125,10,70,23);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Start Time
  //
  edit_starttime_edit=new QTimeEdit(this,"edit_starttime_edit");
  edit_starttime_edit->setGeometry(sizeHint().width()-90,12,80,20);
  label=new QLabel(edit_starttime_edit,
		   tr("Start Time:"),this,"edit_starttime_label");
  label->setGeometry(sizeHint().width()-175,12,80,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // RSS Feed
  //
  edit_feed_box=new QComboBox(this,"edit_feed_box");
  edit_feed_box->setGeometry(115,43,100,20);
  edit_feed_box->insertItem(tr("[none]"));
  label=new QLabel(edit_feed_box,
		   tr("RSS Feed:"),this,"edit_feed_label");
  label->setGeometry(10,43,100,19);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Source
  //
  edit_destination_edit=new QLineEdit(this,"edit_destination_edit");
  edit_destination_edit->setGeometry(115,70,sizeHint().width()-195,20);
  edit_destination_edit->setReadOnly(true);
  label=new QLabel(edit_destination_edit,
		   tr("Source:"),this,"edit_destination_label");
  label->setGeometry(10,70,100,19);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"destination_button");
  button->setGeometry(sizeHint().width()-70,68,60,24);
  button->setFont(day_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCartData()));

  //
  // Description
  //
  edit_description_edit=new QLineEdit(this,"edit_description_edit");
  edit_description_edit->setGeometry(115,97,sizeHint().width()-125,20);
  edit_description_edit->setValidator(validator);
  label=new QLabel(edit_description_edit,
		   tr("Description:"),this,"edit_description_label");
  label->setGeometry(10,97,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Url
  //
  edit_url_edit=new QLineEdit(this,"edit_url_edit");
  edit_url_edit->setGeometry(115,124,sizeHint().width()-125,20);
  edit_url_edit->setMaxLength(255);
  edit_url_edit->setValidator(validator);
  connect(edit_url_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(urlChangedData(const QString &)));
  label=new QLabel(edit_url_edit,tr("Url:"),this,"edit_url_label");
  label->setGeometry(10,124,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Username
  //
  edit_username_edit=new QLineEdit(this,"edit_username_edit");
  edit_username_edit->setGeometry(115,151,150,20);
  edit_username_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_username_label=new QLabel(edit_username_edit,
		   tr("Username:"),this,"edit_username_label");
  edit_username_label->setGeometry(10,151,100,20);
  edit_username_label->setFont(label_font);
  edit_username_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Password
  //
  edit_password_edit=new QLineEdit(this,"edit_password_edit");
  edit_password_edit->setGeometry(360,151,sizeHint().width()-370,20);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_password_label=new QLabel(edit_password_edit,
		   tr("Password:"),this,"edit_password_label");
  edit_password_label->setGeometry(275,151,80,20);
  edit_password_label->setFont(label_font);
  edit_password_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Audio Format
  //
  edit_format_edit=new QLineEdit(this,"edit_format_edit");
  edit_format_edit->setGeometry(115,178,sizeHint().width()-195,20);
  edit_format_edit->setReadOnly(true);
  label=new QLabel(edit_format_edit,
		   tr("Export Format:"),this,"edit_format_label");
  label->setGeometry(5,178,105,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"format_button");
  button->setGeometry(sizeHint().width()-70,176,60,24);
  button->setFont(day_font);
  button->setText(tr("S&et"));
  connect(button,SIGNAL(clicked()),this,SLOT(setFormatData()));

  //
  // Normalize Check Box
  //
  edit_normalize_box=new QCheckBox(this,"edit_normalize_box");
  edit_normalize_box->setGeometry(115,208,15,15);
  edit_normalize_box->setChecked(true);
  label=new QLabel(edit_normalize_box,tr("Normalize"),
		   this,"normalize_check_label");
  label->setGeometry(135,206,83,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeCheckData(bool)));

  //
  // Normalize Level
  //
  edit_normalize_spin=new QSpinBox(this,"edit_normalize_spin");
  edit_normalize_spin->setGeometry(265,206,40,20);
  edit_normalize_spin->setRange(-30,0);
  edit_normalize_label=new QLabel(edit_normalize_spin,tr("Level:"),
				 this,"normalize_spin_label");
  edit_normalize_label->setGeometry(215,206,45,20);
  edit_normalize_label->setFont(label_font);
  edit_normalize_label->setAlignment(AlignRight|AlignVCenter);
  edit_normalize_unit=new QLabel(tr("dBFS"),this,"normalize_unit_label");
  edit_normalize_unit->setGeometry(310,206,40,20);
  edit_normalize_unit->setFont(label_font);
  edit_normalize_unit->setAlignment(AlignLeft|AlignVCenter);

  //
  // Export Metadata Box
  //
  edit_metadata_box=new QCheckBox(this,"edit_metadata_box");
  edit_metadata_box->setGeometry(115,231,15,15);
  label=new QLabel(edit_metadata_box,tr("Export Library Metadata"),
		   this,"metadata_check_label");
  label->setGeometry(135,231,160,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Button Label
  //
  label=new QLabel(tr("Active Days"),this,"active_days_label");
  label->setGeometry(47,263,90,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  //
  // Monday Button
  //
  edit_mon_button=new QCheckBox(this,"edit_mon_button");
  edit_mon_button->setGeometry(20,282,20,20);
  label=new QLabel(edit_mon_button,
		   tr("Monday"),this,"edit_mon_label");
  label->setGeometry(40,282,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Tuesday Button
  //
  edit_tue_button=new QCheckBox(this,"edit_tue_button");
  edit_tue_button->setGeometry(115,282,20,20);
  label=new QLabel(edit_tue_button,
		   tr("Tuesday"),this,"edit_tue_label");
  label->setGeometry(135,282,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Wednesday Button
  //
  edit_wed_button=new QCheckBox(this,"edit_wed_button");
  edit_wed_button->setGeometry(215,282,20,20);
  label=new QLabel(edit_wed_button,
		   tr("Wednesday"),this,"edit_wed_label");
  label->setGeometry(235,282,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Thursday Button
  //
  edit_thu_button=new QCheckBox(this,"edit_thu_button");
  edit_thu_button->setGeometry(335,282,20,20);
  label=new QLabel(edit_thu_button,
		   tr("Thursday"),this,"edit_thu_label");
  label->setGeometry(355,282,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Friday Button
  //
  edit_fri_button=new QCheckBox(this,"edit_fri_button");
  edit_fri_button->setGeometry(440,282,20,20);
  label=new QLabel(edit_fri_button,
		   tr("Friday"),this,"edit_fri_label");
  label->setGeometry(460,282,40,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Saturday Button
  //
  edit_sat_button=new QCheckBox(this,"edit_sat_button");
  edit_sat_button->setGeometry(130,307,20,20);
  label=new QLabel(edit_sat_button,
		   tr("Saturday"),this,"edit_sat_label");
  label->setGeometry(150,307,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Sunday Button
  //
  edit_sun_button=new QCheckBox(this,"edit_sun_button");
  edit_sun_button->setGeometry(300,307,20,20);
  label=new QLabel(edit_sun_button,
		   tr("Sunday"),this,"edit_sun_label");
  label->setGeometry(320,307,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this,"edit_oneshot_box");
  edit_oneshot_box->setGeometry(20,342,15,15);
  label=new QLabel(edit_oneshot_box,
		   tr("Make OneShot"),this,"edit_oneshot_label");
  label->setGeometry(40,343,115,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Event Offset
  //
  edit_eventoffset_spin=new QSpinBox(this,"edit_eventoffset_spin");
  edit_eventoffset_spin->setGeometry(245,340,45,20);
  edit_eventoffset_spin->setRange(-30,30);
  label=new QLabel(edit_eventoffset_spin,tr("Event Offset:"),
		   this,"edit_eventoffset_label");
  label->setGeometry(140,340,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignRight);
  label=new QLabel(edit_eventoffset_spin,tr("days"),
		   this,"edit_eventoffset_unit");
  label->setGeometry(295,335,40,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignLeft);

  //
  //  Save As Button
  //
  button=new QPushButton(this,"saveas_button");
  button->setGeometry(sizeHint().width()-300,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Save As\nNew"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveasData()));
  if(adds==NULL) {
    button->hide();
  }

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
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  q=new RDSqlQuery("select NAME from STATIONS where NAME!=\"DEFAULT\"");
  while(q->next()) {
    edit_station_box->insertItem(q->value(0).toString());
    if(edit_recording->station()==q->value(0).toString()) {
      edit_station_box->setCurrentItem(edit_station_box->count()-1);
    }
  }
  delete q;
  edit_active_button->setChecked(edit_recording->isActive());
  edit_starttime_edit->setTime(edit_recording->startTime());
  edit_description_edit->setText(edit_recording->description());
  edit_url_edit->setText(edit_recording->url());
  edit_username_edit->setText(edit_recording->urlUsername());
  edit_password_edit->setText(edit_recording->urlPassword());
  if(!edit_recording->cutName().isEmpty()) {
    edit_destination_edit->setText("Cut "+edit_recording->cutName());
  }
  edit_metadata_box->setChecked(edit_recording->enableMetadata());
  edit_mon_button->setChecked(edit_recording->mon());
  edit_tue_button->setChecked(edit_recording->tue());
  edit_wed_button->setChecked(edit_recording->wed());
  edit_thu_button->setChecked(edit_recording->thu());
  edit_fri_button->setChecked(edit_recording->fri());
  edit_sat_button->setChecked(edit_recording->sat());
  edit_sun_button->setChecked(edit_recording->sun());
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
  int feed_id=edit_recording->feedId();
  sql="select ID,KEY_NAME from FEEDS order by KEY_NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_feed_box->insertItem(q->value(1).toString());
    if(q->value(0).toInt()==feed_id) {
      edit_feed_box->setCurrentItem(edit_feed_box->count()-1);
    }
  }
  delete q;
}


EditUpload::~EditUpload()
{
  delete edit_station_box;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditUpload::sizeHint() const
{
  return QSize(520,441);
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


void EditUpload::urlChangedData(const QString &str)
{
  QUrl url(str);
  QString protocol=url.protocol().lower();
  if((protocol=="ftp")||(protocol=="file")) {
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


void EditUpload::selectCartData()
{
  QString str;

  RDCutDialog *cut=new RDCutDialog(&edit_cutname,rdstation_conf,catch_system,
				   edit_filter);
  switch(cut->exec()) {
      case 0:
	edit_description_edit->setText(RDCutPath(edit_cutname));
	str=QString(tr("Cut"));
	edit_destination_edit->
	  setText(QString().sprintf("%s %s",(const char *)str,
				    (const char *)edit_cutname));
	break;
  }
  delete cut;
}


void EditUpload::setFormatData()
{
  RDStation *station=new RDStation(edit_station_box->currentText());
  RDExportSettingsDialog *dialog=
    new RDExportSettingsDialog(&edit_settings,station,this,"dialog");
  dialog->exec();
  delete dialog;
  delete station;
  edit_format_edit->setText(edit_settings.description());
}


void EditUpload::normalizeCheckData(bool state)
{
  edit_normalize_label->setEnabled(state);
  edit_normalize_spin->setEnabled(state);
  edit_normalize_unit->setEnabled(state);
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
  if(!CheckFormat()) {
    QMessageBox::warning(this,tr("Unsupported Format"),
			 tr("The currently selected export format is unsupported on host ")+edit_station_box->currentText()+"!");
    return;
  }
  if(QUrl::isRelativeUrl(edit_url_edit->text())||
     (edit_url_edit->text().right(1)=="/")) {
    QMessageBox::warning(this,tr("Invalid URL"),tr("The URL is invalid!"));
    return;
  }
  RDUrl url(edit_url_edit->text());
  QString protocol=url.protocol();
  if((protocol!="ftp")&&(protocol!="file")) {
    QMessageBox::warning(this,
			 tr("Invalid URL"),tr("Unsupported URL protocol!"));
    return;
  }
  if((protocol=="file")&&(edit_username_edit->text().isEmpty())) {
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
  done(0);
}


void EditUpload::cancelData()
{
  done(-1);
}


void EditUpload::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->drawRect(10,271,sizeHint().width()-20,62);
  p->end();
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

  RDStation *station=new RDStation(edit_station_box->currentText());
  switch(edit_settings.format()) {
      case RDSettings::Pcm16:
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

  QString sql;
  RDSqlQuery *q;
  sql=QString().sprintf("select STATION_NAME from ENCODERS \
                         where (NAME=\"%s\")&&(STATION_NAME=\"%s\")",
			(const char *)RDEscapeString(edit_settings.
						     formatName()),
			(const char *)RDEscapeString(edit_station_box->
						     currentText()));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    res=true;
  }
  delete q;

  return res;
}


void EditUpload::Save()
{
  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(edit_station_box->currentText());
  edit_recording->setType(RDRecording::Upload);
  edit_recording->setStartTime(edit_starttime_edit->time());
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_destination_edit->text().right(10));
  edit_recording->setUrl(edit_url_edit->text());
  edit_recording->setUrlUsername(edit_username_edit->text());
  edit_recording->setUrlPassword(edit_password_edit->text());
  edit_recording->setEnableMetadata(edit_metadata_box->isChecked());
  edit_recording->setMon(edit_mon_button->isChecked());
  edit_recording->setTue(edit_tue_button->isChecked());
  edit_recording->setWed(edit_wed_button->isChecked());
  edit_recording->setThu(edit_thu_button->isChecked());
  edit_recording->setFri(edit_fri_button->isChecked());
  edit_recording->setSat(edit_sat_button->isChecked());
  edit_recording->setSun(edit_sun_button->isChecked());
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
  edit_recording->setFeedId(edit_feed_box->currentText());
}


bool EditUpload::CheckEvent(bool include_myself)
{
  QString sql=
    QString().sprintf("select ID from RECORDINGS \
                       where (STATION_NAME=\"%s\")&&\
                       (TYPE=%d)&&(START_TIME=\"%s\")&&\
                       (URL=\"%s\")&&(CUT_NAME=\"%s\")",
		      (const char *)edit_station_box->currentText(),
		      RDRecording::Upload,
		      (const char *)edit_starttime_edit->time().
		      toString("hh:mm:ss"),
		      (const char *)edit_url_edit->text(),
		      (const char *)edit_destination_edit->text().right(10));
  if(edit_sun_button->isChecked()) {
    sql+="&&(SUN=\"Y\")";
  }
  if(edit_mon_button->isChecked()) {
    sql+="&&(MON=\"Y\")";
  }
  if(edit_tue_button->isChecked()) {
    sql+="&&(TUE=\"Y\")";
  }
  if(edit_wed_button->isChecked()) {
    sql+="&&(WED=\"Y\")";
  }
  if(edit_thu_button->isChecked()) {
    sql+="&&(THU=\"Y\")";
  }
  if(edit_fri_button->isChecked()) {
    sql+="&&(FRI=\"Y\")";
  }
  if(edit_sat_button->isChecked()) {
    sql+="&&(SAT=\"Y\")";
  }
  if(!include_myself) {
    sql+=QString().sprintf("&&(ID!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  bool res=!q->first();
  delete q;

  return res;
}

