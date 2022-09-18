// recordlistmodel.cpp
//
// Data model for Rivendell RDCatch events.
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddeck.h>
#include <rdescape_string.h>
#include <recordlistmodel.h>

#include "colors.h"

RecordListModel::RecordListModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_sort_column=0;
  d_sort_order=Qt::AscendingOrder;
  d_sort_clauses[Qt::AscendingOrder]="asc";
  d_sort_clauses[Qt::DescendingOrder]="desc";

  //
  // Column Attributes
  //
  unsigned left=Qt::AlignLeft|Qt::AlignVCenter;
  unsigned center=Qt::AlignCenter;
  unsigned right=Qt::AlignRight|Qt::AlignVCenter;

  d_headers.push_back(tr("Description"));       // 00
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`DESCRIPTION`");

  d_headers.push_back(tr("Location"));          // 01
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`STATION_NAME`");

  d_headers.push_back(tr("Start"));             // 02
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`START_TIME`");

  d_headers.push_back(tr("End"));               // 03
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`END_TIME`");

  d_headers.push_back(tr("Router"));            // 04
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`CHANNEL`");

  d_headers.push_back(tr("Source"));            // 05
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`SWITCH_INPUT`");

  d_headers.push_back(tr("Destination"));       // 06
  d_alignments.push_back(left);
  d_order_columns.push_back("`RECORDINGS`.`SWITCH_OUTPUT`");

  d_headers.push_back(tr("Su"));                // 07
  d_alignments.push_back(center);
  d_order_columns.push_back("`RECORDINGS`.`SUN`");

  d_headers.push_back(tr("Mo"));                // 08
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Tu"));                // 09
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("We"));                // 10
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Th"));                // 11
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Fr"));                // 12
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Sa"));                // 13
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Format"));            // 14
  d_alignments.push_back(left);
  d_order_columns.push_back("");

  d_headers.push_back(tr("One Shot"));          // 15
  d_alignments.push_back(center);
  d_order_columns.push_back("`RECORDINGS`.`ONE_SHOT`");

  d_headers.push_back(tr("Autotrim"));          // 16
  d_alignments.push_back(right);
  d_order_columns.push_back("`RECORDINGS`.`TRIM_THRESHOLD`");

  d_headers.push_back(tr("Normalization"));     // 17
  d_alignments.push_back(right);
  d_order_columns.push_back("`RECORDINGS`.`NORMALIZE_LEVEL`");

  d_headers.push_back(tr("Date Offsets"));      // 18
  d_alignments.push_back(center);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Error Status"));            // 19
  d_alignments.push_back(left);
  d_order_columns.push_back("");

  d_headers.push_back(tr("Error Code"));         // 20
  d_alignments.push_back(right);
  d_order_columns.push_back("`RECORDINGS`.`EXIT_CODE`");

  d_headers.push_back(tr("ID"));                // 21
  d_alignments.push_back(right);
  d_order_columns.push_back("`RECORDINGS`.`ID`");

  setFilterSql("");
}


RecordListModel::~RecordListModel()
{
}


QPalette RecordListModel::palette()
{
  return d_palette;
}


void RecordListModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


void RecordListModel::setFont(const QFont &font)
{
  d_font=font;
  d_bold_font=font;
  d_bold_font.setWeight(QFont::Bold);
}


int RecordListModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RecordListModel::rowCount(const QModelIndex &parent) const
{
  return d_texts.size();
}


QVariant RecordListModel::headerData(int section,Qt::Orientation orient,
				    int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RecordListModel::data(const QModelIndex &index,int role) const
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
      if(col==0) {
	return d_bold_font;
      }
      return d_font;

    case Qt::TextColorRole:
      return d_text_colors.at(row);

    case Qt::BackgroundRole:
      if(d_is_nexts.at(row)) {
	printf("NEXT %d: %s\n",row,QColor(EVENT_NEXT_COLOR).name().toUtf8().constData());
	return QColor(EVENT_NEXT_COLOR);
      }
      printf("DEFAULT %d: %s\n",row,d_back_colors.at(row).value<QColor>().name().toUtf8().constData());
      return d_back_colors.at(row);

    default:
      break;
    }
  }

  return QVariant();
}


void RecordListModel::sort(int col,Qt::SortOrder order)
{
  d_sort_column=col;
  d_sort_order=order;
  setFilterSql(d_filter_sql);
}


unsigned RecordListModel::recordId(const QModelIndex &row) const
{
  return d_ids.at(row.row());
}


RDRecording::Type RecordListModel::recordType(const QModelIndex &row) const
{
  return d_types.at(row.row());
}


RDRecording::ExitCode RecordListModel::recordExitCode(const QModelIndex &row)
  const
{
  return d_exit_codes.at(row.row());
}


QString RecordListModel::hostName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(23).toString();
}


QString RecordListModel::cutName(const QModelIndex &row) const
{
  return d_texts.at(row.row()).at(25).toString();
}


bool RecordListModel::recordIsNext(const QModelIndex &row) const
{
  return d_is_nexts.at(row.row());
}


void RecordListModel::setRecordIsNext(const QModelIndex &row,bool state)
{
  if(d_is_nexts.at(row.row())!=state) {
    d_is_nexts[row.row()]=state;
    emit dataChanged(createIndex(row.row(),0),
		     createIndex(row.row(),columnCount()-1));
  }
}


void RecordListModel::setRecordIsNext(unsigned rec_id,bool state)
{
  int index=d_ids.indexOf(rec_id);

  if((index>=0)&&(d_is_nexts.at(index)!=state)) {
    d_is_nexts[index]=state;
    emit dataChanged(createIndex(index,0),createIndex(index,columnCount()-1));
  }
}


void RecordListModel::clearNextRecords()
{
  for(int i=0;i<rowCount();i++) {
    if(d_is_nexts.at(i)) {
      d_is_nexts[i]=false;
      emit dataChanged(createIndex(i,0),createIndex(i,columnCount()-1));
    }
  }
}


RDDeck::Status RecordListModel::recordStatus(const QModelIndex &row) const
{
  return d_statuses.at(row.row());
}


void RecordListModel::setRecordStatus(const QModelIndex &row,
				      RDDeck::Status status)
{
  if(d_statuses.at(row.row())!=status) {
    d_statuses[row.row()]=status;
    UpdateStatus(row.row());
    emit dataChanged(createIndex(row.row(),0),
		     createIndex(row.row(),columnCount()-1));
  }
}


void RecordListModel::setRecordStatus(unsigned rec_id,RDDeck::Status status)
{
  int index=d_ids.indexOf(rec_id);

  if(index>=0) {
    if(d_statuses.at(index)!=status) {
      d_statuses[index]=status;
      UpdateStatus(index);
      emit dataChanged(createIndex(index,0),
		       createIndex(index,columnCount()-1));
    }
  }
}


void RecordListModel::channelCounts(int chan,int *waiting,int *active,
				    unsigned *rec_id)
{
  *waiting=0;
  *active=0;
  *rec_id=0;

  for(int i=0;i<rowCount();i++) {
    if(d_channels.at(i)==chan) {
      switch(d_statuses.at(i)) {
      case RDDeck::Waiting:
	(*active)++;
	(*waiting)++;
	*rec_id=d_ids.at(i);
	break;

      case RDDeck::Ready:
      case RDDeck::Recording:
	(*active)++;
	break;

      case RDDeck::Offline:
      case RDDeck::Idle:
	break;
      }
    }
  }
}


QModelIndex RecordListModel::addRecord(unsigned id)
{
  //
  // Find the insertion offset
  //
  int offset=d_texts.size();
  for(int i=0;i<d_texts.size();i++) {
    if(id<d_ids.at(i)) {
      offset=i;
      break;
    }
  }
  beginInsertRows(QModelIndex(),offset,offset);
  QList<QVariant> list;
  for(int i=0;i<columnCount();i++) {
    list.push_back(QVariant());
  }
  d_ids.insert(offset,id);
  d_channels.insert(offset,-1);
  d_types.insert(offset,RDRecording::Recording);
  d_exit_codes.insert(offset,RDRecording::Ok);
  d_text_colors.insert(offset,QVariant());
  d_back_colors.insert(offset,QVariant());
  d_texts.insert(offset,list);
  d_is_nexts.insert(offset,false);
  d_statuses.insert(offset,RDDeck::Idle);
  d_icons.insert(offset,list);
  updateRowLine(offset);
  endInsertRows();

  return createIndex(offset,0);
}


void RecordListModel::removeRecord(const QModelIndex &row)
{
  beginRemoveRows(QModelIndex(),row.row(),row.row());

  d_ids.removeAt(row.row());
  d_channels.removeAt(row.row());
  d_types.removeAt(row.row());
  d_exit_codes.removeAt(row.row());
  d_is_nexts.removeAt(row.row());
  d_statuses.removeAt(row.row());
  d_text_colors.removeAt(row.row());
  d_back_colors.removeAt(row.row());
  d_texts.removeAt(row.row());
  d_icons.removeAt(row.row());

  endRemoveRows();
}


void RecordListModel::removeRecord(unsigned id)
{
  for(int i=0;i<d_ids.size();i++) {
    if(d_ids.at(i)==id) {
      removeRecord(createIndex(i,0));
      return;
    }
  }
}


void RecordListModel::refresh(const QModelIndex &row)
{
  if(row.row()<d_ids.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where `RECORDINGS`.`ID`=%u",d_ids.at(row.row()));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(row.row(),q);
      emit dataChanged(createIndex(row.row(),0),
		       createIndex(row.row(),columnCount()));
    }
    delete q;
  }
}


bool RecordListModel::refresh(unsigned id)
{
  for(int i=0;i<d_texts.size();i++) {
    if(d_ids.at(i)==id) {
      updateRowLine(i);
      emit dataChanged(createIndex(i,0),
		       createIndex(i,columnCount()));
      return true;
    }
  }
  return false;
}


void RecordListModel::setFilterSql(const QString &sql)
{
  QString fsql=sql;

  d_filter_sql=sql;
  if((d_sort_column<0)||(d_order_columns.at(d_sort_column).isEmpty())) {
    // Use "natural" sort order
    fsql+=" order by `RECORDINGS`.`START_TIME` asc ";
  }
  else {
    fsql+=" order by "+d_order_columns.at(d_sort_column)+" "+
      d_sort_clauses.value(d_sort_order);
  }
  updateModel(fsql);
}


void RecordListModel::notificationReceivedData(RDNotification *notify)
{
  if(notify->type()==RDNotification::CatchEventType) {
    switch(notify->action()) {
    case RDNotification::AddAction:
      if(d_ids.contains(notify->id().toUInt())) {
	refresh(notify->id().toUInt());
      }
      else {
	addRecord(notify->id().toUInt());
      }
      emit updateNextEvents();
      break;

    case RDNotification::ModifyAction:
      refresh(notify->id().toUInt());
      emit updateNextEvents();
      break;

    case RDNotification::DeleteAction:
      removeRecord(notify->id().toUInt());
      emit updateNextEvents();
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void RecordListModel::updateModel(const QString &filter_sql)
{
  QList<QVariant> texts; 
  QList<QVariant> icons;
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Load Router Table
  //
  d_router_names.clear();
  sql=QString("select ")+
    "`STATION_NAME`,"+  // 00
    "`MATRIX`,"+        // 01
    "`NAME` "+          // 02
    "from `MATRICES`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_router_names[q->value(0).toString()][q->value(1).toInt()]=
      q->value(2).toString();
  }
  delete q;

  //
  // Load Model Rows
  //
  sql=sqlFields()+filter_sql;
  beginResetModel();
  d_ids.clear();
  d_channels.clear();
  d_types.clear();
  d_exit_codes.clear();
  d_is_nexts.clear();
  d_statuses.clear();
  d_text_colors.clear();
  d_back_colors.clear();
  d_texts.clear();
  d_icons.clear();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    d_ids.push_back(0);
    d_channels.push_back(-1);
    d_types.push_back(RDRecording::Recording);
    d_exit_codes.push_back(RDRecording::Ok);
    d_text_colors.push_back(QVariant());
    d_back_colors.push_back(QVariant());
    d_texts.push_back(texts);
    d_is_nexts.push_back(false);
    d_statuses.push_back(RDDeck::Idle);
    d_icons.push_back(texts);
    updateRow(d_texts.size()-1,q);
  }
  delete q;
  endResetModel();
}


void RecordListModel::updateRowLine(int line)
{
  if(line<d_texts.size()) {
    QString sql=sqlFields()+
      QString::asprintf("where `RECORDINGS`.`ID`=%u",d_ids.at(line));
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      updateRow(line,q);
    }
    delete q;
  }
}


void RecordListModel::updateRow(int row,RDSqlQuery *q)
{
  QString sql;
  RDSqlQuery *q1=NULL;
  RDCut *cut=NULL;
  QMap<int,QString> names;
  bool ok=false;

  //
  // Event Values
  //
  d_ids[row]=q->value(0).toUInt();
  d_channels[row]=q->value(29).toUInt();
  d_types[row]=(RDRecording::Type)q->value(23).toUInt();
  d_exit_codes[row]=(RDRecording::ExitCode)q->value(25).toUInt();

  //
  // Qt::TextColorRole
  //
  if(q->value(2).toString()=="Y") {
    d_text_colors[row]=QColor(EVENT_ACTIVE_TEXT_COLOR);
  }
  else {
    d_text_colors[row]=QColor(EVENT_INACTIVE_TEXT_COLOR);
  }

  //
  // Qt::BackgroundRole:
  //
  if(q->value(25).toInt()==0) {
    d_back_colors[row]=QVariant();
  }
  else {
    d_back_colors[row]=QColor(EVENT_ERROR_COLOR);
  }      

  //
  // Qt::DecorationType
  //
  QList<QVariant> icons;
  for(int i=0;i<columnCount();i++) {
    icons.push_back(QPixmap());
  }
  icons[0]=
    rda->iconEngine()->catchIcon((RDRecording::Type)q->value(23).toUInt());
  icons[1]=rda->iconEngine()->stationIcon();
  if(!q->value(53).isNull()) {
    icons[6]=QImage::fromData(q->value(53).toByteArray()).
      scaled(22,22,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
  }

  //
  // Qt::DisplayType
  //
  QList<QVariant> texts;
  for(int i=0;i<columnCount();i++) {
    texts.push_back(QString());
  }

  //
  // Universal Attributes
  //
  texts[0]=q->value(1);                   // Description
  if(q->value(7).toString()=="Y") {       // Sun
    texts[7]=tr("Su");
  }
  if(q->value(8).toString()=="Y") {       // Mon
    texts[8]=tr("Mo");
  }
  if(q->value(9).toString()=="Y") {       // Tue
    texts[9]=tr("Tu");
  }
  if(q->value(10).toString()=="Y") {       // Wed
    texts[10]=tr("We");
  }
  if(q->value(11).toString()=="Y") {      // Thu
    texts[11]=tr("Th");
  }
  if(q->value(12).toString()=="Y") {      // Fri
    texts[12]=tr("Fr");
  }
  if(q->value(13).toString()=="Y") {      // Sat
    texts[13]=tr("Sa");
  }
  texts[15]=q->value(26).toString();      // One Shot
  texts[19]=q->value(41).toString();      // Error Status
  texts[20]=QString::asprintf("%d",q->value(25).toInt());  // Error Code
  texts[21]=q->value(0).toString();       // Id

  //
  // Type Specific Attributes
  //
  switch((RDRecording::Type)q->value(23).toInt()) {
  case RDRecording::Recording:
    texts[1]=q->value(3).toString()+
      QString::asprintf(" : %dR",q->value(21).toInt());
    switch((RDRecording::StartType)q->value(27).toUInt()) {
    case RDRecording::HardStart:
      texts[2]=tr("Hard")+": "+rda->timeString(q->value(4).toTime());
      break;

    case RDRecording::GpiStart:
      texts[2]=tr("Gpi")+": "+rda->timeString(q->value(4).toTime())+","+
	rda->timeString(q->value(4).toTime().addMSecs(q->value(28).toInt()))+
	","+
	QString::asprintf("%d:%d,",q->value(29).toInt(),q->value(30).toInt())+
	QTime(0,0,0).addMSecs(q->value(31).toUInt()).toString("mm:ss");
      break;
    }
    switch((RDRecording::EndType)q->value(32).toUInt()) {
    case RDRecording::LengthEnd:
      texts[3]=tr("Len")+": "+RDGetTimeLength(q->value(5).toUInt(),false,false);
      break;

    case RDRecording::HardEnd:
      texts[3]=tr("Hard")+": "+rda->timeString(q->value(33).toTime());
      break;

    case RDRecording::GpiEnd:
      texts[3]=tr("Gpi")+": "+rda->timeString(q->value(33).toTime())+","+
	rda->timeString(q->value(33).toTime().addMSecs(q->value(34).toInt()))+
	QString::asprintf(",%d:%d",q->value(35).toInt(),
			  q->value(36).toInt());
      break;
    }

    texts[6]=CutText(q->value(6).toString());
    icons[6]=rda->iconEngine()->catchIcon(RDRecording::Playout);
    sql=QString("select ")+
      "`SWITCH_STATION`,"+  // 00
      "`SWITCH_MATRIX` "+   // 01
      "from `DECKS` where "+
      "(`STATION_NAME`='"+RDEscapeString(q->value(3).toString())+"')&&"+
      QString::asprintf("(`CHANNEL`=%d)",q->value(21).toInt());
    q1=new RDSqlQuery(sql);
    if(q1->first()) {  // Source
      texts[5]=GetSourceName(q1->value(0).toString(),q1->value(1).toInt(),
			     q->value(14).toInt(),&ok);
      if(ok) {
	icons[5]=rda->iconEngine()->listIcon(RDIconEngine::Input);
      }
    }
    delete q1;
    texts[16]=LevelText(-q->value(17).toUInt());  // Autotrim
    texts[17]=LevelText(q->value(18).toInt());    // Normalization
    if((q->value(19).toUInt()==0)&&(q->value(20).toUInt()==0)) {
      texts[18]=tr("[none]");
    }
    else {
      texts[18]=   // Start/end Date Offsets
	QString::asprintf("%u / %u days",
			  q->value(19).toUInt(),q->value(20).toUInt());
    }
    break;

  case RDRecording::Playout:
    texts[1]=q->value(3).toString()+QString::asprintf(" : %dP",
						      q->value(21).toInt()-128);
    texts[2]=tr("Hard")+": "+rda->timeString(q->value(4).toTime());
    cut=new RDCut(q->value(6).toString());
    if(cut->exists()) {
      texts[3]=tr("Len")+": "+RDGetTimeLength(cut->length(),false,false);
    }
    delete cut;
    texts[5]=CutText(q->value(6).toString());
    icons[5]=rda->iconEngine()->catchIcon(RDRecording::Playout);
    break;

  case RDRecording::MacroEvent:
    texts[1]=q->value(3).toString();
    texts[2]=tr("Hard")+": "+q->value(4).toTime().
      toString(QString::asprintf("hh:mm:ss"));
    texts[5]=CartText(q->value(22).toUInt());
    icons[5]=rda->iconEngine()->catchIcon(RDRecording::MacroEvent);
    break;

  case RDRecording::SwitchEvent:
    texts[1]=q->value(3).toString();
    texts[2]=tr("Hard")+": "+rda->timeString(q->value(4).toTime());
    texts[4]=
      d_router_names.value(q->value(3).toString()).value(q->value(21).toInt());
    texts[5]=GetSourceName(q->value(3).toString(),  // Source
			   q->value(21).toInt(),
			   q->value(14).toInt(),&ok);
    if(ok) {
      icons[5]=rda->iconEngine()->listIcon(RDIconEngine::Input);
    }

    texts[6]=GetDestinationName(q->value(3).toString(),  // Dest
				q->value(21).toInt(),
				q->value(24).toInt());
    icons[6]=rda->iconEngine()->listIcon(RDIconEngine::Output);
    break;

  case RDRecording::Download:
    texts[1]=q->value(3).toString();
    texts[2]=tr("Hard")+": "+rda->timeString(q->value(4).toTime());
    texts[5]=q->value(37).toString();
    icons[5]=rda->iconEngine()->listIcon(RDIconEngine::Url);
    texts[6]=CutText(q->value(6).toString());
    icons[6]=rda->iconEngine()->catchIcon(RDRecording::Playout);
    texts[16]=LevelText(-q->value(17).toUInt());  // Autotrim
    texts[17]=LevelText(q->value(18).toInt());    // Normalization
    break;

  case RDRecording::Upload:
    texts[1]=q->value(3).toString();
    texts[2]=tr("Hard")+": "+rda->timeString(q->value(4).toTime());
    texts[5]=CutText(q->value(6).toString());
    icons[5]=rda->iconEngine()->catchIcon(RDRecording::Playout);
    if(q->value(39).toString().isEmpty()) {
      texts[6]=q->value(37).toString();
      icons[6]=rda->iconEngine()->listIcon(RDIconEngine::Url);
    }
    else {
      texts[6]=q->value(39).toString();    // Feed Key Name
    }
    if(q->value(39).toString().isEmpty()) {
      texts[14]=
	RDSettings::description((RDSettings::Format)q->value(42).toUInt(),
				q->value(43).toUInt(),q->value(44).toUInt(),
				q->value(45).toUInt(),q->value(46).toUInt());
      texts[17]=LevelText(q->value(18).toInt());    // Normalization
    }
    else {
      texts[14]=
	RDSettings::description((RDSettings::Format)q->value(47).toUInt(),
				q->value(48).toUInt(),q->value(49).toUInt(),
				q->value(50).toUInt(),q->value(51).toUInt());
      texts[17]=LevelText(q->value(52).toInt());    // Normalization
    }
    break;

  case RDRecording::LastType:
    break;
  }

  d_icons[row]=icons;
  d_texts[row]=texts;
}


QString RecordListModel::sqlFields() const
{
  QString sql=QString("select ")+
    "`RECORDINGS`.`ID`,"+                // 00
    "`RECORDINGS`.`DESCRIPTION`,"+       // 01
    "`RECORDINGS`.`IS_ACTIVE`,"+         // 02
    "`RECORDINGS`.`STATION_NAME`,"+      // 03
    "`RECORDINGS`.`START_TIME`,"+        // 04
    "`RECORDINGS`.`LENGTH`,"+            // 05
    "`RECORDINGS`.`CUT_NAME`,"+          // 06
    "`RECORDINGS`.`SUN`,"+               // 07
    "`RECORDINGS`.`MON`,"+               // 08
    "`RECORDINGS`.`TUE`,"+               // 09
    "`RECORDINGS`.`WED`,"+               // 10
    "`RECORDINGS`.`THU`,"+               // 11
    "`RECORDINGS`.`FRI`,"+               // 12
    "`RECORDINGS`.`SAT`,"+               // 13
    "`RECORDINGS`.`SWITCH_INPUT`,"+      // 14
    "`RECORDINGS`.`START_GPI`,"+         // 15
    "`RECORDINGS`.`END_GPI`,"+           // 16
    "`RECORDINGS`.`TRIM_THRESHOLD`,"+    // 17
    "`RECORDINGS`.`NORMALIZE_LEVEL`,"+   // 18
    "`RECORDINGS`.`STARTDATE_OFFSET`,"+  // 19
    "`RECORDINGS`.`ENDDATE_OFFSET`,"+    // 20
    "`RECORDINGS`.`CHANNEL`,"+           // 21
    "`RECORDINGS`.`MACRO_CART`,"+        // 22
    "`RECORDINGS`.`TYPE`,"+              // 23
    "`RECORDINGS`.`SWITCH_OUTPUT`,"+     // 24
    "`RECORDINGS`.`EXIT_CODE`,"+         // 25
    "`RECORDINGS`.`ONE_SHOT`,"+          // 26
    "`RECORDINGS`.`START_TYPE`,"+        // 27
    "`RECORDINGS`.`START_LENGTH`,"+      // 28
    "`RECORDINGS`.`START_MATRIX`,"+      // 29
    "`RECORDINGS`.`START_LINE`,"+        // 30
    "`RECORDINGS`.`START_OFFSET`,"+      // 31
    "`RECORDINGS`.`END_TYPE`,"+          // 32
    "`RECORDINGS`.`END_TIME`,"+          // 33
    "`RECORDINGS`.`END_LENGTH`,"+        // 34
    "`RECORDINGS`.`END_MATRIX`,"+        // 35
    "`RECORDINGS`.`END_LINE`,"+          // 36
    "`RECORDINGS`.`URL`,"+               // 37
    "`RECORDINGS`.`QUALITY`,"+           // 38
    "`FEEDS`.`KEY_NAME`,"+               // 39
    "`FEEDS`.`CHANNEL_IMAGE_ID`,"+       // 40
    "`RECORDINGS`.`EXIT_TEXT`,"+         // 41
    "`RECORDINGS`.`FORMAT`,"+            // 42
    "`RECORDINGS`.`CHANNELS`,"+          // 43
    "`RECORDINGS`.`SAMPRATE`,"+          // 44
    "`RECORDINGS`.`BITRATE`,"+           // 45
    "`RECORDINGS`.`QUALITY`,"+           // 46
    "`FEEDS`.`UPLOAD_FORMAT`,"+          // 47
    "`FEEDS`.`UPLOAD_CHANNELS`,"+        // 48
    "`FEEDS`.`UPLOAD_SAMPRATE`,"+        // 49
    "`FEEDS`.`UPLOAD_BITRATE`,"+         // 50
    "`FEEDS`.`UPLOAD_QUALITY`,"+         // 51
    "`FEEDS`.`NORMALIZE_LEVEL`,"+        // 52
    "`FEED_IMAGES`.`DATA` "+             // 53
    "from `RECORDINGS` left join `CUTS` "+
    "on (`RECORDINGS`.`CUT_NAME`=`CUTS`.`CUT_NAME`) left join `FEEDS` "+
    "on (`RECORDINGS`.`FEED_ID`=`FEEDS`.`ID`) left join `FEED_IMAGES` "+
    "on (`FEEDS`.`ID`=`FEED_IMAGES`.`FEED_ID`) ";

    return sql;
}


QString RecordListModel::GetSourceName(QString station,int matrix,int input,
				       bool *ok)
{
  QString input_name=tr("[none]");;
  QString sql=QString("select `NAME` from `INPUTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(station)+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",matrix)+
    QString::asprintf("(`NUMBER`=%d)",input);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if((*ok=q->first())) {
    input_name=q->value(0).toString();
  }
  delete q;
  return input_name;
}


QString RecordListModel::GetDestinationName(QString station,int matrix,
					    int output)
{
  QString output_name;
  QString sql=QString("select `NAME` from `OUTPUTS` where ")+
    "(`STATION_NAME`='"+RDEscapeString(station)+"')&&"+
    QString::asprintf("(`MATRIX`=%d)&&",matrix)+
    QString::asprintf("(`NUMBER`=%d)",output);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output_name=q->value(0).toString();
  }
  delete q;
  return output_name;
}


void RecordListModel::UpdateStatus(int line)
{
  switch(d_statuses.at(line)) {
  case RDDeck::Offline:
    d_back_colors[line]=QColor(EVENT_ERROR_COLOR);
    break;
  
  case RDDeck::Idle:
    d_back_colors[line]=QVariant();
    break;
	  
  case RDDeck::Ready:
    d_back_colors[line]=QColor(EVENT_READY_COLOR);
    break;

  case RDDeck::Waiting:
    d_back_colors[line]=QColor(EVENT_WAITING_COLOR);
    break;
  
  case RDDeck::Recording:
    d_back_colors[line]=QColor(EVENT_ACTIVE_COLOR);
    break;
  }

  RDRecording::ExitCode code=RDRecording::InternalError;
  QString err_text=tr("Unknown");

  //
  // Get Exit Code And Text
  //
  QString sql=QString("select ")+
    "`RECORDINGS`.`EXIT_CODE`,"+  // 00
    "`CUTS`.`ORIGIN_NAME`,"+      // 01
    "`CUTS`.`ORIGIN_DATETIME`,"+  // 02
    "`RECORDINGS`.`EXIT_TEXT` "+  // 03
    "from `RECORDINGS` left join `CUTS` "+
    "on `RECORDINGS`.`CUT_NAME`=`CUTS`.`CUT_NAME` where "+
    QString::asprintf("`RECORDINGS`.`ID`=%u",d_ids.at(line));
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    code=(RDRecording::ExitCode)q->value(0).toInt();
    err_text=q->value(3).toString();
  }
  delete q; 

  //
  // Exit Code/Text
  //
 d_texts[line][20]=QString::asprintf("%u",code);
  switch(code) {
  case RDRecording::Ok:
  case RDRecording::Downloading:
  case RDRecording::Uploading:
  case RDRecording::RecordActive:
  case RDRecording::PlayActive:
  case RDRecording::Waiting:
    d_texts[line][19]=RDRecording::exitString(code);
    break;
	
  case RDRecording::Short:
  case RDRecording::LowLevel:
  case RDRecording::HighLevel:
  case RDRecording::Interrupted:
  case RDRecording::DeviceBusy:
  case RDRecording::NoCut:
  case RDRecording::UnknownFormat:
    d_texts[line][19]=RDRecording::exitString(code);
    d_back_colors[line]=QColor(EVENT_ERROR_COLOR);
    break;
	
  case RDRecording::ServerError:
  case RDRecording::InternalError:
    d_texts[line][19]=RDRecording::exitString(code)+": "+err_text;
    d_back_colors[line]=QColor(EVENT_ERROR_COLOR);
    break;
  }
}


QString RecordListModel::CartText(unsigned cartnum) const
{
  return QString::asprintf("%06u",cartnum);
}


QString RecordListModel::CutText(const QString &cutname) const
{
  return QString::asprintf("%06u:%03d",RDCut::cartNumber(cutname),
			   RDCut::cutNumber(cutname));
}


QString RecordListModel::LevelText(int lvl) const
{
  QString ret=tr("[none]");

  if(lvl<0) {
    ret=QString::asprintf("%d dBFS",lvl/100);
  }

  return ret;
}
