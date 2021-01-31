// rdvguestresourcelistmodel.cpp
//
// Data model for Rivendell Logitek vGuest resource settings
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
#include "rdmatrix.h"
#include "rdvguestresourcelistmodel.h"

RDVguestResourceListModel::RDVguestResourceListModel(RDMatrix *mtx,
						     RDMatrix::VguestType type,
						     QObject *parent)
  : QAbstractTableModel(parent)
{
  d_mtx=mtx;
  d_vguest_type=type;

  //
  // Column Attributes
  //
  //  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  switch(d_vguest_type) {
  case RDMatrix::VguestTypeRelay:
    d_headers.push_back(tr("GPIO Line"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Engine"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Device (Hex)"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Surface"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Bus/Relay"));
    d_alignments.push_back(center);
    break;
      
    case RDMatrix::VguestTypeDisplay:
    d_headers.push_back(tr("Display"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Engine"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Device (Hex)"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Surface"));
    d_alignments.push_back(center);
    break;
  }

  updateModel();
}


RDVguestResourceListModel::~RDVguestResourceListModel()
{
}


QPalette RDVguestResourceListModel::palette()
{
  return d_palette;
}


void RDVguestResourceListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDVguestResourceListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDVguestResourceListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDVguestResourceListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDVguestResourceListModel::headerData(int section,
					       Qt::Orientation orient,
					       int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDVguestResourceListModel::data(const QModelIndex &index,int role) const
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


int RDVguestResourceListModel::resourceId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


int RDVguestResourceListModel::resourceNumber(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toInt();
}


void RDVguestResourceListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("ID=%d ",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDVguestResourceListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDVguestResourceListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "STATION_NAME=\""+RDEscapeString(d_mtx->station())+"\" && "+
    QString().sprintf("MATRIX_NUM=%d && ",d_mtx->matrix())+
    QString().sprintf("VGUEST_TYPE=%u ",d_vguest_type)+
    "order by NUMBER ";
  beginResetModel();
  d_ids.clear();
  d_texts.clear();
  if(d_include_none) {
    d_ids.push_back(-1);
    d_texts.push_back(texts);
    d_texts.back().push_back(tr("[none]"));
    for(int i=1;i<columnCount();i++) {
      d_texts.back().push_back(QVariant());
    }
  }
  //  printf("SQL: %s\n",sql.toUtf8().constData());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(-1);
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDVguestResourceListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("ID=%d ",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDVguestResourceListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  d_ids.back()=q->value(0).toInt();

  //
  // GPIO Line / Display
  texts.push_back(QString().sprintf("%d",q->value(1).toInt()));

  // Engine Number
  if(q->value(2).toInt()>=0) {
    texts.push_back(QString().sprintf("%d",q->value(2).toInt()));
  }
  else {
    texts.push_back("");
  }

  // Device Number
  if(q->value(3).toInt()>=0) {
    texts.push_back(QString().sprintf("%04X",q->value(3).toInt()));
  }
  else {
    texts.push_back("");
  }

  // Surface Number
  if(q->value(4).toInt()>=0) {
    texts.push_back(QString().sprintf("%d",q->value(4).toInt()));
  }
  else {
    texts.push_back("");
  }

  switch(d_vguest_type) {
  case RDMatrix::VguestTypeRelay:
    if(q->value(5).toInt()>=0) {
      texts.push_back(QString().sprintf("%02d",q->value(5).toInt()));
    }
    else {
      texts.push_back("");
    }
    break;

  case RDMatrix::VguestTypeDisplay:
    break;
  }

  d_texts[row]=texts;
}


QString RDVguestResourceListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "ID,"+           // 00
    "NUMBER,"+       // 01
    "ENGINE_NUM,"+   // 02
    "DEVICE_NUM,"+   // 03
    "SURFACE_NUM,"+  // 04
    "RELAY_NUM,"+    // 05
    "BUSS_NUM "+     // 06
    "from VGUEST_RESOURCES ";

  return sql;
}
