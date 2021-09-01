// schedrulesmodel.cpp
//
// Data model for Rivendell schedule code rules.
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
#include "schedrulesmodel.h"

SchedRulesModel::SchedRulesModel(const QString &clk_name,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_clock_name=clk_name;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Code"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Max In A Row"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Min Wait"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Do Not Schedule After"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Or After"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Or After"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  updateModel();
}


SchedRulesModel::~SchedRulesModel()
{
}


QPalette SchedRulesModel::palette()
{
  return d_palette;
}


void SchedRulesModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void SchedRulesModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int SchedRulesModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int SchedRulesModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant SchedRulesModel::headerData(int section,Qt::Orientation orient,
					  int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant SchedRulesModel::data(const QModelIndex &index,int role) const
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


unsigned SchedRulesModel::ruleId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


void SchedRulesModel::refresh()
{
  updateModel();
}


void SchedRulesModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where RULE_LINES.ID=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void SchedRulesModel::refresh(unsigned id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      return;
    }
  }
}


void SchedRulesModel::updateModel()
{
  QList<QVariant> texts; 

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+"where "+
    "`RULE_LINES`.`CLOCK_NAME`=\""+RDEscapeString(d_clock_name)+"\" "+
    "order by `RULE_LINES`.`CODE` ";
  beginResetModel();
  d_texts.clear();
  d_ids.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_texts.push_back(texts);
    d_ids.push_back(0);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void SchedRulesModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where `RULE_LINES`.`ID`=%u",d_ids.at(line)); 
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void SchedRulesModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;

  d_ids[row]=q->value(0).toUInt();

  // Code
  texts.push_back(q->value(1));

  // Max in a Row
  texts.push_back(QString::asprintf("%u",q->value(2).toUInt()));

  // Min. Wait
  texts.push_back(QString::asprintf("%u",q->value(3).toUInt()));

  // Do not schedule after
  texts.push_back(q->value(4));

  // Or after
  texts.push_back(q->value(5));

  // Or after
  texts.push_back(q->value(6));

  // Description
  texts.push_back(q->value(7));

  d_texts[row]=texts;
}


QString SchedRulesModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`RULE_LINES`.`ID`,"+            // 00
    "`SCHED_CODES`.`CODE`,"+         // 01
    "`RULE_LINES`.`MAX_ROW`,"+       // 02
    "`RULE_LINES`.`MIN_WAIT`,"+      // 03
    "`RULE_LINES`.`NOT_AFTER`,"+     // 04
    "`RULE_LINES`.`OR_AFTER`,"+      // 05
    "`RULE_LINES`.`OR_AFTER_II`,"+   // 06
    "`SCHED_CODES`.`DESCRIPTION` "+  // 07
    "from `SCHED_CODES` left join `RULE_LINES` "+
    "on `SCHED_CODES`.`CODE`=`RULE_LINES`.`CODE` ";

    return sql;
}
