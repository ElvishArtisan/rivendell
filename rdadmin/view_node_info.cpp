// view_node_info.cpp
//
// Edit a Rivendell LiveWire Node
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

#include <qlabel.h>
#include <qpushbutton.h>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include <view_node_info.h>

ViewNodeInfo::ViewNodeInfo(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDAdmin - "+tr("Viewing Livewire Node"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // LiveWire Driver
  //
  view_livewire=new RDLiveWire(0,this);
  connect(view_livewire,SIGNAL(connected(unsigned)),
	  this,SLOT(connectedData(unsigned)));

  //
  // Node Hostname
  //
  view_hostname_edit=new QLineEdit(this);
  view_hostname_edit->setGeometry(90,10,190,20);
  view_hostname_edit->setReadOnly(true);
  QLabel *label=new QLabel(tr("Hostname:"),this);
  label->setGeometry(10,10,80,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Node TCP Port
  //
  view_tcpport_edit=new QLineEdit(this);
  view_tcpport_edit->setGeometry(335,10,sizeHint().width()-345,20);
  view_tcpport_edit->setReadOnly(true);
  label=new QLabel(tr("Port:"),this);
  label->setGeometry(305,10,30,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // System Version
  //
  view_system_edit=new QLineEdit(this);
  view_system_edit->setGeometry(135,32,70,20);
  view_system_edit->setReadOnly(true);
  label=new QLabel(tr("System Version:"),this);
  label->setGeometry(10,32,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Protocol Version
  //
  view_protocol_edit=new QLineEdit(this);
  view_protocol_edit->setGeometry(335,32,40,20);
  view_protocol_edit->setReadOnly(true);
  label=new QLabel(tr("Protocol Version:"),this);
  label->setGeometry(210,32,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sources
  //
  view_sources_edit=new QLineEdit(this);
  view_sources_edit->setGeometry(75,54,30,20);
  view_sources_edit->setReadOnly(true);
  label=new QLabel(tr("Sources:"),this);
  label->setGeometry(10,54,60,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Destinations
  //
  view_destinations_edit=new QLineEdit(this);
  view_destinations_edit->setGeometry(225,54,30,20);
  view_destinations_edit->setReadOnly(true);
  label=new QLabel(tr("Destinations:"),this);
  label->setGeometry(120,54,100,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Channels
  //
  view_channels_edit=new QLineEdit(this);
  view_channels_edit->setGeometry(360,54,30,20);
  view_channels_edit->setReadOnly(true);
  label=new QLabel(tr("Channels:"),this);
  label->setGeometry(260,54,95,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // GPIs
  //
  view_gpis_edit=new QLineEdit(this);
  view_gpis_edit->setGeometry(135,76,70,20);
  view_gpis_edit->setReadOnly(true);
  label=new QLabel(tr("GPIs:"),this);
  label->setGeometry(10,76,120,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // GPOs
  //
  view_gpos_edit=new QLineEdit(this);
  view_gpos_edit->setGeometry(305,76,70,20);
  view_gpos_edit->setReadOnly(true);
  label=new QLabel(tr("GPOs:"),this);
  label->setGeometry(210,76,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Sources List
  //
  label=new QLabel(tr("Sources"),this);
  label->setGeometry(15,98,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_sources_view=new RDTableView(this);
  view_sources_view->setGeometry(10,118,sizeHint().width()-20,200);
  view_sources_model=new RDNodeSlotsModel(true,this);
  view_sources_model->setFont(defaultFont());
  view_sources_model->setPalette(palette());
  view_sources_view->setModel(view_sources_model);
  view_sources_view->resizeColumnsToContents();
  connect(view_sources_model,
	  SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
	  this,
	  SLOT(sourceChangedData(const QModelIndex &,const QModelIndex &)));
  connect(view_livewire,SIGNAL(sourceChanged(unsigned,RDLiveWireSource *)),
	  view_sources_model,SLOT(updateSource(unsigned,RDLiveWireSource *)));

  //
  // Destinations List
  //
  label=new QLabel(tr("Destinations"),this);
  label->setGeometry(15,325,90,20);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  view_destinations_view=new RDTableView(this);
  view_destinations_view->
    setGeometry(10,345,sizeHint().width()-20,200);
  view_destinations_model=new RDNodeSlotsModel(false,this);
  view_destinations_model->setFont(defaultFont());
  view_destinations_model->setPalette(palette());
  view_destinations_view->setModel(view_destinations_model);
  view_destinations_view->resizeColumnsToContents();
  connect(view_destinations_model,
	  SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &)),
	  this,
	  SLOT(destinationChangedData(const QModelIndex &,const QModelIndex &)));
  connect(view_livewire,
	  SIGNAL(destinationChanged(unsigned,RDLiveWireDestination *)),
	  view_destinations_model,
	  SLOT(updateDestination(unsigned,RDLiveWireDestination *)));

  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
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


void ViewNodeInfo::exec(const QString &hostname,uint16_t port,
			const QString &passwd,unsigned base_output)
{
  view_hostname_edit->setText(hostname);
  view_tcpport_edit->setText(QString().sprintf("%u",(unsigned)port));
  view_destinations_model->setBaseOutput(base_output);
  view_livewire->connectToHost(hostname,port,passwd,base_output);
  QDialog::exec();
}


void ViewNodeInfo::connectedData(unsigned id)
{
  view_sources_model->setSlotQuantity(view_livewire->sources());
  view_destinations_model->setSlotQuantity(view_livewire->destinations());

  view_protocol_edit->setText(view_livewire->protocolVersion());
  view_system_edit->setText(view_livewire->systemVersion());
  view_sources_edit->setText(QString().sprintf("%d",view_livewire->sources()));
  view_destinations_edit->
    setText(QString().sprintf("%d",view_livewire->destinations()));
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


void ViewNodeInfo::sourceChangedData(const QModelIndex &first,
				     const QModelIndex &last)
{
  view_sources_view->resizeColumnsToContents();
}


void ViewNodeInfo::destinationChangedData(const QModelIndex &first,
					  const QModelIndex &last)
{
  view_destinations_view->resizeColumnsToContents();
}


void ViewNodeInfo::closeData()
{
  done(0);
}
