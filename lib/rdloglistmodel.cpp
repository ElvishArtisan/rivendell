// rdloglistmodel.cpp
//
// Data model for Rivendell log metadata
//
//   (C) Copyright 2020-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdloglistmodel.h"

RDLogListModel::RDLogListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_sort_column=0;
  d_sort_order=Qt::AscendingOrder;
  d_service_names.push_back(tr("ALL"));

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Log Name"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`NAME`");

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`DESCRIPTION`");

  d_headers.push_back(tr("Service"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`SERVICE`");

  d_headers.push_back(tr("Music"));
  d_alignments.push_back(center);
  d_column_fields.push_back("`MUSIC_LINKED`");

  d_headers.push_back(tr("Traffic"));
  d_alignments.push_back(center);
  d_column_fields.push_back("`TRAFFIC_LINKED`");

  d_headers.push_back(tr("Tracks"));
  d_alignments.push_back(center);
  d_column_fields.push_back("`SCHEDULED_TRACKS`");

  d_headers.push_back(tr("Valid From"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`START_DATE`");

  d_headers.push_back(tr("Valid To"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`END_DATE`");

  d_headers.push_back(tr("Auto Refresh"));
  d_alignments.push_back(center);
  d_column_fields.push_back("`AUTO_REFRESH`");

  d_headers.push_back(tr("Origin"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`ORIGIN_USER`");

  d_headers.push_back(tr("Last Linked"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`LINK_DATETIME`");

  d_headers.push_back(tr("Last Modified"));
  d_alignments.push_back(left);
  d_column_fields.push_back("`MODIFIED_DATETIME`");

  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(processNotification(RDNotification *)));
}


RDLogListModel::~RDLogListModel()
{
}


QPalette RDLogListModel::palette()
{
  return d_palette;
}


void RDLogListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDLogListModel::setFont(const QFont &font)
{
  d_font=font;
}


int RDLogListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDLogListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDLogListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDLogListModel::data(const QModelIndex &index,int role) const
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
      // Nothing to do!
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


void RDLogListModel::sort(int col,Qt::SortOrder order)
{
  if((col!=d_sort_column)||(order!=d_sort_order)) {
    d_sort_column=col;
    d_sort_order=order;
    updateModel(d_filter_where_sql,d_filter_limit_sql);
  }
}


QString RDLogListModel::logName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex RDLogListModel::addLog(const QString &name)
{
  QModelIndex ret;

  //
  // Ensure we haven't already been added
  //
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==name) {
      return ret;
    }
  }

  //
  // Make sure we match the current filter
  //
  QString sql=sqlFields()+"where "+
    "`NAME`='"+RDEscapeString(name)+"' "+
    d_filter_where_sql;
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    QList<QVariant> list;

    beginInsertRows(QModelIndex(),d_texts.size(),d_texts.size());
    d_icons.push_back(list);

    list.push_back(name);
    d_texts.push_back(list);

    updateRowLine(d_texts.size()-1);
    endInsertRows();
    ret=createIndex(d_texts.size()-1,0);
  }
  delete q;

  return ret;
}


void RDLogListModel::removeLog(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void RDLogListModel::removeLog(const QString &logname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==logname) {
      removeLog(createIndex(i,0));
      return;
    }
  }
}


void RDLogListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where `NAME`='"+RDEscapeString(d_texts.at(row.row()).at(0).toString())+
      "'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDLogListModel::refresh(const QString &logname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0)==logname) {
      updateRowLine(i);
      return;
    }
  }
}


void RDLogListModel::setFilterSql(const QString &where_sql,
				  const QString &limit_sql)
{
    updateModel(where_sql,limit_sql);
    d_filter_where_sql=where_sql;
    d_filter_limit_sql=limit_sql;
}


void RDLogListModel::processNotification(RDNotification *notify)
{
  if(notify->type()==RDNotification::LogType) {
    switch(notify->action()) {
    case RDNotification::AddAction:
      addLog(notify->id().toString());
      break;

    case RDNotification::ModifyAction:
      refresh(notify->id().toString());
      break;

    case RDNotification::DeleteAction:
      removeLog(notify->id().toString());
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void RDLogListModel::updateModel(const QString &where_sql,
				 const QString &limit_sql)
{
  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    " where "+
    "(`TYPE`=0)&&"+        
    "(`LOG_EXISTS`='Y') "+
    where_sql+" ";
  sql+="order by "+d_column_fields.at(d_sort_column)+" ";
  if(d_sort_order==Qt::DescendingOrder) {
    sql+="desc ";
  }
  sql+=limit_sql;

  beginResetModel();
  d_texts.clear();
  d_icons.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QList<QVariant> texts; 
    d_texts.push_back(texts);
    QList<QVariant> icons;
    d_icons.push_back(icons);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDLogListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where `NAME`='"+RDEscapeString(d_texts.at(line).at(0).toString())+"'";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
      emit dataChanged(createIndex(line,0),createIndex(line,columnCount()));
    }
    delete q;
  }
}


void RDLogListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;
  QList<QVariant> icons;
  // Log Name
  texts.push_back(q->value(0));
  if((q->value(7).toInt()==q->value(8).toInt())&&
     ((q->value(9).toInt()==0)||(q->value(10).toString()=="Y"))&&
     ((q->value(11).toInt()==0)||(q->value(12).toString()=="Y"))) {
    icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::GreenCheck));
  }
  else {
    icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::RedX));
  }

  // Description
  texts.push_back(q->value(1));
  icons.push_back(QVariant());

  // Service
  texts.push_back(q->value(2));
  icons.push_back(QVariant());

  // Music State
  texts.push_back(QString());
  if(q->value(9).toInt()==0) {
    icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::WhiteBall));
  }
  else {
    if(q->value(10).toString()=="Y") {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::GreenBall));
    }
    else {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::RedBall));
    }
  }

  // Traffic State
  texts.push_back(QString());
  if(q->value(11).toInt()==0) {
    icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::WhiteBall));
  }
  else {
    if(q->value(12).toString()=="Y") {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::GreenBall));
    }
    else {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::RedBall));
    }
  }

  // Tracks
  texts.push_back(QString::asprintf("%d / %d",
				    q->value(7).toInt(),
				    q->value(8).toInt()));
  if(q->value(8).toInt()==0) {
    icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::WhiteBall));
  }
  else {
    if(q->value(8).toInt()==q->value(7).toInt()) {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::GreenBall));
    }
    else {
      icons.push_back(rda->iconEngine()->listIcon(RDIconEngine::RedBall));
    }
  }

  // Start Date
  if(q->value(3).toDate().isNull()) {
    texts.push_back(tr("Always"));
  }
  else {
    texts.push_back(rda->shortDateString(q->value(3).toDate()));
  }
  icons.push_back(QVariant());

  // End Date
  if(q->value(4).toDate().isNull()) {
    texts.push_back(tr("Always"));
  }
  else {
    texts.push_back(rda->shortDateString(q->value(4).toDate()));
  }
  icons.push_back(QVariant());

  // Auto Refresh
  texts.push_back(q->value(15));
  icons.push_back(QVariant());

  // Origin
  texts.push_back(q->value(5).toString()+QString(" - ")+
		  q->value(6).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
  icons.push_back(QVariant());

  // Last Linked
  texts.push_back(q->value(13).toDateTime().
		  toString("MM/dd/yyyy - hh:mm:ss"));
  icons.push_back(QVariant());

  // Last Modified
  texts.push_back(q->value(14).toDateTime().
		  toString("MM/dd/yyyy - hh:mm:ss"));
  icons.push_back(QVariant());

  d_texts[row]=texts;
  d_icons[row]=icons;
}


QString RDLogListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`NAME`,"+               // 00
    "`DESCRIPTION`,"+        // 01
    "`SERVICE`,"+            // 02
    "`START_DATE`,"+         // 03
    "`END_DATE`,"+           // 04
    "`ORIGIN_USER`,"+        // 05
    "`ORIGIN_DATETIME`,"+    // 06
    "`COMPLETED_TRACKS`,"+   // 07
    "`SCHEDULED_TRACKS`,"+   // 08
    "`MUSIC_LINKS`,"+        // 09
    "`MUSIC_LINKED`,"+       // 10
    "`TRAFFIC_LINKS`,"+      // 11
    "`TRAFFIC_LINKED`,"+     // 12
    "`LINK_DATETIME`,"+      // 13
    "`MODIFIED_DATETIME`,"+  // 14
    "`AUTO_REFRESH` "+       // 15
    "from LOGS ";
    return sql;
}
