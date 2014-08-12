// edit_cartslots.cpp
//
// Edit Rivendell CartSlot Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_cartslots.cpp,v 1.1.2.7 2013/12/23 18:47:22 cvs Exp $
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

#include <qmessagebox.h>

#include <rd.h>
#include <rdslotoptions.h>
#include <rdcart_dialog.h>
#include <rdescape_string.h>

#include <globals.h>
#include <edit_cartslots.h>

EditCartSlots::EditCartSlots(RDStation *station,RDStation *cae_station,
			     QWidget *parent,const char *name)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;

  edit_station=station;
  edit_cae_station=cae_station;
  edit_previous_slot=0;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("RDAdmin - Edit CartSlots"));

  //
  // Fonts
  //
  QFont font("helvetica",12,QFont::Normal);
  font.setPixelSize(12);
  QFont label_font("helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont title_font("helvetica",14,QFont::Bold);
  title_font.setPixelSize(14);

  //
  // Global Settings
  //
  QLabel *label=new QLabel(tr("Global Settings"),this);
  label->setGeometry(10,10,sizeHint().width()-20,20);
  label->setFont(title_font);
  label->setAlignment(AlignCenter);  

  //
  // Slot Columns
  //
  edit_slot_columns_spin=new QSpinBox(this);
  edit_slot_columns_spin->setGeometry(127,32,50,20);
  edit_slot_columns_spin->setRange(1,RDCARTSLOTS_MAX_COLUMNS);
  connect(edit_slot_columns_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(quantityChangedData(int)));
  label=new QLabel(edit_slot_columns_spin,tr("Slot Columns:"),this);
  label->setGeometry(10,32,112,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Slot Rows
  //
  edit_slot_rows_spin=new QSpinBox(this);
  edit_slot_rows_spin->setGeometry(127,54,50,20);
  edit_slot_rows_spin->setRange(1,RDCARTSLOTS_MAX_ROWS);
  connect(edit_slot_rows_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(quantityChangedData(int)));
  label=new QLabel(edit_slot_rows_spin,tr("Slot Rows:"),this);
  label->setGeometry(10,54,112,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Slot Selector
  //
  edit_slot_box=new QComboBox(this);
  edit_slot_box->setGeometry(107,98,60,20);
  connect(edit_slot_box,SIGNAL(activated(int)),
	  this,SLOT(slotChangedData(int)));
  label=new QLabel(edit_slot_box,tr("Slot"),this);
  label->setGeometry(10,98,92,20);
  label->setFont(title_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  label=new QLabel(tr("Settings"),this);
  label->setGeometry(172,98,sizeHint().width()-172,20);
  label->setFont(title_font);

  //
  // Channel Assignments
  //
  label=new QLabel(tr("Channel Assignments"),this);
  label->setGeometry(10,123,sizeHint().width()-20,20);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);  

  //
  // Card Selector
  //
  edit_card_spin=new QSpinBox(this);
  edit_card_spin->setGeometry(127,145,50,20);
  edit_card_spin->setRange(-1,cae_station->cards()-1);
  edit_card_spin->setSpecialValueText(tr("None"));
  connect(edit_card_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(cardChangedData(int)));
  label=new QLabel(edit_card_spin,tr("Card:"),this);
  label->setGeometry(10,145,112,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Input Port Selector
  //
  edit_input_spin=new QSpinBox(this);
  edit_input_spin->setGeometry(127,167,50,20);
  edit_input_spin->setRange(-1,RD_MAX_PORTS-1);
  edit_input_spin->setSpecialValueText(tr("None"));
  label=new QLabel(edit_input_spin,tr("Input Port:"),this);
  label->setGeometry(10,167,112,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Output Port Selector
  //
  edit_output_spin=new QSpinBox(this);
  edit_output_spin->setGeometry(127,189,50,20);
  edit_output_spin->setRange(-1,RD_MAX_PORTS-1);
  edit_output_spin->setSpecialValueText(tr("None"));
  label=new QLabel(edit_output_spin,tr("Output Port:"),this);
  label->setGeometry(10,189,112,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Default Settings
  //
  label=new QLabel(tr("Default Settings"),this);
  label->setGeometry(10,218,sizeHint().width()-20,20);
  label->setFont(label_font);
  label->setAlignment(AlignCenter);

  //
  // Service
  //
  edit_service_box=new QComboBox(this);
  edit_service_box->setGeometry(127,240,120,20);
  edit_service_label=
    new QLabel(edit_service_box,tr("Service:"),this);
  edit_service_label->setGeometry(10,240,112,20);
  edit_service_label->setFont(font);
  edit_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  sql="select NAME from SERVICES order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_service_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Slot Mode
  //
  edit_mode_box=new QComboBox(this);
  edit_mode_box->setGeometry(127,262,150,20);
  edit_mode_box->insertItem(tr("User previous mode"));
  for(int i=0;i<RDSlotOptions::LastMode;i++) {
    edit_mode_box->insertItem(RDSlotOptions::modeText((RDSlotOptions::Mode)i));
  }
  connect(edit_mode_box,SIGNAL(activated(int)),this,SLOT(modeData(int)));
  label=new QLabel(edit_mode_box,tr("Slot Mode:"),this);
  label->setGeometry(10,262,112,20);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Play Mode
  //
  edit_play_mode_box=new QComboBox(this);
  edit_play_mode_box->setGeometry(127,284,150,20);
  edit_play_mode_box->insertItem(tr("Use previous mode"));
  edit_play_mode_box->insertItem(tr("Full"));
  edit_play_mode_box->insertItem(tr("Hook"));
  edit_play_mode_label=new QLabel(edit_play_mode_box,tr("Play Mode:"),this);
  edit_play_mode_label->setGeometry(10,284,112,20);
  edit_play_mode_label->setFont(font);
  edit_play_mode_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart Action
  //
  edit_cartaction_box=new QComboBox(this);
  edit_cartaction_box->setGeometry(127,306,150,20);
  edit_cartaction_box->insertItem(tr("Use previous cart"));
  edit_cartaction_box->insertItem(tr("Do Nothing"));
  edit_cartaction_box->insertItem(tr("Load Specified Cart"));
  connect(edit_cartaction_box,SIGNAL(activated(int)),
	  this,SLOT(cartActionData(int)));
  edit_cartaction_label=
    new QLabel(edit_cartaction_box,tr("At Startup:"),this);
  edit_cartaction_label->setGeometry(10,306,112,20);
  edit_cartaction_label->setFont(font);
  edit_cartaction_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Cart
  //
  edit_cart_edit=new QLineEdit(this);
  edit_cart_edit->setGeometry(147,333,60,20);
  edit_cart_edit->setReadOnly(true);
  edit_cart_label=new QLabel(edit_cart_edit,tr("Cart:"),this);
  edit_cart_label->setGeometry(10,333,132,20);
  edit_cart_label->setFont(font);
  edit_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_cart_button=new QPushButton(tr("Select"),this);
  edit_cart_button->setGeometry(212,333,65,26);
  edit_cart_button->setFont(font);
  connect(edit_cart_button,SIGNAL(clicked()),this,SLOT(cartSelectData()));

  //
  // Stop Action
  //
  edit_stop_action_box=new QComboBox(this);
  edit_stop_action_box->setGeometry(127,360,150,20);
  edit_stop_action_box->insertItem(tr("Use previous action"));
  for(int i=0;i<RDSlotOptions::LastStop;i++) {
    edit_stop_action_box->
      insertItem(RDSlotOptions::stopActionText((RDSlotOptions::StopAction)i));
  }
  edit_stop_action_label=new QLabel(edit_stop_action_box,tr("At Playout End:"),this);
  edit_stop_action_label->setGeometry(10,360,112,20);
  edit_stop_action_label->setFont(font);
  edit_stop_action_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Close Button
  //
  QPushButton *button=new QPushButton(tr("Close"),this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(label_font);
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Load Values
  //
  edit_slot_columns_spin->setValue(station->cartSlotColumns());
  edit_slot_rows_spin->setValue(station->cartSlotRows());
  quantityChangedData(0);
  ReadSlot(edit_slot_box->currentItem());
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
  int slot=edit_slot_box->currentItem();
  int slot_quan=
    edit_slot_columns_spin->value()*edit_slot_rows_spin->value();

  edit_slot_box->clear();
  for(int i=0;i<slot_quan;i++) {
    edit_slot_box->insertItem(QString().sprintf("%d",i+1));
    if(i==slot) {
      edit_slot_box->setCurrentItem(i);
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
    cartActionData(edit_cartaction_box->currentItem());
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

  if(admin_cart_dialog->exec(&cartnum,RDCart::All,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    edit_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditCartSlots::closeData()
{
  edit_station->setCartSlotColumns(edit_slot_columns_spin->value());
  edit_station->setCartSlotRows(edit_slot_rows_spin->value());
  WriteSlot(edit_slot_box->currentItem());
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

  sql=QString("select CARD,INPUT_PORT,OUTPUT_PORT,DEFAULT_MODE,")+
    "DEFAULT_HOOK_MODE,"+
    "DEFAULT_STOP_ACTION,DEFAULT_CART_NUMBER,SERVICE_NAME from CARTSLOTS "+
    "where (STATION_NAME=\""+RDEscapeString(edit_station->name())+"\")&&"+
    QString().sprintf("(SLOT_NUMBER=%u)",slotnum);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    edit_card_spin->setValue(q->value(0).toInt());
    edit_input_spin->setValue(q->value(1).toInt());
    edit_output_spin->setValue(q->value(2).toInt());
    cardChangedData(edit_card_spin->value());
    edit_mode_box->setCurrentItem(q->value(3).toInt()+1);
    edit_play_mode_box->setCurrentItem(q->value(4).toInt()+1);
    edit_stop_action_box->setCurrentItem(q->value(5).toInt()+1);
    switch(q->value(6).toInt()) {
    case -1:
      edit_cartaction_box->setCurrentItem(0);
      break;

    case 0:
      edit_cartaction_box->setCurrentItem(1);
      break;

    default:
      edit_cartaction_box->setCurrentItem(2);
      edit_cart_edit->setText(QString().sprintf("%06d",q->value(6).toInt()));
    }
    cartActionData(edit_cartaction_box->currentItem());
    modeData(edit_mode_box->currentItem());
    for(int i=0;i<edit_service_box->count();i++) {
      if(q->value(7).toString()==edit_service_box->text(i)) {
	edit_service_box->setCurrentItem(i);
      }
    }
  }
  delete q;
}


void EditCartSlots::WriteSlot(unsigned slotnum)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update CARTSLOTS set ")+
    QString().sprintf("CARD=%d,INPUT_PORT=%d,OUTPUT_PORT=%d,",
		      edit_card_spin->value(),
		      edit_input_spin->value(),
		      edit_output_spin->value())+
    QString().sprintf("DEFAULT_MODE=%d,",
		      edit_mode_box->currentItem()-1)+
    QString().sprintf("DEFAULT_HOOK_MODE=%d,",
		      edit_play_mode_box->currentItem()-1)+
    QString().sprintf("DEFAULT_STOP_ACTION=%d,",
		      edit_stop_action_box->currentItem()-1);
  switch(edit_cartaction_box->currentItem()) {
  case 0:
    sql+="DEFAULT_CART_NUMBER=-1,";
    break;

  case 1:
    sql+="DEFAULT_CART_NUMBER=0,";
    break;

  default:
    sql+=QString().sprintf("DEFAULT_CART_NUMBER=%d,",
			   edit_cart_edit->text().toInt());
    break;
  }
  sql+="SERVICE_NAME=\""+RDEscapeString(edit_service_box->currentText())+"\" ";
  sql+="where (STATION_NAME=\""+RDEscapeString(edit_station->name())+"\")&&";
  sql+=QString().sprintf("(SLOT_NUMBER=%u)",slotnum);
  q=new RDSqlQuery(sql);
  delete q;
}
