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
  : QAbstractItemModel(parent)
{
  d_log_icons=new RDLogIcons();
  d_font_metrics=NULL;
  d_bold_font_metrics=NULL;
  d_show_notes=false;

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
  if(d_font_metrics!=NULL) {
    delete d_font_metrics;
  }
  d_font_metrics=new QFontMetrics(d_font);

  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
  if(d_bold_font_metrics!=NULL) {
    delete d_bold_font_metrics;
  }
  d_bold_font_metrics=new QFontMetrics(d_bold_font);
}


QModelIndex RDLibraryModel::index(int row,int col,
				  const QModelIndex &parent) const
{
  if(!parent.isValid()) {
    return createIndex(row,col,(quint32)0);
  }
  if((parent.column()==0)&&(parent.internalId()==0)) {
    return createIndex(row,col,(quint32)(1+parent.row()));
  }
  return QModelIndex();
}


QModelIndex RDLibraryModel::parent(const QModelIndex &index) const
{
  if(index.isValid()&&(index.internalId()>0)) {
    return createIndex(index.internalId()-1,0,(quint32)0);
  }
  return QModelIndex();
}


int RDLibraryModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDLibraryModel::rowCount(const QModelIndex &parent) const
{
  if(parent.isValid()) {
    if(parent.internalId()==0) {
      return d_cut_texts.at(parent.row()).size();
    }
    return d_cut_texts.at(parent.internalId()-1).size();
  }
  return d_texts.size();
}


bool RDLibraryModel::hasChildren(const QModelIndex &parent) const
{
  if(parent.isValid()) {
    if((parent.internalId()==0)&&(parent.column()==0)) {
      return d_cut_texts.at(parent.row()).size()>0;
    }
    return false;
  }
  return true;
}


Qt::ItemFlags RDLibraryModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled;
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
  QPixmap pix;

  if(!index.isValid()) {
    return QVariant();
  }
  if(index.internalId()>0) {   // Cuts
    if(row<d_cut_texts.at(index.internalId()-1).size()) {
      switch((Qt::ItemDataRole)role) {
      case Qt::DisplayRole:
	return d_cut_texts.at(index.internalId()-1).at(row).at(col);

      case Qt::ToolTipRole:
	if(d_show_notes) {
	  return d_notes.at(index.internalId()-1);
	}

      case Qt::TextAlignmentRole:
	return d_alignments.at(col);

      case Qt::SizeHintRole:
	return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+
		     d_font_metrics->width(d_cut_texts.
		     at(index.internalId()-1).at(row).at(col).toString()),
		     RD_LISTWIDGET_ITEM_HEIGHT);
      default:
	break;
      }
    }    
  }
  else {   // Carts
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

      case Qt::ToolTipRole:
	if(d_show_notes) {
	  return d_notes.at(row);
	}

      case Qt::TextColorRole:
	if(col==1) {
	  return d_group_colors.value(d_texts.at(row).at(1).toString());
	}
	break;

      case Qt::BackgroundRole:
	return d_background_colors.at(row);

      case Qt::SizeHintRole:
	return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+
		     (d_icons.at(row).at(col).value<QPixmap>().width())+
		     d_font_metrics->width(d_texts.at(row).at(col).toString()),
		     RD_LISTWIDGET_ITEM_HEIGHT);

      default:
	break;
      }
    }
  }

  return QVariant();
}


bool RDLibraryModel::isCart(const QModelIndex &index) const
{
  return index.internalId()==0;
}


unsigned RDLibraryModel::cartNumber(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isCart(index)) {
      return d_cart_numbers.at(index.row());
    }
    return d_cart_numbers.at(index.internalId()-1);
  }
  return 0;
}


QModelIndex RDLibraryModel::cartRow(unsigned cartnum) const
{
  int pos=d_cart_numbers.indexOf(cartnum);

  if(pos<0) {
    return QModelIndex();
  }
  return createIndex(pos,0,0);
}


RDCart::Type RDLibraryModel::cartType(const QModelIndex &index) const
{
  if(isCart(index)) {
    return d_cart_types.at(index.row());
  }
  return d_cart_types.at(index.internalId()-1);
}


QString RDLibraryModel::cutName(const QModelIndex &index) const
{
  if(isCart(index)) {
    return QString();
  }
  return RDCut::cutName(d_cart_numbers.at(index.internalId()-1),
			d_cut_texts.at(index.internalId()-1).at(index.row()).
			at(0).toString().right(3).toInt());
}


QString RDLibraryModel::cartOwnedBy(const QModelIndex &index)
{
  if(isCart(index)) {
    return d_texts.at(index.row()).at(20).toString();
  }
  return d_texts.at(index.internalId()-1).at(20).toString();
}


QModelIndex RDLibraryModel::addCart(unsigned cartnum)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(cartnum<d_cart_numbers.at(i)) {
      offset=i;
      break;
    }
  }
  
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  QList<QList<QVariant> > list_list;
  list_list.push_back(list);
  d_icons.insert(offset,list);
  d_texts.insert(offset,list);
  d_notes.insert(offset,QVariant());
  d_cart_numbers.insert(offset,0);
  d_cut_texts.insert(offset,list_list);
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

  return createIndex(offset,0,0);
}


void RDLibraryModel::removeCart(unsigned cartnum)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_cart_numbers.at(i)==cartnum) {
      beginRemoveRows(QModelIndex(),i,i);

      d_texts.removeAt(i);
      d_notes.removeAt(i);
      d_cart_numbers.removeAt(i);
      d_cut_texts.removeAt(i);
      d_background_colors.removeAt(i);
      d_cart_types.removeAt(i);
      d_icons.removeAt(i);

      endRemoveRows();
      emit rowCountChanged(d_texts.size());
      return;
    }
  }
}


void RDLibraryModel::refreshRow(const QModelIndex &index)
{
  if(isCart(index)) {
    updateCartLine(index.row());
    emit dataChanged(index,createIndex(index.row(),columnCount(),0));
  }
}


void RDLibraryModel::refreshCart(unsigned cartnum)
{
  QString cartnum_str=QString().sprintf("%06u",cartnum);
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toString()==cartnum_str) {
      updateCartLine(i);
      emit dataChanged(createIndex(i,0,0),createIndex(i,columnCount(),0));
    }
  }
}


bool RDLibraryModel::showNotes() const
{
  return d_show_notes;
}


void RDLibraryModel::setShowNotes(int state)
{
  d_show_notes=state;
}


void RDLibraryModel::setFilterSql(const QString &sql)
{
  //  printf("FILTER SQL: %s\n",sql.toUtf8().constData());
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
  // Placeholder values
  //
  QList<QVariant> list;
  QList<QVariant> icons;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
    icons.push_back(QVariant());
  }
  QList<QList<QVariant> > list_list;
  list_list.push_back(list);

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
  //  printf("SQL: %s\n",sql.toUtf8().constData());
  beginResetModel();
  d_texts.clear();
  d_notes.clear();
  d_cart_numbers.clear();
  d_cut_texts.clear();
  d_background_colors.clear();
  d_cart_types.clear();
  d_icons.clear();
  unsigned prev_cartnum=0;
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toUInt()!=prev_cartnum) {
      d_texts.push_back(list);
      d_notes.push_back(QVariant());
      d_cart_numbers.push_back(0);
      d_cut_texts.push_back(list_list);
      d_background_colors.push_back(QVariant());
      d_cart_types.push_back(RDCart::All);
      d_icons.push_back(icons);
      updateRow(d_texts.size()-1,q);
      prev_cartnum=q->value(0).toUInt();
    }
  }
  delete q;
  endResetModel();
  emit rowCountChanged(d_texts.size());
}


void RDLibraryModel::updateCartLine(int cartline)
{
  QString sql=sqlFields()+
    "where "+
    "CART.NUMBER="+d_texts.at(cartline).at(0).toString();
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(cartline,q);
    emit dataChanged(createIndex(cartline,0,0),
		     createIndex(cartline,columnCount(),(quint32)0));
  }
  delete q;
}


void RDLibraryModel::updateRow(int row,RDSqlQuery *q)
{
  unsigned cartnum=q->value(0).toUInt();

  //
  // Text Values (Qt::Display)
  //
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
  d_cart_numbers[row]=q->value(0).toUInt();
  d_texts[row][1]=q->value(12);   // Group
  if(q->value(16).toUInt()==1) {
    d_texts[row][2]=RDGetTimeLength(q->value(1).toUInt());  // Total Length
    d_texts[row][3]=              // Talk Length
      RDGetTimeLength(q->value(28).toUInt()-q->value(27).toUInt());
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
  d_notes[row]=q->value(30).toString();

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

  //  printf("start at: %d\n",q->at());

  //
  // Cut Attributes
  //
  d_cut_texts[row].clear();
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }

  do {
    if(q->value(24).isNull()) {
      return;  // No cuts!
    }
    // Process
    d_cut_texts[row].push_back(list);
    d_cut_texts[row].back()[0]=tr("Cut")+  // Cut Number
      QString().sprintf(" %03d",RDCut::cutNumber(q->value(24).toString()));
    d_cut_texts[row].back()[2]=  // Length
      RDGetTimeLength(q->value(26).toUInt()-q->value(25).toUInt());
    d_cut_texts[row].back()[3]=  // Talk Length
      RDGetTimeLength(q->value(28).toUInt()-q->value(27).toUInt());
    d_cut_texts[row].back()[4]=q->value(29).toString();  // Description
  } while(q->next()&&(q->value(0).toUInt()==cartnum));
  q->previous();
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
  "CUTS.CUT_NAME,"+           // 24
  "CUTS.START_POINT,"+        // 25
  "CUTS.END_POINT,"+          // 26
  "CUTS.TALK_START_POINT,"+   // 27
  "CUTS.TALK_END_POINT,"+     // 28
  "CUTS.DESCRIPTION,"+        // 29
  "CART.NOTES "+              // 30
  "from CART "+
  "left join GROUPS on CART.GROUP_NAME=GROUPS.NAME "+
  "left join CUTS on CART.NUMBER=CUTS.CART_NUMBER ";

  return sql;
}


QByteArray RDLibraryModel::DumpIndex(const QModelIndex &index,const QString &caption) const
{
  QByteArray ret;

  if(!caption.isEmpty()) {
    ret+=(caption+": ").toUtf8();
  }
  if(index.isValid()) {
    ret+=QString().sprintf("QModelIndex(%d,%d,%llu)",
			   index.row(),index.column(),index.internalId()).
      toUtf8();
  }
  else {
    ret+=QString("QModelIndex()").toUtf8();
  }
  return ret;
}
