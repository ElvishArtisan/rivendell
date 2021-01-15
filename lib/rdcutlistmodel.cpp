// rdcutlistmodel.cpp
//
// Data model for Rivendell cut metadata
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

#include "rdconf.h"
#include "rdcutlistmodel.h"
#include "rdescape_string.h"

RDCutListModel::RDCutListModel(bool use_weighting,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_cart_number=0;
  d_use_weighting=use_weighting;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  if(d_use_weighting) {                      // 00
    d_headers.push_back(tr("Wt"));
    d_alignments.push_back(right);
  }
  else {
    d_headers.push_back(tr("Ord"));
    d_alignments.push_back(right);
  }

  d_headers.push_back(tr("Description"));   // 01
  d_alignments.push_back(left);

  d_headers.push_back(tr("Length"));        // 02
  d_alignments.push_back(right);

  d_headers.push_back(tr("Last Played"));   // 03
  d_alignments.push_back(center);

  d_headers.push_back(tr("# of Plays"));    // 04
  d_alignments.push_back(right);

  d_headers.push_back(tr("Source"));        // 05
  d_alignments.push_back(left);

  d_headers.push_back(tr("Ingest"));        // 06
  d_alignments.push_back(left);

  d_headers.push_back(tr("Outcue"));        // 07
  d_alignments.push_back(left);

  d_headers.push_back(tr("Start Date"));    // 08
  d_alignments.push_back(center);

  d_headers.push_back(tr("End Date"));      // 09
  d_alignments.push_back(center);

  d_headers.push_back(tr("Daypart Start")); // 10
  d_alignments.push_back(left);

  d_headers.push_back(tr("Daypart End"));   // 11
  d_alignments.push_back(left);

  d_headers.push_back(tr("Name"));          // 12
  d_alignments.push_back(left);

  d_headers.push_back(tr("SHA1"));          // 13
  d_alignments.push_back(left);
}


QPalette RDCutListModel::palette()
{
  return d_palette;
}


void RDCutListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDCutListModel::setFont(const QFont &font)
{
  d_font=font;
}


int RDCutListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDCutListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDCutListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDCutListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(d_row_index.at(row)).at(col);

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      return d_font;

    case Qt::TextColorRole:
      // Nothing to do!
      break;

    case Qt::BackgroundRole:
      return d_colors.at(d_row_index.at(row));

    default:
      break;
    }
  }

  return QVariant();
}


QString RDCutListModel::cutName(const QModelIndex &row) const
{
  return d_texts.at(d_row_index.at(row.row())).at(12).toString();
}


QModelIndex RDCutListModel::addCut(const QString &name)
{
  QList<QVariant> list;

  beginResetModel();

  for(int i=0;i<columnCount();i++) {
    list.push_back(QString());
  }
  list[12]=name;
  d_texts.push_back(list);
  d_colors.push_back(d_palette.color(QPalette::Background));
  d_row_index.push_back(d_row_index.size());
  updateCutLine(d_texts.size()-1);
  sortRows(d_use_weighting);
  endResetModel();

  for(int i=0;i<d_row_index.size();i++) {
    if(d_row_index.at(i)==(d_row_index.size()-1)) {
      return createIndex(i,0);
    }
  }
  return QModelIndex();
}


void RDCutListModel::removeCut(const QModelIndex &row)
{
  beginResetModel();
  d_texts.removeAt(d_row_index.at(row.row()));
  d_colors.removeAt(d_row_index.at(row.row()));
  d_row_index.removeAt(row.row());
  for(int i=0;i<d_row_index.size();i++) {
    if(d_row_index.at(i)>row.row()) {
      d_row_index[i]--;
    }
  }
  sortRows(d_use_weighting);
  endResetModel();
}


void RDCutListModel::removeCut(const QString &cutname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(12)==cutname) {
      removeCut(createIndex(i,0));
      return;
    }
  }
}


void RDCutListModel::refresh(const QModelIndex &row)
{
  updateCutLine(row.row());
}


void RDCutListModel::refresh(const QString &cutname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(d_row_index.at(i)).at(12)==cutname) {
      updateCutLine(i);
      return;
    }
  }
}


unsigned RDCutListModel::cartNumber() const
{
  return d_cart_number;
}


QModelIndex RDCutListModel::row(const QString &cutname) const
{
  for(int i=0;i<d_row_index.size();i++) {
    if(d_texts.at(d_row_index.at(i)).at(12).toString()==cutname) {
      return createIndex(i,0);
    }
  }
  return QModelIndex();
}


bool RDCutListModel::playOrderDuplicates(QList<int> *values) const
{
  if(d_use_weighting) {
    return false;
  }
  values->clear();
  for(int i=0;i<d_row_index.size();i++) {
    for(int j=1;j<d_row_index.size();j++) {
      if(i!=j) {
	if(d_texts.at(i).at(0)==d_texts.at(j).at(0)) {
	  if(!values->contains(d_texts.at(j).at(0).toInt())) {
	    values->push_back(d_texts.at(j).at(0).toInt());
	  }
	}
      }
    }
  }
  return values->size()>0;
}


void RDCutListModel::setCartNumber(unsigned cartnum)
{
  if(cartnum!=d_cart_number) {
    beginResetModel();
    d_texts.clear();
    d_colors.clear();
    d_row_index.clear();
    QList<QVariant> text;
    for(int i=0;i<columnCount();i++) {
      text.push_back(QVariant());
    }
    QString sql=sqlFields()+QString().sprintf("where CART_NUMBER=%u ",cartnum);
    if(d_use_weighting) {
      sql+="order by CUT_NAME";
    }
    else {
      sql+="order by PLAY_ORDER";
    }
    RDSqlQuery *q=new RDSqlQuery(sql);
    int row=0;
    while(q->next()) {
      d_texts.push_back(text);
      d_colors.push_back(d_palette.color(QPalette::Background));
      d_row_index.push_back(row++);
      updateRow(d_texts.size()-1,q);
    }
    d_cart_number=cartnum;
    endResetModel();
  }
}


void RDCutListModel::processNotification(RDNotification *notify)
{
}


void RDCutListModel::updateRow(int row,RDSqlQuery *q)
{
  QDateTime current_datetime=QDateTime::currentDateTime();

  //
  // Text Values
  //
  if(d_use_weighting) {
    d_texts[d_row_index.at(row)][0]=QString().sprintf("%d",q->value(1).toInt());
  }
  else {
    d_texts[d_row_index.at(row)][0]=QString().sprintf("%d",q->value(0).toInt());
  }
  d_texts[d_row_index.at(row)][1]=q->value(2);
  d_texts[d_row_index.at(row)][2]=RDGetTimeLength(q->value(3).toUInt());
  if(q->value(5).toUInt()>0) {
    d_texts[d_row_index.at(row)][3]=q->value(4).toDateTime().toString("M/d/yy");
  }
  else {
    d_texts[d_row_index.at(row)][3]=tr("Never");
  }
  d_texts[d_row_index.at(row)][4]=q->value(5).toString();
  QString user=q->value(8).toString()+"@";
  if(q->value(8).toString().isEmpty()) {
    user="";
  }
  if(q->value(9).toString().isEmpty()) {
    d_texts[d_row_index.at(row)][5]="["+tr("unknown")+"]";
  }
  else {
    d_texts[d_row_index.at(row)][5]=user+q->value(9).toString();
  }
  if(!q->value(6).toDateTime().isNull()) {
    d_texts[d_row_index.at(row)][6]=q->value(7).toString()+" - "+
      q->value(6).toDateTime().toString("M/d/yy hh:mm:ss");
  }
  d_texts[d_row_index.at(row)][7]=q->value(10).toString();
  if(!q->value(14).toDateTime().isNull()) {
    d_texts[d_row_index.at(row)][8]=
      q->value(14).toDateTime().toString("M/d/yyyy hh:mm:ss");
  }
  else {
    d_texts[d_row_index.at(row)][8]=tr("None");
  }
  if(!q->value(15).toDateTime().isNull()) {
    d_texts[d_row_index.at(row)][9]=q->value(15).toDateTime().toString("M/d/yyyy hh:mm:ss");
  }
  else {
    d_texts[d_row_index.at(row)][9]=tr("None");
  }
  if(!q->value(17).isNull()) {
    d_texts[d_row_index.at(row)][10]=q->value(16).toTime().toString("hh:mm:ss");
    d_texts[d_row_index.at(row)][11]=q->value(17).toTime().toString("hh:mm:ss");
  }
  else {
    d_texts[d_row_index.at(row)][10]=tr("None");
    d_texts[d_row_index.at(row)][11]=tr("None");
  }
  d_texts[d_row_index.at(row)][12]=q->value(11).toString();
  if(q->value(25).toString().isEmpty()) {
    d_texts[d_row_index.at(row)][13]="["+tr("not available")+"]";
  }
  else {
    d_texts[d_row_index.at(row)][13]=q->value(25).toString();
  }

  //
  // Background Color
  //
  if(d_use_weighting&&(q->value(1).toInt()==0)){// zero weight
    d_colors[d_row_index.at(row)]=QColor(RD_CART_ERROR_COLOR);
  } 
  else {
    switch(ValidateCut(q,12,RDCart::NeverValid,current_datetime)) {
    case RDCart::NeverValid:
      d_colors[d_row_index.at(row)]=QColor(RD_CART_ERROR_COLOR);
      break;
	
    case RDCart::ConditionallyValid:
      if((!q->value(13).isNull())&&
	 (q->value(13).toDateTime()<current_datetime)) {
	d_colors[d_row_index.at(row)]=QColor(RD_CART_ERROR_COLOR);
      }
      else {
	d_colors[d_row_index.at(row)]=QColor(RD_CART_CONDITIONAL_COLOR);
      }
      break;

    case RDCart::FutureValid:
      d_colors[d_row_index.at(row)]=QColor(RD_CART_FUTURE_COLOR);
      break;
	
    case RDCart::EvergreenValid:
      d_colors[d_row_index.at(row)]=QColor(RD_CART_EVERGREEN_COLOR);
      break;

    case RDCart::AlwaysValid:
      d_colors[d_row_index.at(row)]=d_palette.color(QPalette::Background);
      break;
    }
  }
}


void RDCutListModel::updateCutLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where CUT_NAME=\""+RDEscapeString(d_texts.at(d_row_index.at(line)).
					 at(12).toString())+"\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      beginResetModel();
      updateRow(line,q);
      sortRows(d_use_weighting);
      endResetModel();
    }
  }
}


QString RDCutListModel::sqlFields() const
{
  QString sql;

  sql=QString("select ")+
    "PLAY_ORDER,"+          // 00
    "WEIGHT,"+              // 01   
    "DESCRIPTION,"+         // 02
    "LENGTH,"+              // 03
    "LAST_PLAY_DATETIME,"+  // 04
    "PLAY_COUNTER,"+        // 05
    "ORIGIN_DATETIME,"+     // 06
    "ORIGIN_NAME,"+         // 07
    "ORIGIN_LOGIN_NAME,"+   // 08
    "SOURCE_HOSTNAME,"+     // 09
    "OUTCUE,"+              // 10
    "CUT_NAME,"+            // 11
    "LENGTH,"+              // 12  offsets begin here
    "EVERGREEN,"+           // 13
    "START_DATETIME,"+      // 14
    "END_DATETIME,"+        // 15
    "START_DAYPART,"+       // 16
    "END_DAYPART,"+         // 17
    "MON,"+                 // 18
    "TUE,"+                 // 19
    "WED,"+                 // 20
    "THU,"+                 // 21
    "FRI,"+                 // 22
    "SAT,"+                 // 23
    "SUN,"+                 // 24
    "SHA1_HASH "+           // 25
    "from CUTS ";

  return sql;
}


void RDCutListModel::sortRows(int use_weighting)
{
  if(use_weighting==1) {  // Sort by cutname
    bool modified;
    do {
      modified=false;
      for(int i=0;i<(d_row_index.size()-1);i++) {
	if(d_texts.at(d_row_index.at(i)).at(12).toString()>
	   d_texts.at(d_row_index.at(i+1)).at(12).toString()) {
	  d_row_index.swap(i,i+1);
	  modified=true;
	}
      }
    } while(modified);
  }
  else {  // Sort by play order
    bool modified;
    do {
      modified=false;
      for(int i=0;i<(d_row_index.size()-1);i++) {
	if(d_texts.at(d_row_index.at(i)).at(0).toInt()>
	   d_texts.at(d_row_index.at(i+1)).at(0).toInt()) {
	  d_row_index.swap(i,i+1);
	  modified=true;
	}
      }
    } while(modified);
  }
}


RDCart::Validity RDCutListModel::ValidateCut(RDSqlQuery *q,unsigned offset,
					     RDCart::Validity prev_validity,
					     const QDateTime &datetime)
{
  if(prev_validity==RDCart::AlwaysValid) {
    return RDCart::AlwaysValid;
  }
  if(q->value(offset).toInt()==0) {                   // Length
    return prev_validity;
  }
  if(q->value(offset+1).toString()=="Y") {            // Evergreen
    return RDCart::EvergreenValid;
  }
  if(q->value(offset+5+datetime.date().dayOfWeek()).toString()!="Y") {
    return prev_validity;
  }
  if(!q->value(offset+2).isNull()) {                  // Start DateTime
    if(q->value(offset+2).toDateTime()>datetime) {
      return RDCart::FutureValid;
    }
  }
  if(!q->value(offset+3).isNull()) {                  // End DateTime
    if(q->value(offset+3).toDateTime()<datetime) {
      return prev_validity;
    }
  }
  if(!q->value(offset+4).isNull()) {                  // Start Daypart
    if(q->value(offset+4).toTime()>datetime.time()) {
      return prev_validity;
    }
  }
  if(!q->value(offset+5).isNull()) {                  // End Daypart
    if(q->value(offset+5).toTime()<datetime.time()) {
      return prev_validity;
    }
  }

  return RDCart::AlwaysValid;
}
