// rdclockmodel.cpp
//
// Abstract a Rivendell Log Manager Clock.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdclockmodel.h"
#include "rdconf.h"

RDClockModel::RDClockModel(RDStation *station,QObject *parent)
  : QAbstractTableModel(parent), RDClock(station)
{
  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Start"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("End"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Trans"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Event"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Length"));
  d_alignments.push_back(right);
}


QPalette RDClockModel::palette()
{
  return d_palette;
}


void RDClockModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDClockModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDClockModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDClockModel::rowCount(const QModelIndex &parent) const
{
  return RDClock::size();
}


QVariant RDClockModel::headerData(int section,Qt::Orientation orient,
			     int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDClockModel::data(const QModelIndex &index,int role) const
{
  int col=index.column();
  int row=index.row();

  if(row<size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      switch(col) {
      case 0:  // Start
	return eventLine(row)->startTime().toString("mm:ss.zzz").left(7);

      case 1:  // End
	return eventLine(row)->startTime().addMSecs(eventLine(row)->length()).
	  toString("mm:ss.zzz").left(7);

      case 2:  // Trans
	return RDLogLine::transText(eventLine(row)->firstTransType());

      case 3:
	return eventLine(row)->name()+" ["+
	  eventLine(row)->propertiesText()+"]";

      case 4:
	return RDGetTimeLength(eventLine(row)->length(),false,true).trimmed();
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
      return eventLine(row)->color();

    default:
      break;
    }
  }

  return QVariant();
}


void RDClockModel::clear()
{
  beginResetModel();
  RDClock::clear();
  endResetModel();
}


bool RDClockModel::load()
{
  beginResetModel();
  bool state=RDClock::load();
  endResetModel();

  return state;
}


RDEventLine *RDClockModel::eventLine(const QModelIndex &row) const
{
  return RDClock::eventLine(row.row());
}


RDEventLine *RDClockModel::eventLine(int line) const
{
  return RDClock::eventLine(line);
}


int RDClockModel::insert(const QString &event_name,const QTime &start,int len)
{
  int line=preInsert(event_name,start);

  if(line<0) {
    return -1;
  }
  beginInsertRows(QModelIndex(),line,line);
  execInsert(line,event_name,start,len);
  endInsertRows();

  return line;
}


void RDClockModel::remove(int line)
{
  beginRemoveRows(QModelIndex(),line,line);
  RDClock::remove(line);
  endRemoveRows();
}


void RDClockModel::refresh(const QModelIndex &row)
{
  emit dataChanged(createIndex(row.row(),0),
		   createIndex(row.row(),columnCount()));
}
