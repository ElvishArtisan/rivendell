// rdmacrocartmodel.cpp
//
// Data model for Rivendell services
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
#include "rdmacrocartmodel.h"

RDMacroCartModel::RDMacroCartModel(unsigned cartnum,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_cart_number=cartnum;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Line"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Command"));
  d_alignments.push_back(left);

  updateModel();
}


RDMacroCartModel::~RDMacroCartModel()
{
}


QPalette RDMacroCartModel::palette()
{
  return d_palette;
}


void RDMacroCartModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDMacroCartModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDMacroCartModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDMacroCartModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDMacroCartModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDMacroCartModel::data(const QModelIndex &index,int role) const
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


unsigned RDMacroCartModel::cartNumber() const
{
  return d_cart_number;
}


int RDMacroCartModel::lineCount() const
{
  return rowCount()-1;
}


bool RDMacroCartModel::isEndHandle(const QModelIndex &row) const
{
  return row.row()==lineCount();
}


QString RDMacroCartModel::code(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(1).toString();
}


QString RDMacroCartModel::allCode() const
{
  QString macro;
  for(int i=0;i<lineCount();i++) {
    macro+=d_texts.at(i).at(1).toString();
  }
  return macro;
}


QModelIndex RDMacroCartModel::addLine(const QModelIndex &row,const QString &rml)
{
  //
  // Find the insertion offset
  //
  int offset=row.row();
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[0]=QString::asprintf("%d",offset+1);
  list[1]=rml;
  d_texts.insert(offset,list);
  endInsertRows();

  return createIndex(offset,0);
}


void RDMacroCartModel::removeLine(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_texts.removeAt(row.row());

  endRemoveRows();
}


void RDMacroCartModel::refresh(const QModelIndex &row,const QString &rml)
{
  if(row.row()<d_texts.size()) {
    d_texts[row.row()][1]=rml;
    emit dataChanged(createIndex(row.row(),1),createIndex(row.row(),1));
  }
}


void RDMacroCartModel::save() const
{
  QString sql=QString("update `CART` set ")+
    "`MACROS`=\""+RDEscapeString(allCode())+"\" where "+
    QString::asprintf("`NUMBER`=%u",d_cart_number);
  RDSqlQuery::apply(sql);
}


void RDMacroCartModel::updateModel()
{
  QList<QVariant> texts; 
  for(int i=0;i<columnCount();i++) {
    texts.push_back(QVariant());
  }

  QString sql=QString("select ")+
    "`MACROS` "+  // 00
    "from `CART` where "+
    QString::asprintf("`NUMBER`=%u",d_cart_number);
  beginResetModel();
  d_texts.clear();
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    QStringList f0=q->value(0).toString().split("!",QString::SkipEmptyParts);
    for(int i=0;i<f0.size();i++) {
      d_texts.push_back(texts);
      d_texts.back()[0]=QString::asprintf("%d",i+1);
      d_texts.back()[1]=f0.at(i)+"!";
    }
  }
  delete q;
  d_texts.push_back(texts);
  d_texts.back()[1]=tr("--- End of Cart ---");
  endResetModel();
}
