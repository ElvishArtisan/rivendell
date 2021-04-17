// rdstationlistmodel.cpp
//
// Data model for Rivendell hosts
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
#include "rdstationlistmodel.h"

RDStationListModel::RDStationListModel(bool incl_none,
				       const QString &localhost_name,
				       QObject *parent)
  : QAbstractTableModel(parent)
{
  d_localhost_name=localhost_name;
  d_include_none=incl_none;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("IP Address"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Default User"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Short Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Xport Host"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("CAE Host"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Report Editor"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Web Browser"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Maint Pool"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Drag & Drop"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("HPI Ver"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("JACK Ver"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("ALSA Ver"));
  d_alignments.push_back(left);

  updateModel();
}


RDStationListModel::~RDStationListModel()
{
}


QPalette RDStationListModel::palette()
{
  return d_palette;
}


void RDStationListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDStationListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDStationListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDStationListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDStationListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDStationListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      return d_icons.at(row).at(col);

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


QString RDStationListModel::stationName(const QModelIndex &row) const
{
  return d_hostnames.at(row.row());
}


QModelIndex RDStationListModel::addStation(const QString &hostname)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(hostname.toLower()<d_hostnames.at(i).toLower()) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  if(hostname==d_localhost_name) {
    list[0]="localhost";
  }
  else {
    list[0]=hostname;
  }
  d_hostnames.insert(offset,hostname);
  d_texts.insert(offset,list);
  d_icons.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDStationListModel::removeStation(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_hostnames.removeAt(row.row());
  d_texts.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void RDStationListModel::removeStation(const QString &hostname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_hostnames.at(i)==hostname) {
      removeStation(createIndex(i,0));
      return;
    }
  }
}


void RDStationListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where `STATIONS`.`NAME`='"+RDEscapeString(d_hostnames.at(row.row()))+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDStationListModel::refresh(const QString &grpname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==grpname) {
      updateRowLine(i);
      return;
    }
  }
}


void RDStationListModel::updateModel()
{
  QList<QVariant> texts; 
  QList<QVariant> icons;

  RDSqlQuery *q=NULL;
  QString sql=sqlFields();
  sql+="order by `NAME` ";
  beginResetModel();
  d_hostnames.clear();
  d_texts.clear();
  d_icons.clear();
  if(d_include_none) {
    d_hostnames.push_back(tr("[none]"));
    d_texts.push_back(texts);
    d_texts.back().push_back(tr("[none]"));
    d_icons.push_back(texts);
    for(int i=1;i<columnCount();i++) {
      d_texts.back().push_back(QVariant());
      d_icons.back().push_back(QVariant());
    }
    d_icons.push_back(texts);
  }    
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_hostnames.push_back(QString());
    d_texts.push_back(texts);
    d_icons.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDStationListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where `NAME`='"+RDEscapeString(d_hostnames.at(line))+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDStationListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;
  QList<QVariant> icons;

  // Hostname
  d_hostnames[row]=q->value(0).toString();
  if(q->value(0).toString()==d_localhost_name) {
    texts.push_back("localhost");
  }
  else {
    texts.push_back(q->value(0));
  }
  icons.push_back(rda->iconEngine()->stationIcon());

  // Description
  texts.push_back(q->value(1));
  icons.push_back(QVariant());

  // IP Address
  texts.push_back(q->value(2));
  icons.push_back(QVariant());

  // Default User
  texts.push_back(q->value(3));
  if(q->value(4).toString()=="Y") {
    icons.push_back(rda->iconEngine()->userIcon(RDUser::TypeLocalUser));
  }
  else {
    icons.push_back(rda->iconEngine()->userIcon(RDUser::TypeExternalUser));
  }

  // Short Name
  texts.push_back(q->value(5));
  icons.push_back(QVariant());

  // Xport Host
  if(q->value(6).toString().toLower().trimmed()=="localhost") {
    texts.push_back(q->value(0));
  }
  else {
    texts.push_back(q->value(6));
  }
  icons.push_back(rda->iconEngine()->stationIcon());

  // CAE Host
  if(q->value(7).toString().toLower().trimmed()=="localhost") {
    texts.push_back(q->value(0));
  }
  else {
    texts.push_back(q->value(7));
  }
  icons.push_back(rda->iconEngine()->stationIcon());

  // Report Editor
  if(q->value(8).toString().isEmpty()) {
    texts.push_back("vi");
  }
  else {
    texts.push_back(q->value(8).toString().
		    split("/",QString::SkipEmptyParts).last());
  }
  icons.push_back(QVariant());

  // Web Editor
  if(q->value(9).toString().isEmpty()) {
    texts.push_back(tr("[none]"));
  }
  else {
    texts.push_back(q->value(9).toString().
		    split("/",QString::SkipEmptyParts).last());
  }
  icons.push_back(QVariant());

  // Maint Pool
  texts.push_back(q->value(10));
  icons.push_back(QVariant());

  // Drag & Drop
  texts.push_back(q->value(11));
  icons.push_back(QVariant());

  if(q->value(12).toString()=="Y") {
    // HPI Ver
    if(q->value(13).toString().isEmpty()) {
      texts.push_back(tr("[none]"));
    }
    else {
      texts.push_back(q->value(13));
    }
    icons.push_back(QVariant());
    
    // Jack Ver
    if(q->value(14).toString().isEmpty()) {
      texts.push_back(tr("[none]"));
    }
    else {
      texts.push_back(q->value(14));
    }
    icons.push_back(QVariant());
    
    // ALSA Ver
    if(q->value(15).toString().isEmpty()) {
      texts.push_back(tr("[none]"));
    }
    else {
      texts.push_back(q->value(15));
    }
    icons.push_back(QVariant());
  }
  else {
    // HPI Ver
    texts.push_back(tr("[unavailable]"));
    icons.push_back(QVariant());
    
    // Jack Ver
    texts.push_back(tr("[unavailable]"));
    icons.push_back(QVariant());
    
    // ALSA Ver
    texts.push_back(tr("[unavailable]"));
    icons.push_back(QVariant());
  }

  d_texts[row]=texts;
  d_icons[row]=icons;
}


QString RDStationListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`STATIONS`.`NAME`,"+                    // 00
    "`STATIONS`.`DESCRIPTION`,"+             // 01
    "`STATIONS`.`IPV4_ADDRESS`,"+            // 02
    "`STATIONS`.`DEFAULT_NAME`,"+            // 03
    "`USERS`.`LOCAL_AUTH`,"+                 // 04
    "`STATIONS`.`SHORT_NAME`,"+              // 05
    "`STATIONS`.`HTTP_STATION`,"+            // 06
    "`STATIONS`.`CAE_STATION`,"+             // 07
    "`STATIONS`.`REPORT_EDITOR_PATH`,"+      // 08
    "`STATIONS`.`BROWSER_PATH`,"+            // 09
    "`STATIONS`.`SYSTEM_MAINT`,"+            // 10
    "`STATIONS`.`ENABLE_DRAGDROP`,"+         // 11
    "`STATIONS`.`STATION_SCANNED`,"+         // 12
    "`STATIONS`.`HPI_VERSION`,"+             // 13
    "`STATIONS`.`JACK_VERSION`,"+            // 14
    "`STATIONS`.`ALSA_VERSION` "+            // 15
    "from `STATIONS` left join `USERS` "+
    "on `STATIONS`.`DEFAULT_NAME`=`USERS`.`LOGIN_NAME` ";

    return sql;
}
