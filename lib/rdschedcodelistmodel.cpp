// rdschedcodelistmodel.cpp
//
// Data model for Rivendell schedule codes
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
#include "rdschedcodelistmodel.h"

RDSchedCodeListModel::RDSchedCodeListModel(bool incl_none,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_include_none=incl_none;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Code"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  updateModel();
}


RDSchedCodeListModel::~RDSchedCodeListModel()
{
}


QPalette RDSchedCodeListModel::palette()
{
  return d_palette;
}


void RDSchedCodeListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDSchedCodeListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDSchedCodeListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDSchedCodeListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDSchedCodeListModel::headerData(int section,Qt::Orientation orient,
					  int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDSchedCodeListModel::data(const QModelIndex &index,int role) const
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


QString RDSchedCodeListModel::schedCode(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex RDSchedCodeListModel::addSchedCode(const QString &scode)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(scode.toLower()<d_texts.at(i).at(0).toString().toLower()) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=scode;
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDSchedCodeListModel::removeSchedCode(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDSchedCodeListModel::removeSchedCode(const QString &scode)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==scode) {
      removeSchedCode(createIndex(i,0));
      return;
    }
  }
}


void RDSchedCodeListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where `CODE`='"+
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


void RDSchedCodeListModel::refresh(const QString &scode)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==scode) {
      updateRowLine(i);
      return;
    }
  }
}


void RDSchedCodeListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields();
  sql+="order by `CODE` ";
  beginResetModel();
  d_texts.clear();

  if(d_include_none) {
    d_texts.push_back(texts);
    d_texts.back().push_back(tr("[none]"));
    d_texts.back().push_back(QString());
  }

  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDSchedCodeListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where `CODE`='"+RDEscapeString(d_texts.at(line).at(line).toString())+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDSchedCodeListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Code
  texts.push_back(q->value(0));

  // Description
  texts.push_back(q->value(1));

  d_texts[row]=texts;
}


QString RDSchedCodeListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`CODE`,"+         // 00
    "`DESCRIPTION` "+  // 01
    "from `SCHED_CODES` ";

    return sql;
}
