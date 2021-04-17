// rdgpiolistmodel.cpp
//
// Data model for Rivendell GPI/GPO configuration
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
#include "rdgpiolistmodel.h"

RDGpioListModel::RDGpioListModel(RDMatrix *mtx,RDMatrix::GpioType type,
				 QObject *parent)
  : QAbstractTableModel(parent)
{
  d_mtx=mtx;
  d_type=type;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  if(type==RDMatrix::GpioInput) {
    d_table="`GPIS`";

    d_headers.push_back(tr("Gpi"));
    d_alignments.push_back(right);
  }
  else {
    d_table="`GPOS`";

    d_headers.push_back(tr("Gpo"));
    d_alignments.push_back(right);
  }

  d_headers.push_back(tr("ON Macro Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("ON Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("OFF Macro Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("OFF Description"));
  d_alignments.push_back(left);

  updateModel();
}


RDGpioListModel::~RDGpioListModel()
{
}


QPalette RDGpioListModel::palette()
{
  return d_palette;
}


void RDGpioListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDGpioListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDGpioListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDGpioListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDGpioListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDGpioListModel::data(const QModelIndex &index,int role) const
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


int RDGpioListModel::lineId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


void RDGpioListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields(true)+
      QString().sprintf("where ID=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(true,row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;

    sql=sqlFields(false)+
      "where "+d_table+QString().sprintf(".ID=%u",d_ids.at(row.row()));
    q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(false,row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDGpioListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDGpioListModel::updateModel()
{
  QList<QVariant> texts; 

  beginResetModel();

  //
  // Set up the canvas
  //
  for(int i=0;i<columnCount();i++) {
    texts.push_back(QVariant());
  }
  d_texts.clear();
  d_ids.clear();
  int size=d_mtx->gpis();
  if(d_type==RDMatrix::GpioOutput) {
    size=d_mtx->gpos();
  }
  for(int i=0;i<size;i++) {
    d_texts.push_back(texts);
    d_ids.push_back(-1);
  }

  //
  // The ON Values
  //
  RDSqlQuery *q=NULL;
  QString sql=sqlFields(true)+
    "where "+
    d_table+".`STATION_NAME`='"+RDEscapeString(d_mtx->station())+"' && "+
    d_table+QString().sprintf(".`MATRIX`=%d ",d_mtx->matrix())+
    "order by "+d_table+".`NUMBER` ";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    updateRow(true,q->value(1).toInt()-1,q);
  }
  delete q;

  //
  // The OFF Values
  //
  sql=sqlFields(false)+
    "where "+
    d_table+".`STATION_NAME`='"+RDEscapeString(d_mtx->station())+"' && "+
    d_table+QString().sprintf(".`MATRIX`=%d ",d_mtx->matrix())+
    "order by "+d_table+".`NUMBER` ";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    updateRow(false,q->value(1).toInt()-1,q);
  }
  delete q;

  endResetModel();
}


void RDGpioListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    //
    // The ON Values
    //
    QString sql=sqlFields(true)+
      QString().sprintf("where `ID`=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(true,line,q);
    }
    delete q;

    //
    // The OFF Values
    //
    sql=sqlFields(false)+
      QString().sprintf("where `ID`=%u",d_ids.at(line));
    q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(false,line,q);
    }
    delete q;
  }
}


void RDGpioListModel::updateRow(bool on_values,int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  if(on_values) {
    // GPIO Line
    d_texts[row][0]=QString().sprintf("%d",q->value(1).toInt());

    // ON Macro Cart
    if(q->value(2).toUInt()==0) {
      d_texts[row][1]="";
    }
    else {
      d_texts[row][1]=QString().sprintf("%06u",q->value(2).toUInt());
    }

    // ON Description
    if(q->value(2).toUInt()==0) {
      d_texts[row][2]=tr("[unassigned]");
    }
    else {
      d_texts[row][2]=q->value(3);
    }

    d_ids[row]=q->value(0).toInt();
  }
  else {
    // OFF Macro Cart
    if(q->value(2).toUInt()==0) {
      d_texts[row][3]="";
    }
    else {
      d_texts[row][3]=QString().sprintf("%06u",q->value(2).toUInt());
    }

    // OFF Description
    if(q->value(2).toUInt()==0) {
      d_texts[row][4]=tr("[unassigned]");
    }
    else {
      d_texts[row][4]=q->value(3);
    }
  }
}


QString RDGpioListModel::sqlFields(bool on_fields) const
{
  QString cart_field=".`OFF_MACRO_CART`";
  if(on_fields) {
    cart_field=".`MACRO_CART`";
  }
  QString sql=QString("select ")+
    d_table+".`ID`,"+          // 00
    d_table+".`NUMBER`,"+      // 01
    d_table+cart_field+","   // 02
    "`CART`.`TITLE` "+           // 03
    "from "+d_table+" left join `CART` "+
    "on "+d_table+cart_field+"=`CART`.`NUMBER` ";

    return sql;
}
