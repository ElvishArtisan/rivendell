// rdnodeslotsmodel.h
//
// Data model for Rivendell switcher nodes.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDNODESLOTSMODEL_H
#define RDNODESLOTSMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>
#include <QTimer>

#include <rddb.h>
#include <rdlivewire.h>
#include <rdmatrix.h>
#include <rdnotification.h>
#include <rduser.h>

class RDNodeSlotsModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDNodeSlotsModel(bool is_src,QObject *parent=0);
  ~RDNodeSlotsModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void setBaseOutput(int output);
  void setSlotQuantity(int slot_quan);

 public slots:
  void updateSource(unsigned id,RDLiveWireSource *src);
  void updateDestination(unsigned id,RDLiveWireDestination *dst);

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<int> d_ids;
  bool d_is_source;
  int d_base_output;
};


#endif  // RDNODESLOTSMODEL_H
