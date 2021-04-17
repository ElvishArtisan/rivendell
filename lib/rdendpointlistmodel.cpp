// rdendpointlistmodel.cpp
//
// Data model for Rivendell switcher endpoints
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
#include "rdendpointlistmodel.h"

RDEndpointListModel::RDEndpointListModel(RDMatrix *mtx,RDMatrix::Endpoint ep,
					 bool incl_none,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_mtx=mtx;
  d_endpoint=ep;
  d_include_none=incl_none;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  if(ep==RDMatrix::Input) {
    d_table_name="`INPUTS`";
    d_headers.push_back(tr("Input"));
    d_alignments.push_back(left);
  }
  else {
    d_table_name="`OUTPUTS`";
    d_headers.push_back(tr("Output"));
    d_alignments.push_back(left);
  }

  switch(d_mtx->type()) {
  case RDMatrix::LogitekVguest:
    //list_readonly=false;
    d_headers.push_back(tr("Engine"));
    d_alignments.push_back(center);

    d_headers.push_back(tr("Device (Hex)"));
    d_alignments.push_back(center);
    break;
      
    case RDMatrix::LiveWireLwrpAudio:
      //list_readonly=true;
      d_headers.push_back(tr("Node"));
      d_alignments.push_back(center);

      d_headers.push_back(tr("Slot"));
      d_alignments.push_back(center);
      break;

    case RDMatrix::SasUsi:
      //list_readonly=true;
      break;

    default:
      //list_readonly=false;
      break;
  }

  updateModel();
}


RDEndpointListModel::~RDEndpointListModel()
{
}


QPalette RDEndpointListModel::palette()
{
  return d_palette;
}


void RDEndpointListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDEndpointListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDEndpointListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDEndpointListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDEndpointListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDEndpointListModel::data(const QModelIndex &index,int role) const
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
	if(d_endpoint==RDMatrix::Input) {
	  return rda->iconEngine()->listIcon(RDIconEngine::Input);
	}
	else {
	  return rda->iconEngine()->listIcon(RDIconEngine::Output);
	}
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


int RDEndpointListModel::endpointId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


int RDEndpointListModel::endpointNumber(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(1).toInt();
}


QString RDEndpointListModel::endpointName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


int RDEndpointListModel::engineNumber(const QModelIndex &row) const
{
  if(d_mtx->type()==RDMatrix::LogitekVguest) {
    return d_texts.at(row.row()).at(2).toInt();
  }
  return -1;
}


int RDEndpointListModel::deviceNumber(const QModelIndex &row) const
{
  if(d_mtx->type()==RDMatrix::LogitekVguest) {
    return d_texts.at(row.row()).at(3).toString().toInt(NULL,16);
  }
  return -1;
}


void RDEndpointListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("`ID`=%d ",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDEndpointListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDEndpointListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "`STATION_NAME`='"+RDEscapeString(d_mtx->station())+"' && "+
    QString().sprintf("`MATRIX`=%d ",d_mtx->matrix())+
    "order by "+d_table_name+".`NUMBER` ";
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


void RDEndpointListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      d_table_name+QString().sprintf(".`ID`=%d ",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDEndpointListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  d_ids.back()=q->value(0).toInt();

  //
  // The first two fields are always the same
  ///
  // Name
  texts.push_back(q->value(2));

  // Number
  texts.push_back(QString().sprintf("%05d",q->value(1).toInt()));

  switch(d_mtx->type()) {
  case RDMatrix::LogitekVguest:
    // Engine Number
    if(q->value(3).toInt()>=0) {
      texts.push_back(QString().sprintf("%02d",q->value(3).toInt()));
    }
    else {
      texts.push_back("");
    }

    // Device Number
    if(q->value(4).toInt()>=0) {
      texts.push_back(QString().sprintf("%04X",q->value(4).toInt()));
    }
    else {
      texts.push_back("");
    }
    break;

    case RDMatrix::LiveWireLwrpAudio:
    // Hostname
    texts.push_back(q->value(3));

    // Slot
    texts.push_back(QString().sprintf("%d",q->value(4).toInt()));
    break;

  default:
    break;
  }

  d_texts[row]=texts;
}


QString RDEndpointListModel::sqlFields() const
{
  QString sql;

  switch(d_mtx->type()) {
  case RDMatrix::LogitekVguest:
    sql=QString("select ")+
      "`ID`,"+          // 00
      "`NUMBER`,"+      // 01
      "`NAME`,"+        // 02
      "`ENGINE_NUM`,"+  // 03
      "`DEVICE_NUM` "+  // 04
      "from "+d_table_name+" ";
    break;

  case RDMatrix::LiveWireLwrpAudio:
    sql=QString("select ")+
      "`ID`,"+             // 00
      "`NUMBER`,"+         // 01
      "`NAME`,"+           // 02
      "`NODE_HOSTNAME`,"+  // 03
      "`NODE_SLOT` "+      // 04
      "from "+d_table_name+" ";
    break;

  default:
    sql=QString("select ")+
      "`ID`,"+      // 00
      "`NUMBER`,"+  // 01
      "`NAME` "+    // 02
      "from "+d_table_name+" ";
    break;
  }

  return sql;
}
