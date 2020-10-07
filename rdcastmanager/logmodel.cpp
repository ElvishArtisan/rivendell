// logmodel.cpp
//
// Read-only data model for Rivendell logs
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <rdescape_string.h>
#include <rdlog_line.h>

#include "logmodel.h"

LogModel::LogModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_log=NULL;
  d_fms=NULL;
  d_bold_fms=NULL;
  d_log_icons=new RDLogIcons();

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Start Time"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Trans"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Group"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Length"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Title"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Artist"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Client"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Agency"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Label"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Source"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Ext Data"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Line ID"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Count"));
  d_alignments.push_back(right);
}


LogModel::~LogModel()
{
  if(d_fms!=NULL) {
    delete d_fms;
  }
  if(d_bold_fms!=NULL) {
    delete d_bold_fms;
  }
  delete d_log_icons;
}


void LogModel::setLogEvent(RDLogEvent *log)
{
  if(log->size()>0) {
    beginInsertRows(QModelIndex(),0,log->size()-1);
    endInsertRows();
  }
  d_log=log;
}


void LogModel::clearLogEvent()
{
  if((d_log!=NULL)&&(d_log->size()>0)) {
    beginRemoveRows(QModelIndex(),0,d_log->size()-1);
    endRemoveRows();
  }
}


void LogModel::setFont(const QFont &font)
{
  d_font=font;
  if(d_fms!=NULL) {
    delete d_fms;
  }
  d_fms=new QFontMetrics(d_font);
  d_bold_font=font;
  d_bold_font.setBold(true);
  if(d_bold_fms!=NULL) {
    delete d_bold_fms;
  }
  d_bold_fms=new QFontMetrics(d_bold_font);
}


int LogModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int LogModel::rowCount(const QModelIndex &parent) const
{
  if(d_log==NULL) {
    return 0;
  }
  return d_log->size();
}


QVariant LogModel::headerData(int section,Qt::Orientation orient,int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant LogModel::data(const QModelIndex &index,int role) const
{
  QString str;
  RDLogLine *ll=NULL;
  int col=index.column();
  int row=index.row();

  if((ll=d_log->logLine(row))!=NULL) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      switch(index.column()) {
      case 0:  // Start Time
	return ll->startTimeText();

      case 1:  // Transition
	return RDLogLine::transText(ll->transType());

      case 2:  // Cart Number
	return ll->cartNumberText();

      case 3:  // Group
	return ll->groupName();

      case 4:  // Length
	return ll->forcedLengthText();

      case 5:  // Title
	return ll->titleText();

      case 6:  // Artist
	return ll->artist();

      case 7:  // Client
	return ll->client();

      case 8:  // Agency
	return ll->agency();

      case 9:  // Label
	return ll->markerLabel();

      case 10:  // Source
	return RDLogLine::sourceText(ll->source());

      case 11:  // Ext Data
	return ll->extData();

      case 12:  // Line ID
	return QString().sprintf("%d",ll->id());

      case 13:  // Count
	return QString().sprintf("%d",row);
      }
      break;

    case Qt::DecorationRole:
      if(col==0) {
	return d_log_icons->typeIcon(ll->type(),ll->source());
      }
      break;

    case Qt::FontRole:
      if(col==3) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      switch(col) {
      case 0:
	if(ll->timeType()==RDLogLine::Hard) {
	  return Qt::blue;
	}
	break;

      case 3:
	return ll->groupColor();
      }
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    default:
      break;
    }
  }
  return QVariant();
}
