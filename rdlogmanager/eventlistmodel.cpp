// eventlistmodel.cpp
//
// Data model for Rivendell rdlogmanager(1) events
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

#include <QPainter>

#include "rdapplication.h"
#include "rdconf.h"
#include "rdescape_string.h"
#include "rdevent_line.h"
#include "eventlistmodel.h"

EventListModel::EventListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Trans"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Properties"));
  d_alignments.push_back(left);

  updateModel();
}


EventListModel::~EventListModel()
{
}


QPalette EventListModel::palette()
{
  return d_palette;
}


void EventListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void EventListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int EventListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int EventListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant EventListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant EventListModel::data(const QModelIndex &index,int role) const
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


QString EventListModel::eventName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString();
}


QModelIndex EventListModel::addEvent(const QString &name)
{
  //
  // Find the insertion offset
  //
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
  list[0]=name;
  d_texts.insert(offset,list);
  d_icons.insert(offset,QVariant());
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void EventListModel::removeEvent(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void EventListModel::removeEvent(const QString &name)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toString()==name) {
      removeEvent(createIndex(i,0));
      return;
    }
  }
}


void EventListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      "`EVENTS`.`NAME`=\""+
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


void EventListModel::refresh(const QString &name)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toString()==name) {
      updateRowLine(i);
      return;
    }
  }
}


void EventListModel::setFilterSql(const QString &sql)
{
  if(sql!=d_filter_sql) {
    d_filter_sql=sql;
    updateModel();
  }
}


void EventListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    d_filter_sql+
    "order by `EVENTS`.`NAME` ";
  beginResetModel();
  d_texts.clear();
  d_icons.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    d_icons.push_back(QVariant());
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void EventListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      "`EVENTS`.`NAME`=\""+
      RDEscapeString(d_texts.at(line).at(0).toString())+"\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void EventListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Name
  texts.push_back(q->value(0));

  // Trans
  texts.
    push_back(RDLogLine::transText((RDLogLine::TransType)q->value(3).toUInt()));

  // Properties
  texts.push_back(RDEventLine::propertiesText(q->value(2).toInt(),
			       (RDLogLine::TransType)q->value(3).toUInt(),
			       (RDLogLine::TimeType)q->value(4).toUInt(),
			       q->value(5).toInt(),
			       RDBool(q->value(6).toString()),
			       (RDEventLine::ImportSource)q->value(7).toUInt(),
			       !q->value(8).toString().isEmpty()));

  d_texts[row]=texts;
  d_icons[row]=MakeIcon(q->value(1).toString());
}


QString EventListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`NAME`,"+              // 00
    "`COLOR`,"+             // 01
    "`PREPOSITION`,"+       // 02
    "`FIRST_TRANS_TYPE`,"+  // 03
    "`TIME_TYPE`,"+         // 04
    "`GRACE_TIME`,"+        // 05
    "`USE_AUTOFILL`,"+      // 06
    "`IMPORT_SOURCE`,"+     // 07
    "`NESTED_EVENT` "+      // 08
    "from `EVENTS` ";

    return sql;
}


QPixmap EventListModel::MakeIcon(const QString &color) const
{
  QPixmap pix(QSize(15,15));
  QPainter *p=new QPainter();
  p->begin(&pix);
  if(QColor(color).isValid()) {
    p->fillRect(0,0,15,15,QColor(color));
  }
  else {
    p->fillRect(0,0,15,15,d_palette.color(QPalette::Background));
  }
  p->end();
  delete p;

  return pix;
}
