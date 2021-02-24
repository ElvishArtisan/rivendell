// rdgpiologmodel.cpp
//
// Data model for Rivendell GPIO log records
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
#include "rdgpiologmodel.h"

RDGpioLogModel::RDGpioLogModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_matrix_number=0;
  d_gpio_type=RDMatrix::GpioInput;
  d_date_filter=QDate::currentDate();
  d_state_filter=tr("On");

  //
  // Column Attributes
  //
  //  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Time"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Line"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("State"));
  d_alignments.push_back(center);

  updateModel();
}


RDGpioLogModel::~RDGpioLogModel()
{
}


QPalette RDGpioLogModel::palette()
{
  return d_palette;
}


void RDGpioLogModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDGpioLogModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDGpioLogModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDGpioLogModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDGpioLogModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDGpioLogModel::data(const QModelIndex &index,int role) const
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
      if(col==2) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      return d_text_colors.at(row);

    case Qt::BackgroundRole:
      // Nothing to do!
      break;

    default:
      break;
    }
  }

  return QVariant();
}


RDMatrix::GpioType RDGpioLogModel::gpioType() const
{
  return d_gpio_type;
}


int RDGpioLogModel::matrixNumber() const
{
  return d_matrix_number;
}


QDate RDGpioLogModel::dateFilter() const
{
  return d_date_filter;
}


QString RDGpioLogModel::stateFilter() const
{
  return d_state_filter;
}


void RDGpioLogModel::addEvent(int line,bool state)
{
  if((d_state_filter==tr("Off")&&state)||
     (d_state_filter==tr("On")&&(!state))) {
    return;
  }

  beginInsertRows(QModelIndex(),d_texts.size(),d_texts.size());
  QList<QVariant> texts;

  // Time
  texts.push_back(QTime::currentTime().toString("hh:mm:ss"));

  // Line
  texts.push_back(QString().sprintf("%d",line+1));

  // State
  if(state) {
    texts.push_back(tr("On"));
    d_text_colors.push_back(QColor(Qt::darkGreen));
  }
  else {
    texts.push_back(tr("Off"));
    d_text_colors.push_back(QColor(Qt::darkRed));
  }
  
  d_texts.push_back(texts);

  endInsertRows();
}


void RDGpioLogModel::setGpioType(RDMatrix::GpioType type)
{
  if(type!=d_gpio_type) {
    d_gpio_type=type;
    updateModel();
  }
}


void RDGpioLogModel::setMatrixNumber(int matrix_num)
{
  if(matrix_num!=d_matrix_number) {
    d_matrix_number=matrix_num;
    updateModel();
  }
}


void RDGpioLogModel::setDateFilter(const QDate &date)
{
  if(date!=d_date_filter) {
    d_date_filter=date;
    updateModel();
  }
}


void RDGpioLogModel::setStateFilter(const QString &str)
{
  if(str!=d_state_filter) {
    d_state_filter=str;
    updateModel();
  }
}


void RDGpioLogModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
  "where "+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",d_matrix_number)+
    QString().sprintf("(TYPE=%d)&&",d_gpio_type)+
    "(EVENT_DATETIME>=\""+d_date_filter.toString("yyyy-MM-dd")+
    " 00:00:00\")&&"+
    "(EVENT_DATETIME<\""+d_date_filter.addDays(1).
    toString("yyyy-MM-dd")+" 00:00:00\")";
  if(d_state_filter==tr("On")) {
    sql+="&&(EDGE=1)";
  }
  if(d_state_filter==tr("Off")) {
    sql+="&&(EDGE=0)";
  }
  sql+="order by EVENT_DATETIME ";
  beginResetModel();
  d_texts.clear();
  d_text_colors.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    d_text_colors.push_back(QVariant());
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RDGpioLogModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  // Time
  texts.push_back(q->value(0).toDateTime().toString("hh:mm:ss"));

  // Line
  texts.push_back(QString().sprintf("%d",q->value(1).toInt()));

  // State
  if(q->value(2).toUInt()==0) {
    texts.push_back(tr("Off"));
    d_text_colors[row]=QColor(Qt::darkRed);
  }
  else {
    texts.push_back(tr("On"));
    d_text_colors[row]=QColor(Qt::darkGreen);
  }

  d_texts[row]=texts;
}


QString RDGpioLogModel::sqlFields() const
{
  QString sql=QString("select ")+
    "EVENT_DATETIME,"+   // 00
    "NUMBER,"+           // 01
    "EDGE "+             // 02
    "from GPIO_EVENTS ";

    return sql;
}
