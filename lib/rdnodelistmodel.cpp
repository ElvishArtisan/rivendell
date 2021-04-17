// rdnodelistmodel.cpp
//
// Data model for Rivendell PyPAD instances
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
#include "rdnodelistmodel.h"

RDNodeListModel::RDNodeListModel(RDMatrix *mtx,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_mtx=mtx;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Hostname"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("First Output"));
  d_alignments.push_back(right);

  updateModel();
}


RDNodeListModel::~RDNodeListModel()
{
}


QPalette RDNodeListModel::palette()
{
  return d_palette;
}


void RDNodeListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDNodeListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDNodeListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDNodeListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDNodeListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDNodeListModel::data(const QModelIndex &index,int role) const
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


int RDNodeListModel::nodeId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


QModelIndex RDNodeListModel::addNode(int id)
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


void RDNodeListModel::removeNode(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDNodeListModel::removeNode(int id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      removeNode(createIndex(i,0));
      return;
    }
  }
}


void RDNodeListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("`ID`=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDNodeListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDNodeListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where `STATION_NAME`='"+RDEscapeString(d_mtx->station())+"' && "+
    QString().sprintf("`MATRIX`=%d ",d_mtx->matrix())+
    "order by `DESCRIPTION` ";
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


void RDNodeListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("`ID`=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDNodeListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Description
  texts.push_back(q->value(1));

  // Hostname
  texts.push_back(q->value(2));

  // Base Output
  texts.push_back(QString().sprintf("%d",q->value(3).toInt()));

  d_ids[row]=q->value(0).toInt();
  d_texts[row]=texts;
}


QString RDNodeListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`ID`,"            // 00
    "`DESCRIPTION`,"+  // 01
    "`HOSTNAME`,"+     // 02
    "`BASE_OUTPUT` "   // 03
    "from `SWITCHER_NODES` ";

    return sql;
}
