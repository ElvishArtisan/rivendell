// rdpypadlistmodel.cpp
//
// Data model for Rivendell PyPAD instances
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
#include "rdpypadlistmodel.h"

RDPypadListModel::RDPypadListModel(const QString &hostname,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_station_name=hostname;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("ID"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Script Path"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Exit Code"));
  d_alignments.push_back(right);

  d_state_timer=new QTimer(this);
  d_state_timer->setSingleShot(true);
  connect(d_state_timer,SIGNAL(timeout()),this,SLOT(stateTimeoutData()));
  d_state_timer->start(0);

  updateModel();
}


RDPypadListModel::~RDPypadListModel()
{
  delete d_state_timer;
}


QPalette RDPypadListModel::palette()
{
  return d_palette;
}


void RDPypadListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDPypadListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDPypadListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDPypadListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDPypadListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDPypadListModel::data(const QModelIndex &index,int role) const
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
	return d_state_icons.at(row);
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


int RDPypadListModel::instanceId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


QModelIndex RDPypadListModel::addInstance(int id)
{
  //
  // Find the insertion offset
  //
  int offset=d_ids.size();
  for(int i=0;i<d_ids.size();i++) {
    if(id<d_ids.at(i)) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_texts.insert(offset,list);
  d_ids.insert(offset,id);
  d_state_icons.
    insert(offset,rda->iconEngine()->listIcon(RDIconEngine::RedBall));
  d_states.insert(offset,false);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDPypadListModel::removeInstance(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_state_icons.removeAt(row.row());
  d_states.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDPypadListModel::removeInstance(int id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      removeInstance(createIndex(i,0));
      return;
    }
  }
}


void RDPypadListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("PYPAD_INSTANCES.ID=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDPypadListModel::refresh(int id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


bool RDPypadListModel::isActive(int id) const
{
  int index=d_ids.indexOf(id);
  if(index>=0) {
    return d_states.at(index);
  }
  return false;
}


void RDPypadListModel::stateTimeoutData()
{
  QString sql;
  RDSqlQuery *q;
  int line=-1;

  sql=QString("select ")+
    "ID,"+          // 00
    "IS_RUNNING,"+  // 01
    "EXIT_CODE "+   // 02
    "from PYPAD_INSTANCES where "+
    "STATION_NAME=\""+RDEscapeString(d_station_name)+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((line=d_ids.indexOf(q->value(0).toInt()))>=0) {
      if((q->value(1).toString()=="Y")!=d_states.at(line)) {
	if(d_states.at(line)) {
	  d_state_icons[line]=
	    rda->iconEngine()->listIcon(RDIconEngine::RedBall);
	}
	else {
	  d_state_icons[line]=
	    rda->iconEngine()->listIcon(RDIconEngine::GreenBall);
	}
	d_states[line]=!d_states.at(line);
	emit dataChanged(createIndex(line,0),createIndex(line,0));
      }
      if(d_texts.at(line).at(3)!=
	 QString().sprintf("%d",q->value(2).toInt())) {
	d_texts[line][3]=QString().sprintf("%d",q->value(2).toInt());
	emit dataChanged(createIndex(line,3),createIndex(line,3));
      }
    }
  }
  delete q;
  d_state_timer->start(3000);
}


void RDPypadListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "PYPAD_INSTANCES.STATION_NAME=\""+RDEscapeString(d_station_name)+"\" "+
    "order by PYPAD_INSTANCES.ID ";
  beginResetModel();
  d_texts.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(0);
    d_state_icons.
      push_back(rda->iconEngine()->listIcon(RDIconEngine::RedBall));
    d_states.push_back(false);
    d_texts.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDPypadListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString().sprintf("PYPAD_INSTANCES.ID=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDPypadListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // ID
  texts.push_back(QString().sprintf("%d",q->value(0).toInt()));

  // Description
  texts.push_back(q->value(1));

  // Script Path
  texts.push_back(q->value(2));

  // Exit Code
  texts.push_back(QString().sprintf("%d",q->value(3).toInt()));

  d_ids[row]=q->value(0).toInt();
  d_texts[row]=texts;
}


QString RDPypadListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "PYPAD_INSTANCES.ID,"            // 00
    "PYPAD_INSTANCES.DESCRIPTION,"+  // 01
    "PYPAD_INSTANCES.SCRIPT_PATH,"   // 02
    "PYPAD_INSTANCES.EXIT_CODE "+    // 03
    "from PYPAD_INSTANCES ";

    return sql;
}
