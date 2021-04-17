// rdreplicatorlistmodel.cpp
//
// Data model for Rivendell services
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
#include "rdreplicatorlistmodel.h"

RDReplicatorListModel::RDReplicatorListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Type"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Host"));
  d_alignments.push_back(left);

  updateModel();
}


RDReplicatorListModel::~RDReplicatorListModel()
{
}


QPalette RDReplicatorListModel::palette()
{
  return d_palette;
}


void RDReplicatorListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDReplicatorListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDReplicatorListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDReplicatorListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDReplicatorListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDReplicatorListModel::data(const QModelIndex &index,int role) const
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


QString RDReplicatorListModel::replicatorName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex RDReplicatorListModel::addReplicator(const QString &replname)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(replname.toLower()<d_texts.at(i).at(0).toString().toLower()) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=replname;
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDReplicatorListModel::removeReplicator(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDReplicatorListModel::removeReplicator(const QString &replname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==replname) {
      removeReplicator(createIndex(i,0));
      return;
    }
  }
}


void RDReplicatorListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where `REPLICATORS`.`NAME`='"+
      RDEscapeString(d_texts.at(row.row()).at(0).toString())+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDReplicatorListModel::refresh(const QString &replname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==replname) {
      updateRowLine(i);
      return;
    }
  }
}


void RDReplicatorListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields();
  sql+="order by `REPLICATORS`.`NAME` ";
  beginResetModel();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDReplicatorListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where `REPLICATORS`.`NAME`='"+
      RDEscapeString(d_texts.at(line).at(line).toString())+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDReplicatorListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Name
  texts.push_back(q->value(0));

  // Type
  texts.push_back(RDReplicator::typeString((RDReplicator::Type)q->value(1).
					   toUInt()));

  // Description
  texts.push_back(q->value(2));

  // Host
  texts.push_back(q->value(3));

  d_texts[row]=texts;
}


QString RDReplicatorListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`REPLICATORS`.`NAME`,"+          // 00
    "`REPLICATORS`.`TYPE_ID`,"+       // 01
    "`REPLICATORS`.`DESCRIPTION`,"+   // 02
    "`REPLICATORS`.`STATION_NAME` "+  // 03
    "from `REPLICATORS` ";

    return sql;
}
