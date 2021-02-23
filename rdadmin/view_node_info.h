// view_node_info.h
//
// Edit a Rivendell Livewire Node
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

#ifndef VIEW_NODE_INFO_H
#define VIEW_NODE_INFO_H

#include <QLineEdit>

#include <rddialog.h>
#include <rdnodeslotsmodel.h>
#include <rdtableview.h>

class ViewNodeInfo : public RDDialog
{
 Q_OBJECT
 public:
  ViewNodeInfo(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void exec(const QString &hostname,uint16_t port,const QString &passwd,
	    unsigned base_output);

 private slots:
  void connectedData(unsigned id);
  void sourceChangedData(const QModelIndex &first,const QModelIndex &last);
  void destinationChangedData(const QModelIndex &first,const QModelIndex &last);
  void closeData();

 private:
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
  RDTableView *view_sources_view;
  RDNodeSlotsModel *view_sources_model;
  RDTableView *view_destinations_view;
  RDNodeSlotsModel *view_destinations_model;
};


#endif  // VIEW_NODE_INFO

