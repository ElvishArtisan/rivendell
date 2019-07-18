// rdnexus.cpp
//
// A container class for MusicMaster Nexus API
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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
//
//   RDNexus class interfaces with the MusicMaster scheduling program
//   through their Nexus API (https://www.musicmaster.com/docs/nexus_api/).
//

#include <QDomDocument>
#include <QString>
#include <QStringList>
#include <QDebug>

#include <curl/curl.h>
#include <syslog.h>

#include "rdconf.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdgroup_list.h"
#include "rdhttpserver.h"
#include "rdnexus.h"

size_t RDNexusCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  QString *xml=(QString *)userdata;
  for(unsigned i=0;i<(size*nmemb);i++) {
    *xml+=((const char *)ptr)[i];
  }
  return size*nmemb;
}

RDNexus::RDNexus()
{
  resp_valid=-1;
  resp_status="";
  resp_command="";
  resp_station="";
  resp_messageid=-1;

  nexus_debug=false;

  nexus_server="";
  nexus_port=0;

  nexus_station="";

  nexus_curl=NULL;

  nexus_http_server=NULL;

  //
  // Load Rivendell Service to Nexus Station Map
  //
  loadStationMap();
}

RDNexus::~RDNexus()
{
  if(nexus_curl!=NULL) {
    curl_easy_cleanup(nexus_curl);
  }
  if(slist!=NULL) {
    curl_slist_free_all(slist);
  }
}


void RDNexus::initializeCurl(int timeout)
{
  if(nexus_curl!=NULL) {
    curl_easy_cleanup(nexus_curl);
    nexus_curl=NULL;
  }
  if(slist!=NULL) {
    curl_slist_free_all(slist);
    slist=NULL;
  }
  
  QString url=nexus_server;

  if(nexus_port) {
    url+=QString(":%1").arg(nexus_port);
  }

  if ((nexus_curl=curl_easy_init())!=NULL) {
    curl_easy_setopt(nexus_curl,CURLOPT_URL,qPrintable(url));

    slist=curl_slist_append(slist, "Content-Type: text/plain");
    curl_easy_setopt(nexus_curl,CURLOPT_HTTPHEADER,slist);

    curl_easy_setopt(nexus_curl,CURLOPT_WRITEFUNCTION,RDNexusCallback);
    curl_easy_setopt(nexus_curl,CURLOPT_WRITEDATA,&nexus_response);
    curl_easy_setopt(nexus_curl,CURLOPT_TIMEOUT,timeout);
  }
}

void RDNexus::setDebug(bool debug)
{
  nexus_debug=debug;
}


bool RDNexus::debug()
{
  return nexus_debug;
}


//
// Return RDNexusServer settings from NEXUS_SERVER table
//
void RDNexus::nexusServer(RDNexusServer &server)
{
  RDSqlQuery *q;
  QString sql;

  server.clear();

  sql=QString("select ENABLED,ADDRESS,PORT,STATION_NAME from NEXUS_SERVER");
  q=new RDSqlQuery(sql);

  if(q->next()) {
    server.enabled=RDBool(q->value(0).toString());
    server.address=q->value(1).toString();
    server.port=q->value(2).toInt();
    server.host=q->value(3).toString();
  }

  delete q;
}

//
// Set RDNexusServer settings in NEXUS_SERVER table
//
bool RDNexus::setNexusServer(RDNexusServer &server)
{
  QString sql;

  RDSqlQuery::apply("delete from NEXUS_SERVER");

  sql=QString("insert into NEXUS_SERVER set ");
  sql+=QString("ENABLED='%1',").arg(RDYesNo(server.enabled));
  sql+=QString("ADDRESS='%1',").arg(server.address);
  sql+=QString("PORT=%1,").arg(server.port);
  sql+=QString("STATION_NAME='%1'").arg(server.host);
  fprintf(stderr,"%s\n",(const char *)sql);

  return RDSqlQuery::apply(sql);
}

//
// Set Nexus server address and port
//
void RDNexus::setServer(QString server,int port,int timeout)
{
  nexus_server=server;
  nexus_port=port;

  initializeCurl(timeout);
}


//
// Returns the Nexus station
//
QString RDNexus::station()
{
  return nexus_station;
}


//
// Set Nexus station (database)
//
void RDNexus::setStation(QString name)
{
  if(nexus_station!=name) {
    nexus_station=name;

    //
    // Set default category for this station
    //

    //
    // Load this station's field map
    //
    loadFieldMap();
  }
}

//
// Returns the Nexus default category for station
//
QString RDNexus::stationCategory()
{
  for(int i=0;i<nexus_stationmaplist.size();i++) {
    if(nexus_stationmaplist.at(i).station==nexus_station) {
      return nexus_stationmaplist.at(i).category;
    }
  }

  return QString();
}


//
// Loads the Rivendell to Nexus field map table for specified Nexus station
// Key=Rivendell Cart Field Name
// Value=Nexus Song Field Name
//
void RDNexus::loadFieldMap()
{
  RDNexusFieldMap fieldmap;
  RDSqlQuery *q;
  QString sql;

  nexus_fieldmaplist.clear();
  nexus_fieldcartmap.clear();
  nexus_fieldsongmap.clear();

  if(nexus_station.isEmpty()) {
    return;
  }

  sql=QString("select RD_NAME,NEXUS_NAME,NEXUS_ID from NEXUS_FIELDS where STATION='%1'").arg(nexus_station);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    fieldmap.rd_name=q->value(0).toString();
    fieldmap.nexus_name=q->value(1).toString();
    fieldmap.nexus_id=q->value(2).toInt();

    if(!fieldmap.nexus_name.isEmpty()&&fieldmap.nexus_id) {
      nexus_fieldmaplist.append(fieldmap);

      nexus_fieldcartmap.insert(fieldmap.nexus_id,fieldmap.rd_name);
      nexus_fieldsongmap.insert(fieldmap.nexus_id,fieldmap.nexus_name);
    }
  }

  delete q;
}

void RDNexus::getFieldMapList(RDNexusFieldMapList &fieldmaplist)
{
  fieldmaplist=nexus_fieldmaplist;
}

//
// Returns Nexus Field Name for Rivendell Name
//
QString RDNexus::fieldMap(QString rdname)
{
  if(!nexus_fieldmaplist.size()) {
    loadFieldMap();
  }

//  This needs to be updated to the nexus_fieldmapist;
//  return nexus_fieldmaplist.value(rdname);
    return QString();
}

//
// Loads Nexus Station to Rivendell Service map table
// Key=Nexus Station
// Value=Rivendell Service
//
void RDNexus::loadStationMap()
{
  RDSqlQuery *q;
  QString sql;

  nexus_stationmaplist.clear();

  sql=QString("select STATION,CATEGORY,RD_SERVICE,RD_GROUP_NAME,RD_SCHED_CODE,AUTO_MERGE from NEXUS_STATIONS order by STATION");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    RDNexusStationMap station;

    station.station=q->value(0).toString();
    station.category=q->value(1).toString();
    station.service=q->value(2).toString();
    station.group=q->value(3).toString();
    station.schedcode=q->value(4).toString();
    station.automerge=RDBool(q->value(5).toString());

    nexus_stationmaplist.append(station);
  }
  delete q;
}

void RDNexus::saveStationMap(const RDNexusStationMap &station)
{
  QString sql;

  sql=QString("delete from NEXUS_STATIONS where RD_SERVICE='%1'").arg(station.service);
  RDSqlQuery::apply(sql);

  qDebug() << sql;

  sql=QString("insert into NEXUS_STATIONS set ");
  sql+=QString("STATION='%1',").arg(station.station);
  sql+=QString("AUTO_MERGE='%1',").arg(RDYesNo(station.automerge));
  sql+=QString("RD_SERVICE='%1',").arg(station.service);
  sql+=QString("RD_GROUP_NAME='%1',").arg(station.group);
  sql+=QString("RD_SCHED_CODE='%1'").arg(station.schedcode);
  RDSqlQuery::apply(sql);
 
  qDebug() << sql;

  //
  // Refresh station map
  //
  loadStationMap();
}

//
// 
//
void RDNexus::stationMapList(RDNexusStationMapList &list)
{
  list=nexus_stationmaplist;
}

//
// Returns Station Map for Nexus Station Name
//
bool RDNexus::stationMapForStation(const QString &station,RDNexusStationMap &map)
{
  map.clear();

  for(int i=0;i<nexus_stationmaplist.size();i++) {
    if(nexus_stationmaplist.at(i).station==station) {
      map=nexus_stationmaplist.at(i);
      return true;
    }
  }

  return false;
}

#if 0
//
// Returns Station Map for Rivendell Service Name
//
bool RDNexus::stationMapForService(const QString &service,RDNexusStationMap &map)
{
  map.clear();

  for(int i=0;i<nexus_stationmaplist.size();i++) {
    if(nexus_stationmaplist.at(i).service==service) {
      map=nexus_stationmaplist.at(i);
      return true;
    }
  }

  return false;
}


//
// Returns Station Map for Rivendell Group Name
//
bool RDNexus::stationMapForGroup(const QString &group,RDNexusStationMap &map)
{
  map.clear();

  for(int i=0;i<nexus_stationmaplist.size();i++) {
    if(nexus_stationmaplist.at(i).group==group) {
      map=nexus_stationmaplist.at(i);
      return true;
    }
  }

  return false;
}
#endif

//
// Saves the Rivendell to Nexus field map table for nexus_station
//
void RDNexus::saveFieldMap(const RDNexusFieldMapList &fieldmaplist)
{
  saveFieldMap(nexus_station,fieldmaplist);
}

//
// Saves the Rivendell to Nexus field map table for specified station
//
void RDNexus::saveFieldMap(const QString &station,const RDNexusFieldMapList &fieldmaplist)
{
  QString sql;

  sql=QString("delete from NEXUS_FIELDS where STATION='%1'").arg(station);
  RDSqlQuery::apply(sql);

  for (int i=0;i<fieldmaplist.size();i++) {
    sql="insert into NEXUS_FIELDS set ";
    sql+=QString("STATION='%1',").arg(station);
    sql+=QString("RD_NAME='%1',").arg(fieldmaplist.at(i).rd_name);
    sql+=QString("NEXUS_NAME='%1',").arg(fieldmaplist.at(i).nexus_name);
    sql+=QString("NEXUS_ID=%1").arg(fieldmaplist.at(i).nexus_id);
    RDSqlQuery::apply(sql);
  }

  //
  // Load Updated Field Map
  //
  loadFieldMap();
}

QString RDNexus::server() const
{
  return nexus_server;
}

int RDNexus::port() const
{
  return nexus_port;
}

bool RDNexus::isValid()
{
  return resp_valid;
}

QString RDNexus::responseStatus()
{
  return resp_status;
}

QString RDNexus::responseCommand()
{
  return resp_command;
}

QString RDNexus::responseError()
{
  return resp_error;
}

int RDNexus::responseMessageId()
{
  return resp_messageid;
}

void RDNexus::prepareRequest(QDomDocument &doc,QString command)
{
  QDomProcessingInstruction xmlProcessingInstruction = doc.createProcessingInstruction("xml", "version=\"1.0\"");
  doc.appendChild(xmlProcessingInstruction);
  QDomElement root = doc.createElement("mmRequest");
  root.setAttribute("command",command);
  doc.appendChild(root);

  nexus_response="";
}

void RDNexus::addRequestAttribute(QDomDocument &doc,QString attribute,QString value)
{
  QDomElement root = doc.documentElement();
  root.setAttribute(attribute,value);
}

bool RDNexus::nexusRequest(QDomDocument &doc)
{
  CURLcode code;
  QString xml = doc.toString();
  int tries=1;

  resp_status="error";
  resp_command="";
  resp_messageid=0;
  resp_valid=false;

  if(nexus_server.isEmpty()) {
    resp_error="Nexus server address has not been specified";
    return false;
  }

  if(nexus_curl==NULL) {
    resp_error="CURL has not been initialized";
    return false;
  }

  if(nexus_debug) {
    qDebug() << xml;
  }

  curl_easy_setopt(nexus_curl,CURLOPT_POSTFIELDS,(const char *)xml);

  do {
    code=curl_easy_perform(nexus_curl);
  } while(code!=CURLE_OK&&tries--);

  if(code==CURLE_OK) { 
    return(nexusResponse());
  }

  resp_status="error";
  resp_command="";
  resp_messageid=0;
  resp_valid=false;
  resp_error=curl_easy_strerror(code);

  syslog(LOG_ERR,QString("Nexus server failure %1: %2").arg(code).arg(resp_error));

  return resp_valid;
}

bool RDNexus::nexusResponse()
{
  QString err;
  QByteArray resp=nexus_response.toUtf8();

  resp_doc.setContent(resp,true,&err);
  if(nexus_debug) {
    qDebug() << resp;
  }

  resp_element=resp_doc.documentElement();

  if(!resp_element.isNull()) {
    resp_status=resp_element.attribute("status").toLower();
    resp_command=resp_element.attribute("command").toLower();
    resp_station=resp_element.attribute("station");
    resp_messageid=resp_element.attribute("messageId").toInt();
    resp_valid=(resp_status=="ok");
  }
  else {
    resp_status="error";
    resp_command="";
    resp_station="";
    resp_messageid=0;
    resp_valid=false;
    resp_error="Nexus returned invalid response";
  }

  if(resp_valid) {
    resp_error="";
  }
  else {
    QDomElement child=resp_element.firstChildElement("errors");

    while(!child.isNull()) {
      if(child.tagName().toLower()=="error") {
	resp_error=child.text();
      }
      child=child.firstChildElement("error");
    }
  }

  return resp_valid;
}

//
//
//
bool RDNexus::ping()
{
  QDomDocument doc;

  prepareRequest(doc,"ping");

  nexusRequest(doc);

  emit ping(resp_valid);

  return resp_valid;
}

//
//
//
bool RDNexus::getServerInfo(RDNexusServerInfo &info) {
  QDomDocument doc;

  info.clear();

  prepareRequest(doc,"getServerInfo");

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetServerInfo(info);

  return true;
}

//
//
//
QString RDNexus::getAPIVersion()
{
  QDomDocument doc;

  prepareRequest(doc,"getAPIVersion");

  if(!nexusRequest(doc)) {
    return QString();
  }

  return processGetAPIVersion();
}

bool RDNexus::getStations(RDNexusStationList &list)
{
  QDomDocument doc;

  list.clear();

  prepareRequest(doc,"getStations");

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetStations(list);

  return true;
}

//
//
//
bool RDNexus::getStationInfo(RDNexusStationInfo &info)
{
  QDomDocument doc;

  info.clear();

  if(nexus_station.isEmpty()) {
    return false;
  }

  prepareRequest(doc,"getStationInfo");
  addRequestAttribute(doc,"station",nexus_station);

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetStationInfo(info);

  return true;
}

//
// Get list of library categories
//
bool RDNexus::getCategoryList(RDNexusCategoryList &list)
{
  QDomDocument doc;

  if(nexus_station.isEmpty()) {
    return false;
  }

  prepareRequest(doc,"getCategoryList");
  addRequestAttribute(doc,"station",nexus_station);

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetCategoryList(list);

  return true;
}


bool RDNexus::getSongsByQuery(RDNexusSongInfoList &list)
{
  QDomDocument doc;

  list.clear();

  if(nexus_station.isEmpty()) {
    return false;
  }

  prepareRequest(doc,"getSongsByQuery");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement fields = doc.createElement("fields");
  contents.appendChild(fields);

  for (int i=0;i<nexus_fieldmaplist.size();i++) {
    QDomElement field = doc.createElement("field");
    fields.appendChild(field);
    field.setAttribute("name",nexus_fieldmaplist.at(i).nexus_name);
  }

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetSongsByQuery(list);

  return true;
}

bool RDNexus::getSongInfo(int id,RDNexus::IdType type,RDNexusSongInfo &info)
{
  RDNexusIdList idlist;
  RDNexusSongInfoList infolist;
  int r;

  info.clear();

  idlist.append(id);
  infolist.append(info);

  r=getSongInfo(idlist,type,infolist);

  if(infolist.size()) {
    info=infolist[0];
  }

  return r;
}


bool RDNexus::getSongInfo(RDNexusIdList &idlist,RDNexus::IdType type,RDNexusSongInfoList &infolist)
{
  QDomDocument doc;

  infolist.clear();

  prepareRequest(doc,"getSongInfo");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement songlist = doc.createElement("songList");
  contents.appendChild(songlist);

  QDomElement fields = doc.createElement("fields");
  contents.appendChild(fields);

  for(int i=0;i<idlist.size();i++) {
    QDomElement song = doc.createElement("song");
    songlist.appendChild(song);
    switch(type) {
      case RDNexus::CutId:
        song.setAttribute("cutId",idlist[i]);
        break;

      case RDNexus::SongId:
      default:
        song.setAttribute("songId",idlist[i]);
        break;
    }
  }

  for (int i=0;i<nexus_fieldmaplist.size();i++) {
    QDomElement field = doc.createElement("field");
    fields.appendChild(field);
    field.setAttribute("name",nexus_fieldmaplist.at(i).nexus_name);
  }

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetSongInfo(infolist);

  return true;
}

bool RDNexus::importSong(RDNexusSongInfo &info)
{
  RDNexusSongInfoList infolist;

  infolist.append(info);

  return importSongs(infolist);
}

//
// Returns MusicMaster Song ID of the newly imported song
//
bool RDNexus::importSongs(RDNexusSongInfoList &infolist)
{
  RDNexusIdList idlist;
  QDomDocument doc;

  prepareRequest(doc,"importSongs");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root=doc.documentElement();
  QDomElement contents=doc.createElement("contents");
  root.appendChild(contents);

  QDomElement songlist=doc.createElement("songList");
  contents.appendChild(songlist);

  QDomElement fields=doc.createElement("fields");
  contents.appendChild(fields);

  for(int i=0;i<infolist.size();i++) {
    QDomElement song;
    QDomElement field;
    QDomText value;

    song=doc.createElement("song");
    songlist.appendChild(song);

    addSongTag(doc,song,"NUMBER",QString::number(infolist.at(i).cutid));
    addSongTag(doc,song,"ARTIST",infolist.at(i).artist);
    addSongTag(doc,song,"TITLE",infolist.at(i).title);
    addSongTag(doc,song,"FORCED_LENGTH",infolist.at(i).runtime);
    addSongTag(doc,song,"YEAR",infolist.at(i).year);
    addSongTag(doc,song,"NOTES",infolist.at(i).notes);
    addSongTag(doc,song,"GROUP_NAME",infolist.at(i).group);
    addSongTag(doc,song,"CATEGORY",infolist.at(i).category);
    addSongTag(doc,song,"MINIMUM_TALK_LENGTH",infolist.at(i).intro);
    addSongTag(doc,song,"AVERAGE_SEGUE_LENGTH",infolist.at(i).ending);

    //
    // When importing new songs, include Artist and Title Keywords Fields
    //
    addSongTag(doc,song,"ARTIST_KEYWORDS",infolist.at(i).artist);
    addSongTag(doc,song,"TITLE_KEYWORDS",infolist.at(i).title);
  }

  if(!nexusRequest(doc)) {
    return false;
  }

  processImportSongs(idlist);

  return true;
}

//
// Adds <field id="x">value</field> element to <song> element
//
void RDNexus::addSongTag(QDomDocument &d,QDomElement &e,QString rdname,QString value)
{
  QDomElement f;
  QDomText v;

  //
  // Check if we have a valid Nexus field mapping
  //
  if(!nexus_fieldcartmap.key(rdname)) {
    qDebug() << QString("Station %1 does not contain a valid field map for Rivendell %2").arg(nexus_station).arg(rdname);
    return;
  }

  // Cart Number
  f = d.createElement("field");
  e.appendChild(f);
  f.setAttribute("id",nexus_fieldcartmap.key(rdname));
  v = d.createTextNode(value);
  f.appendChild(v);
}


bool RDNexus::updateSong(RDNexusSongInfo &info)
{
  RDNexusSongInfoList infolist;

  infolist.append(info);

  return updateSongs(infolist);
}

bool RDNexus::updateSongs(RDNexusSongInfoList &infolist)
{
  QDomDocument doc;

  prepareRequest(doc,"updateSongs");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement songlist = doc.createElement("songList");
  contents.appendChild(songlist);

  for(int i=0;i<infolist.size();i++) {
    QDomElement song;
    QDomElement field;
    QDomText value;

    song=doc.createElement("song");
    songlist.appendChild(song);
    song.setAttribute("cutId",infolist.at(i).cutid);

    addSongTag(doc,song,"NUMBER",QString::number(infolist.at(i).cutid));
    addSongTag(doc,song,"ARTIST",infolist.at(i).artist);
    addSongTag(doc,song,"TITLE",infolist.at(i).title);
    addSongTag(doc,song,"FORCED_LENGTH",infolist.at(i).runtime);
    addSongTag(doc,song,"YEAR",infolist.at(i).year);
    addSongTag(doc,song,"NOTES",infolist.at(i).notes);
    addSongTag(doc,song,"GROUP_NAME",infolist.at(i).group);
    addSongTag(doc,song,"CATEGORY",infolist.at(i).category);
    addSongTag(doc,song,"MAXIMUM_TALK_LENGTH",infolist.at(i).intro);
    addSongTag(doc,song,"AVERAGE_SEGUE_LENGTH",infolist.at(i).ending);
  }

  return nexusRequest(doc);
}

bool RDNexus::deleteSong(int songid,RDNexus::IdType type)
{
  RDNexusIdList idlist;

  idlist.append(songid);

  return deleteSongs(idlist,type);
}

bool RDNexus::deleteSong(const RDNexusSongInfo &info)
{
  RDNexusIdList idlist;

  idlist.append(info.songid);

  return deleteSongs(idlist,SongId);
}

bool RDNexus::deleteSongs(RDNexusIdList &idlist,RDNexus::IdType type)
{
  QDomDocument doc;

  prepareRequest(doc,"deleteSongs");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement songlist = doc.createElement("songList");
  contents.appendChild(songlist);

  for(int i=0;i<idlist.size();i++) {
    QDomElement song = doc.createElement("song");
    songlist.appendChild(song);
    switch(type) {
      case RDNexus::CutId:
        song.setAttribute("cutId",idlist[i]);
        break;

      case RDNexus::SongId:
      default:
        song.setAttribute("songId",idlist[i]);
        break;
    }
  }

  return nexusRequest(doc);
}

int RDNexus::publishMetadata(RDNexusIdList &list)
{
  QDomDocument doc;
  int count=0;

  list.clear();

  prepareRequest(doc,"publishMetadata");
  addRequestAttribute(doc,"station",nexus_station);

  if (nexusRequest(doc)) {
    count=processPublishMetadata(list);
  }

  return count;
}

//
//
//
bool RDNexus::modifySchedule(RDNexusModifyScheduleList &list)
{
  QDomDocument doc;

  prepareRequest(doc,"modifySchedule");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement songlist = doc.createElement("changes");
  contents.appendChild(songlist);

  nexusRequest(doc);

  return resp_valid;
}


bool RDNexus::reconcileElement(unsigned historyid,QString &mode,QString &airtime)
{
  QDomDocument doc;

  prepareRequest(doc,"reconcileElements");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  QDomElement contents = doc.createElement("contents");
  root.appendChild(contents);

  QDomElement elements = doc.createElement("elements");
  contents.appendChild(elements);

  QDomElement element=doc.createElement("element");
  elements.appendChild(element);
  element.setAttribute("historyId",historyid);
  element.setAttribute("mode",mode);
  element.setAttribute("airTime",airtime);

  return nexusRequest(doc);
}

//
//
//
bool RDNexus::getSchedule(QDateTime start,QDateTime end,RDNexusElementList &list)
{
  QDomDocument doc;
  QDomElement c;
  QDomElement e;
  QDomText t;

  prepareRequest(doc,"getSchedule");
  addRequestAttribute(doc,"station",nexus_station);

  QDomElement root = doc.documentElement();
  c = doc.createElement("contents");
  root.appendChild(c);

  e = doc.createElement("startTime");
  c.appendChild(e);
  t = doc.createTextNode(start.toString(RDNEXUS_DATETIME_HOUR));
  e.appendChild(t);

  e = doc.createElement("endTime");
  c.appendChild(e);
  t = doc.createTextNode(end.toString(RDNEXUS_DATETIME_HOUR));
  e.appendChild(t);

  if(!nexusRequest(doc)) {
    return false;
  }

  processGetSchedule(list);

  return true;
}

//
// Returns a map for MusicMaster fields
// <ID,Field Name>
//
bool RDNexus::getFieldList(RDNexusFieldList &list)
{
  QDomDocument doc;

  list.clear();

  if(nexus_station.isEmpty()) {
    return false;
  }

  prepareRequest(doc,"getFieldList");
  addRequestAttribute(doc,"station",nexus_station);

  if (!nexusRequest(doc)) {
    qDebug() << "nexusRequest Failed";
    return false;
  }

  processGetFieldList(list);

  return true;
}

void RDNexus::processGetServerInfo(RDNexusServerInfo &info)
{
  QDomNodeList nodes;

  nodes=resp_doc.elementsByTagName("contents");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement();
    while(!e.isNull()) {
      if(e.tagName().toLower()=="version") {
        info.version=e.text();
      }
      else if(e.tagName().toLower()=="time") {
        info.time=QDateTime::fromString(e.text(),"yyyy-MM-dd HH:mm:ss");
      }
      else if(e.tagName().toLower()=="timezone") {
        info.timeZone=e.text();
      }
      else if(e.tagName().toLower()=="timeoffset") {
        info.timeOffset=e.text();
      }
      e=e.nextSiblingElement();
    }
  }
}

void RDNexus::processGetStationInfo(RDNexusStationInfo &info)
{
  QDomNodeList nodes;

  nodes=resp_doc.elementsByTagName("contents");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement();
    while(!e.isNull()) {
      if(e.tagName().toLower()=="logo") {
        info.logo=e.text();
      }
      else if(e.tagName().toLower()=="guid") {
        info.guid=e.text();
      }
      else if(e.tagName().toLower()=="lastscheduled") {
        info.lastScheduled=QDateTime::fromString(e.text(),"yyyy-MM-ddTHH:mm:ss");
      }
      e=e.nextSiblingElement();
    }
  }
}

QString RDNexus::processGetAPIVersion()
{
  QDomNodeList nodes;

  nodes=resp_doc.elementsByTagName("contents");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement();
    while(!e.isNull()) {
      if(e.tagName().toLower()=="version") {
        return e.text();
      }
      e=e.nextSiblingElement();
    }
  }

  return QString();
}

void RDNexus::processGetStations(RDNexusStationList &list)
{
  RDNexusStation station;
  QDomNodeList nodes;

  nodes=resp_doc.elementsByTagName("contents");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement();

    while(!e.isNull()) {
      if(e.tagName()=="stations") {
        QDomElement s=e.firstChildElement();
        while(!s.isNull()) {
          QDomElement value=s.firstChildElement();
          while(!value.isNull()) {
            if(value.tagName().toLower()=="id") {
              station.id=value.text();
            }
            else if(value.tagName().toLower()=="description") {
              station.description=value.text();
            }
            else if(value.tagName().toLower()=="interface") {
              station.interface=value.text();
            }
            else if(value.tagName().toLower()=="status") {
              station.status=value.text().toInt();
            }
            value=value.nextSiblingElement();
          }
          list.append(station);
          station.clear();
          s=s.nextSiblingElement();
        }
      }
      e=e.firstChildElement();
    }
  }
}


void RDNexus::processGetCategoryList(QList<RDNexusCategory> &list)
{
  QDomNodeList nodes;
  RDNexusCategory category;

  nodes=resp_doc.elementsByTagName("categories");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement("category");
    while(!e.isNull()) {
      category.id=e.attribute("id","");
      category.code=e.attribute("code","");
      category.name=e.attribute("name","");
      category.nonmx=(e.attribute("nonmx","")=="1");
      category.backcolor=e.attribute("backcolor","");
      category.forecolor=e.attribute("forecolor","");
      list.append(category);
      category.clear();
      e=e.nextSiblingElement();
    }
  }
}

int RDNexus::processPublishMetadata(RDNexusIdList &list)
{
  int count=0;

  QDomElement child=resp_element.firstChildElement();
  while(!child.isNull()) {
    if(child.tagName()=="songList") {
      count=child.attribute("recordCount","0").toInt();
      break;
    }
    child=child.firstChildElement();
  }

  if (count) {
    QDomElement sibling=child.firstChildElement("song");
    while(!sibling.isNull()) {
      list.append(sibling.attribute("songId","0").toInt());
      sibling=sibling.nextSiblingElement();
    }
  }

  return count;
}

void RDNexus::processGetSchedule(RDNexusElementList &list)
{
  QDomNodeList nodes;
  QDomNode node;
  QDomElement e;

  list.clear();

  nodes=resp_doc.elementsByTagName("schedule");

  if (nodes.length()) {
    e=nodes.at(0).toElement();
    QDomElement s=e.firstChildElement("element");
    while(!s.isNull()) {
      RDNexusElement element;

      element.station=resp_station;
      element.songid=s.attribute("songId").toInt();
      element.cutid=s.attribute("cutId").toInt();
      element.historyid=s.attribute("historyId").toInt();
      element.airdate=s.attribute("airDate");
      element.airtime=s.attribute("airTime");
      element.runtime=s.attribute("runTime");
      element.classcode=s.attribute("class");
      element.type=s.attribute("type");
      element.description=s.text();

      list.append(element);

      s=s.nextSiblingElement();
    }
  }

  //
  // Update list with cutid from <song> elements
  //
  if (list.size()) {
    nodes=resp_doc.elementsByTagName("song");

    if (nodes.length()) {
      QDomElement s=nodes.at(0).toElement();
    
      while(!s.isNull()) {
        int songid=s.attribute("songId").toInt();
        int cutid=s.attribute("cutId").toInt();

        for(int i=0;i<list.size();i++) {
          if(list.at(i).songid==songid) {
            RDNexusElement element=list.at(i);
            element.cutid=cutid;
            list.replace(i,element);
          }
        }
        s=s.nextSiblingElement();
      }
    }
  }
}

void RDNexus::processContents(QDomElement *element,RDNexusMap *map)
{
  QDomElement child=element->firstChildElement("contents");
  child=child.firstChildElement();
  while(!child.isNull()) {
    map->insert(child.tagName().toLower(),child.text());
    child=child.nextSiblingElement();
  }
}

void RDNexus::processContentsList(QDomElement *element,QString tagName,RDNexusMap *map,RDNexusMap *list)
{
  int i=0;
  QDomElement child=element->firstChildElement();
  while(!child.isNull()) {
    if(child.tagName()==tagName) {
      child=child.firstChildElement();
      while(!child.isNull()) {
        QDomElement value=child.firstChildElement();
        while(!value.isNull()) {
          list->insert(QString().sprintf("%s%d",(const char *)value.tagName().toLower(),i),value.text());
          value=value.nextSiblingElement();
        }
        i++;
        map->insert(child.tagName().toLower(),QString::number(i));
        child=child.nextSiblingElement();
      }
    }
    child=child.firstChildElement();
  }
}

void RDNexus::processGetFieldList(RDNexusFieldList &list)
{
  QDomNodeList nodes;
  QDomNode node;
  RDNexusField field;

  nodes=resp_doc.elementsByTagName("fields");

  if (nodes.length()) {
    QDomElement e=nodes.at(0).toElement().firstChildElement("field");
    while(!e.isNull()) {
      field.id=(e.attribute("id","0").toInt());
      field.type=(e.attribute("type","0").toInt());
      field.len=(e.attribute("len","0").toInt());
      field.name=(e.attribute("name",""));
      field.abbr=(e.attribute("abbr",""));
      field.locked=(e.attribute("locked","")=="1");
      field.priority=(e.attribute("priority","0").toInt());
      list.append(field);
      field.clear();
      e=e.nextSiblingElement();
    }
  }
}

//
// Process Nexus "importSongs" command response.
//
// Fills idlist with Nexus SongIds for the imported songs
//
void RDNexus::processImportSongs(RDNexusIdList &idlist)
{
  QDomNodeList nodes;
  QDomNode node;
  QDomElement e;

  idlist.clear();

  nodes=resp_doc.elementsByTagName("song");

  if(nodes.length()) {
    QDomElement song=nodes.at(0).toElement();
    while(!song.isNull()) {
        idlist.append(song.attribute("songId","").toInt());
        song=song.nextSiblingElement();
    }
  }
}


void RDNexus::processGetSongsByQuery(RDNexusSongInfoList &infolist)
{
  processGetSongInfo(infolist);
}


void RDNexus::processGetSongInfo(RDNexusSongInfoList &infolist)
{
  QDomNodeList nodes;
  int count=0;

  nodes=resp_doc.elementsByTagName("songList");

  if(nodes.length()) {
    QDomElement songlist=nodes.at(0).toElement();
    if(!songlist.isNull()) {
      count=songlist.attribute("recordCount","").toInt();
    }
  }

  if(count) {
    nodes=resp_doc.elementsByTagName("song");
    for(int i=0;i<nodes.size();i++) {
      RDNexusSongInfo info;
      info.clear();
      QDomElement song=nodes.at(i).toElement();
      info.songid=song.attribute("songId","").toInt();
      info.cutid=song.attribute("cutId","").toInt();
      QDomElement field=song.firstChildElement("field");
      while(!field.isNull()) {
#if 0
        if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="NUMBER") {
          info.cutid=field.text().toUInt();
        }
#endif
        if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="GROUP_NAME") {
          info.group=field.text().toUInt();
        }
        if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="ARTIST") {
          info.artist=field.text();
        }
        else if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="TITLE") {
          info.title=field.text();
        }
        else if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="YEAR") {
          info.year=field.text();
        }
        else if(nexus_fieldcartmap.value(field.attribute("id").toInt())=="NOTES") {
          info.notes=field.text();
        }
        field=field.nextSiblingElement();
      }
      infolist.append(info);
    }
  }
}

void RDNexus::startHttpServer(int port)
{
  nexus_http_server=new RDHttpServer();

  nexus_http_server->listen(QHostAddress::Any,port);

  connect(nexus_http_server,SIGNAL(requestReceived(RDHttpRequest)),this,SLOT(requestReceivedSlot(RDHttpRequest)));
}


RDHttpServer * RDNexus::httpServer()
{
  return nexus_http_server;
}

//
// Receives XML requests from Nexus
//
void RDNexus::requestReceivedSlot(RDHttpRequest request)
{
  RDHttpResponse response;

  fputs("**requestReceivedSlot**\n",stderr);

  nexus_http_server->sendResponse(response);

  if(nexus_debug) {
    qDebug() << request.headers;
    qDebug() << request.body;
  }

  nexus_response=QString::fromAscii(request.body);

  if(nexusResponse()) {
    if(resp_command=="publishmetadata") {
      fputs("**publishmetadata**\n",stderr);
      RDNexusIdList list;
      processPublishMetadata(list);
      if(list.size()) {
        emit publishMetaData(resp_station,list);
      }
    }
    if(resp_command=="publishschedule") {
      fputs("**publishschedule**\n",stderr);
      RDNexusElementList list;
      processGetSchedule(list);
      if(list.size()) {
        emit publishSchedule(resp_station,list);
      }
    }
  }
}


void RDNexus::field(QString &rd_name)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("select NEXUS_NAME from NEXUS_FIELDS where (RD_NAME='%1'&&STATION='%2')")
    .arg(rd_name).arg(nexus_station);
  q=new RDSqlQuery(sql);
  delete q;
}

int RDNexus::indexOfHistoryId(int historyid,RDNexusElementList &list)
{
  for(int i=0;i<list.size();i++) {
    if(list.at(i).historyid==historyid) {
     return i;
    }
  }

  return -1;
}


//
// Given a Rivendell GROUP returns a list of Rivendell Services
//
bool RDNexus::groupServices(const QString &group,QStringList &list)
{
  RDGroupList *grouplist;

  list.clear();

  grouplist=new RDGroupList();
  grouplist->loadGroup(group);

  for(int i=0;i<grouplist->size();i++) {
    list.append(grouplist->service(i));
  }

  return(list.size()>0);
}

//
// Given a Rivendell GROUP returns a list of RDNexusStationMapList
//
bool RDNexus::groupStations(const QString &group,RDNexusStationMapList &list)
{
  QStringList servicelist;

  list.clear();

  if(!groupServices(group,servicelist)) {
    return false;
  }

  for(int s=0;s<servicelist.size();s++) {
    for(int i=0;i<nexus_stationmaplist.size();i++) {
      if(servicelist.at(s).upper()==nexus_stationmaplist.at(i).service.upper()) {
        list.append(nexus_stationmaplist.at(i));
      }
    }
  }

  return(list.size()>0);
}


bool RDNexus::exists(int id,RDNexus::IdType type)
{
  RDNexusSongInfo info;

  getSongInfo(id,type,info);

  qDebug() << "*****info.songid=" << info.songid;

  return(info.songid>0);
}


bool RDNexus::exists(int id,RDNexus::IdType type,const RDNexusSongInfoList &list)
{
  for(int i=0;i<list.size();i++) {
    if(type==RDNexus::SongId&&list.at(i).songid==id) {
      return true;
    }
    if(type==RDNexus::CutId&&list.at(i).cutid==id) {
      return true;
    }
  }

  return false;
}

