// edit_download.cpp
//
// Edit a Rivendell Download Event
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_download.cpp,v 1.20 2011/08/30 23:35:44 cvs Exp $
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
#include <qurl.h>

#include <rdurl.h>
#include <rddb.h>
#include <rd.h>
#include <rdcut_dialog.h>
#include <rdcut_path.h>
#include <rdtextvalidator.h>
#include <rddeck.h>

#include <edit_download.h>
#include <globals.h>


EditDownload::EditDownload(int id,std::vector<int> *adds,QString *filter,
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

  setCaption(tr("Edit Download"));

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
  // Description
  //
  edit_description_edit=new QLineEdit(this,"edit_description_edit");
  edit_description_edit->setGeometry(115,43,sizeHint().width()-125,20);
  edit_description_edit->setValidator(validator);
  label=new QLabel(edit_description_edit,
		   tr("Description:"),this,"edit_description_label");
  label->setGeometry(10,43,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Url
  //
  edit_url_edit=new QLineEdit(this,"edit_url_edit");
  edit_url_edit->setGeometry(115,70,sizeHint().width()-125,20);
  edit_url_edit->setMaxLength(255);
  edit_url_edit->setValidator(validator);
  connect(edit_url_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(urlChangedData(const QString &)));
  label=new QLabel(edit_url_edit,tr("Url:"),this,"edit_url_label");
  label->setGeometry(10,70,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Username
  //
  edit_username_edit=new QLineEdit(this,"edit_username_edit");
  edit_username_edit->setGeometry(115,97,150,20);
  edit_username_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_username_label=new QLabel(edit_username_edit,
		   tr("Username:"),this,"edit_username_label");
  edit_username_label->setGeometry(10,97,100,20);
  edit_username_label->setFont(label_font);
  edit_username_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Password
  //
  edit_password_edit=new QLineEdit(this,"edit_password_edit");
  edit_password_edit->setGeometry(360,97,sizeHint().width()-370,20);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_edit->setMaxLength(64);
  edit_username_edit->setValidator(validator);
  edit_password_label=new QLabel(edit_password_edit,
		   tr("Password:"),this,"edit_password_label");
  edit_password_label->setGeometry(275,97,80,20);
  edit_password_label->setFont(label_font);
  edit_password_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this,"edit_destination_edit");
  edit_destination_edit->setGeometry(115,124,sizeHint().width()-195,20);
  edit_destination_edit->setReadOnly(true);
  label=new QLabel(edit_destination_edit,
		   tr("Destination:"),this,"edit_destination_label");
  label->setGeometry(10,127,100,19);
  label->setFont(label_font);
  label->setAlignment(AlignRight|ShowPrefix);
  QPushButton *button=new QPushButton(this,"destination_button");
  button->setGeometry(sizeHint().width()-70,122,60,24);
  button->setFont(day_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCartData()));

  //
  // Channels
  //
  edit_channels_box=new QComboBox(this,"edit_channels_box");
  edit_channels_box->setGeometry(190,149,40,20);
  edit_channels_box->insertItem("1");
  edit_channels_box->insertItem("2");
  label=
    new QLabel(edit_channels_box,tr("Channels:"),this,"edit_autotrim_label");
  label->setGeometry(120,149,70,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignLeft);

  //
  // Autotrim Controls
  //
  edit_autotrim_box=new QCheckBox(this,"edit_autotrim_box");
  edit_autotrim_box->setGeometry(120,175,15,15);
  connect(edit_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));
  label=new QLabel(edit_autotrim_box,tr("Autotrim"),this);
  label->setGeometry(140,173,80,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignLeft);
  edit_autotrim_spin=new QSpinBox(this,"edit_autotrim_spin");
  edit_autotrim_spin->setGeometry(265,173,40,20);
  edit_autotrim_spin->setRange(-99,-1);
  edit_autotrim_label=
    new QLabel(edit_autotrim_spin,tr("Level:"),this,"edit_autotrim_label");
  edit_autotrim_label->setGeometry(220,173,40,20);
  edit_autotrim_label->setFont(label_font);
  edit_autotrim_label->setAlignment(AlignVCenter|AlignRight);
  edit_autotrim_unit=
    new QLabel(edit_autotrim_spin,tr("dBFS"),this,"edit_autotrim_unit");
  edit_autotrim_unit->setGeometry(310,173,40,20);
  edit_autotrim_unit->setFont(label_font);
  edit_autotrim_unit->setAlignment(AlignVCenter|AlignLeft);

  //
  // Normalize Controls
  //
  edit_normalize_box=new QCheckBox(this,"edit_normalize_box");
  edit_normalize_box->setGeometry(120,199,15,15);
  connect(edit_normalize_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizeToggledData(bool)));
  label=new QLabel(edit_normalize_box,tr("Normalize"),this);
  label->setGeometry(140,197,80,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignLeft);
  edit_normalize_spin=new QSpinBox(this,"edit_normalize_spin");
  edit_normalize_spin->setGeometry(265,197,40,20);
  edit_normalize_spin->setRange(-99,-1);
  edit_normalize_label=
    new QLabel(edit_normalize_spin,tr("Level:"),this,"edit_normalize_label");
  edit_normalize_label->setGeometry(220,197,40,20);
  edit_normalize_label->setFont(label_font);
  edit_normalize_label->setAlignment(AlignVCenter|AlignRight);
  edit_normalize_unit=
    new QLabel(edit_normalize_spin,tr("dBFS"),this,"edit_normalize_unit");
  edit_normalize_unit->setGeometry(310,197,40,20);
  edit_normalize_unit->setFont(label_font);
  edit_normalize_unit->setAlignment(AlignVCenter|AlignLeft);

  //
  // Export Metadata Box
  //
  edit_metadata_box=new QCheckBox(this,"edit_metadata_box");
  edit_metadata_box->setGeometry(120,222,15,15);
  label=new QLabel(edit_metadata_box,tr("Update Library Metadata"),
		   this,"metadata_check_label");
  label->setGeometry(140,222,160,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Button Label
  //
  label=new QLabel(tr("Active Days"),this,"active_days_label");
  label->setGeometry(47,254,90,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  //
  // Monday Button
  //
  edit_mon_button=new QCheckBox(this,"edit_mon_button");
  edit_mon_button->setGeometry(20,273,20,20);
  label=new QLabel(edit_mon_button,
		   tr("Monday"),this,"edit_mon_label");
  label->setGeometry(40,273,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Tuesday Button
  //
  edit_tue_button=new QCheckBox(this,"edit_tue_button");
  edit_tue_button->setGeometry(115,273,20,20);
  label=new QLabel(edit_tue_button,
		   tr("Tuesday"),this,"edit_tue_label");
  label->setGeometry(135,273,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Wednesday Button
  //
  edit_wed_button=new QCheckBox(this,"edit_wed_button");
  edit_wed_button->setGeometry(215,273,20,20);
  label=new QLabel(edit_wed_button,
		   tr("Wednesday"),this,"edit_wed_label");
  label->setGeometry(235,273,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Thursday Button
  //
  edit_thu_button=new QCheckBox(this,"edit_thu_button");
  edit_thu_button->setGeometry(335,273,20,20);
  label=new QLabel(edit_thu_button,
		   tr("Thursday"),this,"edit_thu_label");
  label->setGeometry(355,273,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Friday Button
  //
  edit_fri_button=new QCheckBox(this,"edit_fri_button");
  edit_fri_button->setGeometry(440,273,20,20);
  label=new QLabel(edit_fri_button,
		   tr("Friday"),this,"edit_fri_label");
  label->setGeometry(460,273,40,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Saturday Button
  //
  edit_sat_button=new QCheckBox(this,"edit_sat_button");
  edit_sat_button->setGeometry(130,298,20,20);
  label=new QLabel(edit_sat_button,
		   tr("Saturday"),this,"edit_sat_label");
  label->setGeometry(150,298,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Sunday Button
  //
  edit_sun_button=new QCheckBox(this,"edit_sun_button");
  edit_sun_button->setGeometry(300,298,20,20);
  label=new QLabel(edit_sun_button,
		   tr("Sunday"),this,"edit_sun_label");
  label->setGeometry(320,298,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this,"edit_oneshot_box");
  edit_oneshot_box->setGeometry(20,335,15,15);
  label=new QLabel(edit_oneshot_box,
		   tr("Make OneShot"),this,"edit_oneshot_label");
  label->setGeometry(40,333,115,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Event Offset
  //
  edit_eventoffset_spin=new QSpinBox(this,"edit_eventoffset_spin");
  edit_eventoffset_spin->setGeometry(245,333,45,20);
  edit_eventoffset_spin->setRange(-30,30);
  label=new QLabel(edit_eventoffset_spin,tr("Event Offset:"),
		   this,"edit_eventoffset_label");
  label->setGeometry(140,333,100,20);
  label->setFont(label_font);
  label->setAlignment(AlignVCenter|AlignRight);
  label=new QLabel(edit_eventoffset_spin,tr("days"),
		   this,"edit_eventoffset_unit");
  label->setGeometry(295,333,40,20);
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
  edit_destination_edit->setText("Cut "+edit_recording->cutName());
  edit_channels_box->setCurrentItem(edit_recording->channels()-1);
  if(edit_recording->trimThreshold()>0) {
    edit_autotrim_box->setChecked(true);
    edit_autotrim_spin->setValue(-(edit_recording->trimThreshold()/100));
  }
  else {
    edit_autotrim_box->setChecked(false);
    edit_autotrim_spin->setValue(rdlibrary_conf->trimThreshold()/100);
  }
  autotrimToggledData(edit_autotrim_box->isChecked());
  if(edit_recording->normalizationLevel()<0) {
    edit_normalize_box->setChecked(true);
    edit_normalize_spin->setValue(edit_recording->normalizationLevel()/100);
  }
  else {
    edit_normalize_box->setChecked(false);
    edit_normalize_spin->setValue(rdlibrary_conf->ripperLevel()/100);
  }
  normalizeToggledData(edit_normalize_box->isChecked());
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
}


EditDownload::~EditDownload()
{
  delete edit_station_box;
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
  QString protocol=url.protocol();
  if((protocol=="ftp")||(protocol=="http")||(protocol=="file")) {
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
  QString str;

  RDCutDialog *cut=new RDCutDialog(&edit_cutname,rdstation_conf,catch_system,
				   edit_filter,NULL,NULL,catch_user->name(),
				   false,true);
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
  if(QUrl::isRelativeUrl(edit_url_edit->text())||
     (edit_url_edit->text().right(1)=="/")) {
    QMessageBox::warning(this,tr("Invalid URL"),tr("The URL is invalid!"));
    return;
  }
  RDUrl url(edit_url_edit->text());
  QString protocol=url.protocol();
  if((protocol!="ftp")&&(protocol!="http")&&(protocol!="https")&&
     (protocol!="file")) {
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


void EditDownload::cancelData()
{
  done(-1);
}


void EditDownload::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->drawRect(10,262,sizeHint().width()-20,62);
  p->end();
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
  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(edit_station_box->currentText());
  edit_recording->setType(RDRecording::Download);
  edit_recording->setStartTime(edit_starttime_edit->time());
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setChannels(edit_channels_box->currentItem()+1);
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
}


bool EditDownload::CheckEvent(bool include_myself)
{
  QString sql=
    QString().sprintf("select ID from RECORDINGS \
                       where (STATION_NAME=\"%s\")&&\
                       (TYPE=%d)&&(START_TIME=\"%s\")&&\
                       (URL=\"%s\")&&(CUT_NAME=\"%s\")",
		      (const char *)edit_station_box->currentText(),
		      RDRecording::Download,
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

