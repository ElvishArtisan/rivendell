// view_node_info.cpp
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: view_node_info.cpp,v 1.3.8.1 2013/01/30 21:06:04 cvs Exp $
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
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include <view_node_info.h>


ViewNodeInfo::ViewNodeInfo(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  setCaption(tr("Viewing LiveWire Node"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

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
  view_livewire=new RDLiveWire(0,this,"view_livewire");
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
  view_hostname_edit=new QLineEdit(this,"view_hostname_edit");
  view_hostname_edit->setGeometry(90,10,190,20);
  view_hostname_edit->setReadOnly(true);
  QLabel *label=
    new QLabel(view_hostname_edit,tr("Hostname:"),this,"view_hostname_label");
  label->setGeometry(10,10,80,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Node TCP Port
  //
  view_tcpport_edit=new QLineEdit(this,"view_tcpport_edit");
  view_tcpport_edit->setGeometry(335,10,sizeHint().width()-345,20);
  view_tcpport_edit->setReadOnly(true);
  label=new QLabel(view_tcpport_edit,tr("Port:"),this,"view_tcpport_label");
  label->setGeometry(305,10,30,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // System Version
  //
  view_system_edit=new QLineEdit(this,"view_system_edit");
  view_system_edit->setGeometry(135,32,70,20);
  view_system_edit->setReadOnly(true);
  label=new QLabel(view_system_edit,tr("System Version:"),
		   this,"view_system_label");
  label->setGeometry(10,32,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Protocol Version
  //
  view_protocol_edit=new QLineEdit(this,"view_protocol_edit");
  view_protocol_edit->setGeometry(335,32,40,20);
  view_protocol_edit->setReadOnly(true);
  label=new QLabel(view_protocol_edit,tr("Protocol Version:"),
		   this,"view_protocol_label");
  label->setGeometry(210,32,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Sources
  //
  view_sources_edit=new QLineEdit(this,"view_sources_edit");
  view_sources_edit->setGeometry(75,54,30,20);
  view_sources_edit->setReadOnly(true);
  label=new QLabel(view_sources_edit,tr("Sources:"),
		   this,"view_sources_label");
  label->setGeometry(10,54,60,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Destinations
  //
  view_destinations_edit=new QLineEdit(this,"view_destinations_edit");
  view_destinations_edit->setGeometry(225,54,30,20);
  view_destinations_edit->setReadOnly(true);
  label=new QLabel(view_destinations_edit,tr("Destinations:"),
		   this,"view_destinations_label");
  label->setGeometry(120,54,100,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Channels
  //
  view_channels_edit=new QLineEdit(this,"view_channels_edit");
  view_channels_edit->setGeometry(360,54,30,20);
  view_channels_edit->setReadOnly(true);
  label=new QLabel(view_channels_edit,tr("Channels:"),
		   this,"view_channels_label");
  label->setGeometry(260,54,95,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // GPIs
  //
  view_gpis_edit=new QLineEdit(this,"view_gpis_edit");
  view_gpis_edit->setGeometry(135,76,70,20);
  view_gpis_edit->setReadOnly(true);
  label=new QLabel(view_gpis_edit,tr("GPIs:"),
		   this,"view_gpis_label");
  label->setGeometry(10,76,120,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // GPOs
  //
  view_gpos_edit=new QLineEdit(this,"view_gpos_edit");
  view_gpos_edit->setGeometry(305,76,70,20);
  view_gpos_edit->setReadOnly(true);
  label=new QLabel(view_gpos_edit,tr("GPOs:"),
		   this,"view_gpos_label");
  label->setGeometry(210,76,90,20);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter);

  //
  // Sources List
  //
  label=new QLabel(view_gpos_edit,tr("Sources"),
		   this,"view_sources_label");
  label->setGeometry(15,98,90,20);
  label->setFont(bold_font);
  label->setAlignment(AlignLeft|AlignVCenter);
  view_sources_view=new RDListView(this,"view_sources_view");
  view_sources_view->
    setGeometry(10,118,sizeHint().width()-20,200);
  view_sources_view->setAllColumnsShowFocus(true);
  view_sources_view->setItemMargin(5);
  view_sources_view->addColumn(tr("#"));
  view_sources_view->setColumnAlignment(0,Qt::AlignCenter);
  view_sources_view->addColumn(tr("CHAN"));
  view_sources_view->setColumnAlignment(1,Qt::AlignCenter);
  view_sources_view->addColumn(tr("NAME"));
  view_sources_view->setColumnAlignment(2,Qt::AlignLeft);
  view_sources_view->addColumn(tr("STREAMING"));
  view_sources_view->setColumnAlignment(3,Qt::AlignCenter);
  view_sources_view->addColumn(tr("SHAREABLE"));
  view_sources_view->setColumnAlignment(4,Qt::AlignCenter);
  view_sources_view->addColumn(tr("CHANS"));
  view_sources_view->setColumnAlignment(5,Qt::AlignCenter);
  view_sources_view->addColumn(tr("GAIN"));
  view_sources_view->setColumnAlignment(6,Qt::AlignCenter);

  //
  // Destinations List
  //
  label=new QLabel(view_gpos_edit,tr("Destinations"),
		   this,"view_destinations_label");
  label->setGeometry(15,325,90,20);
  label->setFont(bold_font);
  label->setAlignment(AlignLeft|AlignVCenter);
  view_destinations_view=new RDListView(this,"view_destinations_view");
  view_destinations_view->
    setGeometry(10,345,sizeHint().width()-20,200);
  view_destinations_view->setAllColumnsShowFocus(true);
  view_destinations_view->setItemMargin(5);
  view_destinations_view->addColumn(tr("#"));
  view_destinations_view->setColumnAlignment(0,Qt::AlignCenter);
  view_destinations_view->addColumn(tr("CHAN"));
  view_destinations_view->setColumnAlignment(1,Qt::AlignCenter);
  view_destinations_view->addColumn(tr("NAME"));
  view_destinations_view->setColumnAlignment(2,Qt::AlignLeft);
  view_destinations_view->addColumn(tr("CHANS"));
  view_destinations_view->setColumnAlignment(3,Qt::AlignCenter);
  view_destinations_view->addColumn(tr("LOAD"));
  view_destinations_view->setColumnAlignment(4,Qt::AlignCenter);
  view_destinations_view->addColumn(tr("GAIN"));
  view_destinations_view->setColumnAlignment(5,Qt::AlignCenter);

  //
  //  Close Button
  //
  QPushButton *button=new QPushButton(this,"close_button");
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


void ViewNodeInfo::sourceChangedData(unsigned id,RDLiveWireSource *src)
{
  RDListViewItem *item=(RDListViewItem *)view_sources_view->firstChild();
  while(item!=NULL) {
    if(item->text(0).toInt()==src->slotNumber()) {
      WriteSourceItem(src,item);
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  item=new RDListViewItem(view_sources_view);
  item->setText(0,QString().sprintf("%d",src->slotNumber()));
  WriteSourceItem(src,item);
}


void ViewNodeInfo::destinationChangedData(unsigned id,
					  RDLiveWireDestination *dst)
{
  RDListViewItem *item=(RDListViewItem *)view_destinations_view->firstChild();
  while(item!=NULL) {
    if(item->text(0).toInt()==dst->slotNumber()) {
      WriteDestinationItem(dst,item);
      return;
    }
    item=(RDListViewItem *)item->nextSibling();
  }
  item=new RDListViewItem(view_destinations_view);
  item->setText(0,QString().sprintf("%d",dst->slotNumber()));
  WriteDestinationItem(dst,item);
}


void ViewNodeInfo::closeData()
{
  done(0);
}


void ViewNodeInfo::WriteSourceItem(RDLiveWireSource *src,RDListViewItem *item)
{
  item->setText(1,QString().sprintf("%05d",src->channelNumber()));
  item->setText(2,src->primaryName());
  if(src->rtpEnabled()) {
    item->setText(3,tr("Yes"));
  }
  else {
    item->setText(3,tr("No"));
  }
  if(src->shareable()) {
    item->setText(4,tr("Yes"));
  }
  else {
    item->setText(4,tr("No"));
  }
  item->setText(5,QString().sprintf("%d",src->channels()));
  item->setText(6,QString().sprintf("%4.1f",(float)src->inputGain()/10.0));
}


void ViewNodeInfo::WriteDestinationItem(RDLiveWireDestination *dst,
					RDListViewItem *item)
{
  item->
    setText(1,QString().sprintf("%05u",view_base_output+dst->slotNumber()-1));
  item->setText(2,dst->primaryName());
  item->setText(3,QString().sprintf("%d",dst->channels()));
  item->setText(4,RDLiveWireDestination::loadString(dst->load()));
  item->setText(5,QString().sprintf("%4.1f",(float)dst->outputGain()/10.0));
}
