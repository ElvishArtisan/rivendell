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

  if(d_use_weighting) {
    d_headers.push_back(tr("Wt"));
    d_alignments.push_back(right);
  }
  else {
    d_headers.push_back(tr("Ord"));
    d_alignments.push_back(right);
  }

  d_headers.push_back(tr("Description"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Length"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Last Played"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("# of Plays"));
  d_alignments.push_back(right);

  d_headers.push_back(tr("Source"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Ingest"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Outcue"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Start Date"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("End Date"));
  d_alignments.push_back(center);

  d_headers.push_back(tr("Daypart Start"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Daypart End"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Name"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("SHA1"));
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
      return d_texts.at(row).at(col);

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      return d_font;

    case Qt::TextColorRole:
      // Nothing to do!
      break;

    case Qt::BackgroundRole:
      return d_colors.at(row);

    default:
      break;
    }
  }

  return QVariant();
}


QString RDCutListModel::cutName(int row) const
{
  return d_texts.at(row).at(12).toString();
}


int RDCutListModel::addCut(const QString &name)
{
  QList<QVariant> list;

  beginInsertRows(QModelIndex(),d_texts.size(),d_texts.size());

  for(int i=0;i<columnCount();i++) {
    list.push_back(QString());
  }
  list[12]=name;
  d_texts.push_back(list);
  d_colors.push_back(d_palette.color(QPalette::Background));
  refresh(d_texts.size()-1);
  endInsertRows();

  return d_texts.size()-1;
}


void RDCutListModel::removeCut(int row)
{
  beginRemoveRows(QModelIndex(),row,row);

  d_texts.removeAt(row);
  d_colors.removeAt(row);

  endRemoveRows();
}


void RDCutListModel::removeCut(const QString &cutname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(cutName(i)==cutname) {
      removeCut(i);
      return;
    }
  }
}


void RDCutListModel::refresh(int row)
{
  if(row<d_texts.size()) {
    QString sql=sqlFields()+
      "where CUT_NAME=\""+RDEscapeString(d_texts.at(row).at(12).toString())+
      "\"";
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row,q);
      emit dataChanged(createIndex(row,0),createIndex(row,columnCount()));
    }
  }
}


void RDCutListModel::refresh(const QString &cutname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(12)==cutname) {
      refresh(i);
      return;
    }
  }
}


unsigned RDCutListModel::cartNumber() const
{
  return d_cart_number;
}


void RDCutListModel::setCartNumber(unsigned cartnum)
{
  if(cartnum!=d_cart_number) {
    beginResetModel();
    d_texts.clear();
    d_colors.clear();
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
    while(q->next()) {
      d_texts.push_back(text);
      d_colors.push_back(d_palette.color(QPalette::Background));
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
    d_texts[row][0]=QString().sprintf("%d",q->value(1).toInt());
  }
  else {
    d_texts[row][0]=QString().sprintf("%d",q->value(0).toInt());
  }
  d_texts[row][1]=q->value(2);
  d_texts[row][2]=RDGetTimeLength(q->value(3).toUInt());
  if(q->value(5).toUInt()>0) {
    d_texts[row][3]=q->value(4).toDateTime().toString("M/d/yy");
  }
  else {
    d_texts[row][3]=tr("Never");
  }
  d_texts[row][4]=q->value(5).toString();
  QString user=q->value(8).toString()+"@";
  if(q->value(8).toString().isEmpty()) {
    user="";
  }
  if(q->value(9).toString().isEmpty()) {
    d_texts[row][5]="["+tr("unknown")+"]";
  }
  else {
    d_texts[row][5]=user+q->value(9).toString();
  }
  if(!q->value(6).toDateTime().isNull()) {
    d_texts[row][6]=q->value(7).toString()+" - "+
      q->value(6).toDateTime().toString("M/d/yy hh:mm:ss");
  }
  d_texts[row][7]=q->value(10).toString();
  if(!q->value(14).toDateTime().isNull()) {
    d_texts[row][8]=q->value(14).toDateTime().toString("M/d/yyyy hh:mm:ss");
  }
  else {
    d_texts[row][8]=tr("None");
  }
  if(!q->value(15).toDateTime().isNull()) {
    d_texts[row][9]=q->value(15).toDateTime().toString("M/d/yyyy hh:mm:ss");
  }
  else {
    d_texts[row][9]=tr("None");
  }
  if(!q->value(17).isNull()) {
    d_texts[row][10]=q->value(16).toTime().toString("hh:mm:ss");
    d_texts[row][11]=q->value(17).toTime().toString("hh:mm:ss");
  }
  else {
    d_texts[row][10]=tr("None");
    d_texts[row][11]=tr("None");
  }
  d_texts[row][12]=q->value(11).toString();
  if(q->value(25).toString().isEmpty()) {
    d_texts[row][13]="["+tr("not available")+"]";
  }
  else {
    d_texts[row][13]=q->value(25).toString();
  }

  //
  // Background Color
  //
  if(d_use_weighting&&(q->value(1).toInt()==0)){// zero weight
    d_colors[row]=RD_CART_ERROR_COLOR;
    //if(pass==0) {
    //  err=true;
    //}
  } 
  else {
    /*
    RDCart::Validity validity=ValidateCut(q,12,RDCart::NeverValid,current_datetime);
    printf("validity[%d]: %d\n",row,validity);
    switch(validity) {
    */
    switch(ValidateCut(q,12,RDCart::NeverValid,current_datetime)) {
    case RDCart::NeverValid:
      d_colors[row]=RD_CART_ERROR_COLOR;
      //if(pass==0) {
      //err=true;
      //}
      break;
	
    case RDCart::ConditionallyValid:
      if((!q->value(13).isNull())&&
	 (q->value(13).toDateTime()<current_datetime)) {
	d_colors[row]=RD_CART_ERROR_COLOR;
      }
      else {
	d_colors[row]=RD_CART_CONDITIONAL_COLOR;
      }
      //if(pass==0) {
      //err=true;
      //}
      break;

    case RDCart::FutureValid:
      d_colors[row]=RD_CART_FUTURE_COLOR;
      //if(pass==0) {
      //err=true;
      //}
      break;
	
    case RDCart::EvergreenValid:
      d_colors[row]=RD_CART_EVERGREEN_COLOR;
      //if(pass==0) {
      //err=true;
      //}
      break;

    case RDCart::AlwaysValid:
      d_colors[row]=d_palette.color(QPalette::Background);
      break;
    }
  }

  /*
  if(q->value(5).toUInt()>0) {
    l->setText(3,q->value(4).toDateTime().toString("M/d/yy"));
    }
    else {
      l->setText(3,tr("Never"));
    }
    l->setText(4,q->value(5).toString());
    QString user=q->value(8).toString()+"@";
    if(q->value(8).toString().isEmpty()) {
      user="";
    }
    if(q->value(9).toString().isEmpty()) {
      l->setText(5,"["+tr("unknown")+"]");
    }
    else {
      l->setText(5,user+q->value(9).toString());
    }
    if(!q->value(6).toDateTime().isNull()) {
      l->setText(6,q->value(7).toString()+" - "+
		 q->value(6).toDateTime().toString("M/d/yy hh:mm:ss"));
    }
    l->setText(7,q->value(10).toString());
    if(!q->value(14).toDateTime().isNull()) {
      l->setText(8,q->value(14).toDateTime().toString("M/d/yyyy hh:mm:ss"));
    }
    else {
      l->setText(8,tr("None"));
    }
    if(!q->value(15).toDateTime().isNull()) {
      l->setText(9,q->value(15).toDateTime().toString("M/d/yyyy hh:mm:ss"));
    }
    else {
      l->setText(9,tr("None"));
    }
    if(!q->value(17).isNull()) {
      l->setText(10,q->value(16).toTime().toString("hh:mm:ss"));
      l->setText(11,q->value(17).toTime().toString("hh:mm:ss"));
    }
    else {
      l->setText(10,tr("None"));
      l->setText(11,tr("None"));
    }
    l->setText(12,q->value(11).toString());
    if(q->value(25).toString().isEmpty()) {
      l->setText(13,"["+tr("not available")+"]");
    }
    else {
      l->setText(13,q->value(25).toString());
    }
    total_length+=q->value(3).toUInt();
    pass++;
  }
  if(q->size()>0) {
    rdcart_average_length=total_length/q->size();
  }
  else {
    rdcart_average_length=0;
  }
  delete q;
  if(((l=(RDListViewItem *)rdcart_cut_list->firstChild())!=NULL)&&
     ((!err)||rdcart_select_cut)) {
    rdcart_cut_list->setSelected(l,true);
    rdcart_select_cut=false;
  }
  */




  /*
  QList<QVariant> texts;
  QList<QVariant> icons;
  // Log Name
  texts.push_back(q->value(0));
  if((q->value(7).toInt()==q->value(8).toInt())&&
     ((q->value(9).toInt()==0)||(q->value(10).toString()=="Y"))&&
     ((q->value(11).toInt()==0)||(q->value(12).toString()=="Y"))) {
    icons.push_back(d_log_icons->listIcon(RDLogIcons::GreenCheck));
  }
  else {
    icons.push_back(d_log_icons->listIcon(RDLogIcons::RedX));
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
    icons.push_back(d_log_icons->listIcon(RDLogIcons::WhiteBall));
  }
  else {
    if(q->value(10).toString()=="Y") {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::GreenBall));
    }
    else {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::RedBall));
    }
  }

  // Traffic State
  texts.push_back(QString());
  if(q->value(11).toInt()==0) {
    icons.push_back(d_log_icons->listIcon(RDLogIcons::WhiteBall));
  }
  else {
    if(q->value(12).toString()=="Y") {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::GreenBall));
    }
    else {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::RedBall));
    }
  }

  // Tracks
  texts.push_back(QString().sprintf("%d / %d",
				    q->value(7).toInt(),
				    q->value(8).toInt()));
  if(q->value(8).toInt()==0) {
    icons.push_back(d_log_icons->listIcon(RDLogIcons::WhiteBall));
  }
  else {
    if(q->value(8).toInt()==q->value(7).toInt()) {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::GreenBall));
    }
    else {
      icons.push_back(d_log_icons->listIcon(RDLogIcons::RedBall));
    }
  }

  // Start Date
  if(q->value(3).toDate().isNull()) {
    texts.push_back(tr("Always"));
  }
  else {
    texts.push_back(q->value(3).toDate().toString("MM/dd/yyyy"));
  }
  icons.push_back(QVariant());

  // End Date
  if(q->value(4).toDate().isNull()) {
    texts.push_back(tr("Always"));
  }
  else {
    texts.push_back(q->value(4).toDate().toString("MM/dd/yyyy"));
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
  */
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
