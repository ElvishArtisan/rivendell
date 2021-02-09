// importcartsmodel.cpp
//
// Data model for Rivendell hosts
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include "importcartsmodel.h"

ImportCartsModel::ImportCartsModel(const QString &evt_name,
				   ImportCartsModel::ImportType type,
				   bool auto_first_trans,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_event_name=evt_name;
  d_import_type=type;
  d_auto_first_trans=auto_first_trans;

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  //  unsigned center=Qt::AlignCenter;
  //  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Cart"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Group"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Length"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Title"));
  d_alignments.push_back(left);

  d_headers.push_back(tr("Transition"));
  d_alignments.push_back(left);

  updateModel();
}


ImportCartsModel::~ImportCartsModel()
{
}


QPalette ImportCartsModel::palette()
{
  return d_palette;
}


void ImportCartsModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void ImportCartsModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


bool ImportCartsModel::firstTransitionIsAuto() const
{
  return d_auto_first_trans;
}


int ImportCartsModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int ImportCartsModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant ImportCartsModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant ImportCartsModel::data(const QModelIndex &index,int role) const
{
  QString str;
  int col=index.column();
  int row=index.row();

  if(row<d_texts.size()) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      if((row==0)&&(col==4)&&d_auto_first_trans) {
	return tr("[auto]");
      }
      return d_texts.at(row).at(col);

    case Qt::DecorationRole:
      if(col==0) {
	return d_icons.at(row);
      }
      break;

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    case Qt::FontRole:
      if((col==1)&&((d_event_types.at(row)==RDLogLine::Cart)||
		    (d_event_types.at(row)==RDLogLine::Macro))) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      if((col==1)&&((d_event_types.at(row)==RDLogLine::Cart)||
		    (d_event_types.at(row)==RDLogLine::Macro))) {
	return d_group_colors.value(d_texts.at(row).at(1).toString());
      }
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


RDLogLine::Type ImportCartsModel::eventType(const QModelIndex &row) const
{
  return d_event_types.at(row.row());
}


RDLogLine::TransType ImportCartsModel::transType(const QModelIndex &row) const
{
  return d_trans_types.at(row.row());
}


void ImportCartsModel::setTransType(const QModelIndex &row,
				    RDLogLine::TransType type)
{
  if(d_trans_types.at(row.row())!=type) {
    d_trans_types[row.row()]=type;
    d_texts[row.row()][4]=RDLogLine::transText(type);
    emit dataChanged(createIndex(row.row(),4),createIndex(row.row(),4));
  }
}


int ImportCartsModel::length(const QModelIndex &row) const
{
  return d_lengths.at(row.row());
}


QModelIndex ImportCartsModel::addMarker(const QModelIndex &row,
					const QString &str)
{
  beginInsertRows(QModelIndex(),row.row(),row.row());
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[1]=tr("Marker");
  list[3]=tr("[Log Note]");
  list[4]=tr("PLAY");
  d_texts.insert(row.row(),list);
  d_event_types.insert(row.row(),RDLogLine::Marker);
  d_trans_types.insert(row.row(),RDLogLine::Play);
  d_lengths.insert(row.row(),0);
  d_marker_comments.insert(row.row(),str);
  d_icons.insert(row.row(),rda->iconEngine()->typeIcon(RDLogLine::Marker));
  endInsertRows();

  return createIndex(row.row(),0);
}


QModelIndex ImportCartsModel::addTrack(const QModelIndex &row,
				       const QString &str)
{
  beginInsertRows(QModelIndex(),row.row(),row.row());
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  list[1]=tr("Track");
  list[3]=tr("[Voice Track]");
  list[4]=tr("PLAY");
  d_texts.insert(row.row(),list);
  d_event_types.insert(row.row(),RDLogLine::Track);
  d_trans_types.insert(row.row(),RDLogLine::Play);
  d_lengths.insert(row.row(),0);
  d_marker_comments.insert(row.row(),str);
  d_icons.insert(row.row(),rda->iconEngine()->typeIcon(RDLogLine::Track));
  endInsertRows();

  return createIndex(row.row(),0);
}


QString ImportCartsModel::markerComments(const QModelIndex &row) const
{
  return d_marker_comments.at(row.row());
}


void ImportCartsModel::setMarkerComments(const QModelIndex &row,
					 const QString &str)
{
  d_marker_comments[row.row()]=str;
}


unsigned ImportCartsModel::cartNumber(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(0).toString().toUInt();
}


void ImportCartsModel::setCartNumber(const QModelIndex &row,unsigned cartnum)
{
  if(d_texts.at(row.row()).at(0).toString().toUInt()!=cartnum) {
    int total_len=totalLength();
    d_texts[row.row()][0]=QString().sprintf("%06u",cartnum);
    QString sql=QString("select ")+
      "NUMBER "          // 00
      "TYPE,"+           // 01
      "FORCED_LENGTH "+  // 02
      "from CART where "+
      QString().sprintf("NUMBER=%u",cartnum);
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      d_texts[row.row()][3]=q->value(0);
      d_lengths[row.row()]=q->value(2).toInt();
      switch((RDCart::Type)q->value(1).toUInt()) {
      case RDCart::Audio:
	d_icons[row.row()]=rda->iconEngine()->typeIcon(RDLogLine::Cart);
	break;

      case RDCart::Macro:
	d_icons[row.row()]=rda->iconEngine()->typeIcon(RDLogLine::Macro);
	break;

      case RDCart::All:
	break;
      }
    }
    delete q;
    emit dataChanged(createIndex(row.row(),0),createIndex(row.row(),3));
    if(total_len!=totalLength()) {
      emit totalLengthChanged(totalLength());
    }
  }
}


int ImportCartsModel::totalLength() const
{
  int ret=0;

  for(int i=0;i<lineCount();i++) {
    ret+=d_lengths.at(i);
  }

  return ret;
}


void ImportCartsModel::removeItem(const QModelIndex &row)
{
  int prev_len=totalLength();

  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_marker_comments.removeAt(row.row());
  d_texts.removeAt(row.row());
  d_event_types.removeAt(row.row());
  d_trans_types.removeAt(row.row());
  d_lengths.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();

  if(prev_len!=totalLength()) {
    emit totalLengthChanged(totalLength());
  }
}


int ImportCartsModel::lineCount() const
{
  return rowCount()-1;
}


void ImportCartsModel::save(RDLogLine::TransType first_trans)
{
  QString sql=QString("delete from EVENT_LINES where ")+
    "EVENT_NAME=\""+RDEscapeString(d_event_name)+"\" && "+
    QString().sprintf("TYPE=%u",d_import_type);
  RDSqlQuery::apply(sql);

  for(int i=0;i<lineCount();i++) {
    RDLogLine::TransType trans=
      RDLogLine::transTypeFromString(d_texts.at(i).at(4).toString());
    if(trans==RDLogLine::NoTrans) {
      trans=RDLogLine::Play;
    }
    sql=QString("insert into EVENT_LINES set ")+
      "EVENT_NAME=\""+RDEscapeString(d_event_name)+"\","+\
      QString().sprintf("TYPE=%u,",d_import_type)+
      QString().sprintf("COUNT=%d,",i)+
      QString().sprintf("EVENT_TYPE=%u,",d_event_types.at(i));
    if((i==0)&&(first_trans!=RDLogLine::NoTrans)) {
      sql+=QString().sprintf("TRANS_TYPE=%u,",first_trans);
    }
    else {
      sql+=QString().sprintf("TRANS_TYPE=%u,",d_trans_types.at(i));
    }
    switch(d_event_types.at(i)) {
    case RDLogLine::Cart:
    case RDLogLine::Macro:
      sql+=QString().
	sprintf("CART_NUMBER=%u,",d_texts.at(i).at(0).toString().toUInt())+
	"MARKER_COMMENT=null";
      break;

    case RDLogLine::Marker:
    case RDLogLine::Track:
      sql+=QString("CART_NUMBER=null,")+
	"MARKER_COMMENT=\""+RDEscapeString(d_marker_comments.at(i))+"\"";
      break;

    case RDLogLine::MusicLink:
    case RDLogLine::TrafficLink:
    case RDLogLine::Chain:
    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::UnknownType:
      break;
    }

    RDSqlQuery::apply(sql);
  }
}


QModelIndex ImportCartsModel::processCartDrop(int line,RDLogLine *ll)
{
  int prev_len=totalLength();

  if(ll->cartNumber()==0) {  // Remove line
    if(line>=lineCount()) {  // Don't remove the end marker!
      return QModelIndex();
    }
    beginRemoveRows(QModelIndex(),line,line);
    d_texts.removeAt(line);
    d_event_types.removeAt(line);
    d_trans_types.removeAt(line);
    d_lengths.removeAt(line);
    d_icons.removeAt(line);
    endRemoveRows();
  }
  else {  // Add line
    beginInsertRows(QModelIndex(),line,line);
    QList<QVariant> list;
    for(int i=0;i<columnCount();i++) {
      list.push_back(QVariant());
    }
    list[0]=QString().sprintf("%06u",ll->cartNumber());
    list[1]=ll->groupName();
    list[2]=RDGetTimeLength(ll->forcedLength(),false,false);
    list[3]=ll->title();
    list[4]=tr("PLAY");
    d_texts.insert(line,list);
    d_event_types.insert(line,ll->type());
    d_trans_types.insert(line,ll->transType());
    d_lengths.insert(line,ll->forcedLength());
    d_icons.insert(line,rda->iconEngine()->typeIcon(ll->type()));
    endInsertRows();
  }

  if(prev_len!=totalLength()) {
    emit totalLengthChanged(totalLength());
  }

  return createIndex(line,0);
}


bool ImportCartsModel::moveUp(const QModelIndex &row)
{
  if((row.row()==0)||(row.row()>=lineCount())) {
    return false;
  }
  d_marker_comments.swap(row.row(),row.row()-1);
  d_texts.swap(row.row(),row.row()-1);
  d_icons.swap(row.row(),row.row()-1);
  d_event_types.swap(row.row(),row.row()-1);
  d_trans_types.swap(row.row(),row.row()-1);
  d_lengths.swap(row.row(),row.row()-1);
  //  normalizeTransitions();
  emit dataChanged(createIndex(row.row()-1,0),
		   createIndex(row.row(),columnCount()-1));

  return true;
}


bool ImportCartsModel::moveDown(const QModelIndex &row)
{
  if(row.row()>=(lineCount()-1)) {
    return false;
  }
  d_marker_comments.swap(row.row(),row.row()+1);
  d_texts.swap(row.row(),row.row()+1);
  d_event_types.swap(row.row(),row.row()+1);
  d_trans_types.swap(row.row(),row.row()+1);
  d_lengths.swap(row.row(),row.row()+1);
  d_icons.swap(row.row(),row.row()+1);
  //  normalizeTransitions();
  emit dataChanged(createIndex(row.row(),0),
		   createIndex(row.row()+1,columnCount()-1));

  return true;
}


void ImportCartsModel::updateModel()
{
  //
  // Reload the color table
  //
  d_group_colors.clear();
  QString sql=QString("select ")+
    "NAME,"+   // 00
    "COLOR "+  // 01
    "from GROUPS order by NAME";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    d_group_colors[q->value(0).toString()]=QColor(q->value(1).toString());
  }
  delete q;

  QList<QVariant> texts; 
  QList<QVariant> icons;

  sql=sqlFields()+"where "+
    "EVENT_LINES.EVENT_NAME=\""+RDEscapeString(d_event_name)+"\" && "+
    QString().sprintf("EVENT_LINES.TYPE=%u ",d_import_type)+
    "order by EVENT_LINES.COUNT ";
  beginResetModel();
  d_marker_comments.clear();
  d_texts.clear();
  d_event_types.clear();
  d_trans_types.clear();
  d_lengths.clear();
  d_icons.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_marker_comments.push_back(QString());
    d_texts.push_back(texts);
    d_event_types.push_back(RDLogLine::Cart);
    d_trans_types.push_back(RDLogLine::Play);
    d_lengths.push_back(0);
    d_icons.push_back(QVariant());
    updateRow(d_texts.size()-1,q);
  }
  delete q;

  // End Marker
  d_event_types.push_back(RDLogLine::UnknownType);
  d_trans_types.push_back(RDLogLine::NoTrans);
  d_lengths.push_back(0);
  d_marker_comments.push_back(QString());
  d_texts.push_back(texts);
  d_texts.back().push_back(QVariant());
  d_texts.back().push_back(QVariant());
  d_texts.back().push_back(QVariant());
  d_texts.back().push_back(tr("-- End of List --"));
  d_texts.back().push_back(QVariant());
  d_icons.push_back(QVariant());

  endResetModel();

  emit totalLengthChanged(totalLength());
}


void ImportCartsModel::updateRow(int row,RDSqlQuery *q)
{
  QList<QVariant> texts;
  QList<QVariant> icons;

  d_icons[row]=(rda->iconEngine()->
		typeIcon((RDLogLine::Type)q->value(1).toUInt()));
  d_event_types[row]=(RDLogLine::Type)q->value(1).toUInt();
  d_trans_types[row]=(RDLogLine::TransType)q->value(7).toUInt();
  QString trans_text=RDLogLine::transText(d_trans_types.at(row));
  d_marker_comments[row]=q->value(8).toString();

  switch(d_event_types.at(row)) {
  case RDLogLine::Cart:
  case RDLogLine::Macro:
    // Cart
    texts.push_back(QString().sprintf("%06u",q->value(2).toUInt()));

    // Group
    texts.push_back(q->value(3));

    // Length
    texts.push_back(RDGetTimeLength(q->value(5).toInt(),false,false));
    d_lengths[row]=q->value(5).toInt();

    // Title
    texts.push_back(q->value(6));

    // Transition
    texts.push_back(trans_text);
    break;

  case RDLogLine::Marker:
    // Cart
    texts.push_back(QVariant());

    // Group
    texts.push_back(tr("Marker"));

    // Length
    texts.push_back(QVariant());

    // Title
    texts.push_back(tr("[Log Note]"));

    // Transition
    texts.push_back(trans_text);
    break;

  case RDLogLine::Track:
    // Cart
    texts.push_back(QVariant());

    // Group
    texts.push_back(tr("Track"));

    // Length
    texts.push_back(QVariant());

    // Title
    texts.push_back(tr("[Voice Track]"));

    // Transition
    texts.push_back(trans_text);
    break;

  case RDLogLine::OpenBracket:
  case RDLogLine::CloseBracket:
  case RDLogLine::Chain:
  case RDLogLine::MusicLink:
  case RDLogLine::TrafficLink:
  case RDLogLine::UnknownType:
    break;
  }

  d_texts[row]=texts;
}


QString ImportCartsModel::sqlFields() const
{
  QString sql=QString("select ")+
    "EVENT_LINES.ID,"+              // 00
    "EVENT_LINES.EVENT_TYPE,"+      // 01
    "EVENT_LINES.CART_NUMBER,"+     // 02
    "CART.GROUP_NAME,"+             // 03
    "GROUPS.COLOR,"+                // 04
    "CART.FORCED_LENGTH,"+          // 05
    "CART.TITLE,"+                  // 06
    "EVENT_LINES.TRANS_TYPE,"+      // 07
    "EVENT_LINES.MARKER_COMMENT "+  // 08
    "from EVENT_LINES left join CART "+
    "on EVENT_LINES.CART_NUMBER=CART.NUMBER "+
    "left join GROUPS "+
    "on CART.GROUP_NAME=GROUPS.NAME ";

    return sql;
}
