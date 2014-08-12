// edit_cartevent.cpp
//
// Edit a Rivendell Netcatch Cart Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cartevent.cpp,v 1.27.8.1 2012/11/26 20:19:41 cvs Exp $
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
#include <rdcart_dialog.h>
#include <rdcut_path.h>
#include <rdtextvalidator.h>

#include <globals.h>
#include <edit_cartevent.h>


EditCartEvent::EditCartEvent(int id,std::vector<int> *adds,
			     QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QString temp;
  int cartnum;

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

  setCaption(tr("Edit Cart Event"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");

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
  // Cart Number
  //
  edit_destination_edit=new QLineEdit(this,"edit_destination_edit");
  edit_destination_edit->setGeometry(115,70,60,20);
  edit_destination_edit->setReadOnly(false);
  label=new QLabel(edit_destination_edit,
		   tr("Cart Number:"),this,"edit_destination_label");
  label->setGeometry(10,73,100,19);
  label->setFont(label_font);
  label->setAlignment(AlignRight|ShowPrefix);
  QPushButton *button=new QPushButton(this,"destination_button");
  button->setGeometry(185,68,60,24);
  button->setFont(day_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectCartData()));

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
  if(edit_cart!=NULL) {
    edit_destination_edit->
      setText(QString().sprintf("%06d",edit_cart->number()));
  }
  edit_mon_button->setChecked(edit_recording->mon());
  edit_tue_button->setChecked(edit_recording->tue());
  edit_wed_button->setChecked(edit_recording->wed());
  edit_thu_button->setChecked(edit_recording->thu());
  edit_fri_button->setChecked(edit_recording->fri());
  edit_sat_button->setChecked(edit_recording->sat());
  edit_sun_button->setChecked(edit_recording->sun());
  edit_oneshot_box->setChecked(edit_recording->oneShot());
}


EditCartEvent::~EditCartEvent()
{
  delete edit_station_box;
  if(edit_deck!=NULL) {
    delete edit_deck;
  }
}


QSize EditCartEvent::sizeHint() const
{
  return QSize(520,255);
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
  switch(catch_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
				 catch_user->name(),catch_user->password())) {
      case 0:
	if(edit_cart!=NULL) {
	  delete edit_cart;
	}
	edit_cart=new RDCart(cartnum);
	edit_destination_edit->setText(QString().sprintf("%d",cartnum));
	edit_description_edit->setText(edit_cart->title());
	break;
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
  done(0);
}


void EditCartEvent::cancelData()
{
  done(-1);
}


void EditCartEvent::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->drawRect(10,109,sizeHint().width()-20,62);
  p->end();
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
  edit_recording->setIsActive(edit_active_button->isChecked());
  edit_recording->setStation(edit_station_box->currentText());
  edit_recording->setType(RDRecording::MacroEvent);
  edit_recording->setStartTime(edit_starttime_edit->time());
  edit_recording->setDescription(edit_description_edit->text());
  edit_recording->setMacroCart(edit_cart->number());
  edit_recording->setMon(edit_mon_button->isChecked());
  edit_recording->setTue(edit_tue_button->isChecked());
  edit_recording->setWed(edit_wed_button->isChecked());
  edit_recording->setThu(edit_thu_button->isChecked());
  edit_recording->setFri(edit_fri_button->isChecked());
  edit_recording->setSat(edit_sat_button->isChecked());
  edit_recording->setSun(edit_sun_button->isChecked());
  edit_recording->setOneShot(edit_oneshot_box->isChecked());
}


bool EditCartEvent::CheckEvent(bool include_myself)
{
  QString sql=
    QString().sprintf("select ID from RECORDINGS \
                       where (STATION_NAME=\"%s\")&&\
                       (TYPE=%d)&&(START_TIME=\"%s\")&&(MACRO_CART=%u)",
		      (const char *)edit_station_box->currentText(),
		      RDRecording::MacroEvent,
		      (const char *)edit_starttime_edit->time().
		      toString("hh:mm:ss"),edit_destination_edit->text().
		      toUInt());
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
