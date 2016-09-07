// view_node_info.cpp
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "view_node_info.h"

ViewNodeInfo::ViewNodeInfo(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle("RDAdmin - "+tr("View LiveWire Node"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // LiveWire Driver
  //
  view_livewire=new RDLiveWire(0,this);
  connect(view_livewire,SIGNAL(connected(unsigned)),
	  this,SLOT(connectedData(unsigned)));
  connect(view_livewire,SIGNAL(sourceChanged(unsigned,RDLiveWireSource *)),
	  this,SLOT(sourceChangedData(unsigned,RDLiveWireSource *)));
  connect(view_livewire,
	  SIGNAL(destinationChanged(unsigned,RDLiveWireDestination *)),
	  this,
	  SLOT(destinationChangedData(unsigned,RDLiveWireDestination *)));

  //
  // Node Hostname
  //
  view_hostname_edit=new QLineEdit(this);
  view_hostname_edit->setGeometry(90,10,190,20);
  view_hostname_edit->setReadOnly(true);
  QLabel *label=new QLabel(view_hostname_edit,tr("Hostname:"),this);
  label->setGeometry(10,10,80,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node TCP Port
  //
  view_tcpport_edit=new QLineEdit(this);
  view_tcpport_edit->setGeometry(335,10,sizeHint().width()-345,20);
  view_tcpport_edit->setReadOnly(true);
  label=new QLabel(view_tcpport_edit,tr("Port:"),this);
  label->setGeometry(305,10,30,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // System Version
  //
  view_system_edit=new QLineEdit(this);
  view_system_edit->setGeometry(135,32,70,20);
  view_system_edit->setReadOnly(true);
  label=new QLabel(view_system_edit,tr("System Version:"),this);
  label->setGeometry(10,32,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Protocol Version
  //
  view_protocol_edit=new QLineEdit(this);
  view_protocol_edit->setGeometry(335,32,40,20);
  view_protocol_edit->setReadOnly(true);
  label=new QLabel(view_protocol_edit,tr("Protocol Version:"),this);
  label->setGeometry(210,32,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sources
  //
  view_sources_edit=new QLineEdit(this);
  view_sources_edit->setGeometry(75,54,30,20);
  view_sources_edit->setReadOnly(true);
  label=new QLabel(view_sources_edit,tr("Sources:"),this);
  label->setGeometry(10,54,60,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Destinations
  //
  view_destinations_edit=new QLineEdit(this);
  view_destinations_edit->setGeometry(225,54,30,20);
  view_destinations_edit->setReadOnly(true);
  label=new QLabel(view_destinations_edit,tr("Destinations:"),this);
  label->setGeometry(120,54,100,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channels
  //
  view_channels_edit=new QLineEdit(this);
  view_channels_edit->setGeometry(360,54,30,20);
  view_channels_edit->setReadOnly(true);
  label=new QLabel(view_channels_edit,tr("Channels:"),this);
  label->setGeometry(260,54,95,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // GPIs
  //
  view_gpis_edit=new QLineEdit(this);
  view_gpis_edit->setGeometry(135,76,70,20);
  view_gpis_edit->setReadOnly(true);
  label=new QLabel(view_gpis_edit,tr("GPIs:"),this);
  label->setGeometry(10,76,120,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // GPOs
  //
  view_gpos_edit=new QLineEdit(this);
  view_gpos_edit->setGeometry(305,76,70,20);
  view_gpos_edit->setReadOnly(true);
  label=new QLabel(view_gpos_edit,tr("GPOs:"),this);
  label->setGeometry(210,76,90,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sources List
  //
  label=new QLabel(view_gpos_edit,tr("Sources"),this);
  label->setGeometry(15,98,90,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_sources_widget=new RDTableWidget(this);
  view_sources_widget->setColumnCount(7);
  view_sources_widget->setGeometry(10,118,sizeHint().width()-20,200);
  view_sources_widget->
    setHorizontalHeaderItem(0,new QTableWidgetItem(tr("#")));
  view_sources_widget->
    setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Channel")));
  view_sources_widget->
    setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Name")));
  view_sources_widget->
    setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Active")));
  view_sources_widget->
    setHorizontalHeaderItem(4,new QTableWidgetItem(tr("Shareable")));
  view_sources_widget->
    setHorizontalHeaderItem(5,new QTableWidgetItem(tr("Chanenels")));
  view_sources_widget->
    setHorizontalHeaderItem(6,new QTableWidgetItem(tr("Gain")));

  //
  // Destinations List
  //
  label=new QLabel(view_gpos_edit,tr("Destinations"),this);
  label->setGeometry(15,325,90,20);
  label->setFont(bold_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_destinations_widget=new RDTableWidget(this);
  view_destinations_widget->setGeometry(10,345,sizeHint().width()-20,200);
  view_destinations_widget->setColumnCount(6);
  view_destinations_widget->
    setHorizontalHeaderItem(0,new QTableWidgetItem(tr("#")));
  view_destinations_widget->
    setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Output")));
  view_destinations_widget->
    setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Name")));
  view_destinations_widget->
    setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Chanenels")));
  view_destinations_widget->
    setHorizontalHeaderItem(4,new QTableWidgetItem(tr("Load")));
  view_destinations_widget->
    setHorizontalHeaderItem(5,new QTableWidgetItem(tr("Gain")));

  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ViewNodeInfo::sizeHint() const
{
  return QSize(400,615);
}


QSizePolicy ViewNodeInfo::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ViewNodeInfo::exec(const QString &hostname,Q_UINT16 port,
			const QString &passwd,unsigned base_output)
{
  view_hostname_edit->setText(hostname);
  view_tcpport_edit->setText(QString().sprintf("%u",(unsigned)port));
  view_base_output=base_output;
  view_livewire->connectToHost(hostname,port,passwd,base_output);
  QDialog::exec();
}


void ViewNodeInfo::connectedData(unsigned id)
{
  view_protocol_edit->setText(view_livewire->protocolVersion());
  view_system_edit->setText(view_livewire->systemVersion());
  view_sources_edit->setText(QString().sprintf("%d",view_livewire->sources()));
  view_sources_widget->setRowCount(view_livewire->sources());
  for(int i=0;i<view_livewire->sources();i++) {
    view_sources_widget->
      setItem(i,0,new QTableWidgetItem(QString().sprintf("%d",i+1)));
    for(int j=0;j<6;j++) {
      view_sources_widget->setItem(i,j+1,new QTableWidgetItem(QString()));
    }
  }
  view_destinations_edit->
    setText(QString().sprintf("%d",view_livewire->destinations()));
  view_destinations_widget->setRowCount(view_livewire->destinations());
  for(int i=0;i<view_livewire->destinations();i++) {
    view_destinations_widget->
      setItem(i,0,new QTableWidgetItem(QString().sprintf("%d",i+1)));
    for(int j=0;j<5;j++) {
      view_destinations_widget->setItem(i,j+1,new QTableWidgetItem(QString()));
    }
  }
  view_channels_edit->
    setText(QString().sprintf("%d",view_livewire->channels()));
  view_gpis_edit->
    setText(QString().sprintf("%d [%d X %d]",
		     RD_LIVEWIRE_GPIO_BUNDLE_SIZE*view_livewire->gpis(),
			      view_livewire->gpis(),
			      RD_LIVEWIRE_GPIO_BUNDLE_SIZE));
  view_gpos_edit->
    setText(QString().sprintf("%d [%d X %d]",
		     RD_LIVEWIRE_GPIO_BUNDLE_SIZE*view_livewire->gpos(),
			      view_livewire->gpos(),
			      RD_LIVEWIRE_GPIO_BUNDLE_SIZE));
}


void ViewNodeInfo::sourceChangedData(unsigned id,RDLiveWireSource *src)
{
  if(src->channelNumber()<0) {
    view_sources_widget->item(src->slotNumber()-1,1)->
      setData(Qt::DisplayRole,QString());
  }
  else {
    view_sources_widget->item(src->slotNumber()-1,1)->
      setData(Qt::DisplayRole,QString().sprintf("%05d",src->channelNumber()));
  }
  view_sources_widget->item(src->slotNumber()-1,2)->
    setData(Qt::DisplayRole,src->primaryName());
  if(src->rtpEnabled()) {
    view_sources_widget->item(src->slotNumber()-1,3)->
      setData(Qt::DisplayRole,"Yes");
  }
  else {
    view_sources_widget->item(src->slotNumber()-1,3)->
      setData(Qt::DisplayRole,"No");
  }
  if(src->shareable()) {
    view_sources_widget->item(src->slotNumber()-1,4)->
      setData(Qt::DisplayRole,"Yes");
  }
  else {
    view_sources_widget->item(src->slotNumber()-1,4)->
      setData(Qt::DisplayRole,"No");
  }
  view_sources_widget->item(src->slotNumber()-1,5)->
    setData(Qt::DisplayRole,QString().sprintf("%d",src->channels()));
  view_sources_widget->item(src->slotNumber()-1,6)->
    setData(Qt::DisplayRole,
	    QString().sprintf("%4.1f",(float)src->inputGain()/10.0));
  if(src->slotNumber()==view_livewire->sources()) {
    view_sources_widget->resizeColumnsToContents();
  }
}


void ViewNodeInfo::destinationChangedData(unsigned id,
					  RDLiveWireDestination *dst)
{
  if(view_base_output<0) {
    view_destinations_widget->item(dst->slotNumber()-1,1)->
      setData(Qt::DisplayRole,QString());
  }
  else {
    view_destinations_widget->item(dst->slotNumber()-1,1)->
      setData(Qt::DisplayRole,
	      QString().sprintf("%u",view_base_output+dst->slotNumber()-1));
  }
  view_destinations_widget->item(dst->slotNumber()-1,2)->
    setData(Qt::DisplayRole,dst->primaryName());
  view_destinations_widget->item(dst->slotNumber()-1,3)->
    setData(Qt::DisplayRole,QString().sprintf("%d",dst->channels()));
  view_destinations_widget->item(dst->slotNumber()-1,4)->
    setData(Qt::DisplayRole,RDLiveWireDestination::loadString(dst->load()));
  view_destinations_widget->item(dst->slotNumber()-1,5)->
    setData(Qt::DisplayRole,
	    QString().sprintf("%4.1f",(float)dst->outputGain()/10.0));
  if(dst->slotNumber()==view_livewire->destinations()) {
    view_destinations_widget->resizeColumnsToContents();
  }
}


void ViewNodeInfo::closeData()
{
  done(0);
}
