// rdgpioslotsmodel.cpp
//
// Data model for Livewire multicast GPIO slots
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

#include "rdapplication.h"
#include "rdescape_string.h"
#include "rdgpioslotsmodel.h"

RDGpioSlotsModel::RDGpioSlotsModel(RDMatrix *mtx,int slot_quan,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_mtx=mtx;
  d_slot_quantity=slot_quan;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Lines"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Source #"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Surface Address"));
  d_alignments.push_back(left);


  updateModel();
}


RDGpioSlotsModel::~RDGpioSlotsModel()
{
}


QPalette RDGpioSlotsModel::palette()
{
  return d_palette;
}


void RDGpioSlotsModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDGpioSlotsModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDGpioSlotsModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDGpioSlotsModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDGpioSlotsModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDGpioSlotsModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      // Nothing to do!
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      if(col==0) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      // Nothing to do!
      break;

    case Qt::BackgroundRole:
      // Nothing to do!
      break;

    default:
      break;
    }
  }

  return QVariant();
}


int RDGpioSlotsModel::slotId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


QModelIndex RDGpioSlotsModel::addSlot(int id)
{
  //
  // Find the insertion offset
  //
  int offset=d_ids.size();
  for(int i=0;i<d_ids.size();i++) {
    if(id<d_ids.at(i)) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_texts.insert(offset,list);
  d_ids.insert(offset,id);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDGpioSlotsModel::removeSlot(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDGpioSlotsModel::removeSlot(int id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      removeSlot(createIndex(i,0));
      return;
    }
  }
}


void RDGpioSlotsModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      QString().sprintf("where ID=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDGpioSlotsModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDGpioSlotsModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "STATION_NAME=\""+RDEscapeString(d_mtx->station())+"\" && "+
    QString().sprintf("MATRIX=%d && ",d_mtx->matrix())+
    QString().sprintf("SLOT<%d ",d_slot_quantity/5)+
    "order by SLOT ";
  beginResetModel();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(0);
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDGpioSlotsModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      QString().sprintf("where ID=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDGpioSlotsModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Lines
  texts.push_back(QString().sprintf("%d - %d",5*row+1,5*row+5));

  // Source Number
  if(q->value(2).toInt()>0) {
    texts.push_back(QString().sprintf("%05d",q->value(2).toInt()));
  }
  else {
    texts.push_back(tr("[none]"));
  }

  // Surface Address
  if(q->value(3).toString().isEmpty()) {
    texts.push_back(tr("[all]"));
  }
  else {
    texts.push_back(q->value(3));
  }

  d_ids[row]=q->value(0).toInt();
  d_texts[row]=texts;
}


QString RDGpioSlotsModel::sqlFields() const
{
  QString sql=QString("select ")+
    "ID,"             // 00
    "SLOT,"+          // 01
    "SOURCE_NUMBER,"  // 02
    "IP_ADDRESS "+    // 03
    "from LIVEWIRE_GPIO_SLOTS ";

    return sql;
}
