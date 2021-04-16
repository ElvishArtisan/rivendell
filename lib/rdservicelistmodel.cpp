// rdservicelistmodel.cpp
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
#include "rdservicelistmodel.h"

RDServiceListModel::RDServiceListModel(bool incl_none,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_include_none=incl_none;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Pgm Code"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Track Group"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Log Shelf Life"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("ELR Shelf Life"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Auto Refresh"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Chain Log"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Import Markers"));
  d_alignments.push_back(center);

  updateModel();
}


RDServiceListModel::~RDServiceListModel()
{
}


QPalette RDServiceListModel::palette()
{
  return d_palette;
}


void RDServiceListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDServiceListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDServiceListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDServiceListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDServiceListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDServiceListModel::data(const QModelIndex &index,int role) const
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


QString RDServiceListModel::serviceName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex RDServiceListModel::addService(const QString &svcname)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(svcname.toLower()<d_texts.at(i).at(0).toString().toLower()) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=svcname;
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDServiceListModel::removeService(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDServiceListModel::removeService(const QString &svcname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==svcname) {
      removeService(createIndex(i,0));
      return;
    }
  }
}


void RDServiceListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where SERVICES.NAME=\""+
      RDEscapeString(d_texts.at(row.row()).at(0).toString())+
      "\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDServiceListModel::refresh(const QString &grpname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==grpname) {
      updateRowLine(i);
      return;
    }
  }
}


void RDServiceListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields();
  sql+="order by NAME ";
  beginResetModel();
  d_texts.clear();
  if(d_include_none) {
    d_texts.push_back(texts);
    d_texts.back().push_back(tr("[none]"));
    for(int i=1;i<columnCount();i++) {
      d_texts.back().push_back(QVariant());
    }
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDServiceListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where NAME=\""+RDEscapeString(d_texts.at(line).at(line).toString())+"\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDServiceListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Login Name
  texts.push_back(q->value(0));

  // Description
  texts.push_back(q->value(1));

  // Program Code
  texts.push_back(q->value(2));

  // Track Group
  texts.push_back(q->value(3));

  // Log Shelf Life
  texts.push_back(q->value(4));

  // ELR Shelf Life
  texts.push_back(q->value(5));

  // Auto Refresh
  texts.push_back(q->value(6));

  // Chain Log
  texts.push_back(q->value(7));

  // Import Markers
  texts.push_back(q->value(8));

  d_texts[row]=texts;
}


QString RDServiceListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "SERVICES.NAME,"+                    // 00
    "SERVICES.DESCRIPTION,"+             // 01
    "SERVICES.PROGRAM_CODE,"+            // 02
    "SERVICES.TRACK_GROUP,"+             // 03
    "SERVICES.DEFAULT_LOG_SHELFLIFE,"+   // 04
    "SERVICES.ELR_SHELFLIFE,"+           // 05
    "SERVICES.AUTO_REFRESH,"+            // 06
    "SERVICES.CHAIN_LOG,"+               // 07
    "SERVICES.INCLUDE_IMPORT_MARKERS "+  // 08
    "from SERVICES ";

    return sql;
}
