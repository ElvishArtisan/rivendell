// rdpodcastlistmodel.cpp
//
// Data model for Rivendell podcast episodes
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
#include "rdpodcast.h"
#include "rdpodcastlistmodel.h"

RDPodcastListModel::RDPodcastListModel(unsigned feed_id,QObject *parent)
  : QAbstractTableModel(parent)
{
  QString sql=QString("select ")+
    "`IS_SUPERFEED` "+  // 00
    "from `FEEDS` where "+
    QString::asprintf("`ID`=%u",feed_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toString()=="Y") {
      sql=QString("select ")+
	"`MEMBER_FEED_ID` "+  // 00
	"from `SUPERFEED_MAPS` where "+
	QString::asprintf("`FEED_ID`=%u ",feed_id);
      RDSqlQuery *q1=new RDSqlQuery(sql);
      while(q1->next()) {
	d_feed_ids.push_back(q1->value(0).toUInt());
      }
      delete q1;
    }
    else {
      d_feed_ids.push_back(feed_id);
    }
  }
  else {
    d_feed_ids.push_back(feed_id);
  }
  
  d_font_metrics=NULL;
  d_bold_font_metrics=NULL;
  d_sort_column=0;
  d_sort_order=Qt::AscendingOrder;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Title"));       // 00
  d_alignments.push_back(left);
  d_column_fields.push_back("`PODCASTS`.`ITEM_TITLE`");

  d_headers.push_back(tr("Status"));      // 01
  d_alignments.push_back(center);
  d_column_fields.push_back("`PODCASTS`.`STATUS`");

  d_headers.push_back(tr("Start"));       // 02
  d_alignments.push_back(left);
  d_column_fields.push_back("`PODCASTS`.`EFFECTIVE_DATETIME`");

  d_headers.push_back(tr("Expiration"));  // 03
  d_alignments.push_back(left);
  d_column_fields.push_back("`PODCASTS`.`EXPIRATION_DATETIME`");

  d_headers.push_back(tr("Length"));      // 04
  d_alignments.push_back(right);
  d_column_fields.push_back("`PODCASTS`.`AUDIO_LENGTH`");

  d_headers.push_back(tr("Feed"));        // 05
  d_alignments.push_back(center);
  d_column_fields.push_back("`FEEDS`.`KEY_NAME`");

  d_headers.push_back(tr("Category"));    // 06
  d_alignments.push_back(left);
  d_column_fields.push_back("`FEEDS`.`CHANNEL_CATEGORY`");

  d_headers.push_back(tr("Posted By"));   // 07
  d_alignments.push_back(left);
  d_column_fields.push_back("`PODCASTS`.`ORIGIN_LOGIN_NAME`");

  d_headers.push_back(tr("ID"));          // 08
  d_alignments.push_back(right);
  d_column_fields.push_back("`PODCASTS`.`ID`");

  d_headers.push_back(tr("SHA1"));        // 09
  d_alignments.push_back(left);
  d_column_fields.push_back("`PODCASTS`.`SHA1_HASH`");
}


RDPodcastListModel::~RDPodcastListModel()
{
}


QPalette RDPodcastListModel::palette()
{
  return d_palette;
}


void RDPodcastListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RDPodcastListModel::setFont(const QFont &font)
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


int RDPodcastListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDPodcastListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RDPodcastListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDPodcastListModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      if(col==0) {  // Item Icon
	return d_item_images.
	  value(d_item_image_ids.at(row),
		rda->iconEngine()->
		applicationIcon(RDIconEngine::RdCastManager,32));
      }
      if(col==1) {  // Status Indicator
	return d_status_pixmaps.at(row);
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

    case Qt::SizeHintRole:
      if(col==0) {
	return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+32+
		     d_bold_font_metrics->
		     width(d_texts.at(row).at(col).toString()),40);
      }
      return QSize(RD_LISTWIDGET_ITEM_WIDTH_PADDING+
		   d_font_metrics->
		   width(d_texts.at(row).at(col).toString()),40);

    default:
      break;
    }
  }

  return QVariant();
}


void RDPodcastListModel::sort(int col,Qt::SortOrder order)
{
  if((col!=d_sort_column)||(order!=d_sort_order)) {
    d_sort_column=col;
    d_sort_order=order;
    updateModel();
  }
}


unsigned RDPodcastListModel::castId(const QModelIndex &row) const
{
  return d_cast_ids.at(row.row());
}


QModelIndex RDPodcastListModel::addCast(unsigned cast_id)
{
  QModelIndex ret;

  if(!d_cast_ids.contains(cast_id)) {
    beginInsertRows(QModelIndex(),0,0);
    QList<QVariant> list;
    for(int i=0;i<columnCount();i++) {
      list.push_back(QVariant());
    }
    d_cast_ids.insert(0,cast_id);
    d_texts.insert(0,list);
    d_status_pixmaps.insert(0,QVariant());
    d_item_image_ids.insert(0,-1);
    updateRowLine(0);
    endInsertRows();
    ret=createIndex(0,0);
  }

  return ret;
}


void RDPodcastListModel::removeCast(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_cast_ids.removeAt(row.row());
  d_texts.removeAt(row.row());
  d_status_pixmaps.removeAt(row.row());
  d_item_image_ids.removeAt(row.row());

  endRemoveRows();
}


void RDPodcastListModel::removeCast(unsigned cast_id)
{
  for(int i=0;i<d_cast_ids.size();i++) {
    if(d_cast_ids.at(i)==cast_id) {
      removeCast(createIndex(i,0));
      return;
    }
  }
}


void RDPodcastListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString::asprintf("`PODCASTS`.`ID`=%u",d_cast_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      loadItemImage(q->value(13).toInt());
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


void RDPodcastListModel::refresh(unsigned cast_id)
{
  for(int i=0;i<d_cast_ids.size();i++) {
    if(d_cast_ids.at(i)==cast_id) {
      updateRowLine(i);
      return;
    }
  }
}


void RDPodcastListModel::setFilterSql(const QString &sql)
{
  if(sql!=d_filter_sql) {
    d_filter_sql=sql;
    updateModel();
  }
}


void RDPodcastListModel::processNotification(RDNotification *notify)
{
  unsigned cast_id=0;
  RDPodcast *cast=NULL;

  if(notify->type()==RDNotification::FeedItemType) {
    switch(notify->action()) {
    case RDNotification::AddAction:
      cast_id=notify->id().toUInt();
      cast=new RDPodcast(rda->config(),cast_id);
      //      if(cast->feedId()==d_feed_id) {
      if(d_feed_ids.contains(cast->feedId())) {
	addCast(cast_id);
      }
      delete cast;
      break;

    case RDNotification::DeleteAction:
      removeCast(cast_id);
      break;

    case RDNotification::ModifyAction:
      refresh(cast_id);
      break;

    case RDNotification::LastAction:
    case RDNotification::NoAction:
      break;
    }
  }
}


void RDPodcastListModel::updateModel()
{
  QList<QVariant> texts;
  QList<int> image_ids;

  RDSqlQuery *q=NULL;
  QString sql=sqlFields()+
    "where ";
  //  sql+=QString::asprintf("`PODCASTS`.`FEED_ID`=%u ",d_feed_ids.at(0));
  for(int i=0;i<d_feed_ids.size();i++) {
    sql+=(QString::asprintf("`PODCASTS`.`FEED_ID`=%u || ",d_feed_ids.at(i)));
  }
  sql=sql.left(sql.length()-3);
  sql+=d_filter_sql+
    " order by "+d_column_fields.at(d_sort_column)+" ";
  if(d_sort_order==Qt::DescendingOrder) {
    sql+="desc ";
  }

  beginResetModel();
  d_cast_ids.clear();
  d_texts.clear();
  d_status_pixmaps.clear();
  d_item_image_ids.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_cast_ids.push_back(0);
    d_texts.push_back(texts);
    d_status_pixmaps.push_back(QVariant());
    d_item_image_ids.push_back(-1);
    updateRow(d_texts.size()-1,q);
    if(!image_ids.contains(q->value(13).toInt())) {
      image_ids.push_back(q->value(13).toInt());
    }
  }
  delete q;

  //
  // Load item thumbnail images
  //
  d_item_images.clear();
  for(int i=0;i<image_ids.size();i++) {
    sql=QString("select ")+
      "`DATA_MID_THUMB` "+  // 00
      "from `FEED_IMAGES` where "+
      QString::asprintf("`ID`=%d",image_ids.at(i));
    q=new RDSqlQuery(sql);
    if(q->first()) {
      d_item_images[image_ids.at(i)]=
	QImage::fromData(q->value(0).toByteArray()).
	scaled(32,32,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    else {
      d_item_images[image_ids.at(i)]=
	rda->iconEngine()->applicationIcon(RDIconEngine::RdCastManager,32);
    }
    delete q;
  }

  endResetModel();
}


void RDPodcastListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      "where "+
      QString::asprintf("`PODCASTS`.`ID`=%u",d_cast_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
      loadItemImage(q->value(13).toInt());
    }
    delete q;
  }
}


void RDPodcastListModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;
  QList<QVariant> icons;

  // Title
  texts.push_back(q->value(2));

  // Item Image ID
  d_item_image_ids[row]=q->value(13).toInt();

  // Status
  texts.push_back(QVariant());
  switch((RDPodcast::Status)q->value(1).toUInt()) {
  case RDPodcast::StatusActive:
    if(q->value(3).toDateTime()<=QDateTime::currentDateTime()) {
      d_status_pixmaps[row]=
	rda->iconEngine()->listIcon(RDIconEngine::GreenBall);
    }
    else {
      d_status_pixmaps[row]=rda->iconEngine()->listIcon(RDIconEngine::BlueBall);
    }
    break;


  case RDPodcast::StatusPending:
    d_status_pixmaps[row]=rda->iconEngine()->listIcon(RDIconEngine::RedBall);
    break;

  case RDPodcast::StatusExpired:
    d_status_pixmaps[row]=rda->iconEngine()->listIcon(RDIconEngine::WhiteBall);
  }

  // Start
  texts.push_back(rda->shortDateTimeString(q->value(3).toDateTime(),false));
  icons.push_back(QVariant());

  // Expiration
  if(q->value(4).toDateTime().isValid()) {
    texts.push_back(rda->shortDateTimeString(q->value(4).toDateTime(),false));
  }
  else {
    texts.push_back(tr("Never"));
  }
  icons.push_back(QVariant());

  // Length
  texts.push_back(RDGetTimeLength(q->value(5).toInt(),false,false));
  icons.push_back(QVariant());

  // Feed Keyname
  texts.push_back(q->value(7));
  icons.push_back(QVariant());

  // Category
  texts.push_back(q->value(8));
  icons.push_back(QVariant());

  // Posted By
  if(q->value(9).isNull()) {
    texts.push_back(tr("unknown")+" "+tr("at")+" "+
		    rda->shortDateTimeString(q->value(11).toDateTime()));
    }
  else {
    texts.push_back(q->value(9).toString()+" "+tr("on")+" "+
		    q->value(10).toString()+" "+tr("at")+" "+
		    rda->shortDateTimeString(q->value(11).toDateTime()));
  }
  icons.push_back(QVariant());

  // Cast ID
  texts.push_back(QString::asprintf("%u",q->value(0).toUInt()));

  // SHA1
  if(q->value(12).toString().isEmpty()) {
    texts.push_back(tr("[none]"));
  }
  else {
    texts.push_back(q->value(12));
  }
  icons.push_back(QVariant());

  d_cast_ids[row]=q->value(0).toUInt();
  d_texts[row]=texts;
}


QString RDPodcastListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`PODCASTS`.`ID`,"+                   // 00
    "`PODCASTS`.`STATUS`,"+               // 01
    "`PODCASTS`.`ITEM_TITLE`,"+           // 02
    "`PODCASTS`.`EFFECTIVE_DATETIME`,"+   // 03
    "`PODCASTS`.`EXPIRATION_DATETIME`,"+  // 04
    "`PODCASTS`.`AUDIO_TIME`,"+           // 05
    "`PODCASTS`.`ITEM_DESCRIPTION`,"+     // 06
    "`FEEDS`.`KEY_NAME`,"+                // 07
    "`PODCASTS`.`ITEM_CATEGORY`,"+        // 08
    "`PODCASTS`.`ORIGIN_LOGIN_NAME`,"+    // 09
    "`PODCASTS`.`ORIGIN_STATION`,"+       // 10
    "`PODCASTS`.`ORIGIN_DATETIME`,"+      // 11
    "`PODCASTS`.`SHA1_HASH`,"+            // 12
    "`PODCASTS`.`ITEM_IMAGE_ID` "+        // 13
    "from `PODCASTS` left join `FEEDS` "+
    "on `PODCASTS`.`FEED_ID`=`FEEDS`.`ID` ";

  return sql;
}


void RDPodcastListModel::loadItemImage(int image_id)
{
  if(!d_item_images.contains(image_id)) {
    QString sql=QString("select ")+
      "`DATA_MID_THUMB` "+  // 00
      "from `FEED_IMAGES` where "+
      QString::asprintf("`ID`=%d",image_id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      d_item_images[image_id]=
	QImage::fromData(q->value(0).toByteArray()).
	scaled(32,32,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    else {
      d_item_images[image_id]=
	rda->iconEngine()->applicationIcon(RDIconEngine::RdCastManager,32);
    }
    delete q;
  }
}


QString RDPodcastListModel::imageFP(const QByteArray &img)
{
  uint32_t sum=0;

  for(int i=0;i<img.size();i++) {
    sum+=img.at(i)&0xff;
  }

  return QString::asprintf("%08X",sum);
}
