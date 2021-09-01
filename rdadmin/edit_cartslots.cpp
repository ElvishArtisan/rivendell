// edit_cartslots.cpp
//
// Edit Rivendell CartSlot Configuration
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QMessageBox>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdslotoptions.h>

#include "edit_cartslots.h"
#include "globals.h"

EditCartSlots::EditCartSlots(RDStation *station,RDStation *cae_station,
			     QWidget *parent)
  : RDDialog(parent)
{
  edit_station=station;
  edit_cae_station=cae_station;
  edit_previous_slot=0;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Edit CartSlots"));

  //
  // Global Settings
  //
  QLabel *label=new QLabel(tr("Global Settings"),this);
  label->setGeometry(10,10,sizeHint().width()-20,20);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);  

  //
  // Slot Columns
  //
  edit_slot_columns_spin=new QSpinBox(this);
  edit_slot_columns_spin->setGeometry(127,32,50,20);
  edit_slot_columns_spin->setRange(1,RDCARTSLOTS_MAX_COLUMNS);
  connect(edit_slot_columns_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(quantityChangedData(int)));
  label=new QLabel(tr("Slot Columns:"),this);
  label->setGeometry(10,32,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Slot Rows
  //
  edit_slot_rows_spin=new QSpinBox(this);
  edit_slot_rows_spin->setGeometry(127,54,50,20);
  edit_slot_rows_spin->setRange(1,RDCARTSLOTS_MAX_ROWS);
  connect(edit_slot_rows_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(quantityChangedData(int)));
  label=new QLabel(tr("Slot Rows:"),this);
  label->setGeometry(10,54,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Slot Selector
  //
  edit_slot_box=new QComboBox(this);
  edit_slot_box->setGeometry(107,98,60,20);
  connect(edit_slot_box,SIGNAL(activated(int)),
	  this,SLOT(slotChangedData(int)));
  label=new QLabel(tr("Slot"),this);
  label->setGeometry(10,98,92,20);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  label=new QLabel(tr("Settings"),this);
  label->setGeometry(172,98,sizeHint().width()-172,20);
  label->setFont(sectionLabelFont());

  //
  // Channel Assignments
  //
  label=new QLabel(tr("Channel Assignments"),this);
  label->setGeometry(10,123,sizeHint().width()-20,20);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);  

  //
  // Card Selector
  //
  edit_card_spin=new QSpinBox(this);
  edit_card_spin->setGeometry(127,145,55,20);
  edit_card_spin->setRange(-1,cae_station->cards()-1);
  edit_card_spin->setSpecialValueText(tr("None"));
  connect(edit_card_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(cardChangedData(int)));
  label=new QLabel(tr("Card:"),this);
  label->setGeometry(10,145,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Input Port Selector
  //
  edit_input_spin=new QSpinBox(this);
  edit_input_spin->setGeometry(127,167,55,20);
  edit_input_spin->setRange(-1,RD_MAX_PORTS-1);
  edit_input_spin->setSpecialValueText(tr("None"));
  label=new QLabel(tr("Input Port:"),this);
  label->setGeometry(10,167,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Output Port Selector
  //
  edit_output_spin=new QSpinBox(this);
  edit_output_spin->setGeometry(127,189,55,20);
  edit_output_spin->setRange(-1,RD_MAX_PORTS-1);
  edit_output_spin->setSpecialValueText(tr("None"));
  label=new QLabel(tr("Output Port:"),this);
  label->setGeometry(10,189,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Default Settings
  //
  label=new QLabel(tr("Default Settings"),this);
  label->setGeometry(10,218,sizeHint().width()-20,20);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Service
  //
  edit_service_box=new QComboBox(this);
  edit_service_box->setGeometry(127,240,120,20);
  edit_service_model=new RDServiceListModel(true,this);
  edit_service_model->setFont(defaultFont());
  edit_service_model->setPalette(palette());
  edit_service_box->setModel(edit_service_model);
  edit_service_label=new QLabel(tr("Service:"),this);
  edit_service_label->setGeometry(10,240,112,20);
  edit_service_label->setFont(labelFont());
  edit_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Slot Mode
  //
  edit_mode_box=new QComboBox(this);
  edit_mode_box->setGeometry(127,262,150,20);
  edit_mode_box->insertItem(0,tr("User previous mode"));
  for(int i=0;i<RDSlotOptions::LastMode;i++) {
    edit_mode_box->insertItem(edit_mode_box->count(),
			      RDSlotOptions::modeText((RDSlotOptions::Mode)i));
  }
  connect(edit_mode_box,SIGNAL(activated(int)),this,SLOT(modeData(int)));
  label=new QLabel(tr("Slot Mode:"),this);
  label->setGeometry(10,262,112,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Play Mode
  //
  edit_play_mode_box=new QComboBox(this);
  edit_play_mode_box->setGeometry(127,284,150,20);
  edit_play_mode_box->insertItem(0,tr("Use previous mode"));
  edit_play_mode_box->insertItem(1,tr("Full"));
  edit_play_mode_box->insertItem(2,tr("Hook"));
  edit_play_mode_label=new QLabel(tr("Play Mode:"),this);
  edit_play_mode_label->setGeometry(10,284,112,20);
  edit_play_mode_label->setFont(labelFont());
  edit_play_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Action
  //
  edit_cartaction_box=new QComboBox(this);
  edit_cartaction_box->setGeometry(127,306,150,20);
  edit_cartaction_box->insertItem(0,tr("Use previous cart"));
  edit_cartaction_box->insertItem(1,tr("Do Nothing"));
  edit_cartaction_box->insertItem(2,tr("Load Specified Cart"));
  connect(edit_cartaction_box,SIGNAL(activated(int)),
	  this,SLOT(cartActionData(int)));
  edit_cartaction_label=new QLabel(tr("At Startup:"),this);
  edit_cartaction_label->setGeometry(10,306,112,20);
  edit_cartaction_label->setFont(labelFont());
  edit_cartaction_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart
  //
  edit_cart_edit=new QLineEdit(this);
  edit_cart_edit->setGeometry(147,333,60,20);
  edit_cart_edit->setReadOnly(true);
  edit_cart_label=new QLabel(tr("Cart:"),this);
  edit_cart_label->setGeometry(10,333,132,20);
  edit_cart_label->setFont(labelFont());
  edit_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_cart_button=new QPushButton(tr("Select"),this);
  edit_cart_button->setGeometry(212,333,50,20);
  edit_cart_button->setFont(subButtonFont());
  connect(edit_cart_button,SIGNAL(clicked()),this,SLOT(cartSelectData()));

  //
  // Stop Action
  //
  edit_stop_action_box=new QComboBox(this);
  edit_stop_action_box->setGeometry(127,360,150,20);
  edit_stop_action_box->insertItem(0,tr("Use previous action"));
  for(int i=0;i<RDSlotOptions::LastStop;i++) {
    edit_stop_action_box->
      insertItem(edit_stop_action_box->count(),
		 RDSlotOptions::stopActionText((RDSlotOptions::StopAction)i));
  }
  edit_stop_action_label=new QLabel(tr("At Playout End:"),this);
  edit_stop_action_label->setGeometry(10,360,112,20);
  edit_stop_action_label->setFont(labelFont());
  edit_stop_action_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(tr("Close"),this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Load Values
  //
  edit_slot_columns_spin->setValue(station->cartSlotColumns());
  edit_slot_rows_spin->setValue(station->cartSlotRows());
  quantityChangedData(0);
  ReadSlot(edit_slot_box->currentIndex());
}


EditCartSlots::~EditCartSlots()
{
}


QSize EditCartSlots::sizeHint() const
{
  return QSize(300,455);
}


void EditCartSlots::quantityChangedData(int index)
{
  int slot=edit_slot_box->currentIndex();
  int slot_quan=
    edit_slot_columns_spin->value()*edit_slot_rows_spin->value();

  edit_slot_box->clear();
  for(int i=0;i<slot_quan;i++) {
    edit_slot_box->insertItem(edit_slot_box->count(),
			      QString::asprintf("%d",i+1));
    if(i==slot) {
      edit_slot_box->setCurrentIndex(i);
    }
  }
  if(slot>slot_quan) {
    QMessageBox::information(this,"RDAdmin",tr("Slot selected has changed!"));
  }
}


void EditCartSlots::slotChangedData(int slotnum)
{
  WriteSlot(edit_previous_slot);
  ReadSlot(slotnum);
  edit_previous_slot=slotnum;
}


void EditCartSlots::cardChangedData(int card)
{
  edit_input_spin->setDisabled(card<0);
  edit_output_spin->setDisabled(card<0);
  if(card<0) {
    edit_input_spin->setValue(-1);
    edit_output_spin->setValue(-1);
  }
  else {
    edit_input_spin->setRange(-1,edit_cae_station->cardInputs(card)-1);
    edit_output_spin->setRange(-1,edit_cae_station->cardOutputs(card)-1);
  }
}


void EditCartSlots::modeData(int mode)
{
  edit_play_mode_label->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  edit_play_mode_box->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  edit_cartaction_label->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  edit_cartaction_box->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  edit_stop_action_label->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  edit_stop_action_box->
    setDisabled((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode);
  if((RDSlotOptions::Mode)mode!=RDSlotOptions::BreakawayMode) {
    cartActionData(0);
  }
  else {
    cartActionData(edit_cartaction_box->currentIndex());
  }
}


void EditCartSlots::cartActionData(int action)
{
  edit_cart_label->setEnabled(action==2);
  edit_cart_edit->setEnabled(action==2);
  edit_cart_button->setEnabled(action==2);
}


void EditCartSlots::cartSelectData()
{
  int cartnum=edit_cart_edit->text().toInt();

  if(admin_cart_dialog->exec(&cartnum,RDCart::All,QString(),NULL)==0) {
    edit_cart_edit->setText(QString::asprintf("%06d",cartnum));
  }
}


void EditCartSlots::closeData()
{
  edit_station->setCartSlotColumns(edit_slot_columns_spin->value());
  edit_station->setCartSlotRows(edit_slot_rows_spin->value());
  WriteSlot(edit_slot_box->currentIndex());
  done(0);
}


void EditCartSlots::closeEvent(QCloseEvent *e)
{
  closeData();
}


void EditCartSlots::ReadSlot(unsigned slotnum)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Ensure that the record exists
  //
  RDSlotOptions *opts=new RDSlotOptions(edit_station->name(),slotnum);
  delete opts;

  sql=QString("select ")+
    "`CARD`,"+                 // 00
    "`INPUT_PORT`,"+           // 01
    "`OUTPUT_PORT`,"+          // 02
    "`DEFAULT_MODE`,"+         // 03
    "`DEFAULT_HOOK_MODE`,"+    // 04
    "`DEFAULT_STOP_ACTION`,"+  // 05
    "`DEFAULT_CART_NUMBER`,"+  // 06
    "`SERVICE_NAME` "+         // 07
    "from `CARTSLOTS` where "+
    "(`STATION_NAME`='"+RDEscapeString(edit_station->name())+"')&&"+
    QString::asprintf("(`SLOT_NUMBER`=%u)",slotnum);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_card_spin->setValue(q->value(0).toInt());
    edit_input_spin->setValue(q->value(1).toInt());
    edit_output_spin->setValue(q->value(2).toInt());
    cardChangedData(edit_card_spin->value());
    edit_mode_box->setCurrentIndex(q->value(3).toInt()+1);
    edit_play_mode_box->setCurrentIndex(q->value(4).toInt()+1);
    edit_stop_action_box->setCurrentIndex(q->value(5).toInt()+1);
    switch(q->value(6).toInt()) {
    case -1:
      edit_cartaction_box->setCurrentIndex(0);
      break;

    case 0:
      edit_cartaction_box->setCurrentIndex(1);
      break;

    default:
      edit_cartaction_box->setCurrentIndex(2);
      edit_cart_edit->setText(QString::asprintf("%06d",q->value(6).toInt()));
    }
    cartActionData(edit_cartaction_box->currentIndex());
    modeData(edit_mode_box->currentIndex());
    for(int i=0;i<edit_service_box->count();i++) {
      if(q->value(7).toString()==edit_service_box->itemText(i)) {
	edit_service_box->setCurrentIndex(i);
      }
    }
  }
  delete q;
}


void EditCartSlots::WriteSlot(unsigned slotnum)
{
  QString sql;

  sql=QString("update `CARTSLOTS` set ")+
    QString::asprintf("`CARD`=%d,",edit_card_spin->value())+
    QString::asprintf("`INPUT_PORT`=%d,",edit_input_spin->value())+
    QString::asprintf("`OUTPUT_PORT`=%d,",edit_output_spin->value())+
    QString::asprintf("`DEFAULT_MODE`=%d,",
		      edit_mode_box->currentIndex()-1)+
    QString::asprintf("`DEFAULT_HOOK_MODE`=%d,",
		      edit_play_mode_box->currentIndex()-1)+
    QString::asprintf("`DEFAULT_STOP_ACTION`=%d,",
		      edit_stop_action_box->currentIndex()-1);
  switch(edit_cartaction_box->currentIndex()) {
  case 0:
    sql+="`DEFAULT_CART_NUMBER`=-1,";
    break;

  case 1:
    sql+="`DEFAULT_CART_NUMBER`=0,";
    break;

  default:
    sql+=QString::asprintf("`DEFAULT_CART_NUMBER`=%d,",
			   edit_cart_edit->text().toInt());
    break;
  }
  sql+="`SERVICE_NAME`='"+RDEscapeString(edit_service_box->currentText())+"' ";
  sql+="where (`STATION_NAME`='"+RDEscapeString(edit_station->name())+"')&&";
  sql+=QString::asprintf("(`SLOT_NUMBER`=%u)",slotnum);
  RDSqlQuery::apply(sql);
}
