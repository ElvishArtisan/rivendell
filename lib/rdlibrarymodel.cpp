// rdlibrarymodel.cpp
//
// Data model for the Rivendell cart library
//
//   (C) Copyright 2021-2024 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdconf.h"
#include "rdescape_string.h"
#include "rdlibrarymodel.h"
#include "rdtimeprobe.h"

//
// Uncomment this to enable model update profiling
//
// #define RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

RDLibraryModel::RDLibraryModel(QObject *parent)
  : QAbstractItemModel(parent)
{
  d_font_metrics=NULL;
  d_bold_font_metrics=NULL;
  d_show_notes=false;
  d_filter_sql="";
  d_cart_limit=RD_MAX_CART_NUMBER+1;  // Effectively "unlimited"
  d_sort_column=0;
  d_sort_order=Qt::AscendingOrder;
  d_sort_clauses[Qt::AscendingOrder]="asc";
  d_sort_clauses[Qt::DescendingOrder]="desc";
  d_filter_set=false;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Cart"));              // 00
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`NUMBER`");

  d_headers.push_back(tr("Group"));             // 01
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`GROUP_NAME`");

  d_headers.push_back(tr("Length"));            // 02
  d_alignments.push_back(right);
  d_order_columns.push_back("`CART`.`FORCED_LENGTH`");

  d_headers.push_back(tr("Talk"));              // 03
  d_alignments.push_back(right);
  d_order_columns.push_back("`CART`.`MAXIMUM_TALK_LENGTH`");

  d_headers.push_back(tr("Title"));             // 04
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`TITLE`");

  d_headers.push_back(tr("Artist"));            // 05
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`ARTIST`");

  d_headers.push_back(tr("Start"));             // 06
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`START_DATETIME`");

  d_headers.push_back(tr("End"));               // 07
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`END_DATETIME`");

  d_headers.push_back(tr("Album"));             // 08
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`ALBUM`");

  d_headers.push_back(tr("Label"));             // 09
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`LABEL`");

  d_headers.push_back(tr("Composer"));          // 10
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`COMPOSER`");

  d_headers.push_back(tr("Conductor"));         // 11
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`CONDUCTOR`");

  d_headers.push_back(tr("Publisher"));         // 12
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`PUBLISHER`");

  d_headers.push_back(tr("Client"));            // 13
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`CLIENT`");

  d_headers.push_back(tr("Agency"));            // 14
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`AGENCY`");

  d_headers.push_back(tr("User Defined"));      // 15
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`USER_DEFINED`");

  d_headers.push_back(tr("Plays"));             // 16
  d_alignments.push_back(right);
  d_order_columns.push_back("`CUTS`.`PLAY_COUNTER`");

  d_headers.push_back(tr("Last Played"));       // 17
  d_alignments.push_back(center);
  d_order_columns.push_back("`CUTS`.`LAST_PLAY_DATETIME`");

  d_headers.push_back(tr("Ingested On"));       // 18
  d_alignments.push_back(center);
  d_order_columns.push_back("`CUTS`.`ORIGIN_DATETIME`");

  d_headers.push_back(tr("Cuts"));              // 19
  d_alignments.push_back(right);
  d_order_columns.push_back("`CART`.`CUT_QUANTITY`");

  d_headers.push_back(tr("Last Cut Played"));   // 20
  d_alignments.push_back(right);
  d_order_columns.push_back("`CART`.`LAST_CUT_PLAYED`");

  d_headers.push_back(tr("Enforce Length"));    // 21
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`ENFORCE_LENGTH`");

  d_headers.push_back(tr("Length Deviation"));  // 22
  d_alignments.push_back(center);
  d_order_columns.push_back("`CART`.`LENGTH_DEVIATION`");

  d_headers.push_back(tr("Owned By"));          // 23
  d_alignments.push_back(left);
  d_order_columns.push_back("`CART`.`OWNER`");

  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(processNotification(RDNotification *)));
}


RDLibraryModel::~RDLibraryModel()
{
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
    return createIndex(row,col,(quintptr)0);
  }
  if((parent.column()==0)&&(parent.internalId()==0)) {
    return createIndex(row,col,(quintptr)(1+parent.row()));
  }
  return QModelIndex();
}


QModelIndex RDLibraryModel::parent(const QModelIndex &index) const
{
  if(index.isValid()&&(index.internalId()>0)) {
    return createIndex(index.internalId()-1,0,(quintptr)0);
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


void RDLibraryModel::sort(int col,Qt::SortOrder order)
{
  d_sort_column=col;
  d_sort_order=order;
  if(d_filter_set) {
    setFilterSql(d_filter_sql,d_cart_limit);
  }
  /*
  printf("RDLibraryModel::sort():\n");
  printf("  d_filter_sql: %s\n",d_filter_sql.toUtf8().constData());
  printf("  d_cart_limit: %d\n",d_cart_limit);
  printf("\n");
  */
}


bool RDLibraryModel::isCart(const QModelIndex &index) const
{
  return index.internalId()==0;
}


bool RDLibraryModel::isCut(const QModelIndex &index) const
{
  return index.internalId()>0;
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


QString RDLibraryModel::cutName(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isCut(index)) {
      return d_cut_cutnames.at(index.internalId()-1).at(index.row());
    }
  }
  return QString();
}


QModelIndex RDLibraryModel::cartRow(unsigned cartnum) const
{
  int pos=d_cart_numbers.indexOf(cartnum);

  if(pos<0) {
    return QModelIndex();
  }
  return createIndex(pos,0,(quintptr)0);
}


QModelIndex RDLibraryModel::cutRow(const QString &cutname) const
{
  int cartpos=d_cart_numbers.indexOf(RDCut::cartNumber(cutname));

  if(cartpos<0) {
    return QModelIndex();
  }
  int cutpos=d_cut_cutnames.at(cartpos).indexOf(cutname);
  if(cutpos<0) {
    return QModelIndex();
  }
  return createIndex(cutpos,0,cartpos+1);
}


RDCart::Type RDLibraryModel::cartType(const QModelIndex &index) const
{
  if(isCart(index)) {
    return d_cart_types.at(index.row());
  }
  return d_cart_types.at(index.internalId()-1);
}


QString RDLibraryModel::cartOwnedBy(const QModelIndex &index)
{
  if(isCart(index)) {
    return d_texts.at(index.row()).at(23).toString();
  }
  return d_texts.at(index.internalId()-1).at(23).toString();
}


QModelIndex RDLibraryModel::addCart(unsigned cartnum)
{
  if(d_cart_numbers.contains(cartnum)) {
    return refreshCart(cartnum);
  }
  else {
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
    d_cut_cutnames.insert(offset,QStringList());
    d_background_colors.insert(offset,QVariant());
    d_cart_types.insert(offset,RDCart::All);

    QString sql=sqlFields()+
      "where "+
      QString::asprintf("`CART`.`NUMBER`=%u",cartnum);
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(offset,q);
    }
    delete q;
    endInsertRows();
    emit rowCountChanged(d_texts.size());

    return createIndex(offset,0,(quintptr)0);
  }
}


void RDLibraryModel::removeCart(const QModelIndex &index)
{
  beginRemoveRows(QModelIndex(),index.row(),index.row());

  d_texts.removeAt(index.row());
  d_notes.removeAt(index.row());
  d_cart_numbers.removeAt(index.row());
  d_cut_texts.removeAt(index.row());
  d_cut_cutnames.removeAt(index.row());
  d_background_colors.removeAt(index.row());
  d_cart_types.removeAt(index.row());
  d_icons.removeAt(index.row());

  endRemoveRows();
  emit rowCountChanged(d_texts.size());
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
      d_cut_cutnames.removeAt(i);
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
    emit dataChanged(index,createIndex(index.row(),columnCount(),(quintptr)0));
  }
}


QModelIndex RDLibraryModel::refreshCart(unsigned cartnum)
{
  QString cartnum_str=QString::asprintf("%06u",cartnum);
  for(int i=0;i<d_texts.size();i++) {
    if(d_texts.at(i).at(0).toString()==cartnum_str) {
      updateCartLine(i);
      emit dataChanged(createIndex(i,0,(quintptr)0),
		       createIndex(i,columnCount(),(quintptr)0));
      return createIndex(i,0);
    }
  }
  return QModelIndex();
}


bool RDLibraryModel::showNotes() const
{
  return d_show_notes;
}


int RDLibraryModel::cartLimit() const
{
  return d_cart_limit;
}


QString RDLibraryModel::orderBySql() const
{
  return d_order_by_sql;
}


void RDLibraryModel::setShowNotes(int state)
{
  d_show_notes=state;
}


void RDLibraryModel::setFilterSql(const QString &sql,int cart_limit)
{
  d_filter_sql=sql;
  d_cart_limit=cart_limit;
  d_order_by_sql="";
  QString fsql=sql;

  if(d_sort_column<0) {  // Use "natural" sort order
    d_order_by_sql+=" order by `CART`.`NUMBER` asc ";
  }
  else {
    d_order_by_sql+=" order by "+d_order_columns.at(d_sort_column)+" "+
      d_sort_clauses.value(d_sort_order);
  }
  d_order_by_sql+=", `CUTS`.`PLAY_ORDER` asc ";
  fsql+=d_order_by_sql;
  d_filter_set=true;

  updateModel(fsql);
}


void RDLibraryModel::processNotification(RDNotification *notify)
{
  QString sql;
  RDSqlQuery *q=NULL;

  if(notify->type()==RDNotification::CartType) {
    switch(notify->action()) {
    case RDNotification::AddAction:
      sql=sqlFields()+
	d_filter_sql+
	QString::asprintf(" && `CART`.`NUMBER`=%u",notify->id().toUInt());
      q=new RDSqlQuery(sql);
      if(q->first()) {
	addCart(notify->id().toUInt());
      }
      delete q;
      break;

    case RDNotification::ModifyAction:
      refreshCart(notify->id().toUInt());
      break;

    case RDNotification::DeleteAction:
      removeCart(notify->id().toUInt());
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;

    }
  }
}


void RDLibraryModel::updateModel(const QString &filter_sql)
{
  if(!d_filter_set) {
    return;
  }
#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  printf("%p - filter_sql: %s\n",this,filter_sql.toUtf8().constData());
  RDTimeProbe *probe=new RDTimeProbe();
  probe->printWaypoint("updateModel - 1");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

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

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 2");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

  //
  // Reload the color table
  //
  d_group_colors.clear();
  sql=QString("select ")+
    "`NAME`,"+   // 00
    "`COLOR` "+  // 01
    "from `GROUPS` order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_colors[q->value(0).toString()]=QColor(q->value(1).toString());
  }
  delete q;

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 3");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

  sql=sqlFields()+
    filter_sql;
  beginResetModel();
  d_texts.clear();
  d_notes.clear();
  d_cart_numbers.clear();
  d_cut_texts.clear();
  d_cut_cutnames.clear();
  d_background_colors.clear();
  d_cart_types.clear();
  d_icons.clear();
  unsigned prev_cartnum=0;
  int carts_loaded=0;

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 4");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

  //  printf("RDLibraryModel::updateModel() SQL: %s\n",sql.toUtf8().constData());
  q=new RDSqlQuery(sql);
  while(q->next()&&(carts_loaded<d_cart_limit)) {
    if(q->value(0).toUInt()!=prev_cartnum) {
      d_texts.push_back(list);
      d_notes.push_back(QVariant());
      d_cart_numbers.push_back(0);
      d_cut_texts.push_back(list_list);
      d_cut_cutnames.push_back(QStringList());
      d_background_colors.push_back(QVariant());
      d_cart_types.push_back(RDCart::All);
      d_icons.push_back(icons);
      updateRow(d_texts.size()-1,q);
      prev_cartnum=q->value(0).toUInt();
      carts_loaded++;
    }
    //    printf("carts_loaded: %d\n",carts_loaded);
  }
  delete q; 

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 5");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

  endResetModel();

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 6");
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING

  emit rowCountChanged(d_texts.size());

#ifdef RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
  probe->printWaypoint("updateModel - 7");
  delete probe;
#endif  // RDLIBRARYMODEL_ENABLE_UPDATE_PROFILING
}


void RDLibraryModel::updateCartLine(int cartline)
{
  QString sql=sqlFields()+
    "where "+
    "`CART`.`NUMBER`="+d_texts.at(cartline).at(0).toString();
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(cartline,q);
    emit dataChanged(createIndex(cartline,0,(quintptr)0),
		     createIndex(cartline,columnCount(),(quintptr)0));
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
      d_icons[row][0]=rda->iconEngine()->typeIcon(RDLogLine::Cart);
    }
    else {
      d_icons[row][0]=
	rda->iconEngine()->typeIcon(RDLogLine::Cart,RDLogLine::Tracker);
    }
    break;
    
  case RDCart::Macro:
    d_icons[row][0]=rda->iconEngine()->typeIcon(RDLogLine::Macro);
    break;
    
  case RDCart::All:
    break;
  }
  d_cart_types[row]=(RDCart::Type)q->value(15).toUInt();
  d_texts[row][0]=                // Cart Number
    QString::asprintf("%06d",q->value(0).toUInt());
  d_cart_numbers[row]=q->value(0).toUInt();
  d_texts[row][1]=q->value(12);   // Group
  d_texts[row][2]=RDGetTimeLength(q->value(1).toUInt());  // Total Length
  if(q->value(16).toUInt()==1) {
    d_texts[row][3]=              // Talk Length
      RDGetTimeLength(q->value(28).toUInt()-q->value(27).toUInt());
  }
  else {
    d_texts[row][3]="0:00";
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
  d_texts[row][16]=QString::asprintf("%u",q->value(32).toUInt()); // Play Quan.
  d_texts[row][17]=rda->shortDateString(q->value(33).toDateTime().date());
  d_texts[row][18]=rda->shortDateString(q->value(34).toDateTime().date());
  d_texts[row][19]=               // Cut Quan
    QString::asprintf("%u",q->value(16).toUInt());
  d_texts[row][20]=               // Last Cut Played
    QString::asprintf("%u",q->value(17).toUInt());
  d_texts[row][21]=q->value(18);  // Enforce Length
  d_texts[row][22]=               // Length Deviation
    QString::asprintf("%u",q->value(20).toUInt());
  d_texts[row][23]=q->value(21);  // Owned By
  d_notes[row]=q->value(30).toString();

  if(q->value(15).toUInt()==RDCart::Audio) {
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
	QColor(palette().color(QPalette::Active,QPalette::Base));
      break;

    case RDCart::EvergreenValid:
      d_background_colors[row]=QColor(RD_CART_EVERGREEN_COLOR);
      break;
    }
  }
  if(q->value(15).toUInt()==RDCart::Macro) {
    d_background_colors[row]=
      QColor(palette().color(QPalette::Active,QPalette::Base));
  }

  //
  // Cut Attributes
  //
  d_cut_texts[row].clear();
  d_cut_cutnames[row].clear();
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }

  do {
    if(q->value(24).isNull()) {
      return;  // No cuts!
    }
    // Process
    d_cut_cutnames[row].push_back(q->value(24).toString());
    d_cut_texts[row].push_back(list);
    d_cut_texts[row].back()[0]=tr("Cut")+  // Cut Number
      QString::asprintf(" %03d",RDCut::cutNumber(q->value(24).toString()));
    d_cut_texts[row].back()[2]=  // Length
      RDGetTimeLength(q->value(26).toUInt()-q->value(25).toUInt());
    d_cut_texts[row].back()[3]=  // Talk Length
      RDGetTimeLength(q->value(28).toUInt()-q->value(27).toUInt());
    d_cut_texts[row].back()[4]=q->value(29).toString();  // Description
    d_cut_texts[row].back()[16]=QString::asprintf("%d",q->value(32).toUInt());
    d_cut_texts[row].back()[17]=
      rda->shortDateString(q->value(33).toDateTime().date());
    d_cut_texts[row].back()[18]=
      rda->shortDateString(q->value(34).toDateTime().date());
  } while(q->next()&&(q->value(0).toUInt()==cartnum));
  q->previous();
}


QString RDLibraryModel::sqlFields() const
{
QString sql=QString("select distinct ")+
  "`CART`.`NUMBER`,"+             // 00
  "`CART`.`FORCED_LENGTH`,"+      // 01
  "`CART`.`TITLE`,"+              // 02
  "`CART`.`ARTIST`,"+             // 03
  "`CART`.`ALBUM`,"+              // 04
  "`CART`.`LABEL`,"+              // 05
  "`CART`.`CLIENT`,"+             // 06
  "`CART`.`AGENCY`,"+             // 07
  "`CART`.`USER_DEFINED`,"+       // 08
  "`CART`.`COMPOSER`,"+           // 09
  "`CART`.`PUBLISHER`,"+          // 10
  "`CART`.`CONDUCTOR`,"+          // 11
  "`CART`.`GROUP_NAME`,"+         // 12
  "`CART`.`START_DATETIME`,"+     // 13
  "`CART`.`END_DATETIME`,"+       // 14
  "`CART`.`TYPE`,"+               // 15
  "`CART`.`CUT_QUANTITY`,"+       // 16
  "`CART`.`LAST_CUT_PLAYED`,"+    // 17
  "`CART`.`ENFORCE_LENGTH`,"+     // 18
  "`CART`.`PRESERVE_PITCH`,"+     // 19
  "`CART`.`LENGTH_DEVIATION`,"+   // 20
  "`CART`.`OWNER`,"+              // 21
  "`CART`.`VALIDITY`,"+           // 22
  "`GROUPS`.`COLOR`,"+            // 23
  "`CUTS`.`CUT_NAME`,"+           // 24
  "`CUTS`.`START_POINT`,"+        // 25
  "`CUTS`.`END_POINT`,"+          // 26
  "`CUTS`.`TALK_START_POINT`,"+   // 27
  "`CUTS`.`TALK_END_POINT`,"+     // 28
  "`CUTS`.`DESCRIPTION`,"+        // 29
  "`CART`.`NOTES`,"+              // 30
  "`CUTS`.`PLAY_ORDER`,"+         // 31
  "`CUTS`.`PLAY_COUNTER`,"+       // 32
  "`CUTS`.`LAST_PLAY_DATETIME`,"+ // 33
  "`CUTS`.`ORIGIN_DATETIME` "+    // 34
  "from `CART` "+
  "left join `GROUPS` on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` "+
  "left join `CUTS` on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` "+
  "left join `CART_SCHED_CODES` "+
  "on `CART`.`NUMBER`=`CART_SCHED_CODES`.`CART_NUMBER` ";

  return sql;
}


QByteArray RDLibraryModel::DumpIndex(const QModelIndex &index,const QString &caption) const
{
  QByteArray ret;

  if(!caption.isEmpty()) {
    ret+=(caption+": ").toUtf8();
  }
  if(index.isValid()) {
    ret+=QString::asprintf("QModelIndex(%d,%d,%llu)",
			   index.row(),index.column(),index.internalId()).
      toUtf8();
  }
  else {
    ret+=QString("QModelIndex()").toUtf8();
  }
  return ret;
}
