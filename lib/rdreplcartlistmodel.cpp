// rdreplcartlistmodel.cpp
//
// Data model for replicator carts
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
#include "rdreplcartlistmodel.h"

RDReplCartListModel::RDReplCartListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Cart"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Title"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Last Posted"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Posted Filename"));
  d_alignments.push_back(left);

  //
  // Refresh Timer
  //
  d_refresh_timer=new QTimer(this);
  d_refresh_timer->setSingleShot(true);
  connect(d_refresh_timer,SIGNAL(timeout()),this,SLOT(refreshTimeoutData()));
}


RDReplCartListModel::~RDReplCartListModel()
{
  delete d_refresh_timer;
}


QPalette RDReplCartListModel::palette()
{
  return d_palette;
}


void RDReplCartListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDReplCartListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDReplCartListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDReplCartListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDReplCartListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDReplCartListModel::data(const QModelIndex &index,int role) const
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


unsigned RDReplCartListModel::cartId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


QModelIndex RDReplCartListModel::addCart(unsigned id)
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
  d_icons.insert(offset,QVariant());
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RDReplCartListModel::removeCart(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_icons.removeAt(row.row());
  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDReplCartListModel::removeCart(unsigned id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      removeCart(createIndex(i,0));
      return;
    }
  }
}


void RDReplCartListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      QString().sprintf("where `REPL_CART_STATE`.`ID`=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDReplCartListModel::refresh(unsigned id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


QString RDReplCartListModel::replicatorName() const
{
  return d_replicator_name;
}


void RDReplCartListModel::setReplicatorName(const QString &repl_name)
{
  if(repl_name!=d_replicator_name) {
    d_refresh_timer->stop();
    d_replicator_name=repl_name;
    updateModel();
    d_refresh_timer->start(5000);
  }
}


void RDReplCartListModel::refreshTimeoutData()
{
  QString sql;
  RDSqlQuery *q;
  int line;

  sql=QString("select ")+
    "`ID`,"+             // 00
    "`ITEM_DATETIME` "+  // 01
    "from `REPL_CART_STATE` where "+
    "`REPLICATOR_NAME`='"+RDEscapeString(d_replicator_name)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((line=d_ids.indexOf(q->value(0).toUInt()))>=0) {
      if(d_texts.at(line).at(2)!=
	 q->value(1).toDateTime().toString("hh:mm:ss dd/MM/yyyy")) {
	d_texts[line][2]=
	  q->value(1).toDateTime().toString("hh:mm:ss dd/MM/yyyy");
	emit dataChanged(createIndex(line,2),createIndex(line,2));
      }
    }
  }
  delete q;
  d_refresh_timer->start(5000);
}


void RDReplCartListModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where "+
    "`REPLICATOR_NAME`='"+RDEscapeString(d_replicator_name)+"' "+
    "order by `REPL_CART_STATE`.`CART_NUMBER` ";
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


void RDReplCartListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      QString().sprintf("where `REPL_CART_STATE`.`ID`=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RDReplCartListModel::updateRow(int row,RDSqlQuery *q)
{
  d_ids[row]=q->value(0).toInt();
  switch((RDCart::Type)q->value(1).toInt()) {
  case RDCart::Audio:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Cart);
    break;

  case RDCart::Macro:
    d_icons[row]=rda->iconEngine()->typeIcon(RDLogLine::Macro);
    break;

    case RDCart::All:
      break;
    }
  
  QList<QVariant> texts;

  // Cart Number
  texts.push_back(QString().sprintf("%06u",q->value(2).toUInt()));

  // Title
  texts.push_back(q->value(3));

  // Last Posted
  texts.push_back(q->value(4).toDateTime().toString("hh:mm:ss MM/dd/yyyy"));

  // Posted Filename
  texts.push_back(q->value(5));

  d_texts[row]=texts;
}


QString RDReplCartListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`REPL_CART_STATE`.`ID`,"+               // 00
    "`CART`.`TYPE`,"+                        // 01
    "`REPL_CART_STATE`.`CART_NUMBER`,"+      // 02
    "`CART`.`TITLE`,"+                       // 03
    "`REPL_CART_STATE`.`ITEM_DATETIME`,"+    // 04
    "`REPL_CART_STATE`.`POSTED_FILENAME` "+  // 05
    "from `REPL_CART_STATE` left join `CART` "+
    "on `REPL_CART_STATE`.`CART_NUMBER`=`CART`.`NUMBER` ";

  return sql;
}
