// edit_playout.cpp
//
// Edit a Rivendell RDCatch Playout
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_playout.cpp,v 1.23.4.1 2014/01/07 23:40:36 cvs Exp $
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
#include <rd.h>
#include <rdcut.h>
#include <rdcut_dialog.h>
#include <rdcut_path.h>
#include <rdtextvalidator.h>

#include <edit_playout.h>
#include <globals.h>


EditPlayout::EditPlayout(int id,std::vector<int> *adds,QString *filter,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
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

  setCaption(tr("Edit Playout"));

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
  connect(edit_station_box,SIGNAL(activated(int)),
	  this,SLOT(activateStationData(int)));

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
  edit_description_edit->setGeometry(105,43,sizeHint().width()-115,20);
  edit_description_edit->setValidator(validator);
  label=new QLabel(edit_description_edit,
		   tr("Description:"),this,"edit_description_label");
  label->setGeometry(10,43,90,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Destination
  //
  edit_destination_edit=new QLineEdit(this,"edit_destination_edit");
  edit_destination_edit->setGeometry(105,70,sizeHint().width()-185,20);
  edit_destination_edit->setReadOnly(true);
  label=new QLabel(edit_destination_edit,
		   tr("Destination:"),this,"edit_destination_label");
  label->setGeometry(10,70,90,20);
  label->setFont(label_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"destination_button");
  button->setGeometry(sizeHint().width()-70,65,60,30);
  button->setFont(day_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCutData()));

  //
  // Button Label
  //
  label=new QLabel(tr("Active Days"),this,"active_days_label");
  label->setGeometry(47,101,90,19);
  label->setFont(label_font);
  label->setAlignment(AlignHCenter|ShowPrefix);

  //
  // Monday Button
  //
  edit_mon_button=new QCheckBox(this,"edit_mon_button");
  edit_mon_button->setGeometry(20,120,20,20);
  label=new QLabel(edit_mon_button,
		   tr("Monday"),this,"edit_mon_label");
  label->setGeometry(40,120,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Tuesday Button
  //
  edit_tue_button=new QCheckBox(this,"edit_tue_button");
  edit_tue_button->setGeometry(115,120,20,20);
  label=new QLabel(edit_tue_button,
		   tr("Tuesday"),this,"edit_tue_label");
  label->setGeometry(135,120,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Wednesday Button
  //
  edit_wed_button=new QCheckBox(this,"edit_wed_button");
  edit_wed_button->setGeometry(215,120,20,20);
  label=new QLabel(edit_wed_button,
		   tr("Wednesday"),this,"edit_wed_label");
  label->setGeometry(235,120,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Thursday Button
  //
  edit_thu_button=new QCheckBox(this,"edit_thu_button");
  edit_thu_button->setGeometry(335,120,20,20);
  label=new QLabel(edit_thu_button,
		   tr("Thursday"),this,"edit_thu_label");
  label->setGeometry(355,120,115,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Friday Button
  //
  edit_fri_button=new QCheckBox(this,"edit_fri_button");
  edit_fri_button->setGeometry(440,120,20,20);
  label=new QLabel(edit_fri_button,
		   tr("Friday"),this,"edit_fri_label");
  label->setGeometry(460,120,40,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Saturday Button
  //
  edit_sat_button=new QCheckBox(this,"edit_sat_button");
  edit_sat_button->setGeometry(130,145,20,20);
  label=new QLabel(edit_sat_button,
		   tr("Saturday"),this,"edit_sat_label");
  label->setGeometry(150,145,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Sunday Button
  //
  edit_sun_button=new QCheckBox(this,"edit_sun_button");
  edit_sun_button->setGeometry(300,145,20,20);
  label=new QLabel(edit_sun_button,
		   tr("Sunday"),this,"edit_sun_label");
  label->setGeometry(320,145,60,20);
  label->setFont(day_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // OneShot Button
  //
  edit_oneshot_box=new QCheckBox(this,"edit_oneshot_box");
  edit_oneshot_box->setGeometry(20,180,15,15);
  label=new QLabel(edit_oneshot_box,
		   tr("Make OneShot"),this,"edit_oneshot_label");
  label->setGeometry(40,178,115,20);
  label->setFont(label_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

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
  PopulateDecks(edit_station_box);
  edit_active_button->setChecked(edit_recording->isActive());
  edit_starttime_edit->setTime(edit_recording->startTime());
  edit_description_edit->setText(edit_recording->description());
  edit_cutname=edit_recording->cutName();
  edit_destination_edit->setText(RDCutPath(edit_cutname));
  edit_mon_button->setChecked(edit_recording->mon());
  edit_tue_button->setChecked(edit_recording->tue());
  edit_wed_button->setChecked(edit_recording->wed());
  edit_thu_button->setChecked(edit_recording->thu());
  edit_fri_button->setChecked(edit_recording->fri());
  edit_sat_button->setChecked(edit_recording->sat());
  edit_sun_button->setChecked(edit_recording->sun());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
  activateStationData(edit_station_box->currentItem(),false);
}


EditPlayout::~EditPlayout()
{
  delete edit_station_box;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditPlayout::sizeHint() const
{
  return QSize(540,245);
} 


QSizePolicy EditPlayout::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditPlayout::activateStationData(int id,bool use_temp)
{
  char station[65];
  char gunk[3];
  int chan;
  
  if(edit_station_box->currentText().isEmpty()) {
    return;
  }
  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
  edit_deck=new RDDeck(station,chan);
}


void EditPlayout::selectCutData()
{
  RDCutDialog *cut=new RDCutDialog(&edit_cutname,rdstation_conf,catch_system,
				   edit_filter,NULL,NULL,"",false,false,false,
				   this);
  switch(cut->exec()) {
      case 0:
	edit_destination_edit->setText(edit_cutname);
	edit_description_edit->setText(RDCutPath(edit_cutname));
	break;
  }
  delete cut;
}


void EditPlayout::saveasData()
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


void EditPlayout::okData()
{
  if(!CheckEvent(false)) {
    QMessageBox::warning(this,tr("Duplicate Event"),
		     tr("An event with these parameters already exists!"));
    return;
  }
  Save();
  done(0);
}


void EditPlayout::cancelData()
{
  done(-1);
}


void EditPlayout::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->drawRect(10,109,sizeHint().width()-20,62);
  p->end();
}


void EditPlayout::keyPressEvent(QKeyEvent *e)
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


void EditPlayout::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditPlayout::PopulateDecks(QComboBox *box)
{
  int count=0;

  box->clear();
  QString sql=QString("select STATION_NAME,CHANNEL from DECKS \
where (CARD_NUMBER!=-1)&&(PORT_NUMBER!=-1)&&\
(CHANNEL>128) order by STATION_NAME,CHANNEL");
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    box->insertItem(QString().sprintf("%s : %dP",
	      (const char *)q->value(0).toString(),q->value(1).toInt()-128));
    if((q->value(0).toString()==edit_recording->station())&&
       (q->value(1).toUInt()==edit_recording->channel())) {
      box->setCurrentItem(count);
    }
    count++;
  }
  if(q->size()>0) {
    if(edit_deck!=NULL) {
      delete edit_deck;
    }
    q->first();
    edit_deck=new RDDeck(q->value(0).toString(),q->value(1).toUInt()); 
 }
  delete q;
}


void EditPlayout::Save()
{
  char station[65];
  char gunk[3];
  int chan;

  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(station);
  edit_recording->setType(RDRecording::Playout);
  edit_recording->setChannel(chan+128);
  if(edit_starttime_edit->time().isNull()) {
    edit_recording->setStartTime(edit_starttime_edit->time().addMSecs(1));
  }
  else {
    edit_recording->setStartTime(edit_starttime_edit->time());
  }
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setCutName(edit_cutname);
  edit_recording->setMon(edit_mon_button->isChecked());
  edit_recording->setTue(edit_tue_button->isChecked());
  edit_recording->setWed(edit_wed_button->isChecked());
  edit_recording->setThu(edit_thu_button->isChecked());
  edit_recording->setFri(edit_fri_button->isChecked());
  edit_recording->setSat(edit_sat_button->isChecked());
  edit_recording->setSun(edit_sun_button->isChecked());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


bool EditPlayout::CheckEvent(bool include_myself)
{
  char station[65];
  char gunk[3];
  int chan;
  QTime test_start_time;
  QTime test_end_time;

  RDCut *cut=new RDCut(edit_cutname);
  QTime start_time=edit_starttime_edit->time();
  QTime end_time=edit_starttime_edit->time().addMSecs(cut->length());
  delete cut;

  sscanf((const char *)edit_station_box->currentText(),"%s%s%d",
	 station,gunk,&chan);
  QString sql=
    QString().sprintf("select RECORDINGS.START_TIME,CUTS.LENGTH\
                       from RECORDINGS left join CUTS\
                       on(RECORDINGS.CUT_NAME=CUTS.CUT_NAME)\
                       where (RECORDINGS.STATION_NAME=\"%s\")&&\
                       (RECORDINGS.TYPE=%d)&&(RECORDINGS.CHANNEL=%d)",
		      station,RDRecording::Playout,chan+128);
  if(edit_sun_button->isChecked()) {
    sql+="&&(RECORDINGS.SUN=\"Y\")";
  }
  if(edit_mon_button->isChecked()) {
    sql+="&&(RECORDINGS.MON=\"Y\")";
  }
  if(edit_tue_button->isChecked()) {
    sql+="&&(RECORDINGS.TUE=\"Y\")";
  }
  if(edit_wed_button->isChecked()) {
    sql+="&&(RECORDINGS.WED=\"Y\")";
  }
  if(edit_thu_button->isChecked()) {
    sql+="&&(RECORDINGS.THU=\"Y\")";
  }
  if(edit_fri_button->isChecked()) {
    sql+="&&(RECORDINGS.FRI=\"Y\")";
  }
  if(edit_sat_button->isChecked()) {
    sql+="&&(RECORDINGS.SAT=\"Y\")";
  }
  if(!include_myself) {
    sql+=QString().sprintf("&&(RECORDINGS.ID!=%d)",edit_recording->id());
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    test_start_time=q->value(0).toTime();
    test_end_time=q->value(0).toTime().addMSecs(q->value(1).toUInt());
    if(test_end_time<test_start_time) {
      test_end_time=QTime(23,59,59);
    }
    if(((start_time<test_start_time)&&(start_time<test_end_time)&&
	(end_time<test_start_time)&&(end_time<test_end_time))||
       ((start_time>test_start_time)&&(start_time>test_end_time)&&
	(end_time>test_start_time)&&(end_time>test_end_time))) {
    }
    else {
      delete q;
      return false;
    }
  }
  delete q;

  return true;
}
