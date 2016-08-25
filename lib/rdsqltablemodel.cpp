// rdsqltablemodel.cpp
//
// Two dimensional, SQL-based data model for Rivendell.
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdconf.h"
#include "rddb.h"
#include "rdsqltablemodel.h"

RDSqlTableModel::RDSqlTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  model_columns=0;
}


RDSqlTableModel::~RDSqlTableModel()
{
}


int RDSqlTableModel::columnCount(const QModelIndex &index) const
{
  return model_columns;
}


int RDSqlTableModel::rowCount(const QModelIndex &index) const
{
  return model_display_datas.size();
}


QVariant RDSqlTableModel::data(const QModelIndex &index,int role) const
{
  QVariant value;

  switch(role) {
  case Qt::DisplayRole:
    value=model_display_datas[index.row()][index.column()];
    switch(fieldType(index.column())) {
    case RDSqlTableModel::CartNumberType:
      return QVariant(QString().sprintf("%06u",value.toUInt()));

    case RDSqlTableModel::LengthType:
      return QVariant(RDGetTimeLength(value.toInt(),false,true));

    case RDSqlTableModel::DefaultType:
      return value;
    }
    break;

  default:
    break;
  }
  return QVariant();
}


QVariant RDSqlTableModel::data(int row,int column,int role) const
{
  return data(index(row,column),role);
}


void RDSqlTableModel::setQuery(const QString &sql)
{
  model_sql=sql;
  model_display_datas.clear();

  RDSqlQuery *q=new RDSqlQuery(sql);
  model_columns=q->columns();
  while(q->next()) {
    std::vector<QVariant> row;
    for(int i=0;i<q->columns();i++) {
      row.push_back(q->value(i));
    }
    model_display_datas.push_back(row);
  }
  delete q;
  emit layoutChanged();
}


QVariant RDSqlTableModel::headerData(int section,Qt::Orientation orient,
					int role) const
{
  if((role==Qt::DisplayRole)&&(orient==Qt::Horizontal)) {
    if(model_headers.at(section).isValid()) {
      return model_headers.at(section);
    }
    return QVariant(QString().sprintf("%d",section));
  }
  if((role==Qt::SizeHintRole)&&(orient==Qt::Vertical)) {
    return QVariant(QSize());
  }
  return QAbstractItemModel::headerData(section,orient,role);
}


bool RDSqlTableModel::setHeaderData(int section,Qt::Orientation orient,
				       const QVariant &value,int role)
{
  if((role==Qt::DisplayRole)&&(orient==Qt::Horizontal)) {
    model_headers[section]=value;
    emit headerDataChanged(orient,section,section);
  }
  return QAbstractItemModel::setHeaderData(section,orient,value,role);
}


RDSqlTableModel::FieldType RDSqlTableModel::fieldType(int section) const
{
  try {
    return model_field_types.at(section);
  }
  catch (...) {
    return RDSqlTableModel::DefaultType;
  }
}


void RDSqlTableModel::setFieldType(int section,RDSqlTableModel::FieldType type)
{
  model_field_types[section]=type;
}


void RDSqlTableModel::update()
{
  if(!model_sql.isEmpty()) {
    setQuery(model_sql);
  }
}


bool RDSqlTableModel::insertRows(int row,const QString &sql)
{
  if((row<0)||(row>(int)model_display_datas.size())) {
    return false;
  }
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->size()>0) {
    beginInsertRows(QModelIndex(),row,row+q->size()-1);
    while(q->next()) {
      std::vector<QVariant> row;
      for(int i=0;i<model_columns;i++) {
	row.push_back(q->value(i));
      }
      model_display_datas.push_back(row);
    }
    endInsertRows();
  }
  delete q;
  return true;
}


bool RDSqlTableModel::removeRows(int row,int count,const QModelIndex &parent)
{
  if((row+count)>(int)model_display_datas.size()) {
    return false;
  }
  beginRemoveRows(QModelIndex(),row,row+count-1);
  for(int i=0;i<count;i++) {
    model_display_datas.erase(model_display_datas.begin()+row);
  }
  endRemoveRows();
  return true;
}
