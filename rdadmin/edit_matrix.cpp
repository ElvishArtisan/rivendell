// edit_matrix.cpp
//
// Edit a Rivendell Matrix
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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
#include <QPainter>

#include <rd.h>
#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdmatrix.h>
#include <rdtextvalidator.h>
#include <rdtty.h>

#include "globals.h"
#include "edit_user.h"
#include "edit_matrix.h"
#include "list_endpoints.h"
#include "list_gpis.h"
#include "list_nodes.h"
#include "list_livewiregpios.h"
#include "list_vguest_resources.h"
#include "list_sas_resources.h"

EditMatrix::EditMatrix(RDMatrix *matrix,QWidget *parent)
  : RDDialog(parent)
{
  QString str;

  edit_matrix=matrix;
  edit_stationname=matrix->station();
  edit_matrix_number=matrix->matrix();

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Edit Switcher"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);

  //
  // Matrix Number
  //
  QLabel *label=new QLabel(QString::asprintf("%d",edit_matrix_number),this);
  label->setGeometry(135,10,30,19);
  label=new QLabel(tr("Matrix Number:"),this);
  label->setGeometry(10,10,120,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Matrix Type
  //
  label=new QLabel(RDMatrix::typeString(edit_matrix->type()),this);
  label->setGeometry(135,30,200,19);
  label->setFont(labelFont());
  label=new QLabel(tr("Switcher Type:"),this);
  label->setGeometry(10,30,120,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Descriptive Name
  //
  edit_name_edit=new QLineEdit(this);
  edit_name_edit->setGeometry(135,50,240,19);
  edit_name_edit->setValidator(validator);
  label=new QLabel(tr("Description:"),this);
  label->setGeometry(10,50,120,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary Connection
  //
  label=new QLabel(tr("Primary Connection"),this);
  label->setGeometry(20,74,130,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Primary Type
  //
  edit_porttype_box=new QComboBox(this);
  edit_porttype_box->setGeometry(90,96,70,19);
  edit_porttype_label=new QLabel(tr("Type:"),this);
  edit_porttype_label->setGeometry(15,96,70,19);
  edit_porttype_label->setFont(labelFont());
  edit_porttype_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_porttype_box->insertItem(RDMatrix::TtyPort,tr("Serial"));
  edit_porttype_box->insertItem(RDMatrix::TcpPort,tr("TCP/IP"));
  edit_porttype_box->insertItem(RDMatrix::NoPort,tr("None"));
  connect(edit_porttype_box,SIGNAL(activated(int)),
	  this,SLOT(portTypeActivatedData(int)));

  //
  // Primary Serial Port
  //
  edit_port_box=new QComboBox(this);
  edit_port_box->setGeometry(290,96,90,19);
  edit_port_box->setEditable(false);
  edit_port_label=new QLabel(tr("Serial Port:"),this);
  edit_port_label->setGeometry(195,96,90,19);
  edit_port_label->setFont(labelFont());
  edit_port_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary IP Address
  //
  edit_ipaddress_edit=new QLineEdit(this);
  edit_ipaddress_edit->setGeometry(90,118,115,19);
  edit_ipaddress_label=new QLabel(tr("IP Address:"),this);
  edit_ipaddress_label->setGeometry(15,118,70,19);
  edit_ipaddress_label->setFont(labelFont());
  edit_ipaddress_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary IP Port
  //
  edit_ipport_spin=new QSpinBox(this);
  edit_ipport_spin->setGeometry(290,118,65,19);
  edit_ipport_spin->setRange(0,0xFFFF);
  edit_ipport_label=new QLabel(tr("IP Port:"),this);
  edit_ipport_label->setGeometry(215,118,70,19);
  edit_ipport_label->setFont(labelFont());
  edit_ipport_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary Username
  //
  edit_username_edit=new QLineEdit(this);
  edit_username_edit->setGeometry(90,140,115,19);
  edit_username_label=new QLabel(tr("Username:"),this);
  edit_username_label->setGeometry(15,140,70,19);
  edit_username_label->setFont(labelFont());
  edit_username_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary Password
  //
  edit_password_edit=new QLineEdit(this);
  edit_password_edit->setGeometry(290,140,115,19);
  edit_password_edit->setEchoMode(QLineEdit::Password);
  edit_password_label=new QLabel(tr("Password:"),this);
  edit_password_label->setGeometry(215,140,70,19);
  edit_password_label->setFont(labelFont());
  edit_password_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Primary Start Cart
  //
  edit_start_cart_edit=new QLineEdit(this);
  edit_start_cart_edit->setGeometry(120,164,80,19);
  edit_start_cart_label=new QLabel(tr("Startup Cart:"),this);
  edit_start_cart_label->setGeometry(15,164,100,19);
  edit_start_cart_label->setFont(labelFont());
  edit_start_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_start_cart_button=new QPushButton(tr("Select"),this);
  edit_start_cart_button->setFont(subButtonFont());
  edit_start_cart_button->setGeometry(205,162,60,24);
  connect(edit_start_cart_button,SIGNAL(clicked()),this,SLOT(startCartData()));

  //
  // Primary Stop Cart
  //
  edit_stop_cart_edit=new QLineEdit(this);
  edit_stop_cart_edit->setGeometry(120,188,80,19);
  edit_stop_cart_label=new QLabel(tr("Shutdown Cart:"),this);
  edit_stop_cart_label->setGeometry(15,188,100,19);
  edit_stop_cart_label->setFont(labelFont());
  edit_stop_cart_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_stop_cart_button=new QPushButton(tr("Select"),this);
  edit_stop_cart_button->setFont(subButtonFont());
  edit_stop_cart_button->setGeometry(205,186,60,24);
  connect(edit_stop_cart_button,SIGNAL(clicked()),this,SLOT(stopCartData()));

  //
  // Backup Connection
  //
  label=new QLabel(tr("Backup Connection"),this);
  label->setGeometry(20,221,130,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Backup Type
  //
  edit_porttype2_box=new QComboBox(this);
  edit_porttype2_box->setGeometry(90,243,70,19);
  edit_porttype2_label=new QLabel(tr("Type:"),this);
  edit_porttype2_label->setGeometry(15,243,70,19);
  edit_porttype2_label->setFont(labelFont());
  edit_porttype2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_porttype2_box->insertItem(RDMatrix::TtyPort,tr("Serial"));
  edit_porttype2_box->insertItem(RDMatrix::TcpPort,tr("TCP/IP"));
  edit_porttype2_box->insertItem(RDMatrix::NoPort,tr("None"));
  connect(edit_porttype2_box,SIGNAL(activated(int)),
	  this,SLOT(portType2ActivatedData(int)));

  //
  // Backup Serial Port
  //
  edit_port2_box=new QComboBox(this);
  edit_port2_box->setGeometry(290,243,90,19);
  edit_port2_box->setEditable(false);
  edit_port2_label=new QLabel(tr("Serial Port:"),this);
  edit_port2_label->setGeometry(195,243,90,19);
  edit_port2_label->setFont(labelFont());
  edit_port2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Backup IP Address
  //
  edit_ipaddress2_edit=new QLineEdit(this);
  edit_ipaddress2_edit->setGeometry(90,265,115,19);
  edit_ipaddress2_label=new QLabel(tr("IP Address:"),this);
  edit_ipaddress2_label->setGeometry(15,265,70,19);
  edit_ipaddress2_label->setFont(labelFont());
  edit_ipaddress2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Backup IP Port
  //
  edit_ipport2_spin=new QSpinBox(this);
  edit_ipport2_spin->setGeometry(290,265,65,19);
  edit_ipport2_spin->setRange(0,0xFFFF);
  edit_ipport2_label=new QLabel(tr("IP Port:"),this);
  edit_ipport2_label->setGeometry(215,265,70,19);
  edit_ipport2_label->setFont(labelFont());
  edit_ipport2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Backup Username
  //
  edit_username2_edit=new QLineEdit(this);
  edit_username2_edit->setGeometry(90,288,115,19);
  edit_username2_label=new QLabel(tr("Username:"),this);
  edit_username2_label->setGeometry(15,288,70,19);
  edit_username2_label->setFont(labelFont());
  edit_username2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Backup Password
  //
  edit_password2_edit=new QLineEdit(this);
  edit_password2_edit->setGeometry(290,288,115,19);
  edit_password2_edit->setEchoMode(QLineEdit::Password);
  edit_password2_label=new QLabel(tr("Password:"),this);
  edit_password2_label->setGeometry(215,288,70,19);
  edit_password2_label->setFont(labelFont());
  edit_password2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Backup Start Cart
  //
  edit_start_cart2_edit=new QLineEdit(this);
  edit_start_cart2_edit->setGeometry(120,312,80,19);
  edit_start_cart2_label=new QLabel(tr("Startup Cart:"),this);
  edit_start_cart2_label->setGeometry(15,312,100,19);
  edit_start_cart2_label->setFont(labelFont());
  edit_start_cart2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_start_cart2_button=new QPushButton(tr("Select"),this);
  edit_start_cart2_button->setFont(subButtonFont());
  edit_start_cart2_button->setGeometry(205,310,60,24);
  connect(edit_start_cart2_button,SIGNAL(clicked()),
	  this,SLOT(startCart2Data()));

  //
  // Backup Stop Cart
  //
  edit_stop_cart2_edit=new QLineEdit(this);
  edit_stop_cart2_edit->setGeometry(120,336,80,19);
  edit_stop_cart2_label=new QLabel(tr("Shutdown Cart:"),this);
  edit_stop_cart2_label->setGeometry(15,336,100,19);
  edit_stop_cart2_label->setFont(labelFont());
  edit_stop_cart2_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_stop_cart2_button=new QPushButton(tr("Select"),this);
  edit_stop_cart2_button->setFont(subButtonFont());
  edit_stop_cart2_button->setGeometry(205,334,60,24);
  connect(edit_stop_cart2_button,SIGNAL(clicked()),this,SLOT(stopCart2Data()));

  //
  // Card Number
  //
  edit_card_box=new QSpinBox(this);
  edit_card_box->setGeometry(75,371,60,19);
  edit_card_box->setRange(0,9999);
  edit_card_label=new QLabel(tr("Card:"),this);
  edit_card_label->setGeometry(10,371,60,19);
  edit_card_label->setFont(labelFont());
  edit_card_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Inputs
  //
  edit_inputs_box=new QSpinBox(this);
  edit_inputs_box->setGeometry(230,371,50,19);
  edit_inputs_box->setRange(0,MAX_ENDPOINTS);
  edit_inputs_label=new QLabel(tr("Inputs:"),this);
  edit_inputs_label->setGeometry(175,371,50,19);
  edit_inputs_label->setFont(labelFont());
  edit_inputs_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_inputs_box,SIGNAL(valueChanged(int)),
	  this,SLOT(inputsChangedData(int)));

  //
  // Outputs
  //
  edit_outputs_box=new QSpinBox(this);
  edit_outputs_box->setGeometry(355,371,50,19);
  edit_outputs_box->setRange(0,MAX_ENDPOINTS);
  edit_outputs_label=new QLabel(tr("Outputs:"),this);
  edit_outputs_label->setGeometry(280,371,70,19);
  edit_outputs_label->setFont(labelFont());
  edit_outputs_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Device
  //
  edit_device_edit=new QLineEdit(this);
  edit_device_edit->setGeometry(75,396,90,19);
  edit_device_edit->setValidator(validator);
  edit_device_label=new QLabel(tr("Device:"),this);
  edit_device_label->setGeometry(5,396,65,19);
  edit_device_label->setFont(labelFont());
  edit_device_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // GPIs
  //
  edit_gpis_box=new QSpinBox(this);
  edit_gpis_box->setGeometry(230,396,50,19);
  edit_gpis_box->setRange(0,MAX_GPIO_PINS);
  edit_gpis_label=new QLabel(tr("GPIs:"),this);
  edit_gpis_label->setGeometry(175,396,50,19);
  edit_gpis_label->setFont(labelFont());
  edit_gpis_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_gpis_box,SIGNAL(valueChanged(int)),
	  this,SLOT(gpisChangedData(int)));

  //
  // GPOs
  //
  edit_gpos_box=new QSpinBox(this);
  edit_gpos_box->setGeometry(355,396,50,19);
  edit_gpos_box->setRange(0,MAX_GPIO_PINS);
  edit_gpos_label=new QLabel(tr("GPOs:"),this);
  edit_gpos_label->setGeometry(280,396,70,19);
  edit_gpos_label->setFont(labelFont());
  edit_gpos_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(edit_gpos_box,SIGNAL(valueChanged(int)),
	  this,SLOT(gposChangedData(int)));

  //
  // Layer
  //
  edit_layer_box=new QComboBox(this);
  edit_layer_box->setGeometry(75,421,50,19);
  edit_layer_label=new QLabel(tr("Layer:"),this);
  edit_layer_label->setGeometry(10,421,60,19);
  edit_layer_label->setFont(labelFont());
  edit_layer_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  edit_layer_box->insertItem(0,"V");
  edit_layer_box->insertItem(1,"A");
  edit_layer_box->insertItem(2,"B");
  edit_layer_box->insertItem(3,"C");
  edit_layer_box->insertItem(4,"D");
  edit_layer_box->insertItem(5,"E");
  edit_layer_box->insertItem(6,"F");
  edit_layer_box->insertItem(7,"G");
  edit_layer_box->insertItem(8,"H");
  edit_layer_box->insertItem(9,"I");
  edit_layer_box->insertItem(10,"J");
  edit_layer_box->insertItem(11,"K");
  edit_layer_box->insertItem(12,"L");
  edit_layer_box->insertItem(13,"M");
  edit_layer_box->insertItem(14,"N");
  edit_layer_box->insertItem(15,"O");

  //
  // Displays
  //
  edit_displays_box=new QSpinBox(this);
  edit_displays_box->setGeometry(355,421,50,19);
  edit_displays_box->setRange(0,1024);
  edit_displays_label=new QLabel(tr("Displays:"),this);
  edit_displays_label->setGeometry(280,421,70,19);
  edit_displays_label->setFont(labelFont());
  edit_displays_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Configure Inputs Button
  //
  edit_inputs_button=new QPushButton(this);
  edit_inputs_button->setGeometry(35,446,80,50);
  edit_inputs_button->setFont(buttonFont());
  edit_inputs_button->setText(tr("Configure\nInputs"));
  connect(edit_inputs_button,SIGNAL(clicked()),this,SLOT(inputsButtonData()));

  //
  //  Configure Outputs Button
  //
  edit_outputs_button=new QPushButton(this);
  edit_outputs_button->setGeometry(125,446,80,50);
  edit_outputs_button->setFont(buttonFont());
  edit_outputs_button->setText(tr("Configure\nOutputs"));
  connect(edit_outputs_button,SIGNAL(clicked()),
	  this,SLOT(outputsButtonData()));

  //
  //  Configure GPIs Button
  //
  edit_gpis_button=new QPushButton(this);
  edit_gpis_button->setGeometry(215,446,80,50);
  edit_gpis_button->setDefault(true);
  edit_gpis_button->setFont(buttonFont());
  edit_gpis_button->setText(tr("Configure\nGPIs"));
  connect(edit_gpis_button,SIGNAL(clicked()),this,SLOT(gpisButtonData()));

  //
  //  Configure GPOs Button
  //
  edit_gpos_button=new QPushButton(this);
  edit_gpos_button->setGeometry(305,446,80,50);
  edit_gpos_button->setDefault(true);
  edit_gpos_button->setFont(buttonFont());
  edit_gpos_button->setText(tr("Configure\nGPOs"));
  connect(edit_gpos_button,SIGNAL(clicked()),this,SLOT(gposButtonData()));

  //
  //  LiveWire Nodes Button
  //
  edit_livewire_button=new QPushButton(this);
  edit_livewire_button->setGeometry(35,506,80,50);
  edit_livewire_button->setFont(buttonFont());
  edit_livewire_button->setText(tr("LiveWire\nNodes"));
  connect(edit_livewire_button,SIGNAL(clicked()),
	  this,SLOT(livewireButtonData()));

  //
  //  Livewire GPIOs Button
  //
  edit_livewire_gpio_button=new QPushButton(this);
  edit_livewire_gpio_button->setGeometry(125,506,80,50);
  edit_livewire_gpio_button->setFont(buttonFont());
  edit_livewire_gpio_button->setText(tr("LiveWire\nGPIOs"));
  connect(edit_livewire_gpio_button,SIGNAL(clicked()),
	  this,SLOT(livewireGpioButtonData()));

  //
  //  vGuest Switches Button
  //
  edit_vguestrelays_button=new QPushButton(this);
  edit_vguestrelays_button->setGeometry(215,506,80,50);
  edit_vguestrelays_button->setFont(buttonFont());
  edit_vguestrelays_button->setText(tr("vGuest\nSwitches"));
  connect(edit_vguestrelays_button,SIGNAL(clicked()),
	  this,SLOT(vguestRelaysButtonData()));

  //
  //  vGuest Displays Button
  //
  edit_vguestdisplays_button=new QPushButton(this);
  edit_vguestdisplays_button->setGeometry(305,506,80,50);
  edit_vguestdisplays_button->setFont(buttonFont());
  edit_vguestdisplays_button->setText(tr("vGuest\nDisplays"));
  connect(edit_vguestdisplays_button,SIGNAL(clicked()),
	  this,SLOT(vguestDisplaysButtonData()));

  //
  //  SAS Switches Button
  //
  edit_sasresources_button=new QPushButton(this);
  edit_sasresources_button->setGeometry(170,561,80,50);
  edit_sasresources_button->setFont(buttonFont());
  edit_sasresources_button->setText(tr("SAS\nSwitches"));
  connect(edit_sasresources_button,SIGNAL(clicked()),
	  this,SLOT(sasResourcesButtonData()));

  //
  //  Ok Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  edit_name_edit->setText(edit_matrix->name());
  str=QString(tr("Serial"));
  for(int i=0;i<MAX_TTYS;i++) {
    edit_port_box->
      insertItem(edit_port_box->count(),str+QString::asprintf("%d",i));
    edit_port2_box->
      insertItem(edit_port2_box->count(),str+QString::asprintf("%d",i));
  }
  edit_porttype_box->
    setCurrentIndex((int)edit_matrix->portType(RDMatrix::Primary));
  edit_porttype2_box->
    setCurrentIndex((int)edit_matrix->portType(RDMatrix::Backup));
  switch((RDMatrix::PortType)edit_porttype_box->currentIndex()) {
  case RDMatrix::TtyPort:
    edit_port_box->setCurrentIndex(edit_matrix->port(RDMatrix::Primary));
    edit_port2_box->setCurrentIndex(edit_matrix->port(RDMatrix::Backup));
    break;

  case RDMatrix::TcpPort:
    edit_ipaddress_edit->
      setText(edit_matrix->ipAddress(RDMatrix::Primary).toString());
    edit_ipport_spin->setValue(edit_matrix->ipPort(RDMatrix::Primary));
    edit_ipaddress2_edit->
      setText(edit_matrix->ipAddress(RDMatrix::Backup).toString());
    edit_ipport2_spin->setValue(edit_matrix->ipPort(RDMatrix::Backup));
    break;

  case RDMatrix::NoPort:
    break;
  }
  edit_card_box->setValue(edit_matrix->card());
  edit_inputs_box->setValue(edit_matrix->inputs());
  edit_outputs_box->setValue(edit_matrix->outputs());
  edit_device_edit->setText(edit_matrix->gpioDevice());
  edit_gpis_box->setValue(edit_matrix->gpis());
  edit_gpos_box->setValue(edit_matrix->gpos());
  edit_username_edit->setText(edit_matrix->username(RDMatrix::Primary));
  edit_password_edit->setText(edit_matrix->password(RDMatrix::Primary));
  edit_username2_edit->setText(edit_matrix->username(RDMatrix::Backup));
  edit_password2_edit->setText(edit_matrix->password(RDMatrix::Backup));
  if(edit_matrix->startCart(RDMatrix::Primary)>0) {
    edit_start_cart_edit->
      setText(QString::asprintf("%06u",
				edit_matrix->startCart(RDMatrix::Primary)));
  }
  if(edit_matrix->stopCart(RDMatrix::Primary)>0) {
    edit_stop_cart_edit->
      setText(QString::asprintf("%06u",
				edit_matrix->stopCart(RDMatrix::Primary)));
  }
  if(edit_matrix->startCart(RDMatrix::Backup)>0) {
    edit_start_cart2_edit->
      setText(QString::asprintf("%06u",
				edit_matrix->startCart(RDMatrix::Backup)));
  }
  if(edit_matrix->stopCart(RDMatrix::Backup)>0) {
    edit_stop_cart2_edit->
      setText(QString::asprintf("%06u",
				edit_matrix->stopCart(RDMatrix::Backup)));
  }
  edit_displays_box->setValue(edit_matrix->displays());
  if(edit_matrix->layer()=='V') {
    edit_layer_box->setCurrentIndex(0);
  }
  else {
    edit_layer_box->setCurrentIndex(edit_matrix->layer()-'@');
  }

  RDMatrix::Type type=edit_matrix->type();

  //
  // Connection Sections
  //
  edit_porttype_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::PortTypeControl));
  edit_porttype_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::PortTypeControl));
  edit_porttype2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::PortTypeControl));
  edit_porttype2_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::PortTypeControl));

  edit_port_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::SerialPortControl));
  edit_port_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::SerialPortControl));
  edit_port2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::SerialPortControl));
  edit_port2_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::SerialPortControl));

  edit_ipaddress_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::IpAddressControl));
  edit_ipaddress_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::IpAddressControl));
  edit_ipaddress2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::IpAddressControl));
  edit_ipaddress2_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::IpAddressControl));

  edit_ipport_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::IpPortControl));
  edit_ipport_spin->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::IpPortControl));
  edit_ipport2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::IpPortControl));
  edit_ipport2_spin->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::IpPortControl));

  edit_username_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::UsernameControl));
  edit_username_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::UsernameControl));
  edit_username2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::UsernameControl));
  edit_username2_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::UsernameControl));

  edit_password_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::PasswordControl));
  edit_password_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::PasswordControl));
  edit_password2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::PasswordControl));
  edit_password2_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::PasswordControl));

  edit_start_cart_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::StartupCartControl));
  edit_start_cart_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::StartupCartControl));
  edit_start_cart_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::StartupCartControl));

  edit_start_cart2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::StartupCartControl));
  edit_start_cart2_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::StartupCartControl));
  edit_start_cart2_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::StartupCartControl));

  edit_stop_cart_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::ShutdownCartControl));
  edit_stop_cart_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::ShutdownCartControl));
  edit_stop_cart_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Primary,
				       RDMatrix::ShutdownCartControl));

  edit_stop_cart2_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::ShutdownCartControl));
  edit_stop_cart2_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::ShutdownCartControl));
  edit_stop_cart2_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::Backup,
				       RDMatrix::ShutdownCartControl));

  //
  // Device Settings Section
  //
  edit_card_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::CardControl));
  edit_card_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::CardControl));

  edit_device_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GpioDeviceControl));
  edit_device_edit->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GpioDeviceControl));

  edit_layer_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::LayerControl));
  edit_layer_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::LayerControl));

  edit_inputs_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::InputsControl));
  edit_inputs_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::InputsControl));

  edit_outputs_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::OutputsControl));
  edit_outputs_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::OutputsControl));

  edit_gpis_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GpisControl));
  edit_gpis_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GpisControl));
  edit_gpis_box->
    setSingleStep(RDMatrix::defaultControlValue(type,RDMatrix::GpioStepSize));

  edit_gpos_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GposControl));
  edit_gpos_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GposControl));
  edit_gpos_box->
    setSingleStep(RDMatrix::defaultControlValue(type,RDMatrix::GpioStepSize));

  edit_displays_label->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::DisplaysControl));
  edit_displays_box->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::DisplaysControl));

  //
  // Button Section
  //
  edit_inputs_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::InputsButtonControl));
  edit_outputs_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::OutputsButtonControl));
  edit_gpis_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GpisButtonControl));
  edit_gpos_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::GposButtonControl));
  edit_livewire_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::NodesButtonControl));
  edit_livewire_gpio_button->
    setEnabled(RDMatrix::controlActive(type,RDMatrix::LivewireGpioButtonControl));
  edit_vguestrelays_button->
    setEnabled(RDMatrix::controlActive(type,
				       RDMatrix::VguestSwitchesButtonControl));
  edit_vguestdisplays_button->
    setEnabled(RDMatrix::controlActive(type,
				       RDMatrix::VguestDisplaysButtonControl));
  edit_sasresources_button->
    setEnabled(RDMatrix::controlActive(type,
				       RDMatrix::SasSwitchesButtonControl));

  portTypeActivatedData(edit_porttype_box->currentIndex());
  portType2ActivatedData(edit_porttype2_box->currentIndex());
  gpisChangedData(edit_gpis_box->value());
  gposChangedData(edit_gpos_box->value());
}


QSize EditMatrix::sizeHint() const
{
  return QSize(420,686);
} 


QSizePolicy EditMatrix::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditMatrix::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(Qt::black));

  p->drawLine(10,84,20,84);
  p->drawLine(150,84,sizeHint().width()-10,84);
  p->drawLine(sizeHint().width()-10,84,sizeHint().width()-10,218);
  p->drawLine(10,218,sizeHint().width()-10,218);
  p->drawLine(10,84,10,218);

  p->drawLine(10,232,20,232);
  p->drawLine(150,232,sizeHint().width()-10,232);
  p->drawLine(sizeHint().width()-10,232,sizeHint().width()-10,364);
  p->drawLine(10,364,sizeHint().width()-10,364);
  p->drawLine(10,232,10,364);

  p->end();
  delete p;
}


void EditMatrix::portTypeActivatedData(int index)
{
  RDMatrix::Type type=edit_matrix->type();

  switch((RDMatrix::PortType)edit_porttype_box->currentIndex()) {
  case RDMatrix::TtyPort:
    edit_port_box->setEnabled(RDMatrix::controlActive(type,
		    RDMatrix::Primary,RDMatrix::SerialPortControl));
    edit_port_label->setEnabled(RDMatrix::controlActive(type,
		      RDMatrix::Primary,RDMatrix::SerialPortControl));
    edit_username_edit->setEnabled(RDMatrix::controlActive(type,
		         RDMatrix::Primary,RDMatrix::UsernameControl));
    edit_username_label->setEnabled(RDMatrix::controlActive(type,
		          RDMatrix::Primary,RDMatrix::UsernameControl));
    edit_password_edit->setEnabled(RDMatrix::controlActive(type,
		         RDMatrix::Primary,RDMatrix::PasswordControl));
    edit_password_label->setEnabled(RDMatrix::controlActive(type,
		          RDMatrix::Primary,RDMatrix::PasswordControl));
    edit_ipaddress_edit->setDisabled(true);
    edit_ipaddress_label->setDisabled(true);
    edit_ipport_spin->setDisabled(true);
    edit_ipport_label->setDisabled(true);
    edit_start_cart_label->setDisabled(true);
    edit_start_cart_edit->setDisabled(true);
    edit_start_cart_button->setDisabled(true);
    edit_stop_cart_label->setDisabled(true);
    edit_stop_cart_edit->setDisabled(true);
    edit_stop_cart_button->setDisabled(true);
    break;
    
  case RDMatrix::TcpPort:
    edit_port_box->setDisabled(true);
    edit_port_label->setDisabled(true);
    edit_ipaddress_edit->setEnabled(RDMatrix::controlActive(type,
			  RDMatrix::Primary,RDMatrix::IpAddressControl));
    edit_ipaddress_label->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Primary,RDMatrix::IpAddressControl));
    edit_ipport_spin->setEnabled(RDMatrix::controlActive(type,
		       RDMatrix::Primary,RDMatrix::IpPortControl));
    edit_ipport_label->setEnabled(RDMatrix::controlActive(type,
			RDMatrix::Primary,RDMatrix::IpPortControl));
    edit_start_cart_edit->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Primary,RDMatrix::StartupCartControl));
    edit_start_cart_button->setEnabled(RDMatrix::controlActive(type,
			     RDMatrix::Primary,RDMatrix::StartupCartControl));
    edit_start_cart_label->setEnabled(RDMatrix::controlActive(type,
			    RDMatrix::Primary,RDMatrix::StartupCartControl));
    edit_stop_cart_edit->setEnabled(RDMatrix::controlActive(type,
			  RDMatrix::Primary,RDMatrix::ShutdownCartControl));
    edit_stop_cart_button->setEnabled(RDMatrix::controlActive(type,
			    RDMatrix::Primary,RDMatrix::ShutdownCartControl));
    edit_stop_cart_label->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Primary,RDMatrix::ShutdownCartControl));
    break;

  case RDMatrix::NoPort:
    break;
  }
}


void EditMatrix::portType2ActivatedData(int index)
{
  RDMatrix::Type type=edit_matrix->type();

  switch((RDMatrix::PortType)edit_porttype2_box->currentIndex()) {
  case RDMatrix::TtyPort:
    edit_port2_box->setEnabled(RDMatrix::controlActive(type,
		     RDMatrix::Backup,RDMatrix::SerialPortControl));
    edit_port2_label->setEnabled(RDMatrix::controlActive(type,
		       RDMatrix::Backup,RDMatrix::SerialPortControl));
    edit_username2_edit->setEnabled(RDMatrix::controlActive(type,
		          RDMatrix::Backup,RDMatrix::UsernameControl));
    edit_username2_label->setEnabled(RDMatrix::controlActive(type,
		           RDMatrix::Backup,RDMatrix::UsernameControl));
    edit_password2_edit->setEnabled(RDMatrix::controlActive(type,
		          RDMatrix::Backup,RDMatrix::PasswordControl));
    edit_password2_label->setEnabled(RDMatrix::controlActive(type,
		           RDMatrix::Backup,RDMatrix::PasswordControl));
    edit_ipaddress2_edit->setDisabled(true);
    edit_ipaddress2_label->setDisabled(true);
    edit_ipport2_spin->setDisabled(true);
    edit_ipport2_label->setDisabled(true);
    edit_start_cart2_label->setDisabled(true);
    edit_start_cart2_edit->setDisabled(true);
    edit_start_cart2_button->setDisabled(true);
    edit_stop_cart2_label->setDisabled(true);
    edit_stop_cart2_edit->setDisabled(true);
    edit_stop_cart2_button->setDisabled(true);
    break;
    
  case RDMatrix::TcpPort:
    edit_ipaddress2_edit->setEnabled(RDMatrix::controlActive(type,
			  RDMatrix::Backup,RDMatrix::IpAddressControl));
    edit_ipaddress2_label->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Backup,RDMatrix::IpAddressControl));
    edit_ipport2_spin->setEnabled(RDMatrix::controlActive(type,
		       RDMatrix::Backup,RDMatrix::IpPortControl));
    edit_ipport2_label->setEnabled(RDMatrix::controlActive(type,
			RDMatrix::Backup,RDMatrix::IpPortControl));
    edit_start_cart2_edit->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Backup,RDMatrix::StartupCartControl));
    edit_start_cart2_button->setEnabled(RDMatrix::controlActive(type,
			     RDMatrix::Backup,RDMatrix::StartupCartControl));
    edit_start_cart2_label->setEnabled(RDMatrix::controlActive(type,
			    RDMatrix::Backup,RDMatrix::StartupCartControl));
    edit_stop_cart2_edit->setEnabled(RDMatrix::controlActive(type,
			  RDMatrix::Backup,RDMatrix::ShutdownCartControl));
    edit_stop_cart2_button->setEnabled(RDMatrix::controlActive(type,
			    RDMatrix::Backup,RDMatrix::ShutdownCartControl));
    edit_stop_cart2_label->setEnabled(RDMatrix::controlActive(type,
			   RDMatrix::Backup,RDMatrix::ShutdownCartControl));
    edit_port2_box->setDisabled(true);
    edit_port2_label->setDisabled(true);
    break;
		  
  case RDMatrix::NoPort:
    edit_port2_box->setDisabled(true);
    edit_port2_label->setDisabled(true);
    edit_ipaddress2_edit->setDisabled(true);
    edit_ipaddress2_label->setDisabled(true);
    edit_ipport2_spin->setDisabled(true);
    edit_ipport2_label->setDisabled(true);
    edit_username2_edit->setDisabled(true);
    edit_username2_label->setDisabled(true);
    edit_password2_edit->setDisabled(true);
    edit_password2_label->setDisabled(true);
    edit_start_cart2_label->setDisabled(true);
    edit_start_cart2_edit->setDisabled(true);
    edit_start_cart2_button->setDisabled(true);
    edit_stop_cart2_label->setDisabled(true);
    edit_stop_cart2_edit->setDisabled(true);
    edit_stop_cart2_button->setDisabled(true);
    break;
  }
}


void EditMatrix::inputsButtonData()
{
  if(!WriteMatrix()) {
    return;
  }
  ListEndpoints *ep=new ListEndpoints(edit_matrix,RDMatrix::Input,this);
  ep->exec();
  delete ep;
}


void EditMatrix::outputsButtonData()
{
  if(!WriteMatrix()) {
    return;
  }
  ListEndpoints *ep=new ListEndpoints(edit_matrix,RDMatrix::Output,this);
  ep->exec();
  delete ep;
}


void EditMatrix::xpointsButtonData()
{
}


void EditMatrix::gpisButtonData()
{
  if(!WriteMatrix()) {
    return;
  }
  ListGpis *ep=new ListGpis(edit_matrix,RDMatrix::GpioInput,this);
  ep->exec();
  delete ep;
}


void EditMatrix::gposButtonData()
{
  if(!WriteMatrix()) {
    return;
  }
  ListGpis *ep=new ListGpis(edit_matrix,RDMatrix::GpioOutput,this);
  ep->exec();
  delete ep;
}


void EditMatrix::inputsChangedData(int value)
{
  if(RDMatrix::controlActive(edit_matrix->type(),
			     RDMatrix::GpioInputsLinkedControl)) {
    edit_gpis_box->setValue(value);
    edit_gpos_box->setValue(value);
  }
  edit_inputs_button->setEnabled(value>0);
}


void EditMatrix::gpisChangedData(int value)
{
  if(RDMatrix::controlActive(edit_matrix->type(),
			     RDMatrix::GpiGpoLinkedControl)) {
    edit_gpos_box->setValue(value);
  }
  if(RDMatrix::controlActive(edit_matrix->type(),
			     RDMatrix::GpioInputsLinkedControl)) {
    edit_inputs_box->setValue(value);
  }
  edit_gpis_button->setEnabled(value>0);
}


void EditMatrix::gposChangedData(int value)
{
  if(RDMatrix::controlActive(edit_matrix->type(),
			     RDMatrix::GpiGpoLinkedControl)) {
    edit_gpis_box->setValue(value);
  }
  if(RDMatrix::controlActive(edit_matrix->type(),
			     RDMatrix::GpioInputsLinkedControl)) {
    edit_inputs_box->setValue(value);
  }
  edit_gpos_button->setEnabled(value>0);
}


void EditMatrix::livewireButtonData()
{
  ListNodes *dialog=new ListNodes(edit_matrix,this);
  dialog->exec();
  delete dialog;
}


void EditMatrix::livewireGpioButtonData()
{
  AddGpioSlots();
  ListLiveWireGpios *dialog=
    new ListLiveWireGpios(edit_matrix,edit_gpis_box->value(),this);
  dialog->exec();
  delete dialog;
}


void EditMatrix::vguestRelaysButtonData()
{
  AddResources(RDMatrix::VguestTypeRelay);
  ListVguestResources *dialog=
    new ListVguestResources(edit_matrix,RDMatrix::VguestTypeRelay,
			    edit_gpos_box->value(),this);
  dialog->exec();
  delete dialog;
}


void EditMatrix::vguestDisplaysButtonData()
{
  AddResources(RDMatrix::VguestTypeDisplay);
  ListVguestResources *dialog=
    new ListVguestResources(edit_matrix,RDMatrix::VguestTypeDisplay,
			    edit_displays_box->value(),this);
  dialog->exec();
  delete dialog;
}


void EditMatrix::sasResourcesButtonData()
{
  ListSasResources *dialog=
    new ListSasResources(edit_matrix,edit_displays_box->value(),this);
  dialog->exec();
  delete dialog;
}


void EditMatrix::startCartData()
{
  int cartnum=edit_start_cart_edit->text().toUInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    if(cartnum>0) {
      edit_start_cart_edit->setText(QString::asprintf("%06u",cartnum));
    }
    else {
      edit_start_cart_edit->setText("");
    }
  }
}


void EditMatrix::stopCartData()
{
  int cartnum=edit_stop_cart_edit->text().toUInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    if(cartnum>0) {
      edit_stop_cart_edit->setText(QString::asprintf("%06u",cartnum));
    }
    else {
      edit_stop_cart_edit->setText("");
    }
  }
}


void EditMatrix::startCart2Data()
{
  int cartnum=edit_start_cart2_edit->text().toUInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    if(cartnum>0) {
      edit_start_cart2_edit->setText(QString::asprintf("%06u",cartnum));
    }
    else {
      edit_start_cart2_edit->setText("");
    }
  }
}


void EditMatrix::stopCart2Data()
{
  int cartnum=edit_stop_cart2_edit->text().toUInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::Macro,QString(),NULL)==0) {
    if(cartnum>0) {
      edit_stop_cart2_edit->setText(QString::asprintf("%06u",cartnum));
    }
    else {
      edit_stop_cart2_edit->setText("");
    }
  }
}


void EditMatrix::okData()
{
  if(edit_matrix->type()==RDMatrix::LogitekVguest) {
    if((!ConfirmPruneEndpoints(RDMatrix::Input))||
       (!ConfirmPruneEndpoints(RDMatrix::Output))||
       (!ConfirmPruneResources(RDMatrix::VguestTypeRelay))||
       (!ConfirmPruneResources(RDMatrix::VguestTypeDisplay))) {
      return;
    }
    PruneEndpoints(RDMatrix::Input);
    PruneEndpoints(RDMatrix::Output);
    PruneResources(RDMatrix::VguestTypeRelay);
    PruneResources(RDMatrix::VguestTypeDisplay);
  }
  if(edit_matrix->type()==RDMatrix::SasUsi) {
    if((!ConfirmPruneEndpoints(RDMatrix::Input))||
       (!ConfirmPruneEndpoints(RDMatrix::Output))||
       (!ConfirmPruneResources(RDMatrix::VguestTypeNone))) {
      return;
    }
    PruneEndpoints(RDMatrix::Input);
    PruneEndpoints(RDMatrix::Output);
    PruneResources(RDMatrix::VguestTypeNone);
  }

  if(!WriteMatrix()) {
    return;
  }
  done(true);
}


void EditMatrix::cancelData()
{
  done(false);
}


bool EditMatrix::WriteMatrix()
{
  QHostAddress addr;
  QHostAddress addr2;
  RDMatrix::Type type=edit_matrix->type();
  RDTty *tty=NULL;

  //
  // Ensure Sane Values
  //
  switch((RDMatrix::PortType)edit_porttype_box->currentIndex()) {
  case RDMatrix::TcpPort:
    if(!addr.setAddress(edit_ipaddress_edit->text())) {
      QMessageBox::warning(this,"RDAdmin - "+tr("Invalid Address"),
			   tr("The primary IP address is invalid!"));
      return false;
    }
    break;
      
  case RDMatrix::TtyPort:
    tty=new RDTty(rda->station()->name(),edit_port_box->currentIndex());
    if(!tty->active()) {
      QMessageBox::information(this,"RDAdmin - "+tr("Error"),
			       tr("The primary serial device is not active!"));
      delete tty;
      return false;
    }
    delete tty;
    break;

  case RDMatrix::NoPort:
    break;
  }

  if(RDMatrix::controlActive(type,RDMatrix::Backup,RDMatrix::PortTypeControl)) {
    switch((RDMatrix::PortType)edit_porttype2_box->currentIndex()) {
    case RDMatrix::TcpPort:
      if(!addr2.setAddress(edit_ipaddress2_edit->text())) {
	QMessageBox::warning(this,tr("Invalid Address"),
			     tr("The backup IP address is invalid!"));
	return false;
      }
      if(edit_porttype_box->currentIndex()==RDMatrix::TcpPort) {
	if((addr==addr2)&&
	   (edit_ipport_spin->value()==edit_ipport2_spin->value())) {
	  QMessageBox::warning(this,tr("Duplicate Connections"),
		   tr("The primary and backup connections must be different!"));
	  return false;
	}
      }
      break;
		  
    case RDMatrix::TtyPort:
      if(edit_porttype_box->currentIndex()==RDMatrix::TtyPort) {
	if(edit_port_box->currentIndex()==edit_port2_box->currentIndex()) {
	  QMessageBox::warning(this,tr("Duplicate Connections"),
		   tr("The primary and backup connections must be different!"));
	  return false;
	}
      }
      tty=new RDTty(rda->station()->name(),edit_port2_box->currentIndex());
      if(!tty->active()) {
	QMessageBox::information(this,"RDAdmin - "+tr("Error"),
			       tr("The backup serial device is not active!"));
	delete tty;
	return false;
      }
      delete tty;
      break;
		  
    case RDMatrix::NoPort:
      break;
    }
  }

  switch((RDMatrix::PortType)edit_porttype_box->currentIndex()) {
  case RDMatrix::TtyPort:
    edit_matrix->setPortType(RDMatrix::Primary,RDMatrix::TtyPort);
    edit_matrix->setPort(RDMatrix::Primary,edit_port_box->currentIndex());
    edit_matrix->setIpAddress(RDMatrix::Primary,QHostAddress());
    edit_matrix->setIpPort(RDMatrix::Primary,0);
    break;
      
  case RDMatrix::TcpPort:
    edit_matrix->setPortType(RDMatrix::Primary,RDMatrix::TcpPort);
    edit_matrix->setPort(RDMatrix::Primary,-1);
    edit_matrix->setIpAddress(RDMatrix::Primary,addr);
    edit_matrix->setIpPort(RDMatrix::Primary,edit_ipport_spin->value());
    break;
  
  case RDMatrix::NoPort:
    break;
}
  switch((RDMatrix::PortType)edit_porttype2_box->currentIndex()) {
  case RDMatrix::TtyPort:
    edit_matrix->setPortType(RDMatrix::Backup,RDMatrix::TtyPort);
    edit_matrix->setPort(RDMatrix::Backup,edit_port2_box->currentIndex());
    edit_matrix->setIpAddress(RDMatrix::Backup,QHostAddress());
    edit_matrix->setIpPort(RDMatrix::Backup,0);
    break;
      
  case RDMatrix::TcpPort:
    edit_matrix->setPortType(RDMatrix::Backup,RDMatrix::TcpPort);
    edit_matrix->setPort(RDMatrix::Backup,-1);
    edit_matrix->setIpAddress(RDMatrix::Backup,addr2);
    edit_matrix->setIpPort(RDMatrix::Backup,edit_ipport2_spin->value());
    break;
      
  case RDMatrix::NoPort:
    edit_matrix->setPortType(RDMatrix::Backup,RDMatrix::NoPort);
    break;
  }
  if(edit_layer_box->currentIndex()==0) {
    edit_matrix->setLayer('V');
  }
  else {
    edit_matrix->setLayer('@'+edit_layer_box->currentIndex());
  }
  
  //
  // Update Endpoints
  //
  AddEndpoints(RDMatrix::Input);
  AddEndpoints(RDMatrix::Output);
  AddResources(RDMatrix::VguestTypeRelay);
  AddResources(RDMatrix::VguestTypeDisplay);
  AddGpioSlots();
  PurgeGpioSlots();

  //
  // Update GPIO Tables
  //
  WriteGpioTable(RDMatrix::GpioInput);
  WriteGpioTable(RDMatrix::GpioOutput);

  edit_matrix->setName(edit_name_edit->text());
  edit_matrix->setCard(edit_card_box->value());
  edit_matrix->setInputs(edit_inputs_box->value());
  edit_matrix->setOutputs(edit_outputs_box->value());
  edit_matrix->setGpioDevice(edit_device_edit->text());
  edit_matrix->setGpis(edit_gpis_box->value());
  edit_matrix->setGpos(edit_gpos_box->value());
  edit_matrix->setUsername(RDMatrix::Primary,edit_username_edit->text());
  edit_matrix->setPassword(RDMatrix::Primary,edit_password_edit->text());
  edit_matrix->setUsername(RDMatrix::Backup,edit_username2_edit->text());
  edit_matrix->setPassword(RDMatrix::Backup,edit_password2_edit->text());
  edit_matrix->setDisplays(edit_displays_box->value());
  edit_matrix->
    setStartCart(RDMatrix::Primary,edit_start_cart_edit->text().toUInt());
  edit_matrix->
    setStopCart(RDMatrix::Primary,edit_stop_cart_edit->text().toUInt());
  edit_matrix->
    setStartCart(RDMatrix::Backup,edit_start_cart2_edit->text().toUInt());
  edit_matrix->
    setStopCart(RDMatrix::Backup,edit_stop_cart2_edit->text().toUInt());

  return true;
}


void EditMatrix::WriteGpioTable(RDMatrix::GpioType type)
{
  QString sql;
  RDSqlQuery *q;
  QString tablename;
  int line_quan=0;
  switch(type) {
  case RDMatrix::GpioInput:
    tablename="`GPIS`";
    line_quan=edit_gpis_box->value();
    break;

  case RDMatrix::GpioOutput:
    tablename="`GPOS`";
    line_quan=edit_gpos_box->value();
    break;
  }

  if(!RDMatrix::controlActive(edit_matrix->type(),
			      RDMatrix::DynamicGpioControl)) {
    for(int i=0;i<line_quan;i++) {
      sql=QString("select `ID` from ")+tablename+
	" where (`STATION_NAME`='"+RDEscapeString(edit_stationname)+"')&&"+
	QString::asprintf("(`MATRIX`=%d)&&(`NUMBER`=%d)",
			  edit_matrix_number,i+1);
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql=QString("insert into ")+tablename+
	  " set `STATION_NAME`='"+RDEscapeString(edit_stationname)+"',"+
	  QString::asprintf("`MATRIX`=%d,`NUMBER`=%d,`MACRO_CART`=0",
			    edit_matrix_number,i+1);
	RDSqlQuery::apply(sql);
      }
      delete q;
    }

    //
    // Purge Stale Entries
    //
    sql=QString("delete from ")+tablename+
      " where (`STATION_NAME`='"+RDEscapeString(edit_stationname)+
      QString::asprintf("')&&(`MATRIX`=%d)&&(`NUMBER`>%d)",
			edit_matrix_number,line_quan);
    RDSqlQuery::apply(sql);
  }
}


void EditMatrix::AddEndpoints(RDMatrix::Endpoint ep) const
{
  //
  // Ensure that we have endpoint entries for at least the number of
  // endpoints specified in the corresponding 'Inputs'/'Outputs'
  // control. If we have "excess" entries, don't touch them!
  //
  QString sql;
  RDSqlQuery *q=NULL;

  QString table="`INPUTS`";
  int endpoint_quan=edit_inputs_box->value();
  QString name=tr("Input");
  if(ep==RDMatrix::Output) {
    table="`OUTPUTS`";
    endpoint_quan=edit_outputs_box->value();
    name=tr("Output");
  }
  for(int i=0;i<endpoint_quan;i++) {
    sql=QString("select ")+
      "`NUMBER` "+  // 00
      "from "+table+" where "+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
      QString::asprintf("`MATRIX`=%d && ",edit_matrix->matrix())+
      QString::asprintf("`NUMBER`=%d",i+1);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      sql=QString("insert into ")+table+" set "+
	"`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"',"+
	QString::asprintf("`MATRIX`=%d,",edit_matrix->matrix())+
	QString::asprintf("`NUMBER`=%d,",i+1)+
	"`NAME`='"+RDEscapeString(name+QString::asprintf(" %03d",i+1))+"'";
      RDSqlQuery::apply(sql);
    }
  }
  delete q;
}


void EditMatrix::PruneEndpoints(RDMatrix::Endpoint ep) const
{
  QString sql;

  QString table="`INPUTS`";
  int endpoint_quan=edit_inputs_box->value();
  if(ep==RDMatrix::Output) {
    table="`OUTPUTS`";
    endpoint_quan=edit_outputs_box->value();
  }

  sql=QString("delete from ")+table+" where "+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
      QString::asprintf("`MATRIX`=%d && ",edit_matrix->matrix())+
      QString::asprintf("`NUMBER`>%d",endpoint_quan);
  RDSqlQuery::apply(sql);
}


bool EditMatrix::ConfirmPruneEndpoints(RDMatrix::Endpoint ep)
{
  QString sql;
  RDSqlQuery *q=NULL;

  QString table="`INPUTS`";
  int endpoint_quan=edit_inputs_box->value();
  QString name=tr("Inputs");
  if(ep==RDMatrix::Output) {
    table="`OUTPUTS`";
    endpoint_quan=edit_outputs_box->value();
    name=tr("Outputs");
  }

  sql=QString("select ")+
    "`ID` "+  // 00
    "from "+table+" where "+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
      QString::asprintf("`MATRIX`=%d && ",edit_matrix->matrix())+
      QString::asprintf("`NUMBER`>%d",endpoint_quan);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
			    tr("It appears that the")+" "+name+" "+
			    tr("control has been changed to a lower value.")+" "+
			    tr("This may result in configuration data for the \"orphaned\"")+" "+name+" "+tr("being deleted.")+"\n"+
			    "\n"+
			    tr("Continue?"),
			    QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete q;
      return false;
    }
  }
  else {
    delete q;
    return true;
  }
  delete q;
  return true;
}


void EditMatrix::AddResources(RDMatrix::VguestType type) const
{
  //
  // Ensure that we have data entries for at least the number of
  // entries specified in the corresponding 'GPIs'/'GPOs' or 'Displays'
  // controls. If we have "excess" entries, don't touch them!
  //
  QString sql;
  RDSqlQuery *q=NULL;

  int entry_quan=edit_gpis_box->value();
  if(type==RDMatrix::VguestTypeDisplay) {
    entry_quan=edit_displays_box->value();
  }
  for(int i=0;i<entry_quan;i++) {
    sql=QString("select ")+
      "`ID` "+  // 00
      "from `VGUEST_RESOURCES` where "+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
      QString::asprintf("`MATRIX_NUM`=%d && ",edit_matrix->matrix());
    if(edit_matrix->type()==RDMatrix::LogitekVguest) {
      sql+=QString::asprintf("`VGUEST_TYPE`=%d && ",type);
    }
    sql+=QString::asprintf("`NUMBER`=%d",i+1);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      sql=QString("insert into `VGUEST_RESOURCES` set ")+
	"`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"',"+
	QString::asprintf("`MATRIX_NUM`=%d,",edit_matrix->matrix());
      if(edit_matrix->type()==RDMatrix::LogitekVguest) {
	sql+=QString::asprintf("`VGUEST_TYPE`=%d,",type);
      }
      else {
	sql+=QString::asprintf("`VGUEST_TYPE`=%d,",RDMatrix::VguestTypeNone);
      }
      sql+=QString::asprintf("`NUMBER`=%d",i+1);
      RDSqlQuery::apply(sql);
    }
    delete q;
  }
}


void EditMatrix::PruneResources(RDMatrix::VguestType type) const
{
  QString sql;

  int entry_quan=edit_gpis_box->value();
  if(type==RDMatrix::VguestTypeDisplay) {
    entry_quan=edit_displays_box->value();
  }

  sql=QString("delete from `VGUEST_RESOURCES` where ")+
    "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
    QString::asprintf("`MATRIX_NUM`=%d && ",edit_matrix->matrix());
  if(edit_matrix->type()==RDMatrix::LogitekVguest) {
    sql+=QString::asprintf("`VGUEST_TYPE`=%d && ",type);
  }
  sql+=QString::asprintf("`NUMBER`>%d",entry_quan);
  RDSqlQuery::apply(sql);
}


bool EditMatrix::ConfirmPruneResources(RDMatrix::VguestType type)
{
  QString sql;
  RDSqlQuery *q=NULL;

  int entry_quan=edit_gpis_box->value();
  QString name=tr("GPIs");
  if(type==RDMatrix::VguestTypeDisplay) {
    entry_quan=edit_displays_box->value();
    name=tr("Displays");
  }

  sql=QString("select ")+
    "`ID` "+  // 00
    "from `VGUEST_RESOURCES` where "+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
    QString::asprintf("`MATRIX_NUM`=%d && ",edit_matrix->matrix());
  if(edit_matrix->type()==RDMatrix::LogitekVguest) {
    sql+=QString::asprintf("`VGUEST_TYPE`=%d && ",type);
  }
  sql+=QString::asprintf("`NUMBER`>%d",entry_quan);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Warning"),
			    tr("It appears that the")+" "+name+" "+
			    tr("control has been changed to a lower value.")+" "+
			    tr("This may result in configuration data for the \"orphaned\"")+" "+name+" "+tr("being deleted.")+"\n"+
			    "\n"+
			    tr("Continue?"),
			    QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
      delete q;
      return false;
    }
  }
  else {
    delete q;
    return true;
  }
  delete q;
  return true;
}


void EditMatrix::AddGpioSlots()
{
  if(edit_matrix->type()==RDMatrix::LiveWireMcastGpio) {
    int bundle_end=edit_gpis_box->value()/5;
    for(int i=0;i<bundle_end;i++) {
      QString sql=QString("select ")+
	"`ID` "+  // 00
	"from `LIVEWIRE_GPIO_SLOTS` where "+
	"`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
	QString::asprintf("`MATRIX`=%d && ",edit_matrix->matrix())+
	QString::asprintf("`SLOT`=%d",i);
      RDSqlQuery *q=new RDSqlQuery(sql);
      if(!q->first()) {
	sql=QString("insert into `LIVEWIRE_GPIO_SLOTS` set ")+
	  "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"',"+
	  QString::asprintf("`MATRIX`=%d,",edit_matrix->matrix())+
	  QString::asprintf("`SLOT`=%d",i);
	RDSqlQuery::apply(sql);
      }
      delete q;
    }
  }
}


void EditMatrix::PurgeGpioSlots()
{
  if(edit_matrix->type()==RDMatrix::LiveWireMcastGpio) {
    QString sql=QString("delete from `LIVEWIRE_GPIO_SLOTS` where ")+
      "`STATION_NAME`='"+RDEscapeString(edit_matrix->station())+"' && "+
      QString::asprintf("`MATRIX`=%d && ",edit_matrix->matrix())+
      QString::asprintf("`SLOT`>=%d",edit_gpis_box->value()/5);
    RDSqlQuery::apply(sql);
  }
}
