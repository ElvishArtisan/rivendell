// edit_rdlogedit.cpp
//
// Edit an RDLogedit Configuration
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_rdlogedit.cpp,v 1.18.6.2 2014/01/08 18:14:35 cvs Exp $
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
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <rd.h>

#include <globals.h>
#include <edit_rdlogedit.h>
#include <rdtextvalidator.h>
#include <rdcart_dialog.h>


EditRDLogedit::EditRDLogedit(RDStation *station,RDStation *cae_station,
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

  lib_lib=new RDLogeditConf(station->name());

  //
  // Create Fonts
  //
  QFont small_font=QFont("Helvetica",12,QFont::Bold);
  small_font.setPixelSize(12);
  QFont big_font=QFont("Helvetica",14,QFont::Bold);
  big_font.setPixelSize(14);

  //
  // Dialog Name
  //
  setCaption(tr("RDLogedit config for ")+station->name());

  //
  // Input Configuration
  //
  lib_input_card=new RDCardSelector(this,"lib_input_card");
  lib_input_card->setGeometry(10,29,120,117);
  QLabel *label=new QLabel(lib_input_card,tr("INPUT"),this,"lib_input_label");
  label->setGeometry(10,10,110,19);
  label->setFont(big_font);
  label->setAlignment(AlignCenter);

  //
  // Output Configuration
  //
  lib_output_card=new RDCardSelector(this,"lib_output_card");
  lib_output_card->setGeometry(170,29,120,87);
  label=new QLabel(lib_output_card,tr("OUTPUT"),this,"lib_output_label");
  label->setGeometry(170,10,110,19);
  label->setFont(big_font);
  label->setAlignment(AlignCenter);

  //
  // Settings
  //
  QLabel *setting_label=new QLabel(tr("Voice Tracker Settings"),this,"setting_label");
  setting_label->setGeometry(70,79,sizeHint().width()-80,19);
  setting_label->setFont(big_font);
  setting_label->setAlignment(AlignLeft|ShowPrefix);

  //
  // Maximum Record Length
  //
  lib_maxlength_time=new QTimeEdit(this,"lib_maxlength_time");
  lib_maxlength_time->setGeometry(160,100,85,19);
  QLabel *lib_maxlength_label=new QLabel(lib_maxlength_time,
					 tr("&Max Record Time:"),this,
					 "lib_maxlength_label");
  lib_maxlength_label->setGeometry(25,101,130,19);
  lib_maxlength_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Autotrim Level
  //
  lib_threshold_spin=new QSpinBox(this,"lib_threshold_spin");
  lib_threshold_spin->setGeometry(160,124,50,19);
  lib_threshold_spin->setMinValue(-99);
  lib_threshold_spin->setMaxValue(0);
  label=new QLabel(lib_threshold_spin,tr("&AutoTrim Threshold:"),this,
		   "lib_threshold_spin_label");
  label->setGeometry(25,124,130,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  label=new QLabel(tr("dbFS"),this,"lib_threshold_spin_unit");
  label->setGeometry(215,124,120,19);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Normalization Level
  //
  lib_normalization_spin=new QSpinBox(this,"lib_normalization_spin");
  lib_normalization_spin->setGeometry(160,148,50,19);
  lib_normalization_spin->setMinValue(-99);
  lib_normalization_spin->setMaxValue(0);
  label=new QLabel(lib_normalization_spin,tr("&Normalization Level:"),
		   this,"lib_normalization_spin_label");
  label->setGeometry(25,148,130,19);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  label=new QLabel(tr("dbFS"),this,"lib_normalization_spin_unit");
  label->setGeometry(215,148,120,19);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Audio Margin
  //
  lib_preroll_spin=new QSpinBox(this,"lib_preroll_spin");
  lib_preroll_spin->setGeometry(160,172,60,19);
  lib_preroll_spin->setMinValue(0);
  lib_preroll_spin->setMaxValue(10000);
  lib_preroll_spin->setLineStep(100);
  QLabel *lib_preroll_spin_label=new QLabel(lib_preroll_spin,
					    tr("&Audio Margin:"),this,
					    "lib_preroll_spin_label");
  lib_preroll_spin_label->setGeometry(25,172,130,19);
  lib_preroll_spin_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QLabel *lib_preroll_spin_unit=new QLabel(tr("milliseconds"),this,
					   "lib_preroll_spin_unit");
  lib_preroll_spin_unit->setGeometry(225,172,120,19);
  lib_preroll_spin_unit->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Format
  //
  lib_format_box=new QComboBox(this,"lib_name_edit");
  lib_format_box->setGeometry(160,196,150,19);
  connect(lib_format_box,SIGNAL(activated(int)),this,SLOT(formatData(int)));
  QLabel *lib_format_label=new QLabel(lib_format_box,tr("&Format:"),this,
				      "lib_format_label");
  lib_format_label->setGeometry(25,196,130,19);
  lib_format_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Bitrate
  //
  lib_bitrate_box=new QComboBox(this,"lib_name_edit");
  lib_bitrate_box->setGeometry(160,220,130,19);
  QLabel *lib_bitrate_label=new QLabel(lib_bitrate_box,tr("&Bitrate:"),this,
				       "lib_bitrate_label");
  lib_bitrate_label->setGeometry(25,220,130,19);
  lib_bitrate_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Enable Second Start Button
  //
  lib_enable_second_start_box=new QComboBox(this);
  lib_enable_second_start_box->setGeometry(160,244,60,19);
  lib_enable_second_start_box->insertItem(tr("No"));
  lib_enable_second_start_box->insertItem(tr("Yes"));
  QLabel *lib_enable_second_start_label=
   new QLabel(lib_enable_second_start_box,tr("Enable &2nd Start Button:"),this);
  lib_enable_second_start_label->setGeometry(10,244,145,19);
  lib_enable_second_start_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Validator
  //
  QIntValidator *validator=new QIntValidator(this,"validator");
  validator->setRange(1,999999);

  //
  // Play Start Cart
  //
  lib_startcart_edit=new QLineEdit(this,"lib_name_edit");
  lib_startcart_edit->setGeometry(160,268,70,19);
  lib_startcart_edit->setValidator(validator);
  QLabel *lib_startcart_label=
    new QLabel(lib_startcart_edit,tr("Play &Start Cart:"),this,
	       "lib_startcart_label");
  lib_startcart_label->setGeometry(25,268,130,19);
  lib_startcart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"start_select_button");
  button->setGeometry(240,266,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectStartData()));

  //
  // Play End Cart
  //
  lib_endcart_edit=new QLineEdit(this,"lib_name_edit");
  lib_endcart_edit->setGeometry(160,292,70,19);
  lib_endcart_edit->setValidator(validator);
  QLabel *lib_endcart_label=
    new QLabel(lib_endcart_edit,tr("Play &End Cart:"),this,
	       "lib_endcart_label");
  lib_endcart_label->setGeometry(25,292,130,19);
  lib_endcart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"end_select_button");
  button->setGeometry(240,290,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectEndData()));

  //
  // Record Start Cart
  //
  lib_recstartcart_edit=new QLineEdit(this,"lib_name_edit");
  lib_recstartcart_edit->setGeometry(160,316,70,19);
  lib_recstartcart_edit->setValidator(validator);
  QLabel *lib_recstartcart_label=
    new QLabel(lib_recstartcart_edit,tr("&Record Start Cart:"),this,
	       "lib_recstartcart_label");
  lib_recstartcart_label->setGeometry(25,316,130,19);
  lib_recstartcart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"start_select_button");
  button->setGeometry(240,314,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectRecordStartData()));

  //
  // Record End Cart
  //
  lib_recendcart_edit=new QLineEdit(this,"lib_name_edit");
  lib_recendcart_edit->setGeometry(160,340,70,19);
  lib_recendcart_edit->setValidator(validator);
  QLabel *lib_recendcart_label=
    new QLabel(lib_recendcart_edit,tr("Re&cord End Cart:"),this,
	       "lib_recendcart_label");
  lib_recendcart_label->setGeometry(25,340,130,19);
  lib_recendcart_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"end_select_button");
  button->setGeometry(240,338,55,23);
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectRecordEndData()));

  //
  // Default Channels
  //
  lib_channels_box=new QComboBox(this,"lib_name_edit");
  lib_channels_box->setGeometry(160,364,60,19);
  QLabel *lib_channels_label=new QLabel(lib_channels_box,tr("&Channels:"),this,
				       "lib_channels_label");
  lib_channels_label->setGeometry(25,364,130,19);
  lib_channels_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Default Transition Type
  //
  lib_default_transtype_box=new QComboBox(this,"lib_name_edit");
  lib_default_transtype_box->setGeometry(160,388,100,19);
  QLabel *lib_default_transtype_label=
    new QLabel(lib_default_transtype_box,tr("Default Transition:"),
	       this,"lib_default_transtype_label");
  lib_default_transtype_label->setGeometry(20,388,130,19);
  lib_default_transtype_label->
    setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  lib_default_transtype_box->insertItem(tr("Play"));
  lib_default_transtype_box->insertItem(tr("Segue"));
  lib_default_transtype_box->insertItem(tr("Stop"));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(small_font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(small_font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  if(cae_station->scanned()) {
    lib_input_card->setMaxCards(cae_station->cards());
    lib_output_card->setMaxCards(cae_station->cards());
    for(int i=0;i<lib_input_card->maxCards();i++) {
      lib_input_card->setMaxPorts(i,cae_station->cardInputs(i));
      lib_output_card->setMaxPorts(i,cae_station->cardOutputs(i));
    }
  }
  else {
    QMessageBox::information(this,tr("No Audio Configuration Data"),
			     tr("Channel assignments will not be available for this host as audio resource data\nhas not yet been generated.  Please start the Rivendell daemons on the host\nconfigured to run the CAE service in order to populate the audio resources database."));
    lib_input_card->setDisabled(true);
    lib_output_card->setDisabled(true);
  }
  lib_input_card->setCard(lib_lib->inputCard());
  lib_input_card->setPort(lib_lib->inputPort());
  lib_output_card->setCard(lib_lib->outputCard());
  lib_output_card->setPort(lib_lib->outputPort());
  lib_maxlength_time->setTime(QTime().addMSecs(lib_lib->maxLength()));
  lib_threshold_spin->setValue(lib_lib->trimThreshold()/100);
  lib_normalization_spin->setValue(lib_lib->ripperLevel()/100);
  unsigned cart=lib_lib->startCart();
  if(cart>0) {
    lib_startcart_edit->setText(QString().sprintf("%06u",cart));
  }
  if((cart=lib_lib->endCart())>0) {
    lib_endcart_edit->setText(QString().sprintf("%06u",cart));
  }  
  cart=lib_lib->recStartCart();
  if(cart>0) {
    lib_recstartcart_edit->setText(QString().sprintf("%06u",cart));
  }
  if((cart=lib_lib->recEndCart())>0) {
    lib_recendcart_edit->setText(QString().sprintf("%06u",cart));
  }  
  lib_preroll_spin->setValue(lib_lib->tailPreroll());
  lib_format_box->insertItem(tr("PCM16"));
  lib_format_box->insertItem(tr("MPEG Layer 2"));
  lib_format_box->setCurrentItem(lib_lib->format());
  lib_channels_box->insertItem("1");
  lib_channels_box->insertItem("2");
  lib_channels_box->setCurrentItem(lib_lib->defaultChannels()-1);
  ShowBitRates(lib_lib->format(),lib_lib->bitrate());
  lib_enable_second_start_box->setCurrentItem(lib_lib->enableSecondStart());
  lib_default_transtype_box->setCurrentItem(lib_lib->defaultTransType());
}


EditRDLogedit::~EditRDLogedit()
{
  delete lib_input_card;
  delete lib_output_card;
  delete lib_format_box;
  delete lib_channels_box;
  delete lib_bitrate_box;
  delete lib_maxlength_time;
}


QSize EditRDLogedit::sizeHint() const
{
  return QSize(375,478);
} 


QSizePolicy EditRDLogedit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditRDLogedit::formatData(int index)
{
  ShowBitRates(index,lib_lib->bitrate());
}


void EditRDLogedit::selectStartData()
{
  int cartnum=lib_startcart_edit->text().toInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    lib_startcart_edit->setText(QString().sprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectEndData()
{
  int cartnum=lib_endcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    lib_endcart_edit->setText(QString().sprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectRecordStartData()
{
  int cartnum=lib_recstartcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    lib_recstartcart_edit->setText(QString().sprintf("%d",cartnum));
  }
}


void EditRDLogedit::selectRecordEndData()
{
  int cartnum=lib_recendcart_edit->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    lib_recendcart_edit->setText(QString().sprintf("%d",cartnum));
  }
}


void EditRDLogedit::okData()
{
  unsigned rate=0;

  lib_lib->setInputCard(lib_input_card->card());
  lib_lib->setInputPort(lib_input_card->port());
  lib_lib->setOutputCard(lib_output_card->card());
  lib_lib->setOutputPort(lib_output_card->port());
  lib_lib->setMaxLength(QTime().msecsTo(lib_maxlength_time->time()));
  lib_lib->setTrimThreshold(lib_threshold_spin->value()*100);
  lib_lib->setRipperLevel(lib_normalization_spin->value()*100);
  lib_lib->setTailPreroll(lib_preroll_spin->value());
  if(lib_startcart_edit->text().isEmpty()) {
    lib_lib->setStartCart(0);
  }
  else {
    lib_lib->setStartCart(lib_startcart_edit->text().toUInt());
  }
  if(lib_endcart_edit->text().isEmpty()) {
    lib_lib->setEndCart(0);
  }
  else {
    lib_lib->setEndCart(lib_endcart_edit->text().toUInt());
  }
  if(lib_recstartcart_edit->text().isEmpty()) {
    lib_lib->setRecStartCart(0);
  }
  else {
    lib_lib->setRecStartCart(lib_recstartcart_edit->text().toUInt());
  }
  if(lib_recendcart_edit->text().isEmpty()) {
    lib_lib->setRecEndCart(0);
  }
  else {
    lib_lib->setRecEndCart(lib_recendcart_edit->text().toUInt());
  }
  lib_lib->setFormat(lib_format_box->currentItem());
  lib_lib->setDefaultChannels(lib_channels_box->currentItem()+1);
  rate=0;
  if(lib_format_box->currentItem()!=0) {
    sscanf(lib_bitrate_box->currentText(),"%d",&rate);
  }
  lib_lib->setBitrate(rate*1000);
  lib_lib->setEnableSecondStart(lib_enable_second_start_box->currentItem());
  lib_lib->setDefaultTransType(
    (RDLogLine::TransType)lib_default_transtype_box->currentItem());
  done(0);
}


void EditRDLogedit::cancelData()
{
  done(1);
}


void EditRDLogedit::ShowBitRates(int layer,int rate)
{
  lib_bitrate_box->clear();
  switch(layer) {
      case 0:  // PCM16
	lib_bitrate_box->setDisabled(true);
	break;

      case 1:  // MPEG-1 Layer 2
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps/chan"));
	lib_bitrate_box->insertItem(tr("48 kbps/chan"));
	lib_bitrate_box->insertItem(tr("56 kbps/chan"));
	lib_bitrate_box->insertItem(tr("64 kbps/chan"));
	lib_bitrate_box->insertItem(tr("80 kbps/chan"));
	lib_bitrate_box->insertItem(tr("96 kbps/chan"));
	lib_bitrate_box->insertItem(tr("112 kbps/chan"));
	lib_bitrate_box->insertItem(tr("128 kbps/chan"));
	lib_bitrate_box->insertItem(tr("160 kbps/chan"));
	lib_bitrate_box->insertItem(tr("192 kbps/chan"));
	switch(lib_lib->bitrate()) {
	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      break;

	    case 48000:
	      lib_bitrate_box->setCurrentItem(1);
	      break;

	    case 56000:
	      lib_bitrate_box->setCurrentItem(2);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(3);
	      break;

	    case 80000:
	      lib_bitrate_box->setCurrentItem(4);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(5);
	      break;

	    case 112000:
	      lib_bitrate_box->setCurrentItem(6);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(7);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(8);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(9);
	      break;

	    default:
	      lib_bitrate_box->setCurrentItem(7);   // 128 kbps/chan
	      break;
	}
	break;

      case 2:  // MPEG-1 Layer 3
	lib_bitrate_box->setEnabled(true);
	lib_bitrate_box->insertItem(tr("32 kbps/chan"));
	lib_bitrate_box->insertItem(tr("40 kbps/chan"));
	lib_bitrate_box->insertItem(tr("48 kbps/chan"));
	lib_bitrate_box->insertItem(tr("56 kbps/chan"));
	lib_bitrate_box->insertItem(tr("64 kbps/chan"));
	lib_bitrate_box->insertItem(tr("80 kbps/chan"));
	lib_bitrate_box->insertItem(tr("96 kbps/chan"));
	lib_bitrate_box->insertItem(tr("112 kbps/chan"));
	lib_bitrate_box->insertItem(tr("128 kbps/chan"));
	lib_bitrate_box->insertItem(tr("160 kbps/chan"));
	lib_bitrate_box->insertItem(tr("192 kbps/chan"));
	lib_bitrate_box->insertItem(tr("224 kbps/chan"));
	lib_bitrate_box->insertItem(tr("256 kbps/chan"));
	lib_bitrate_box->insertItem(tr("320 kbps/chan"));
	switch(lib_lib->layer()) {
	    case 32000:
	      lib_bitrate_box->setCurrentItem(0);
	      break;

	    case 40000:
	      lib_bitrate_box->setCurrentItem(1);
	      break;

	    case 48000:
	      lib_bitrate_box->setCurrentItem(2);
	      break;

	    case 56000:
	      lib_bitrate_box->setCurrentItem(3);
	      break;

	    case 64000:
	      lib_bitrate_box->setCurrentItem(4);
	      break;

	    case 80000:
	      lib_bitrate_box->setCurrentItem(5);
	      break;

	    case 96000:
	      lib_bitrate_box->setCurrentItem(6);
	      break;

	    case 112000:
	      lib_bitrate_box->setCurrentItem(7);
	      break;

	    case 128000:
	      lib_bitrate_box->setCurrentItem(8);
	      break;

	    case 160000:
	      lib_bitrate_box->setCurrentItem(9);
	      break;

	    case 192000:
	      lib_bitrate_box->setCurrentItem(10);
	      break;

	    case 224000:
	      lib_bitrate_box->setCurrentItem(11);
	      break;

	    case 256000:
	      lib_bitrate_box->setCurrentItem(12);
	      break;

	    case 320000:
	      lib_bitrate_box->setCurrentItem(13);
	      break;
	}
	break;

  }
}
