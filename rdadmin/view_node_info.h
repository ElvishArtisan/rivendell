// view_node_info.h
//
// Edit a Rivendell LiveWire Node
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: view_node_info.h,v 1.3 2010/07/29 19:32:35 cvs Exp $
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

#ifndef VIEW_NODE_INFO_H
#define VIEW_NODE_INFO_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qdialog.h>

#include <rdlivewire.h>
#include <rdlistview.h>
#include <rdlistviewitem.h>


class ViewNodeInfo : public QDialog
{
 Q_OBJECT
 public:
  ViewNodeInfo(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void exec(const QString &hostname,Q_UINT16 port,const QString &passwd,
	    unsigned base_output);

 private slots:
  void connectedData(unsigned id);
  void sourceChangedData(unsigned id,RDLiveWireSource *src);
  void destinationChangedData(unsigned id,RDLiveWireDestination *dst);
  void closeData();

 private:
  void WriteSourceItem(RDLiveWireSource *src,RDListViewItem *item);
  void WriteDestinationItem(RDLiveWireDestination *dst,RDListViewItem *item);
  RDLiveWire *view_livewire;
  QLineEdit *view_hostname_edit;
  QLineEdit *view_tcpport_edit;
  QLineEdit *view_protocol_edit;
  QLineEdit *view_system_edit;
  QLineEdit *view_type_edit;
  QLineEdit *view_sources_edit;
  QLineEdit *view_destinations_edit;
  QLineEdit *view_channels_edit;
  QLineEdit *view_gpis_edit;
  QLineEdit *view_gpos_edit;
  RDListView *view_sources_view;
  RDListView *view_destinations_view;
  unsigned view_base_output;
};


#endif  // VIEW_NODE_INFO

