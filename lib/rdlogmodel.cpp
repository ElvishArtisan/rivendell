// rdlogmodel.cpp
//
// Data model for Rivendell logs
//
//   (C) Copyright 2020-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdlog.h"
#include "rdlog_line.h"
#include "rdlogmodel.h"

RDLogModel::RDLogModel(const QString &logname,bool read_only,QObject *parent)
  : QAbstractTableModel(parent)
{
  d_log_name=logname;
  d_read_only=read_only;

  MakeModel();
}


RDLogModel::RDLogModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  d_read_only=false;

  MakeModel();
}


RDLogModel::~RDLogModel()
{
  if(d_fms!=NULL) {
    delete d_fms;
  }
  if(d_bold_fms!=NULL) {
    delete d_bold_fms;
  }
  for(int i=0;i<d_log_lines.size();i++) {
    delete d_log_lines[i];
  }
}


QPalette RDLogModel::palette()
{
  return d_palette;
}


void RDLogModel::setPalette(const QPalette &pal)
{
  d_palette=pal;
}


QFont RDLogModel::normalFont() const
{
  return d_font;
}


QFont RDLogModel::boldFont() const
{
  return d_bold_font;
}


void RDLogModel::setFont(const QFont &font)
{
  int width;
  QString sql;
  RDSqlQuery *q=NULL;

  d_font=font;
  if(d_fms!=NULL) {
    delete d_fms;
  }
  d_fms=new QFontMetrics(d_font);
  d_bold_font=font;
  d_bold_font.setBold(true);
  if(d_bold_fms!=NULL) {
    delete d_bold_fms;
  }
  d_bold_fms=new QFontMetrics(d_bold_font);

  //
  // Calculate Minimum Column Widths
  //
  if(rda->showTwelveHourTime()) {
    d_size_hints[0]=QSize(40+d_bold_fms->width("T00:00:00.0 AM"),0);
  }
  else {
    d_size_hints[0]=QSize(40+d_bold_fms->width("T00:00:00.0"),0);
  }
  width=d_bold_fms->width(tr("PLAY"));
  if(d_bold_fms->width(tr("SEGUE"))>width) {
    width=d_bold_fms->width(tr("SEGUE"));
  }
  if(d_bold_fms->width(tr("STOP"))>width) {
    width=d_bold_fms->width(tr("STOP"));
  }
  d_size_hints[1]=QSize(10+width,0);
  d_size_hints[2]=QSize(10+d_bold_fms->width("000000"),0);

  width=0;
  sql=QString("select ")+
    "`NAME` "+  // 00
    "from `GROUPS`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(d_bold_fms->width(q->value(0).toString())>width) {
      width=d_bold_fms->width(q->value(0).toString());
    }
  }
  delete q;
  d_size_hints[3]=QSize(10+width,0);
  d_size_hints[4]=QSize(10+d_bold_fms->width("8:88:88"),0);

  width=0;
  for(int i=0;i<RDLogLine::LastSource;i++) {
    RDLogLine::Source src=(RDLogLine::Source)i;
    if(d_bold_fms->width(RDLogLine::sourceText(src))>width) {
      width=d_bold_fms->width(RDLogLine::sourceText(src));
    }
  }
  d_size_hints[10]=QSize(10+width,0);
}


int RDLogModel::columnCount(const QModelIndex &parent) const
{
  return d_headers.size();
}


int RDLogModel::rowCount(const QModelIndex &parent) const
{
  if(d_read_only) {
    return lineCount();
  }
  return 1+lineCount();
}


QVariant RDLogModel::headerData(int section,Qt::Orientation orient,int role) const
{
  if((orient==Qt::Horizontal)&&(role==Qt::DisplayRole)) {
    return d_headers.at(section);
  }
  return QVariant();
}


QVariant RDLogModel::data(const QModelIndex &index,int role) const
{
  QString str;
  RDLogLine *ll=NULL;
  int col=index.column();
  int row=index.row();

  if((ll=logLine(row))!=NULL) {
    switch((Qt::ItemDataRole)role) {
    case Qt::DisplayRole:
      return cellText(col,row,ll);

    case Qt::DecorationRole:
      return cellIcon(col,row,ll);

    case Qt::FontRole:
      return cellTextFont(col,row,ll);

    case Qt::TextColorRole:
      return cellTextColor(col,row,ll);

    case Qt::BackgroundRole:
      return rowBackgroundColor(row,ll);

    case Qt::SizeHintRole:
      return d_size_hints.at(col);

    case Qt::TextAlignmentRole:
      return d_alignments.at(col);

    default:
      break;
    }
  }
  else {  // End of log marker
    if(((Qt::ItemDataRole)role==Qt::DisplayRole)&&(index.column()==5)) {
      return tr("--- end of log ---");
    }
  }
  return QVariant();
}


bool RDLogModel::exists()
{
  return RDLog::exists(d_log_name);
}


bool RDLogModel::exists(int line)
{
  if((int)d_log_lines.size()>line) {
    return true;
  }
  return false;
}


bool RDLogModel::exists(const QTime &hard_time,int except_line)
{
  for(int i=0;i<lineCount();i++) {
    if((logLine(i)->timeType()==RDLogLine::Hard)&&
       (logLine(i)->startTime(RDLogLine::Logged)==hard_time)&&
       (i!=except_line)) {
      return true;
    }
  }
  return false;
}


QString RDLogModel::logName() const
{
  return d_log_name;
}


void RDLogModel::setLogName(QString logname)
{
  //  clear();
  RDLog *log=new RDLog(logname);
  d_log_name=log->name();  // So we normalize the case
  delete log;
}


QString RDLogModel::serviceName() const
{
  return d_service_name;
}


int RDLogModel::load(bool track_ptrs)
{
  RDLogLine line;
  QString sql;
  RDSqlQuery *q;

  beginResetModel();

  //
  // Get the service name
  //
  sql=QString("select `SERVICE` from `LOGS` where ")+
    "`NAME`='"+RDEscapeString(d_log_name)+"'";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    d_service_name=q->value(0).toString();
  }
  delete q;

  RDLog *log=new RDLog(d_log_name);
  d_max_id=log->nextId();
  delete log;

  LoadLines(d_log_name,0,track_ptrs);

  endResetModel();

  return d_log_lines.size();
}

void RDLogModel::saveModified(RDConfig *config,bool update_tracks)
{
  for(int i=0;i<d_log_lines.size();i++) {
    if(d_log_lines[i]->hasBeenModified()) {
      save(config,update_tracks, i);
    }
  }
}

void RDLogModel::save(RDConfig *config,bool update_tracks,int line)
{
  QString sql;
  RDSqlQuery *q;

  if(d_log_name.isEmpty()) {
    return;
  }
  if(line<0) {
    if(exists()) {
      sql=QString("delete from `LOG_LINES` where ")+
	"`LOG_NAME`='"+RDEscapeString(d_log_name)+"'";
      RDSqlQuery::apply(sql);
    }
    if (d_log_lines.size() > 0) {
       QString values = "";
       for(int i=0;i<d_log_lines.size();i++) {
         InsertLineValues(&values, i);
        if (i<d_log_lines.size()-1) {
           values += ",";
         }
       }
       InsertLines(values);
    }
  }
  else {
    sql=QString("delete from `LOG_LINES` where ")+
      "`LOG_NAME`='"+RDEscapeString(d_log_name)+"' && "+
      QString::asprintf("`COUNT`=%d",line);
    q=new RDSqlQuery(sql);
    delete q;
    SaveLine(line);
    // BPM - Clear the modified flag
    d_log_lines[line]->clearModified();
  }
  RDLog *log=new RDLog(d_log_name);
  if(log->nextId()<nextId()) {
    log->setNextId(nextId());
  }
  if(update_tracks) {
    log->updateTracks();
  }
  delete log;
}


int RDLogModel::append(const QString &logname,bool track_ptrs)
{
  return LoadLines(logname,d_max_id,track_ptrs);
}


void RDLogModel::clear()
{
  if(d_log_lines.size()>0) {
    beginResetModel();
    for(int i=0;i<d_log_lines.size();i++) {
      delete d_log_lines.at(i);
    }
    d_log_lines.clear();
    endResetModel();
  }
  d_log_name="";
  d_max_id=0;
}


int RDLogModel::validate(QString *report,const QDate &date)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  int errs=0;
  QDateTime now=QDateTime::currentDateTime();

  //
  // Report Header
  //
  *report="Rivendell Log Exception Report\n";
  *report+=QString("Generated at: ")+
    rda->shortDateString(now.date())+" - "+
    rda->timeString(now.time())+"\n";
  *report+=QString("Log: ")+d_log_name+"\n";
  *report+=QString("Effective Airdate: ")+rda->shortDateString(date)+"\n";
  *report+="\n";

  //
  // Line Scan
  //
  for(int i=0;i<lineCount();i++) {
    if(logLine(i)->cartNumber()>0) {
      sql=QString("select ")+
	"`TYPE`,"+   // 00
	"`TITLE` "+  // 01
	"from `CART` where "+
	QString::asprintf("`NUMBER`=%d",logLine(i)->cartNumber());
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	*report+=QString(" ")+
	  rda->timeString(logLine(i)->startTime(RDLogLine::Logged))+
	  QString::asprintf(" - missing cart %06d",logLine(i)->cartNumber())+
	  "\n";
	errs++;
      }
      else {
	if((RDCart::Type)q->value(0).toInt()==RDCart::Audio) {
	  if(logLine(i)->startTime(RDLogLine::Logged).isNull()) {
	    //
	    // Handle events with no logged start time (e.g. manual inserts)
	    //
		//TODO do we need to verify date here?
	    sql=QString("select `CUT_NAME` from `CUTS` where ")+
	      QString::asprintf("(`CART_NUMBER`=%u)&&",logLine(i)->cartNumber())+
	      "((`START_DATETIME` is null)||"+
	      "(`START_DATETIME`<='"+date.toString("yyyy-MM-dd")+" 23:59:59'))&&"+
	      "((`END_DATETIME` is null)||"+
	      "(`END_DATETIME`>='"+date.toString("yyyy-MM-dd")+" 00:00:00'))&&"+
	      "("+RDDowCode(date.dayOfWeek())+"='Y')&&(LENGTH>0)";
	  }
	  else {
		//TODO Do we need to verify date and logLine(i)->startTime?
	    sql=QString("select `CUT_NAME` from `CUTS` where ")+
	      QString::asprintf("(`CART_NUMBER`=%u)&&",logLine(i)->cartNumber())+
	      "((`START_DATETIME` is null)||"+
	      "(`START_DATETIME`<='"+date.toString("yyyy-MM-dd")+" "+
	      logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	      "'))&&"+
	      "((`END_DATETIME` is null)||"+
	      "(`END_DATETIME`>='"+date.toString("yyyy-MM-dd")+" "+
	      logLine(i)->startTime(RDLogLine::Logged).toString("hh:mm:ss")+
	      "'))&&"+
	      "((`START_DAYPART` is null)||"+
	      "(`START_DAYPART`<='"+
	      logLine(i)->startTime(RDLogLine::Logged).
	      toString("hh:mm:ss")+"'))&&"+
	      "((`END_DAYPART` is null)||"+
	      "(`END_DAYPART`>='"+logLine(i)->startTime(RDLogLine::Logged).
	      toString("hh:mm:ss")+"'))&&"+
	      "("+RDDowCode(date.dayOfWeek())+"='Y')&&(LENGTH>0)";
	  }
	  q1=new RDSqlQuery(sql);
	  if(!q1->first()) {
	    *report+=QString(" ")+
	      rda->timeString(logLine(i)->startTime(RDLogLine::Logged))+
	      QString::asprintf(" - cart %06d [",logLine(i)->cartNumber())+
	      q->value(1).toString()+"] "+QObject::tr("is not playable")+"\n";
	    errs++;
	  }
	  delete q1;
	}
      }
      delete q;
    }
  }
  *report+="\n";
  if(errs==1) {
    *report+=QString::asprintf("%d validation exception found.\n\n",errs);
  }
  else {
    *report+=QString::asprintf("%d validation exceptions found.\n\n",errs);
  }
  return errs;
}


void RDLogModel::update(int line)
{
  if(d_log_name.isEmpty()) {
    return;
  }
  if(d_log_lines[line]->cartNumber()>0) {
    QString sql=QString("select ")+
      "`CART`.`TYPE`,"+                  // 00
      "`CART`.`GROUP_NAME`,"+            // 01
      "`CART`.`TITLE`,"+                 // 02
      "`CART`.`ARTIST`,"+                // 03
      "`CART`.`ALBUM`,"+                 // 04
      "`CART`.`YEAR`,"+                  // 05
      "`CART`.`LABEL`,"+                 // 06
      "`CART`.`CLIENT`,"+                // 07
      "`CART`.`AGENCY`,"+                // 08
      "`CART`.`USER_DEFINED`,"+          // 09
      "`CART`.`FORCED_LENGTH`,"+         // 10
      "`CART`.`CUT_QUANTITY`,"+          // 11
      "`CART`.`LAST_CUT_PLAYED`,"+       // 12
      "`CART`.`PLAY_ORDER`,"+            // 13
      "`CART`.`ENFORCE_LENGTH`,"+        // 14
      "`CART`.`PRESERVE_PITCH`,"+        // 15
      "`CART`.`PUBLISHER`,"+             // 16
      "`CART`.`COMPOSER`,"+              // 17
      "`CART`.`USAGE_CODE`,"+            // 18
      "`CART`.`AVERAGE_SEGUE_LENGTH`,"+  // 19
      "`CART`.`VALIDITY`,"+              // 20
      "`CART`.`NOTES`,"+                 // 21
      "`GROUPS`.`COLOR` "+               // 22
      "from `CART` left join `GROUPS` "+
      "on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` where "+
      QString::asprintf("`CART`.`NUMBER`=%u",d_log_lines[line]->cartNumber());
    RDSqlQuery *q=new RDSqlQuery(sql);
    if(q->first()) {
      switch((RDCart::Type)q->value(0).toInt()) {
      case RDCart::Audio:
	d_log_lines[line]->setType(RDLogLine::Cart);
	break;
	  
      case RDCart::Macro:
	d_log_lines[line]->setType(RDLogLine::Macro);
	break;
	  
      default:
	break;
      }	
      d_log_lines[line]->
	setCartType((RDCart::Type)q->value(0).toInt());              // Cart Type
      d_log_lines[line]->setGroupName(q->value(1).toString());       // Group Name
      d_log_lines[line]->setTitle(q->value(2).toString());           // Title
      d_log_lines[line]->setArtist(q->value(3).toString());          // Artist
      d_log_lines[line]->setPublisher(q->value(16).toString());      // Publisher
      d_log_lines[line]->setComposer(q->value(17).toString());       // Composer
      d_log_lines[line]->setAlbum(q->value(4).toString());           // Album
      d_log_lines[line]->setYear(q->value(5).toDate());              // Year
      d_log_lines[line]->setLabel(q->value(6).toString());           // Label
      d_log_lines[line]->setClient(q->value(7).toString());          // Client
      d_log_lines[line]->setAgency(q->value(8).toString());          // Agency
      d_log_lines[line]->setUserDefined(q->value(9).toString());     // User Defined
      d_log_lines[line]->setUsageCode((RDCart::UsageCode)q->value(16).toInt());
      d_log_lines[line]->setForcedLength(q->value(10).toUInt());   // Forced Length
      d_log_lines[line]->setAverageSegueLength(q->value(19).toUInt());
      d_log_lines[line]->setCutQuantity(q->value(11).toUInt());       // Cut Quantity
      d_log_lines[line]->setLastCutPlayed(q->value(12).toUInt());  // Last Cut Played
      d_log_lines[line]->
	setPlayOrder((RDCart::PlayOrder)q->value(13).toUInt()); // Play Order
      d_log_lines[line]->
	setEnforceLength(RDBool(q->value(14).toString()));     // Enforce Length
      d_log_lines[line]->
	setPreservePitch(RDBool(q->value(15).toString()));     // Preserve Pitch
      d_log_lines[line]->setValidity((RDCart::Validity)q->value(20).toInt());
      d_log_lines[line]->setCartNotes(q->value(21).toString());   // Cart Notes
      d_log_lines[line]->setGroupColor(q->value(22).toString());  // Group Color
    }
    else {
      d_log_lines[line]->setValidity(RDCart::NeverValid);
    }
    delete q;
  }
  emitDataChanged(line);
}


int RDLogModel::lineCount() const
{
  return d_log_lines.size();
}


void RDLogModel::insert(int line,int num_lines,bool preserve_trans)
{
  if(!preserve_trans) {
    if((line>0)&&(d_log_lines[line-1]!=NULL)) {
      d_log_lines[line-1]->setEndPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line-1]->setSegueStartPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line-1]->setSegueEndPoint(-1,RDLogLine::LogPointer);
      emitDataChanged(line-1);
    }
    if(line<(lineCount()-1)) {
      d_log_lines[line]->setStartPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line]->setHasCustomTransition(false);
      emitDataChanged(line);
    }
  }
  if(line<lineCount()) {
    beginInsertRows(QModelIndex(),line,line+num_lines-1);
    for(int i=0;i<num_lines;i++) {
      d_log_lines.insert(line+i,new RDLogLine());
      d_log_lines[line+i]->setId(++d_max_id);
    }
    endInsertRows();
    return;
  }
  if(line>=lineCount()) {
    beginInsertRows(QModelIndex(),lineCount(),lineCount()+num_lines-1);
    for(int i=0;i<num_lines;i++) {
      d_log_lines.push_back(new RDLogLine());
      d_log_lines.back()->setId(++d_max_id);
    }
    endInsertRows();
    return;
  }
}


void RDLogModel::remove(int line,int num_lines,bool preserve_trans)
{
  if(!preserve_trans) {
    if(line>0) {
      d_log_lines[line-1]->setEndPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line-1]->setSegueStartPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line-1]->setSegueEndPoint(-1,RDLogLine::LogPointer);
      emitDataChanged(line-1);
    }
    if(line<((int)d_log_lines.size()-num_lines)) {
      d_log_lines[line+num_lines]->setStartPoint(-1,RDLogLine::LogPointer);
      d_log_lines[line+num_lines]->setHasCustomTransition(false);
      emitDataChanged(line+num_lines);
    }
  }  
  beginRemoveRows(QModelIndex(),line,line+num_lines-1);
  for(int i=0;i<num_lines;i++) {
    delete d_log_lines.at(line);
    d_log_lines.removeAt(line);
  }
  endRemoveRows();
}


void RDLogModel::move(int from_line,int to_line)
{
  int src_offset=0;
  int dest_offset=1;
  RDLogLine *srcline;
  RDLogLine *destline;

  if(to_line<from_line) {
    src_offset=1;
    dest_offset=0;
  }
  insert(to_line+dest_offset,1);
  if((to_line+1)>=lineCount()) {
    to_line=lineCount()-1;
    dest_offset=0;
  }

  if(((destline=logLine(to_line+dest_offset))==NULL)||
     (srcline=logLine(from_line+src_offset))==NULL) {
    remove(to_line+dest_offset,1);
    return;
  }
  *destline=*srcline;
  destline->clearTrackData(RDLogLine::AllTrans);
  remove(from_line+src_offset,1);
}


void RDLogModel::copy(int from_line,int to_line)
{
  RDLogLine *srcline;
  RDLogLine *destline;

  insert(to_line,1);
  if(((destline=logLine(to_line))==NULL)||
     (srcline=logLine(from_line))==NULL) {
    remove(to_line,1);
    return;
  }
  *destline=*srcline;
  destline->clearExternalData();
  destline->clearTrackData(RDLogLine::AllTrans);
  destline->setSource(RDLogLine::Manual);
}


int RDLogModel::length(int from_line,int to_line,QTime *sched_time)
{
  if(sched_time!=NULL) {
    *sched_time=QTime();
  }
  if(to_line<0) {
    to_line=lineCount();
    for(int i=from_line;i<lineCount();i++) {
      if(logLine(i)->timeType()==RDLogLine::Hard) {
	to_line=i;
	i=lineCount();
	if(sched_time!=NULL) {
	  *sched_time=logLine(i)->startTime(RDLogLine::Logged);
	}
      }
    }
  }
  int len=0;
  for(int i=from_line;i<to_line;i++) {
    if(((i+1)>=lineCount())||(logLine(i+1)->transType()!=RDLogLine::Segue)||
       (logLine(i)->segueStartPoint()<0)) {
      len+=logLine(i)->forcedLength();
    }
    else {
      len+=logLine(i)->segueStartPoint()-logLine(i)->startPoint();
    }
  }

  return len;
}


int RDLogModel::lengthToStop(int from_line,QTime *sched_time)
{
  int to_line=-1;

  for(int i=from_line;i<lineCount();i++) {
    if(logLine(i)->transType()==RDLogLine::Stop) {
      to_line=i;
    }
  }
  if(to_line<0) {
    return -1;
  }
  return length(from_line,to_line,sched_time);
}


bool RDLogModel::blockLength(int *nominal_length,int *actual_length,int line)
{
  *nominal_length=0;
  *actual_length=0;
  QTime start_time;
  int start_line=-1;
  QTime end_time;
  int end_line=-1;

  if((line<0)||(line>(lineCount()-1))) {
    *nominal_length=0;
    *actual_length=0;
    return false;
  }

  //
  // Find Block Start
  //
  for(int i=line;i>=0;i--) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      start_time=logLine(i)->startTime(RDLogLine::Logged);
      start_line=i;
      i=-1;
    }
  }
  if(start_line<0) {
    return false;
  }

  //
  // Find Block End
  //
  for(int i=line+1;i<lineCount();i++) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      end_time=logLine(i)->startTime(RDLogLine::Logged);
      end_line=i;
      i=lineCount();
    }
  }
  if(end_line<0) {
    return false;
  }

  //
  // Calculate Lengths
  //
  *nominal_length=start_time.msecsTo(end_time);
  for(int i=start_line;i<end_line;i++) {
	  if((i<(lineCount()+1))&&((logLine(i+1)->transType()==RDLogLine::Segue))) {
      *actual_length+=logLine(i)->averageSegueLength();
    }
    else {
      *actual_length+=logLine(i)->forcedLength();
    }
  }

  return true;
}

QTime RDLogModel::blockStartTime(int line) const
{
  int actual_length=0;
  QTime start_time(0,0,0);
  QTime return_time(0,0,0);
  int start_line=0;

  if((line<0)||(line>(lineCount()-1))) {
    actual_length=0;
    return return_time;
  }

  //
  // Find Block Start
  //
  for(int i=line;i>=0;i--) {
    if(logLine(i)->timeType()==RDLogLine::Hard) {
      start_time=logLine(i)->startTime(RDLogLine::Logged);
      start_line=i;
      i=-1;
    }
  }
  if(start_line == line) {
    return start_time;
  }

  //
  // Calculate Lengths
  //
  for(int i=start_line;i<line;i++) {
    if((i<(lineCount()+1))&&((logLine(i+1)->transType()==RDLogLine::Segue))) {
      if(logLine(i)->segueStartPoint(RDLogLine::LogPointer)<0) {
	actual_length+=100*(logLine(i)->averageSegueLength()/100);
      }
      else {
	if(logLine(i)->startPoint(RDLogLine::LogPointer)<0) {
	  actual_length+=(logLine(i)->segueStartPoint(RDLogLine::LogPointer)-
			  logLine(i)->startPoint(RDLogLine::CartPointer));
	}
	else {
	  actual_length+=(logLine(i)->segueStartPoint(RDLogLine::LogPointer)-
			  logLine(i)->startPoint(RDLogLine::LogPointer));
	}
      }
    }
    else {
      actual_length+=100*(logLine(i)->forcedLength()/100);
    }
  }
  return_time=start_time.addMSecs(actual_length);
  return return_time;
}


RDLogLine *RDLogModel::logLine(int line) const
{
  if((line<0)||(line>=d_log_lines.size())) {
    return NULL;
  }
  return d_log_lines[line];
}


void RDLogModel::setLogLine(int line,RDLogLine *ll)
{
  int id=d_log_lines[line]->id();
  *d_log_lines[line]=*ll;
  d_log_lines[line]->setId(id);
}


RDLogLine *RDLogModel::loglineById(int id, bool ignore_holdovers) const
{
  int line = lineById(id, ignore_holdovers);
  if(line == -1)
    return NULL;
  else
    return d_log_lines[line];
}


int RDLogModel::lineById(int id, bool ignore_holdovers) const
{
  for(int i=0;i<lineCount();i++) {
    if(ignore_holdovers && d_log_lines[i]->isHoldover()) {
      continue;
    }    
    if(d_log_lines[i]->id()==id) {
      return i;
    }
  }
  return -1;
}


int RDLogModel::lineByStartHour(int hour,RDLogLine::StartTimeType type) const
{
  for(int i=0;i<lineCount();i++) {
    if(!d_log_lines[i]->startTime(type).isNull()&&
       (d_log_lines[i]->startTime(type).hour()==hour)) {
      return i;
    }
  }
  return -1;
}


int RDLogModel::lineByStartHour(int hour) const
{
  int line=-1;

  if((line=lineByStartHour(hour,RDLogLine::Initial))<0) {
    if((line=lineByStartHour(hour,RDLogLine::Predicted))<0) {
      line=lineByStartHour(hour,RDLogLine::Imported);
    }
  }
  return line;
}


int RDLogModel::nextTimeStart(QTime after)
{
  for(int i=0;i<d_log_lines.size();i++) {
    if((d_log_lines[i]->timeType()==RDLogLine::Hard)&&
       (d_log_lines[i]->startTime(RDLogLine::Logged)>after)) {
      return i;
    }
  }
  return -1;
}


RDLogLine::TransType RDLogModel::nextTransType(int line)
{
  if(line<(lineCount()-1)) {
    return logLine(line+1)->transType();
  }
  return RDLogLine::Stop;
}


void RDLogModel::removeCustomTransition(int line)
{
  if((line<0)||(line>(lineCount()-1))) {
    return;
  }
  logLine(line)->setStartPoint(-1,RDLogLine::LogPointer);
  logLine(line)->setFadeupPoint(-1,RDLogLine::LogPointer);
  logLine(line)->setFadeupGain(0);
  logLine(line)->setDuckUpGain(0);
  logLine(line)->setHasCustomTransition(false);
  if(line<1) {
    return;
  }
  if(logLine(line-1)->type()!=RDLogLine::Track) {
    logLine(line-1)->setEndPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueStartPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueEndPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setSegueGain(RD_FADE_DEPTH);
    logLine(line-1)->setFadedownPoint(-1,RDLogLine::LogPointer);
    logLine(line-1)->setFadedownGain(0);
    logLine(line-1)->setDuckDownGain(0);
    return;
  }
  if(line<2) {
    return;
  }
  logLine(line-2)->setEndPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueStartPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueEndPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setSegueGain(RD_FADE_DEPTH);
  logLine(line-2)->setFadedownPoint(-1,RDLogLine::LogPointer);
  logLine(line-2)->setFadedownGain(0);
  logLine(line-2)->setDuckDownGain(0);
}


int RDLogModel::nextId() const
{
  int id=-1;
  for(int i=0;i<lineCount();i++) {
    if(d_log_lines[i]->id()>id) {
      id=d_log_lines[i]->id();
    }
  }
  return id+1;
}


int RDLogModel::nextLinkId() const
{
  int id=-1;
  for(int i=0;i<lineCount();i++) {
    if(d_log_lines[i]->linkId()>id) {
      id=d_log_lines[i]->linkId();
    }
  }
  return id+1;
}


QString RDLogModel::xml() const
{
  QString ret;

  ret+="<logList>\n";
  for(int i=0;i<lineCount();i++) {
    ret+=logLine(i)->xml(i);
  }
  ret+="</logList>\n";

  return ret;
}


void RDLogModel::setTransition(int line,RDLogLine::TransType trans)
{
  RDLogLine *ll=NULL;

  if((ll=logLine(line))!=NULL) {
    if(ll->transType()!=trans) {
      ll->setTransType(trans);
      emitDataChanged(line);
    }
  }
}


void RDLogModel::processNotification(RDNotification *notify)
{
  RDLogLine *ll=NULL;

  if(notify->type()==RDNotification::CartType) {
    unsigned cartnum=notify->id().toUInt();
    for(int i=0;i<lineCount();i++) {
      if(((ll=logLine(i))!=NULL)&&
	 ((ll->type()==RDLogLine::Cart)||(ll->type()==RDLogLine::Macro))&&
	 (ll->cartNumber()==cartnum)) {
	ll->refreshCart();
	emitDataChanged(i);
      }
    }
  }
}


void RDLogModel::setStartTimeStyle(RDLogModel::StartTimeStyle style)
{
  if(d_start_time_style!=style) {
    d_start_time_style=style;
    emit dataChanged(createIndex(0,0),createIndex(lineCount(),0));
  }
}


QString RDLogModel::StartTimeString(int line) const
{
  RDLogLine *ll=logLine(line);

  if(ll!=NULL) {
    QString code="H";
    switch(ll->timeType()) {
    case RDLogLine::Hard:
      if(ll->graceTime()<0) {
	code="S";
      }
      return code+rda->tenthsTimeString(ll->startTime(RDLogLine::Logged));
      break;

    default:
      if(d_start_time_style==RDLogModel::Estimated) {
	if(ll->startTime(RDLogLine::Predicted).isNull()) {
	  return rda->tenthsTimeString(blockStartTime(line));
	}
	else {
	  return rda->tenthsTimeString(ll->startTime(RDLogLine::Predicted));
	}
      }
      else {   // Scheduled
	if(ll->startTime(RDLogLine::Logged).isNull()) {
	  return QString("");
	}
	else {
	  return rda->tenthsTimeString(ll->startTime(RDLogLine::Logged));
	}
      }
      break;
    }
  }
  return QString();
}


int RDLogModel::LoadLines(const QString &logname,int id_offset,bool track_ptrs)
{
  RDLogLine line;
  RDSqlQuery *q1;
  QString sql;
  RDSqlQuery *q;
  bool prev_custom=false;
  unsigned lines=0;
  unsigned start_line=d_log_lines.size();

  //
  // Load the group color table
  //
  std::map<QString,QColor> group_colors;
  sql=QString("select ")+
    "`NAME`,"+   // 00
    "`COLOR` "+  // 01
    "from `GROUPS`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    group_colors[q->value(0).toString()]=QColor(q->value(1).toString());
  }
  delete q;

  //
  // Load log lines
  //
  sql=QString("select ")+
    "`LOG_LINES`.`LINE_ID`,"+            // 00
    "`LOG_LINES`.`CART_NUMBER`,"+        // 01
    "`LOG_LINES`.`START_TIME`,"+         // 02
    "`LOG_LINES`.`TIME_TYPE`,"+          // 03
    "`LOG_LINES`.`TRANS_TYPE`,"+         // 04
    "`LOG_LINES`.`START_POINT`,"+        // 05
    "`LOG_LINES`.`END_POINT`,"+          // 06
    "`LOG_LINES`.`SEGUE_START_POINT`,"+  // 07
    "`LOG_LINES`.`SEGUE_END_POINT`,"+    // 08
    "`CART`.`TYPE`,"+                    // 09
    "`CART`.`GROUP_NAME`,"+              // 10
    "`CART`.`TITLE`,"+                   // 11
    "`CART`.`ARTIST`,"+                  // 12
    "`CART`.`ALBUM`,"+                   // 13
    "`CART`.`YEAR`,"+                    // 14
    "`CART`.`LABEL`,"+                   // 15
    "`CART`.`CLIENT`,"+                  // 16
    "`CART`.`AGENCY`,"+                  // 17
    "`CART`.`USER_DEFINED`,"+            // 18
    "`CART`.`CONDUCTOR`,"+               // 19
    "`CART`.`SONG_ID`,"+                 // 20
    "`CART`.`FORCED_LENGTH`,"+           // 21
    "`CART`.`CUT_QUANTITY`,"+            // 22
    "`CART`.`LAST_CUT_PLAYED`,"+         // 23
    "`CART`.`PLAY_ORDER`,"+              // 24
    "`CART`.`ENFORCE_LENGTH`,"+          // 25
    "`CART`.`PRESERVE_PITCH`,"+         // 26
    "`LOG_LINES`.`TYPE`,"+               // 27
    "`LOG_LINES`.`COMMENT`,"+            // 28
    "`LOG_LINES`.`LABEL`,"+              // 29
    "`LOG_LINES`.`GRACE_TIME`,"+         // 30
    "`LOG_LINES`.`SOURCE`,"+             // 31
    "`LOG_LINES`.`EXT_START_TIME`,"+     // 32
    "`LOG_LINES`.`EXT_LENGTH`,"+         // 33
    "`LOG_LINES`.`EXT_DATA`,"+           // 34
    "`LOG_LINES`.`EXT_EVENT_ID`,"+       // 35
    "`LOG_LINES`.`EXT_ANNC_TYPE`,"+      // 36
    "`LOG_LINES`.`EXT_CART_NAME`,"+      // 37
    "`CART`.`ASYNCRONOUS`,"+             // 38
    "`LOG_LINES`.`FADEUP_POINT`,"+       // 39
    "`LOG_LINES`.`FADEUP_GAIN`,"+        // 40
    "`LOG_LINES`.`FADEDOWN_POINT`,"+     // 41
    "`LOG_LINES`.`FADEDOWN_GAIN`,"+      // 42
    "`LOG_LINES`.`SEGUE_GAIN`,"+         // 43
    "`CART`.`PUBLISHER`,"+               // 44
    "`CART`.`COMPOSER`,"+                // 45
    "`CART`.`USAGE_CODE`,"+              // 46
    "`CART`.`AVERAGE_SEGUE_LENGTH`,"+    // 47 
    "`LOG_LINES`.`LINK_EVENT_NAME`,"+    // 48
    "`LOG_LINES`.`LINK_START_TIME`,"+    // 49
    "`LOG_LINES`.`LINK_LENGTH`,"+        // 50
    "`LOG_LINES`.`LINK_ID`,"+            // 51
    "`LOG_LINES`.`LINK_EMBEDDED`,"+      // 52
    "`LOG_LINES`.`ORIGIN_USER`,"+        // 53
    "`LOG_LINES`.`ORIGIN_DATETIME`,"+    // 54
    "`CART`.`VALIDITY`,"+                // 55
    "`LOG_LINES`.`LINK_START_SLOP`,"+    // 56
    "`LOG_LINES`.`LINK_END_SLOP`,"+      // 57
    "`LOG_LINES`.`DUCK_UP_GAIN`,"+       // 58
    "`LOG_LINES`.`DUCK_DOWN_GAIN`,"+     // 59
    "`CART`.`START_DATETIME`,"+          // 60
    "`CART`.`END_DATETIME`,"+            // 61
    "`LOG_LINES`.`EVENT_LENGTH`,"+       // 62
    "`CART`.`USE_EVENT_LENGTH`,"+        // 63
    "`CART`.`NOTES` "+                   // 64
    "from `LOG_LINES` left join `CART` "+
    "on `LOG_LINES`.`CART_NUMBER`=`CART`.`NUMBER` where "+
    "`LOG_LINES`.`LOG_NAME`='"+RDEscapeString(logname)+"' "+
    "order by `COUNT`";
  q=new RDSqlQuery(sql);
  if(q->size()<=0) {
    delete q;
    return 0;
  }
  beginInsertRows(QModelIndex(),lineCount(),q->size()+lineCount()-1);
  for(int i=0;i<q->size();i++) {
    lines++;
    line.clear();
    q->next();
    line.setType((RDLogLine::Type)q->value(27).toInt());       // Type
    line.setId(q->value(0).toInt()+id_offset);                 // Log Line ID
    if((q->value(0).toInt()+id_offset)>d_max_id) {
      d_max_id=q->value(0).toInt()+id_offset;
    }
    line.setStartTime(RDLogLine::Imported,
		      QTime(0,0,0).addMSecs(q->value(2).toInt())); // Start Time
    line.setStartTime(RDLogLine::Logged,
		      QTime(0,0,0).addMSecs(q->value(2).toInt()));
    line.
      setTimeType((RDLogLine::TimeType)q->value(3).toInt());   // Time Type
    line.
      setTransType((RDLogLine::TransType)q->value(4).toInt()); // Trans Type
    line.setMarkerComment(q->value(28).toString());            // Comment
    line.setMarkerLabel(q->value(29).toString());              // Label
    line.setGraceTime(q->value(30).toInt());                   // Grace Time
    line.setUseEventLength(RDBool(q->value(63).toString())); // Use Event Length
    line.setEventLength(q->value(62).toInt());                 // Event Length
    line.setSource((RDLogLine::Source)q->value(31).toUInt());
    line.setLinkEventName(q->value(48).toString());           // Link Event Name
    line.setLinkStartTime(QTime(0,0,0).addMSecs(q->value(49).toInt())); // Link Start Time
    line.setLinkLength(q->value(50).toInt());               // Link Length
    line.setLinkStartSlop(q->value(56).toInt());            // Link Start Slop
    line.setLinkEndSlop(q->value(57).toInt());              // Link End Slop
    line.setLinkId(q->value(51).toInt());                   // Link ID
    line.setLinkEmbedded(RDBool(q->value(52).toString()));  // Link Embedded
    line.setOriginUser(q->value(53).toString());            // Origin User
    line.setOriginDateTime(q->value(54).toDateTime());      // Origin DateTime
    switch(line.type()) {
    case RDLogLine::Cart:
      line.setCartNumber(q->value(1).toUInt());          // Cart Number
      line.setStartPoint(q->value(5).toInt(),RDLogLine::LogPointer);
      line.setEndPoint(q->value(6).toInt(),RDLogLine::LogPointer);
      line.setSegueStartPoint(q->value(7).toInt(),RDLogLine::LogPointer);
      line.setSegueEndPoint(q->value(8).toInt(),RDLogLine::LogPointer);
      line.setCartType((RDCart::Type)q->value(9).toInt());  // Cart Type
      line.setGroupName(q->value(10).toString());       // Group Name
      line.setGroupColor(group_colors[q->value(10).toString()]);
      line.setTitle(q->value(11).toString());           // Title
      line.setArtist(q->value(12).toString());          // Artist
      line.setPublisher(q->value(44).toString());       // Publisher
      line.setComposer(q->value(45).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(64).toString());       // Cart Notes
      line.setConductor(q->value(19).toString());       // Conductor
      line.setSongId(q->value(20).toString());          // Song ID
      line.setUsageCode((RDCart::UsageCode)q->value(46).toInt());
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      if(q->value(7).toInt()<0) {
	line.setAverageSegueLength(q->value(47).toInt());
      }
      else {
	line.
	  setAverageSegueLength(q->value(7).toInt()-q->value(5).toInt());
      }
      line.setCutQuantity(q->value(22).toUInt());       // Cut Quantity
      line.setLastCutPlayed(q->value(23).toUInt());     // Last Cut Played
      line.
	setPlayOrder((RDCart::PlayOrder)q->value(24).toUInt()); // Play Ord
      line.
	setEnforceLength(RDBool(q->value(25).toString())); // Enforce Length
      line.
	setPreservePitch(RDBool(q->value(26).toString())); // Preserve Pitch
      if(!q->value(32).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(32).toTime());
      }
      if(!q->value(33).isNull()) {                      // Ext Length
	line.setExtLength(q->value(33).toInt());
      }
      if(!q->value(34).isNull()) {                      // Ext Data
	line.setExtData(q->value(34).toString());
      }
      if(!q->value(35).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(37).toString());
      }
      if(!q->value(39).isNull()) {                      // FadeUp Point
	line.setFadeupPoint(q->value(39).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(40).isNull()) {                      // FadeUp Gain
	line.setFadeupGain(q->value(40).toInt());
      }
      if(!q->value(41).isNull()) {                      // FadeDown Point
	line.setFadedownPoint(q->value(41).toInt(),RDLogLine::LogPointer);
      }
      if(!q->value(42).isNull()) {                      // FadeDown Gain
	line.setFadedownGain(q->value(42).toInt());
      }
      if(!q->value(43).isNull()) {                      // Segue Gain
	line.setSegueGain(q->value(43).toInt());
      }
      if(!q->value(58).isNull()) {                      // Duck Up Gain
	line.setDuckUpGain(q->value(58).toInt());
      }
      if(!q->value(59).isNull()) {                      // Duck Down Gain
	line.setDuckDownGain(q->value(59).toInt());
      }
      if(!q->value(60).isNull()) {                      // Start Datetime
	line.setStartDatetime(q->value(60).toDateTime());
      }
      if(!q->value(61).isNull()) {                      // End Datetime
	line.setEndDatetime(q->value(61).toDateTime());
      }
      line.setValidity((RDCart::Validity)q->value(55).toInt()); // Validity
      break;

    case RDLogLine::Macro:
      line.setCartNumber(q->value(1).toUInt());          // Cart Number
      line.setCartType((RDCart::Type)q->value(9).toInt());  // Cart Type
      line.setGroupName(q->value(10).toString());       // Group Name
      line.setGroupColor(group_colors[q->value(10).toString()]);
      line.setTitle(q->value(11).toString());           // Title
      line.setArtist(q->value(12).toString());          // Artist
      line.setPublisher(q->value(44).toString());       // Publisher
      line.setComposer(q->value(45).toString());        // Composer
      line.setAlbum(q->value(13).toString());           // Album
      line.setYear(q->value(14).toDate());              // Year
      line.setLabel(q->value(15).toString());           // Label
      line.setClient(q->value(16).toString());          // Client
      line.setAgency(q->value(17).toString());          // Agency
      line.setUserDefined(q->value(18).toString());     // User Defined
      line.setCartNotes(q->value(64).toString());       // Cart Notes
      line.setForcedLength(q->value(21).toUInt());      // Forced Length
      line.setAverageSegueLength(q->value(21).toInt());
      if(!q->value(32).isNull()) {                      // Ext Start Time
	line.setExtStartTime(q->value(32).toTime());
      }
      if(!q->value(33).isNull()) {                      // Ext Length
	line.setExtLength(q->value(33).toInt());
      }
      if(!q->value(34).isNull()) {                      // Ext Data
	line.setExtData(q->value(34).toString());
      }
      if(!q->value(35).isNull()) {                      // Ext Event ID
	line.setExtEventId(q->value(35).toString());
      }
      if(!q->value(36).isNull()) {                      // Ext Annc. Type
	line.setExtAnncType(q->value(36).toString());
      }
      if(!q->value(37).isNull()) {                      // Ext Cart Name
	line.setExtCartName(q->value(37).toString());
      }
      if(!q->value(38).isNull()) {                      // Asyncronous
	line.setAsyncronous(RDBool(q->value(38).toString()));
      }
      break;

    case RDLogLine::Marker:
      break;

    case RDLogLine::Track:
      break;

    case RDLogLine::Chain:
      sql=QString("select `DESCRIPTION` from `LOGS` where ")+
	"`NAME`='"+RDEscapeString(line.markerLabel())+"'";
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	line.setMarkerComment(q1->value(0).toString());
      }
      delete q1;
      break;

    default:
      break;
    }

    line.setHasCustomTransition(prev_custom||(q->value(5).toInt()>=0)||\
				(q->value(39).toInt()>=0));
    if(line.type()==RDLogLine::Cart) {
      prev_custom=(q->value(6).toInt()>=0)||(q->value(7).toInt()>=0)||
	(q->value(8).toInt()>=0)||(q->value(41).toInt()>=0);
    }
    else {
      prev_custom=false;
    }
    line.clearModified();
    d_log_lines.push_back(new RDLogLine(line));
  }
  endInsertRows();
  delete q;

  if(track_ptrs) {
    //
    // Load default cart pointers for "representative" cuts.  This is
    // really only useful when setting up a voice tracker.
    //
    for(int i=start_line;i<lineCount();i++) {
      RDLogLine *ll=logLine(i);
      if(ll->cartType()==RDCart::Audio) {
	sql=QString("select ")+
	  "`START_POINT`,"+        // 00
	  "`END_POINT`,"+          // 01
	  "`SEGUE_START_POINT`,"+  // 02
	  "`SEGUE_END_POINT`,"+    // 03
	  "`TALK_START_POINT`,"+   // 04
	  "`TALK_END_POINT`,"+     // 05
	  "`HOOK_START_POINT`,"+   // 06
	  "`HOOK_END_POINT`,"+     // 07
	  "`FADEUP_POINT`,"+       // 08
	  "`FADEDOWN_POINT`,"+     // 09
	  "`CUT_NAME`,"+           // 10
	  "`ORIGIN_NAME`,"+        // 11
	  "`ORIGIN_DATETIME`,"+    // 12
	  "`DESCRIPTION`,"+        // 13
	  "`ISRC`,"+               // 14
	  "`ISCI`,"+               // 15
	  "`RECORDING_MBID`,"+     // 16
	  "`RELEASE_MBID` "+       // 17
	  "from `CUTS` where "+
	  QString::asprintf("`CART_NUMBER`=%u ",ll->cartNumber())+
	  "order by `CUT_NAME`";
	q=new RDSqlQuery(sql);
	if(q->first()) {
	  ll->setStartPoint(q->value(0).toInt(),RDLogLine::CartPointer);
	  ll->setEndPoint(q->value(1).toInt(),RDLogLine::CartPointer);
	  ll->setSegueStartPoint(q->value(2).toInt(),RDLogLine::CartPointer);
	  ll->setSegueEndPoint(q->value(3).toInt(),RDLogLine::CartPointer);
	  ll->setTalkStartPoint(q->value(4).toInt());
	  ll->setTalkEndPoint(q->value(5).toInt());
	  ll->setHookStartPoint(q->value(6).toInt());
	  ll->setHookEndPoint(q->value(7).toInt());
	  ll->setFadeupPoint(q->value(8).toInt(),RDLogLine::CartPointer);
	  ll->setFadedownPoint(q->value(9).toInt(),RDLogLine::CartPointer);
	  ll->setCutNumber(RDCut::cutNumber(q->value(10).toString()));
	  ll->setOriginUser(q->value(11).toString());
	  ll->setOriginDateTime(q->value(12).toDateTime());
	  ll->setDescription(q->value(13).toString());
	  ll->setIsrc(q->value(14).toString());
	  ll->setIsci(q->value(15).toString());
	  ll->setRecordingMbId(q->value(16).toString());
	  ll->setReleaseMbId(q->value(17).toString());
	}
	delete q;
      }
    }
  }

  return lines;
}


void RDLogModel::InsertLines(QString values) {
  QString sql;
  RDSqlQuery *q;

  sql = QString("insert into LOG_LINES (")+
    "`LOG_NAME`,"+           // 00
    "`LINE_ID`,"+            // 01
    "`COUNT`,"+              // 02
    "`CART_NUMBER`,"+        // 03
    "`START_TIME`,"+         // 04
    "`TIME_TYPE`,"+          // 05
    "`TRANS_TYPE`,"+         // 06
    "`START_POINT`,"+        // 07
    "`END_POINT`,"+          // 08
    "`SEGUE_START_POINT`,"+  // 09
    "`SEGUE_END_POINT`,"+    // 10
    "`TYPE`,"+               // 11
    "`COMMENT`,"+            // 12
    "`LABEL`,"+              // 13
    "`GRACE_TIME`,"+         // 14
    "`SOURCE`,"+             // 15
    "`EXT_START_TIME`,"+     // 16
    "`EXT_LENGTH`,"+         // 17
    "`EXT_DATA`,"+           // 18
    "`EXT_EVENT_ID`,"+       // 19
    "`EXT_ANNC_TYPE`,"+      // 20
    "`EXT_CART_NAME`,"+      // 21
    "`FADEUP_POINT`,"+       // 22
    "`FADEUP_GAIN`,"+        // 23
    "`FADEDOWN_POINT`,"+     // 24
    "`FADEDOWN_GAIN`,"+      // 25
    "`SEGUE_GAIN`,"+         // 26
    "`LINK_EVENT_NAME`,"+    // 27
    "`LINK_START_TIME`,"+    // 28
    "`LINK_LENGTH`,"+        // 29
    "`LINK_ID`,"+            // 30
    "`LINK_EMBEDDED`,"+      // 31
    "`ORIGIN_USER`,"+        // 32
    "`ORIGIN_DATETIME`,"+    // 33
    "`LINK_START_SLOP`,"+    // 34
    "`LINK_END_SLOP`,"+      // 35
    "`DUCK_UP_GAIN`,"+       // 36
    "`DUCK_DOWN_GAIN`,"+     // 37
    "`EVENT_LENGTH`) "+      // 38
    "values "+values;
  q=new RDSqlQuery(sql);
  delete q;
}


void RDLogModel::InsertLineValues(QString *query, int line)
{
  // one line to save query space
  RDLogLine *ll=d_log_lines[line];
  QString sql=QString("(")+
    "'"+RDEscapeString(d_log_name)+"',"+
    QString::asprintf("%d,",ll->id())+
    QString::asprintf("%d,",line)+
    QString::asprintf("%u,",ll->cartNumber())+
    QString::asprintf("%d,",QTime(0,0,0).msecsTo(ll->startTime(RDLogLine::Logged)))+
    QString::asprintf("%d,",ll->timeType())+
    QString::asprintf("%d,",ll->transType())+
    QString::asprintf("%d,",ll->startPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->endPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->segueStartPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->segueEndPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->type())+
    "'"+RDEscapeString(ll->markerComment())+"',"+
    "'"+RDEscapeString(ll->markerLabel())+"',"+
    QString::asprintf("%d,",ll->graceTime())+
    QString::asprintf("%d,",ll->source())+
    RDCheckDateTime(ll->extStartTime(),"hh:mm:ss")+","+
    QString::asprintf("%d,",ll->extLength())+
    "'"+RDEscapeString(ll->extData())+"',"+
    "'"+RDEscapeString(ll->extEventId())+"',"+
    "'"+RDEscapeString(ll->extAnncType())+"',"+
    "'"+RDEscapeString(ll->extCartName())+"',"+
    QString::asprintf("%d,",ll->fadeupPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->fadeupGain())+
    QString::asprintf("%d,",ll->fadedownPoint(RDLogLine::LogPointer))+
    QString::asprintf("%d,",ll->fadedownGain())+
    QString::asprintf("%d,",ll->segueGain())+
    "'"+RDEscapeString(ll->linkEventName())+"',"+
    QString::asprintf("%d,",QTime(0,0,0).msecsTo(ll->linkStartTime()))+
    QString::asprintf("%d,",ll->linkLength())+
    QString::asprintf("%d,",ll->linkId())+
    "'"+RDYesNo(ll->linkEmbedded())+"',"+
    "'"+RDEscapeString(ll->originUser())+"',"+
    RDCheckDateTime(ll->originDateTime(),"yyyy-MM-dd hh:mm:ss")+","+
    QString::asprintf("%d,",ll->linkStartSlop())+
    QString::asprintf("%d,",ll->linkEndSlop())+
    QString::asprintf("%d,",ll->duckUpGain())+
    QString::asprintf("%d,",ll->duckDownGain())+
    QString::asprintf("%d)",ll->eventLength());
  *query += sql;
}

void RDLogModel::SaveLine(int line)
{
  QString values = "";
  InsertLineValues(&values, line);
  InsertLines(values);
}


void RDLogModel::emitDataChanged(int row)
{
  QModelIndex left=createIndex(row,0);
  QModelIndex right=createIndex(row,columnCount());

  emit dataChanged(left,right);
}


void RDLogModel::emitAllDataChanged()
{
  QModelIndex left=createIndex(0,0);
  QModelIndex right=createIndex(lineCount(),columnCount());

  emit dataChanged(left,right);
}


QStringList RDLogModel::headerTexts() const
{
  QStringList ret;

  ret.push_back(tr("Start Time"));
  ret.push_back(tr("Trans"));
  ret.push_back(tr("Cart"));
  ret.push_back(tr("Group"));
  ret.push_back(tr("Length"));
  ret.push_back(tr("Title"));
  ret.push_back(tr("Artist"));
  ret.push_back(tr("Client"));
  ret.push_back(tr("Agency"));
  ret.push_back(tr("Label"));
  ret.push_back(tr("Source"));
  ret.push_back(tr("Ext Data"));
  ret.push_back(tr("Line ID"));
  ret.push_back(tr("Count"));

  return ret;
}


QList<int> RDLogModel::columnAlignments() const
{
  QList<int> ret;
  int left=Qt::AlignLeft|Qt::AlignVCenter;
  int center=Qt::AlignCenter;
  int right=Qt::AlignRight|Qt::AlignVCenter;

  ret.push_back(right);   // Start Time
  ret.push_back(center);  // Trans
  ret.push_back(center);  // Cart
  ret.push_back(center);  // Group
  ret.push_back(right);   // Length
  ret.push_back(left);    // Title
  ret.push_back(left);    // Artist
  ret.push_back(left);    // Client
  ret.push_back(left);    // Agency
  ret.push_back(left);    // Label
  ret.push_back(left);    // Source
  ret.push_back(left);    // Ext Data
  ret.push_back(right);   // Line ID
  ret.push_back(right);   // Count

  return ret;
}


QPixmap RDLogModel::cellIcon(int col,int row,RDLogLine *ll) const
{
  if(col==0) {
    return rda->iconEngine()->typeIcon(ll->type(),ll->source());
  }
  return QPixmap();
}


QString RDLogModel::cellText(int col,int line,RDLogLine *ll) const
{
  switch(col) {
  case 0:  // Start Time
    return StartTimeString(line);

  case 1:  // Transition
    return RDLogLine::transText(ll->transType());

  case 2:  // Cart Number
    return ll->cartNumberText();

  case 3:  // Group
    return ll->groupName();

  case 4:  // Length
    return ll->forcedLengthText();

  case 5:  // Title
    return ll->titleText();

  case 6:  // Artist
    return ll->artist();

  case 7:  // Client
    return ll->client();

  case 8:  // Agency
    return ll->agency();

  case 9:  // Label
    return ll->markerLabel();

  case 10:  // Source
    return RDLogLine::sourceText(ll->source());

  case 11:  // Ext Data
    switch(ll->type()) {
    case RDLogLine::MusicLink:
    case RDLogLine::TrafficLink:
      return ll->linkSummaryText();

    case RDLogLine::Cart:
    case RDLogLine::Marker:
    case RDLogLine::Macro:
    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::Chain:
    case RDLogLine::Track:
    case RDLogLine::UnknownType:
      return ll->extData();
    }
    break;

  case 12:  // Line ID
    return QString::asprintf("%d",ll->id());

  case 13:  // Count
    return QString::asprintf("%d",line);
  }
  return QString();
}


QFont RDLogModel::cellTextFont(int col,int line,RDLogLine *ll) const
{
  if(col==3) {
    return d_bold_font;
  }
  return d_font;
}


QColor RDLogModel::cellTextColor(int col,int line,RDLogLine *ll) const
{
  switch(col) {
  case 0:
    if(ll->timeType()==RDLogLine::Hard) {
      return Qt::blue;
    }
    break;
    
  case 3:
    return ll->groupColor();
  }

  return RDGetTextColor(rowBackgroundColor(line,ll));
}


QColor RDLogModel::rowBackgroundColor(int line,RDLogLine *ll) const
{
  return d_palette.color(QPalette::Base);
}


void RDLogModel::MakeModel()
{
  d_fms=NULL;
  d_bold_fms=NULL;
  d_start_time_style=RDLogModel::Scheduled;
  d_max_id=0;

  QStringList headers=headerTexts();
  QList<int> alignments=columnAlignments();

  if(headers.size()!=alignments.size()) {
    fprintf(stderr,"header/alignment size mismatch\n");
    exit(1);
  }

  for(int i=0;i<headers.size();i++) {
    d_headers.push_back(headers.at(i));
    d_alignments.push_back(alignments.at(i));
    d_size_hints.push_back(QVariant());
  }
}
