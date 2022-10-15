// rdlogimportmodel.cpp
//
// Data model for Rivendell log imports
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdconf.h"
#include "rdescape_string.h"
#include "rdlogimportmodel.h"
#include "rdsvc.h"

RDLogImportModel::RDLogImportModel(const QString &hostname,pid_t proc_id,
				   QObject *parent)
  : QAbstractTableModel(parent)
{
  d_station_name=hostname;
  d_process_id=proc_id;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Start Time"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Trans"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Len"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Title"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("GUID"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Event ID"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Annc Type"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Line Number"));
  d_alignments.push_back(right);

  updateModel();
}


RDLogImportModel::~RDLogImportModel()
{
}


QPalette RDLogImportModel::palette()
{
  return d_palette;
}


void RDLogImportModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDLogImportModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDLogImportModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDLogImportModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDLogImportModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDLogImportModel::data(const QModelIndex &index,int role) const
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
	return d_icons.at(row);
      }
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
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


int RDLogImportModel::lineId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


void RDLogImportModel::refresh()
{
  updateModel();
}


void RDLogImportModel::clear()
{
  beginResetModel();
  d_texts.clear();
  d_icons.clear();
  d_ids.clear();
  endResetModel();
}


void RDLogImportModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "`IMPORTER_LINES`.`STATION_NAME`='"+RDEscapeString(d_station_name)+"'&&"+
    QString::asprintf("`PROCESS_ID`=%u ",d_process_id)+
    "order by `IMPORTER_LINES`.`LINE_ID` ";
  beginResetModel();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(0);
    d_icons.push_back(QVariant());
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDLogImportModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;
  for(int i=0;i<columnCount();i++) {
    texts.push_back(QVariant());
  }

  // Start Time
  QString prefix;
  if((RDLogLine::TimeType)q->value(12).toUInt()==RDLogLine::Hard) {
    if(q->value(13).toInt()<0) {
      prefix="S:";
    }
    else {
      prefix="H:";
    }
  }
  texts[0]=prefix+RDSvc::timeString(q->value(1).toInt(),q->value(2).toInt());

  // Transition Type
  texts[2]=RDLogLine::transText((RDLogLine::TransType)q->value(11).toUInt());

  // Length
  if(!q->value(4).isNull()) {
    texts[3]=RDGetTimeLength(q->value(4).toInt(),false,false);
  }

  // GUID
  texts[5]=q->value(5).toString().trimmed();

  // Event ID
  texts[6]=q->value(6).toString().trimmed();

  // Annc Type
  texts[7]=q->value(7).toString().trimmed();

  // Line
  texts[8]=QString::asprintf("%u",1+q->value(10).toUInt());

  switch((RDLogLine::Type)q->value(9).toUInt()) {
  case RDLogLine::Cart:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Cart);
    texts[1]=q->value(3);  // Cart Number
    texts[4]=q->value(8).toString().trimmed();  // Title
    break;

  case RDLogLine::Marker:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Marker);
    texts[1]=tr("NOTE");  // Cart Number
    texts[4]=q->value(8).toString().trimmed();  // Title
    break;

  case RDLogLine::TrafficLink:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::TrafficLink);
    texts[4]=tr("[spot break]");  // Title
    break;

  case RDLogLine::Track:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Track);
    texts[4]=tr("[voice track]");  // Title
    break;

  case RDLogLine::Macro:
  case RDLogLine::OpenBracket:
  case RDLogLine::CloseBracket:
  case RDLogLine::Chain:
  case RDLogLine::MusicLink:
  case RDLogLine::UnknownType:
    break;
  }

  d_ids[row]=q->value(0).toUInt();
  d_texts[row]=texts;
}


QString RDLogImportModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`ID`,"+             // 00
    "`START_HOUR`,"+     // 01
    "`START_SECS`,"+     // 02
    "`EXT_CART_NAME`,"+  // 03
    "`LENGTH`,"+         // 04
    "`EXT_DATA`,"+       // 05
    "`EXT_EVENT_ID`,"+   // 06
    "`EXT_ANNC_TYPE`,"+  // 07
    "`TITLE`,"+          // 08
    "`TYPE`,"+           // 09
    "`FILE_LINE`,"+      // 10
    "`TRANS_TYPE`,"+     // 11
    "`TIME_TYPE`,"+      // 12
    "`GRACE_TIME` "+     // 13
    "from `IMPORTER_LINES` ";

    return sql;
}
