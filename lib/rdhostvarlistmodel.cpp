// rdhostvarlistmodel.cpp
//
// Data model for Rivendell host variables
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
#include "rdreplicator.h"
#include "rdhostvarlistmodel.h"

RDHostvarListModel::RDHostvarListModel(const QString &hostname,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_station_name=hostname;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Value"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Remark"));
  d_alignments.push_back(left);

  updateModel();
}


RDHostvarListModel::~RDHostvarListModel()
{
}


QPalette RDHostvarListModel::palette()
{
  return d_palette;
}


void RDHostvarListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDHostvarListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDHostvarListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDHostvarListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDHostvarListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDHostvarListModel::data(const QModelIndex &index,int role) const
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


int RDHostvarListModel::varId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


QModelIndex RDHostvarListModel::addVar(int id)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
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
  d_ids.insert(offset,id);
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDHostvarListModel::removeVar(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDHostvarListModel::removeVar(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      removeVar(createIndex(i,0));
      return;
    }
  }
}


void RDHostvarListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where `ID`=%d",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDHostvarListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDHostvarListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    " where `STATION_NAME`='"+RDEscapeString(d_station_name)+"' ";
  sql+="order by `NAME` ";
  beginResetModel();
  d_ids.clear();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(-1);
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDHostvarListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where `ID`=%d ",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDHostvarListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Name
  texts.push_back(q->value(1));

  // Value
  texts.push_back(q->value(2));

  // Remark
  texts.push_back(q->value(3));

  d_texts[row]=texts;
  d_ids[row]=q->value(0).toInt();
}


QString RDHostvarListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`ID`,"+        // 00
    "`NAME`,"+      // 01
    "`VARVALUE`,"+  // 02
    "`REMARK` "+    // 03
    "from `HOSTVARS` ";

  return sql;
}
