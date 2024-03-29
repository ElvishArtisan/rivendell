// rdfeedlistmodel.cpp
//
// Data model for Rivendell RSS feeds
//
//   (C) Copyright 2021-2023 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdfeed.h"
#include "rdfeedlistmodel.h"
#include "rdpodcast.h"

RDFeedListModel::RDFeedListModel(bool is_admin,bool incl_none,QObject *parent)
  : QAbstractItemModel(parent)
{
  d_is_admin=is_admin;
  d_include_none=incl_none;
  d_font_metrics=NULL;
  d_bold_font_metrics=NULL;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Key Name"));       // 00
  d_alignments.push_back(left);

  d_headers.push_back(tr("Feed Title"));     // 01
  d_alignments.push_back(left);

  d_headers.push_back(tr("Casts"));          // 02
  d_alignments.push_back(center);

  d_headers.push_back(tr("Creation Date"));  // 03
  d_alignments.push_back(center);

  d_headers.push_back(tr("Auto Post"));      // 04
  d_alignments.push_back(center);

  d_headers.push_back(tr("Superfeed"));      // 05
  d_alignments.push_back(center);

  d_headers.push_back(tr("Public URL"));     // 06
  d_alignments.push_back(left);

  if(d_is_admin) {
    changeUser();
  }
}


RDFeedListModel::~RDFeedListModel()
{
}


QPalette RDFeedListModel::palette()
{
  return d_palette;
}


void RDFeedListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDFeedListModel::setFont(const QFont &font)
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


QModelIndex RDFeedListModel::index(int row,int col,
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


QModelIndex RDFeedListModel::parent(const QModelIndex &index) const
{
  if(index.isValid()&&(index.internalId()>0)) {
    return createIndex(index.internalId()-1,0,(quintptr)0);
  }
  return QModelIndex();
}


int RDFeedListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDFeedListModel::rowCount(const QModelIndex &parent) const
{
  if(parent.isValid()) {
    if(parent.internalId()==0) {
      return d_cast_texts.at(parent.row()).size();
    }
    return d_cast_texts.at(parent.internalId()-1).size();
  }
  return d_texts.size();
}


bool RDFeedListModel::hasChildren(const QModelIndex &parent) const
{
  if(parent.isValid()) {
    if((parent.internalId()==0)&&(parent.column()==0)) {
      return d_cast_texts.at(parent.row()).size()>0;
    }
    return false;
  }
  return true;
}


Qt::ItemFlags RDFeedListModel::flags(const QModelIndex &index) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}


QVariant RDFeedListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDFeedListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();
  QPixmap pix;

  if(!index.isValid()) {
    return QVariant();
  }
  if(index.internalId()>0) {   // Casts
    if(row<d_cast_texts.at(index.internalId()-1).size()) {
      switch((Qt::ItemDataRole)role) {
      case Qt::DisplayRole:
	return d_cast_texts.at(index.internalId()-1).at(row).at(col);

      case Qt::DecorationRole:
	if(col==0) {
	  return d_cast_icons.at(index.internalId()-1).at(row);
	}
	break;

      case Qt::TextAlignmentRole:
	return d_alignments.at(col);

      case Qt::SizeHintRole:
	return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+
		     d_font_metrics->width(d_cast_texts.
		     at(index.internalId()-1).at(row).at(col).toString()),
		     24);
      default:
	break;
      }
    }    
  }
  else {   // Feeds
    if(row<d_texts.size()) {
      switch((Qt::ItemDataRole)role) {
      case Qt::DisplayRole:
	return d_texts.at(row).at(col);

      case Qt::DecorationRole:
	return d_icons.at(row).at(col);

      case Qt::TextAlignmentRole:
	return d_alignments.at(col);

      case Qt::FontRole:
	if(col==0) {  // Key Name
	  return d_bold_font;
	}
	return d_font;

      case Qt::TextColorRole:
	// Nothing to do here!
	break;

      case Qt::BackgroundRole:
	// Nothing to do here!
	break;

      case Qt::SizeHintRole:
	return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+
		     (d_icons.at(row).at(col).value<QPixmap>().width())+
		     d_font_metrics->width(d_texts.at(row).at(col).toString()),
		     40);

      default:
	break;
      }
    }
  }

  return QVariant();
}


bool RDFeedListModel::isFeed(const QModelIndex &index) const
{
  return index.internalId()==0;
}


bool RDFeedListModel::isCast(const QModelIndex &index) const
{
  return index.internalId()>0;
}


QString RDFeedListModel::keyName(int row) const
{
  return d_key_names.at(row);
}


QString RDFeedListModel::keyName(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isFeed(index)) {
      return d_key_names.at(index.row());
    }
    return d_key_names.at(index.internalId()-1);
  }
  return QString();
}


unsigned RDFeedListModel::feedId(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isFeed(index)) {
      return d_feed_ids.at(index.row());
    }
    return d_feed_ids.at(index.internalId()-1);
  }
  return 0;
}


QString RDFeedListModel::publicUrl(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isFeed(index)) {
      return d_texts.at(index.row()).at(6).toString();
    }
    return d_texts.at(index.internalId()-1).at(6).toString();
  }
  return QString();
}


unsigned RDFeedListModel::castId(const QModelIndex &index) const
{
  if(index.isValid()) {
    if(isCast(index)) {
      return d_cast_ids.at(index.internalId()-1).at(index.row());
    }
  }
  return 0;
}


QModelIndex RDFeedListModel::feedRow(const QString &keyname) const
{
  int pos=d_key_names.indexOf(keyname);

  if(pos<0) {
    return QModelIndex();
  }
  return createIndex(pos,0,(quintptr)0);
}


QModelIndex RDFeedListModel::castRow(unsigned cast_id) const
{
  for(int i=0;i<d_cast_ids.size();i++) {
    for(int j=0;j<d_cast_ids.at(i).size();j++) {
      if(d_cast_ids.at(i).at(j)==cast_id) {
	return createIndex(j,0,i+1);
      }
    }
  }

  return QModelIndex();
}


QModelIndex RDFeedListModel::addFeed(const QString &keyname)
{
  //
  // Find the insertion offset
  //
  int offset=d_key_names.size();
  for(int i=0;i<d_key_names.size();i++) {
    if(keyname.toLower()<d_key_names.at(i).toLower()) {
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
  QList<unsigned> ids_list;
  d_feed_ids.insert(offset,0);
  d_texts.insert(offset,list);
  d_icons.insert(offset,list);
  list_list.push_back(list);
  d_cast_ids.insert(offset,ids_list);
  d_cast_icons.insert(offset,list);
  d_key_names.insert(offset,keyname);
  d_cast_texts.insert(offset,list_list);

  QString sql=sqlFields()+
	"where "+
	"`FEEDS`.`KEY_NAME`='"+RDEscapeString(keyname)+"' ";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(offset,q);
  }
  delete q;
  endInsertRows();
  emit rowCountChanged(d_texts.size());

  return createIndex(offset,0,(quintptr)0);
}


void RDFeedListModel::removeFeed(const QString &keyname)
{
  for(int i=0;i<d_key_names.size();i++) {
    if(d_key_names.at(i)==keyname) {
      beginRemoveRows(QModelIndex(),i,i);

      d_feed_ids.removeAt(i);
      d_texts.removeAt(i);
      d_icons.removeAt(i);
      d_cast_ids.removeAt(i);
      d_cast_icons.removeAt(i);
      d_key_names.removeAt(i);
      d_cast_texts.removeAt(i);

      endRemoveRows();
      emit rowCountChanged(d_texts.size());
      return;
    }
  }
}


void RDFeedListModel::refreshRow(const QModelIndex &index)
{
  if(isFeed(index)) {
    updateFeedLine(index.row());
    emit dataChanged(index,createIndex(index.row(),columnCount(),(quintptr)0));
  }
  else {
    updateCastLine(index.parent().row(),index.row());
  }
}


void RDFeedListModel::refreshFeed(const QString &keyname)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_key_names.at(i)==keyname) {
      updateFeedLine(i);
      emit dataChanged(createIndex(i,0,(quintptr)0),
		       createIndex(i,columnCount(),(quintptr)0));
    }
  }
}


void RDFeedListModel::changeUser()
{
  QString sql;
  RDSqlQuery *q=NULL;
  QString filter_sql="where (`KEY_NAME` is null)||";

  if(d_is_admin) {
    sql=QString("select ")+
      "`FEEDS`.`KEY_NAME` "+  // 00
      "from `FEEDS`";
  }
  else {
    sql=QString("select ")+
      "`FEED_PERMS`.`KEY_NAME` "+  // 00
      "from `FEED_PERMS` where "+
      "`FEED_PERMS`.`USER_NAME`='"+RDEscapeString(rda->user()->name())+"'";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    filter_sql+="(`FEEDS`.`KEY_NAME`='"+RDEscapeString(q->value(0).toString())+"')||";
  }
  delete q;
  filter_sql=filter_sql.left(filter_sql.length()-2);
  
  updateModel(filter_sql);
}


void RDFeedListModel::processNotification(RDNotification *notify)
{
  QString keyname;

  if(notify->type()==RDNotification::FeedType) {
    keyname=notify->id().toString();
    switch(notify->action()) {
    case RDNotification::ModifyAction:
      refreshFeed(keyname);
      break;

    case RDNotification::NoAction:
    case RDNotification::AddAction:
    case RDNotification::DeleteAction:
    case RDNotification::LastAction:
      break;
    }
  }
}




void RDFeedListModel::updateModel(const QString &filter_sql)
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
  QList<unsigned> ids;

  sql=sqlFields()+
    filter_sql+
    "order by `FEEDS`.`KEY_NAME` asc, `PODCASTS`.`ORIGIN_DATETIME` desc";
  //  printf("SQL: %s\n",sql.toUtf8().constData());
  beginResetModel();
  d_feed_ids.clear();
  d_texts.clear();
  d_icons.clear();
  d_cast_ids.clear();
  d_cast_icons.clear();
  d_key_names.clear();
  d_cast_texts.clear();

  if(d_include_none) {
    d_feed_ids.push_back(0);
    d_texts.push_back(list);
    d_texts[0][0]=tr("[none]");
    d_icons.push_back(icons);
    d_key_names.push_back(QString());
    d_cast_ids.push_back(ids);
    d_cast_icons.push_back(list);
    d_cast_texts.push_back(list_list);
  }

  QString prev_keyname;
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toString()!=prev_keyname) {
      d_feed_ids.push_back(0);
      d_texts.push_back(list);
      d_icons.push_back(icons);
      d_cast_ids.push_back(ids);
      d_cast_icons.push_back(list);
      d_key_names.push_back(QString());
      d_cast_texts.push_back(list_list);
      updateRow(d_texts.size()-1,q);
      prev_keyname=q->value(0).toString();
    }
  }
  delete q;
  endResetModel();
  emit rowCountChanged(d_texts.size());
}


void RDFeedListModel::updateFeedLine(int line)
{
  QString sql=sqlFields()+
    "where "+
    "FEEDS.KEY_NAME=\""+RDEscapeString(d_key_names.at(line))+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateRow(line,q);
    emit dataChanged(createIndex(line,0,(quintptr)0),
		     createIndex(line,columnCount(),(quintptr)0));
  }
  delete q;
}


void RDFeedListModel::updateCastLine(int feed_line,int cast_line)
{
  QString sql=QString("select ")+
    castSqlFields()+
    "from `PODCASTS` where "+
    QString::asprintf("`ID`=%u",d_cast_ids.at(feed_line).at(cast_line));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    updateCastRow(feed_line,cast_line,q,0);
    emit dataChanged(createIndex(cast_line,0,(quintptr)feed_line),
		     createIndex(cast_line,3,(quintptr)feed_line));
  }
  delete q;
}


void RDFeedListModel::updateCastRow(int feed_line,int cast_line,
				    RDSqlQuery *q,int q_offset)
{
  switch((RDPodcast::Status)q->value(2+q_offset).toUInt()) {
  case RDPodcast::StatusPending:
    d_cast_icons[feed_line][cast_line]=
      rda->iconEngine()->listIcon(RDIconEngine::RedBall);
    break;

  case RDPodcast::StatusActive:
    if(q->value(4+q_offset).toDateTime()<=QDateTime::currentDateTime()) {
      d_cast_icons[feed_line][cast_line]=
	rda->iconEngine()->listIcon(RDIconEngine::GreenBall);
    }
    else {
      d_cast_icons[feed_line][cast_line]=
	rda->iconEngine()->listIcon(RDIconEngine::BlueBall);
    }
    break;

  case RDPodcast::StatusExpired:
    d_cast_icons[feed_line][cast_line]=
      rda->iconEngine()->listIcon(RDIconEngine::WhiteBall);
    break;
  }
  d_cast_texts[feed_line][cast_line][0]=
    QString::asprintf("%u",q->value(q_offset).toUInt());       // Cast ID
  d_cast_texts[feed_line][cast_line][1]=q->value(1+q_offset);  // Item Title
  d_cast_texts[feed_line][cast_line][3]=
    rda->shortDateString(q->value(3+q_offset).toDateTime().date());
}


void RDFeedListModel::updateRow(int row,RDSqlQuery *q)
{
  int active_casts=0;
  int total_casts=0;
  QString keyname=q->value(1).toString();

  //
  // Feed Attributes
  //
  d_feed_ids[row]=q->value(0).toUInt();
  d_key_names[row]=q->value(1).toString();
  d_texts[row][0]=keyname;  // Key Name
  if(q->value(13).isNull()) {
    d_icons[row][0]=rda->iconEngine()->
      applicationIcon(RDIconEngine::RdCastManager,32);
  }
  else {
    d_icons[row][0]=QImage::fromData(q->value(13).toByteArray());
  }
  d_texts[row][1]=q->value(2);  // Title
  if(q->value(4).toString()=="Y") {
    d_texts[row][2]=tr("[superfeed]");
  }
  else {
    d_texts[row][2]="0 / 0";      // Casts
  }
  d_texts[row][3]=rda->shortDateString(q->value(7).toDate());  // Creation Date
  d_texts[row][4]=q->value(3).toString();  // Autopost
  d_texts[row][5]=q->value(4).toString();  // Superfeed
  d_texts[row][6]=RDFeed::publicUrl(q->value(6).toString(),keyname);

  //
  // Cast Attributes
  //
  d_cast_texts[row].clear();
  d_cast_ids[row].clear();
  d_cast_icons[row].clear();
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }

  do {
    if(q->value(8).isNull()) {
      return;  // No casts!
    }
    // Process
    total_casts++;
    d_cast_ids[row].push_back(q->value(8).toUInt());
    d_cast_icons[row].push_back(QVariant());
    d_cast_texts[row].push_back(list);
    updateCastRow(row,d_cast_icons[row].size()-1,q,8);
  } while(q->next()&&(q->value(1).toString()==keyname));
  q->previous();

  if(q->value(4).toString()=="Y") {
    d_texts[row][2]=tr("[superfeed]");
  }
  else {
    d_texts[row][2]=
      QString::asprintf("%d / %d",active_casts,total_casts);  // Casts
  }
}


QString RDFeedListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`FEEDS`.`ID`,"+                      // 00
    "`FEEDS`.`KEY_NAME`,"+                // 01
    "`FEEDS`.`CHANNEL_TITLE`,"+           // 02
    "`FEEDS`.`ENABLE_AUTOPOST`,"+         // 03
    "`FEEDS`.`IS_SUPERFEED`,"+            // 04
    "`FEEDS`.`ID`,"+                      // 05
    "`FEEDS`.`BASE_URL`,"+                // 06
    "`FEEDS`.`ORIGIN_DATETIME`,"+         // 07
    castSqlFields()+","+                  // 08 - 12
    "`FEED_IMAGES`.`DATA_MID_THUMB` "+    // 13
    "from `FEEDS` left join `FEED_IMAGES` "+
    "on `FEEDS`.`CHANNEL_IMAGE_ID`=`FEED_IMAGES`.`ID` left join `PODCASTS` "+
    "on `FEEDS`.`ID`=`PODCASTS`.`FEED_ID` ";

  return sql;
}


QString RDFeedListModel::castSqlFields() const
{
  QString sql=QString("`PODCASTS`.`ID`,")+ // 00
    "`PODCASTS`.`ITEM_TITLE`,"+            // 01
    "`PODCASTS`.`STATUS`,"+                // 02
    "`PODCASTS`.`ORIGIN_DATETIME`,"+       // 03
    "`PODCASTS`.`EFFECTIVE_DATETIME` ";    // 04
  return sql;
}


QByteArray RDFeedListModel::DumpIndex(const QModelIndex &index,const QString &caption) const
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
