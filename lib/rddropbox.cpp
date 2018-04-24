// rddropbox.cpp
//
// Abstract a Rivendell dropbox configuration.
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdconf.h>
#include <rddropbox.h>
#include <rdescape_string.h>

RDDropbox::RDDropbox(int id)
{
  box_id=id;
}


int RDDropbox::id() const
{
  return box_id;
}


QString RDDropbox::stationName() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"STATION_NAME").toString();
}


void RDDropbox::setStationName(const QString &name) const
{
  SetRow("STATION_NAME",name);
}


QString RDDropbox::groupName() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"GROUP_NAME").toString();
}


void RDDropbox::setGroupName(const QString &name) const
{
  SetRow("GROUP_NAME",name);
}


QString RDDropbox::path() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"PATH").toString();
}


void RDDropbox::setPath(const QString &path) const
{
  SetRow("PATH",path);
}


int RDDropbox::normalizationLevel() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"NORMALIZATION_LEVEL").toInt();
}


void RDDropbox::setNormalizationLevel(int lvl) const
{
  SetRow("NORMALIZATION_LEVEL",lvl);
}


int RDDropbox::autotrimLevel() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"AUTOTRIM_LEVEL").toInt();
}


void RDDropbox::setAutotrimLevel(int lvl) const
{
  SetRow("AUTOTRIM_LEVEL",lvl);
}


bool RDDropbox::singleCart() const
{
  return RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"SINGLE_CART").toString());
}


void RDDropbox::setSingleCart(bool state) const
{
  SetRow("SINGLE_CART",state);
}


unsigned RDDropbox::toCart() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"TO_CART").toUInt();
}


void RDDropbox::setToCart(unsigned cart) const
{
  SetRow("TO_CART",cart);
}


bool RDDropbox::useCartchunkId() const
{
  return 
    RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"USE_CARTCHUNK_ID").toString());
}


void RDDropbox::setUseCartchunkId(bool state) const
{
  SetRow("USE_CARTCHUNK_ID",state);
}


bool RDDropbox::titleFromCartchunkId() const
{
  return 
    RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"TITLE_FROM_CARTCHUNK_ID").toString());
}


void RDDropbox::setTitleFromCartchunkId(bool state) const
{
  SetRow("TITLE_FROM_CARTCHUNK_ID",state);
}


bool RDDropbox::deleteCuts() const
{
  return RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"DELETE_CUTS").toString());
}


void RDDropbox::setDeleteCuts(bool state) const
{
  SetRow("DELETE_CUTS",state);
}


bool RDDropbox::deleteSource() const
{
  return RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"DELETE_SOURCE").
		toString());
}


void RDDropbox::setDeleteSource(bool state) const
{
  SetRow("DELETE_SOURCE",state);
}


QString RDDropbox::metadataPattern() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"METADATA_PATTERN").toString();
}


void RDDropbox::setMetadataPattern(const QString &str) const
{
  SetRow("METADATA_PATTERN",str);
}


QString RDDropbox::userDefined() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"SET_USER_DEFINED").toString();
}


void RDDropbox::setUserDefined(const QString &str) const
{
  SetRow("SET_USER_DEFINED",str);
}


int RDDropbox::startdateOffset() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"STARTDATE_OFFSET").toInt();
}


void RDDropbox::setStartdateOffset(int offset) const
{
  SetRow("STARTDATE_OFFSET",offset);
}


int RDDropbox::enddateOffset() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"ENDDATE_OFFSET").toInt();
}


void RDDropbox::setEnddateOffset(int offset) const
{
  SetRow("ENDDATE_OFFSET",offset);
}


bool RDDropbox::fixBrokenFormats() const
{
  return RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"FIX_BROKEN_FORMATS").
	       toString());
}


void RDDropbox::setFixBrokenFormats(bool state) const
{
  SetRow("FIX_BROKEN_FORMATS",state);
}


QString RDDropbox::logPath() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"LOG_PATH").toString();
}


void RDDropbox::setLogPath(const QString &path) const
{
  SetRow("LOG_PATH",path);
}


bool RDDropbox::createDates() const
{
  return RDBool(RDGetSqlValue("DROPBOXES","ID",box_id,"IMPORT_CREATE_DATES").
	       toString());
}


void RDDropbox::setCreateDates(bool state) const
{
  SetRow("IMPORT_CREATE_DATES",state);
}


int RDDropbox::createStartdateOffset() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"CREATE_STARTDATE_OFFSET").toInt();
}


void RDDropbox::setCreateStartdateOffset(int offset) const
{
  SetRow("CREATE_STARTDATE_OFFSET",offset);
}


int RDDropbox::createEnddateOffset() const
{
  return RDGetSqlValue("DROPBOXES","ID",box_id,"CREATE_ENDDATE_OFFSET").toInt();
}


void RDDropbox::setCreateEnddateOffset(int offset) const
{
  SetRow("CREATE_ENDDATE_OFFSET",offset);
}


int RDDropbox::create(const QString &stationname)
{
  int ret=-1;

  QString sql=QString("insert into DROPBOXES set ")+
    "STATION_NAME=\""+RDEscapeString(stationname)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->lastInsertId().toInt();
  delete q;

  return ret;
}


void RDDropbox::remove(int id)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from DROPBOX_PATHS where ")+
    QString().sprintf("DROPBOX_ID=%d",id);
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("delete from DROPBOXES where ")+
    QString().sprintf("ID=%d",id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDropbox::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update DROPBOXES set ")+
    param+QString().sprintf("=%d where ID=%d",value,box_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDropbox::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update DROPBOXES set ")+
    param+QString().sprintf("=%u where ID=%d",value,box_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDropbox::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update DROPBOXES set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    QString().sprintf("ID=%d",box_id);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDropbox::SetRow(const QString &param,bool value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update DROPBOXES set ")+
    param+"=\""+RDYesNo(value)+"\" where "+
    QString().sprintf("ID=%d",box_id);
  q=new RDSqlQuery(sql);
  delete q;
}
