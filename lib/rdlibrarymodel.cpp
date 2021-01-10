// rdlibrarymodel.cpp
//
// Data model for the Rivendell cart library
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
#include "rdescape_string.h"
#include "rdlibrarymodel.h"

RDLibraryModel::RDLibraryModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_log_icons=new RDLogIcons();

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Cart"));              // 00
  d_alignments.push_back(center);

  d_headers.push_back(tr("Group"));             // 01
  d_alignments.push_back(center);

  d_headers.push_back(tr("Length"));            // 02
  d_alignments.push_back(right);

  d_headers.push_back(tr("Talk"));              // 03
  d_alignments.push_back(right);

  d_headers.push_back(tr("Title"));             // 04
  d_alignments.push_back(left);

  d_headers.push_back(tr("Artist"));            // 05
  d_alignments.push_back(left);

  d_headers.push_back(tr("Start"));             // 06
  d_alignments.push_back(center);

  d_headers.push_back(tr("End"));               // 07
  d_alignments.push_back(center);

  d_headers.push_back(tr("Album"));             // 08
  d_alignments.push_back(left);

  d_headers.push_back(tr("Label"));             // 09
  d_alignments.push_back(left);

  d_headers.push_back(tr("Composer"));          // 10
  d_alignments.push_back(left);

  d_headers.push_back(tr("Conductor"));         // 11
  d_alignments.push_back(left);

  d_headers.push_back(tr("Publisher"));         // 12
  d_alignments.push_back(left);

  d_headers.push_back(tr("Client"));            // 13
  d_alignments.push_back(left);

  d_headers.push_back(tr("Agency"));            // 14
  d_alignments.push_back(left);

  d_headers.push_back(tr("User Defined"));      // 15
  d_alignments.push_back(left);

  d_headers.push_back(tr("Cuts"));              // 16
  d_alignments.push_back(right);

  d_headers.push_back(tr("Last Cut Played"));   // 17
  d_alignments.push_back(right);

  d_headers.push_back(tr("Enforce Length"));    // 18
  d_alignments.push_back(center);

  d_headers.push_back(tr("Length Deviation"));  // 19
  d_alignments.push_back(center);

  d_headers.push_back(tr("Owned By"));          // 20
  d_alignments.push_back(left);
}


RDLibraryModel::~RDLibraryModel()
{
  delete d_log_icons;
}


QPalette RDLibraryModel::palette()
{
  return d_palette;
}


void RDLibraryModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDLibraryModel::setFont(const QFont &font)
{
  d_font=font;

  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RDLibraryModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDLibraryModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDLibraryModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDLibraryModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      return d_icons.at(row).at(col);

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      if(col==1) {  // Group
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      if(col==1) {
	return d_group_colors.value(d_texts.at(row).at(1).toString());
      }
      break;

    case Qt::BackgroundRole:
      return d_background_colors.at(row);

    default:
      break;
    }
  }

  return QVariant();
}


unsigned RDLibraryModel::cartNumber(int row) const
{
  return d_texts.at(row).at(0).toUInt();
}


RDCart::Type RDLibraryModel::cartType(int row) const
{
  return d_cart_types.at(row);
}


QString RDLibraryModel::cartOwnedBy(int row)
{
  return d_texts.at(row).at(20).toString();
}


int RDLibraryModel::addCart(unsigned cartnum)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(cartnum<d_texts.at(i).at(0).toUInt()) {
      offset=i;
      break;
    }
  }
  
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_icons.insert(offset,list);
  d_texts.insert(offset,list);
  d_background_colors.insert(offset,QVariant());
  d_cart_types.insert(offset,RDCart::All);

  QString sql=sqlFields()+
    "where "+
    QString().sprintf("CART.NUMBER=%u",cartnum);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(offset,q);
  }
  delete q;
  endInsertRows();
  emit rowCountChanged(d_texts.size());

  return offset;
}


void RDLibraryModel::removeCart(unsigned cartnum)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toUInt()==cartnum) {
      beginRemoveRows(QModelIndex(),i,i);

      d_texts.removeAt(i);
      d_background_colors.removeAt(i);
      d_cart_types.removeAt(i);
      d_icons.removeAt(i);

      endRemoveRows();
      emit rowCountChanged(d_texts.size());
      return;
    }
  }
}


void RDLibraryModel::refreshRow(int row)
{
  QString sql=sqlFields()+
    "where "+
    QString().sprintf("CART.NUMBER=%u",cartNumber(row));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(row,q);
    emit dataChanged(createIndex(row,0),createIndex(row,columnCount()));
  }
  delete q;
}


void RDLibraryModel::refreshCart(unsigned cartnum)
{
  QString cartnum_str=QString().sprintf("%06u",cartnum);
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toString()==cartnum_str) {
      refreshRow(i);
    }
  }
}


void RDLibraryModel::setFilterSql(const QString &sql)
{
  updateModel(sql);
}


void RDLibraryModel::processNotification(RDNotification *notify)
{
}


void RDLibraryModel::updateModel(const QString &filter_sql)
{
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Reload the color table
  //
  d_group_colors.clear();
  sql=QString("select ")+
    "NAME,"+   // 00
    "COLOR "+  // 01
    "from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_colors[q->value(0).toString()]=QColor(q->value(1).toString());
  }
  delete q;

  sql=sqlFields()+
    filter_sql;
  beginResetModel();
  d_texts.clear();
  d_background_colors.clear();
  d_cart_types.clear();
  d_icons.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QList<QVariant> texts; 
    QList<QVariant> icons;
    for(int i=0;i<columnCount();i++) {
      texts.push_back(QVariant());
      icons.push_back(QVariant());
    }
    d_texts.push_back(texts);
    d_background_colors.push_back(QVariant());
    d_cart_types.push_back(RDCart::All);
    d_icons.push_back(icons);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
  emit rowCountChanged(d_texts.size());
}


void RDLibraryModel::updateRow(int row,RDSqlQuery *q)
{
  switch((RDCart::Type)q->value(15).toUInt()) {
  case RDCart::Audio:
    if(q->value(21).isNull()) {
      d_icons[row][0]=d_log_icons->typeIcon(RDLogLine::Cart);
    }
    else {
      d_icons[row][0]=
	d_log_icons->typeIcon(RDLogLine::Cart,RDLogLine::Tracker);
    }
    break;
    
  case RDCart::Macro:
    d_icons[row][0]=d_log_icons->typeIcon(RDLogLine::Macro);
    break;
    
  case RDCart::All:
    break;
  }
  d_cart_types[row]=(RDCart::Type)q->value(15).toUInt();
  d_texts[row][0]=                // Cart Number
    QString().sprintf("%06d",q->value(0).toUInt());
  d_texts[row][1]=q->value(12);   // Group
  if(q->value(16).toUInt()==1) {
    d_texts[row][2]=RDGetTimeLength(q->value(1).toUInt());  // Total Length
    d_texts[row][3]=              // Talk Length
      RDGetTimeLength(q->value(25).toUInt()-q->value(24).toUInt());
  }

  d_texts[row][4]=q->value(2);    // Title
  d_texts[row][5]=q->value(3);    // Artist
  if(!q->value(13).toDateTime().isNull()) {  // Start DateTime
    d_texts[row][6]=
      q->value(13).toDateTime().toString("MM/dd/yyyy - hh:mm:ss");
  }
  else {
    d_texts[row][6]=QVariant();
  }

  if(!q->value(14).toDateTime().isNull()) {  // End DateTime
    d_texts[row][7]=q->value(14).toDateTime().toString("MM/dd/yyyy - hh:mm:ss");
  }
  else {
    d_texts[row][7]=tr("TFN");
  }
  d_texts[row][8]=q->value(4);    // Album
  d_texts[row][9]=q->value(5);    // Label
  d_texts[row][10]=q->value(9);   // Composer
  d_texts[row][11]=q->value(11);  // Conductor
  d_texts[row][12]=q->value(10);  // Publisher
  d_texts[row][13]=q->value(6);   // Client
  d_texts[row][14]=q->value(7);   // Agency
  d_texts[row][15]=q->value(8);   // User Defined
  d_texts[row][16]=               // Cut Quan
    QString().sprintf("%u",q->value(16).toUInt());
  d_texts[row][17]=               // Last Cut Played
    QString().sprintf("%u",q->value(17).toUInt());
  d_texts[row][18]=q->value(18);  // Enforce Length
  d_texts[row][19]=               // Length Deviation
    QString().sprintf("%u",q->value(20).toUInt());
  d_texts[row][20]=q->value(21);  // Owned By

  switch((RDCart::Validity)q->value(22).toUInt()) {
  case RDCart::NeverValid:
    d_background_colors[row]=QColor(RD_CART_ERROR_COLOR);
    break;

  case RDCart::ConditionallyValid:
    if(q->value(14).toDateTime().isValid()&&
       (q->value(14).toDateTime()<QDateTime::currentDateTime())) {
      d_background_colors[row]= QColor(RD_CART_ERROR_COLOR);
    }
    else {
      d_background_colors[row]=QColor(RD_CART_CONDITIONAL_COLOR);
    }
    break;

  case RDCart::FutureValid:
    d_background_colors[row]=QColor(RD_CART_FUTURE_COLOR);
    break;

  case RDCart::AlwaysValid:
    d_background_colors[row]=
      QColor(palette().color(QPalette::Active,QColorGroup::Base));
    break;

  case RDCart::EvergreenValid:
    d_background_colors[row]=QColor(RD_CART_EVERGREEN_COLOR);
    break;
  }
}


QString RDLibraryModel::sqlFields() const
{
QString sql=QString("select ")+
  "CART.NUMBER,"+             // 00
  "CART.FORCED_LENGTH,"+      // 01
  "CART.TITLE,"+              // 02
  "CART.ARTIST,"+             // 03
  "CART.ALBUM,"+              // 04
  "CART.LABEL,"+              // 05
  "CART.CLIENT,"+             // 06
  "CART.AGENCY,"+             // 07
  "CART.USER_DEFINED,"+       // 08
  "CART.COMPOSER,"+           // 09
  "CART.PUBLISHER,"+          // 10
  "CART.CONDUCTOR,"+          // 11
  "CART.GROUP_NAME,"+         // 12
  "CART.START_DATETIME,"+     // 13
  "CART.END_DATETIME,"+       // 14
  "CART.TYPE,"+               // 15
  "CART.CUT_QUANTITY,"+       // 16
  "CART.LAST_CUT_PLAYED,"+    // 17
  "CART.ENFORCE_LENGTH,"+     // 18
  "CART.PRESERVE_PITCH,"+     // 19
  "CART.LENGTH_DEVIATION,"+   // 20
  "CART.OWNER,"+              // 21
  "CART.VALIDITY,"+           // 22
  "GROUPS.COLOR,"+            // 23
  "CUTS.TALK_START_POINT,"+   // 24
  "CUTS.TALK_END_POINT "+     // 25
  "from CART "+
  "left join GROUPS on CART.GROUP_NAME=GROUPS.NAME "+
  "left join CUTS on CART.NUMBER=CUTS.CART_NUMBER ";

  return sql;
}
