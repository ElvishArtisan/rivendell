// rdmatrixlistmodel.cpp
//
// Data model for Rivendell matrix devices
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
#include "rdmatrixlistmodel.h"

RDMatrixListModel::RDMatrixListModel(const QString &hostname,bool incl_none,
				     QObject *parent)
  : QAbstractTableModel(parent)
{
  d_station_name=hostname;
  d_include_none=incl_none;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Matrix"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Type"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Inputs"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Outputs"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("GPIs"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("GPOs"));
  d_alignments.push_back(right);

  updateModel();
}


RDMatrixListModel::~RDMatrixListModel()
{
}


QPalette RDMatrixListModel::palette()
{
  return d_palette;
}


void RDMatrixListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDMatrixListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDMatrixListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDMatrixListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDMatrixListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDMatrixListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      if(col==0) {
	return rda->iconEngine()->listIcon(RDIconEngine::Switcher);
      }
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


int RDMatrixListModel::matrixId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


int RDMatrixListModel::matrixNumber(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(1).toInt();
}


QModelIndex RDMatrixListModel::addMatrix(RDMatrix *mtx)
{
  //
  // Find the insertion offset
  //
  QString name=mtx->name();
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(name.toLower()<d_texts.at(i).at(0).toString().toLower()) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_ids.insert(offset,mtx->id());
  d_texts.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDMatrixListModel::removeMatrix(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDMatrixListModel::removeMatrix(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      removeMatrix(createIndex(i,0));
      return;
    }
  }
}


void RDMatrixListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("`MATRICES`.`ID`=%d ",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDMatrixListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDMatrixListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "`STATION_NAME`='"+RDEscapeString(d_station_name)+"' "+
    "order by `MATRICES`.`NAME` ";
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
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(-1);
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDMatrixListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("`MATRICES`.`ID`=%d ",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDMatrixListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  d_ids.back()=q->value(0).toInt();

  // Description
  texts.push_back(q->value(1));

  // Matrix Number
  texts.push_back(QString().sprintf("%d",q->value(2).toInt()));

  // Type
  texts.push_back(RDMatrix::typeString((RDMatrix::Type)q->value(3).toUInt()));

  // Inputs
  texts.push_back(QString().sprintf("%d",q->value(4).toInt()));

  // Outputs
  texts.push_back(QString().sprintf("%d",q->value(5).toInt()));

  // GPIs
  texts.push_back(QString().sprintf("%d",q->value(6).toInt()));

  // GPOs
  texts.push_back(QString().sprintf("%d",q->value(7).toInt()));

  d_texts[row]=texts;
}


QString RDMatrixListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`MATRICES`.`ID`,"+       // 00
    "`MATRICES`.`NAME`,"+     // 01
    "`MATRICES`.`MATRIX`,"+   // 02
    "`MATRICES`.`TYPE`,"+     // 03
    "`MATRICES`.`INPUTS`,"+   // 04
    "`MATRICES`.`OUTPUTS`,"+  // 05
    "`MATRICES`.`GPIS`,"+     // 06
    "`MATRICES`.`GPOS` "+     // 07
    "from `MATRICES` ";

    return sql;
}
