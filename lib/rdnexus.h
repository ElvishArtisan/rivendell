// rdnexus.h
//
// A container class for MusicMaster Nexus API
//
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

#ifndef RDNEXUS_H
#define RDNEXUS_H

#include <QNetworkAccessManager>
#include <QDomElement>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QList>

#include "rdhttpserver.h"

#include <curl/curl.h>

typedef QMap<QString,QString> RDNexusMap;
typedef QMapIterator<QString,QString> RDNexusMapIterator;
#if 0
typedef QList<QString> RDNexusList;
#endif
typedef QList<int> RDNexusIdList;
typedef QMap<int,int> RDNexusCartMap;
typedef QMultiMap<QString,QString> RDNexusScheduleMap;
typedef QList<RDNexusMap> RDNexusListMap;

typedef QMap<QString,QString> RDNexusServiceMap;
typedef QMap<QString,QString> RDNexusGroupMap;

#define RDNEXUS_DATE "yyyy-MM-dd"
#define RDNEXUS_DATETIME "yyyy-MM-ddThh:mm:ss"
#define RDNEXUS_DATETIME_HOUR "yyyy-MM-ddThh:00:00"
#define RDNEXUS_DATETIME_FORMAT Qt::ISODate
#define RDNEXUS_DEFAULT_PORT 8080
#define RDNEXUS_DEFAULT_TIMEOUT 30

class RDNexusServer
{
  public:
    bool enabled;         // Nexus Integration Enabled
    QString address;      // Nexus Server IP Address
    int port;             // Nexus Port
    QString host;         // Rivendell Host

    void clear() {
      enabled=false;
      address="";
      port=0;
      host="";
    }
};

class RDNexusStationMap
{
  public:
    QString station;      // Nexus Station
    QString category;     // Nexus Default Category
    QString service;      // Rivendell Service
    QString group;        // Rivendell Group
    QString schedcode;    // Rivendell Scheduler Code
    bool automerge;       // Enable Auto Merge

    void clear() {
      station="";
      category="";
      service="";
      group="";
      schedcode="";
      automerge=false;
    }
};

typedef QList<RDNexusStationMap> RDNexusStationMapList;

class RDNexusServerInfo
{
  public:
    QString version;
    QDateTime time;
    QString timeZone;
    QString timeOffset;

    void clear() {
      version="";
      time=QDateTime();
      timeZone="";
      timeOffset="";
    }
};

class RDNexusStation
{
  public:
    QString id;
    QString description;
    QString interface;
    int status;

    void clear() {
      id="";
      description="";
      interface="";
      status=0;
    }
};

typedef QList<RDNexusStation> RDNexusStationList;

class RDNexusStationInfo
{
  public:
    QString logo;
    QString guid;
    QDateTime lastScheduled;

    void clear() {
      logo="";
      guid="";
      lastScheduled=QDateTime();
    }
};

class RDNexusCategory
{
  public:
    QString id;
    QString code;
    QString name;
    bool nonmx;
    QString backcolor;
    QString forecolor;

    void clear() {
      id="";
      code="";
      name="";
      nonmx=false;
      backcolor="";
      forecolor="";
    }
};

typedef QList<RDNexusCategory> RDNexusCategoryList;
 
class RDNexusField
{
  public:
    int id;
    int type;
    int len;
    QString name;
    QString abbr;
    bool locked;
    int priority;

    void clear() {
      id=0;
      type=0;
      len=0;
      name="";
      abbr="";
      locked=false;
      priority=0;
    }
};

typedef QList<RDNexusField> RDNexusFieldList;

class RDNexusFieldMap
{
  public:
    QString rd_name;
    QString nexus_name;
    int nexus_id;
    QMap<int,QString> nexus_map;
    QMap<int,QString> rd_map;

    void clear() {
      rd_name="";
      nexus_name="";
      nexus_id=0;
      nexus_map.clear();
      rd_map.clear();
    }
};

typedef QList<RDNexusFieldMap> RDNexusFieldMapList;
typedef QMap<int,QString> RDNexusFieldCartMap;
typedef QMap<int,QString> RDNexusFieldSongMap;

class RDNexusSongInfo
{
  public:
    int songid;
    int cutid;
    QString artist;
    QString title;
    QString notes;
    QString year;
    QString runtime;
    QString group;
    QString category;
    QString intro;
    QString ending;

    void clear() {
      songid=0;
      cutid=0;
      artist="";
      title="";
      notes="";
      year="";
      runtime="00:00";
      ending="00:00";
      group="";
      category="";
      intro="";
    }
};

typedef QList<RDNexusSongInfo> RDNexusSongInfoList;

class RDNexusElement
{
  public:
    QString station;
    int songid;
    int cutid;
    int historyid;
    QString airdate;
    QString airtime;
    QString runtime;
    QString classcode;
    QString type;
    bool sweep;
    bool fill;
    QString transition;
    QString description;
  
    void clear() {
      station="";
      songid=0;
      cutid=0;
      historyid=0;
      airdate="";
      airtime="";
      runtime="";
      classcode="";
      type="";
      sweep=false;
      fill=false;
      transition="";
      description="";
    }
};

typedef QList<RDNexusElement> RDNexusElementList;

class RDNexusModifySchedule
{
  public:
    int historyid;
    int cutid;
    int mode;
    int pos;
    QDateTime airtime;

    void clear() {
      historyid=0;
      cutid=0;
      mode=0;
      pos=0;
      QDateTime airtime;
    }
};

typedef QList<RDNexusModifySchedule> RDNexusModifyScheduleList;

class RDNexus : public QObject
{
  Q_OBJECT

  public:
    enum IdType {SongId=1,CutId};
    enum QueueType {Add=1,Modify,Delete};
    RDNexus();
    ~RDNexus();
    bool debug();
    void setDebug(bool debug);
    void nexusServer(RDNexusServer &server);
    bool setNexusServer(RDNexusServer &server);
    QString server() const;
    void setServer(QString server,int port=RDNEXUS_DEFAULT_PORT,int timeout=RDNEXUS_DEFAULT_TIMEOUT);
    QString station();
    void setStation(QString name);
    QString stationCategory();
    int port() const;
    int stationPollSecs() const;
    int fieldPollSecs() const;
    bool isValid();
    void field(QString &rd_name);
    QString responseCommand();
    QString responseStatus();
    QString responseError();
    int responseMessageId();
    bool ping();
    QString getAPIVersion();
    bool getServerInfo(RDNexusServerInfo &info);
    bool getStations(RDNexusStationList &list);
    bool getStationInfo(RDNexusStationInfo &info);
    bool getCategoryList(RDNexusCategoryList &list);
    int publishMetadata(RDNexusIdList &list);
    bool publishSchedule(QDateTime start,QDateTime end);
    bool getSchedule(QDateTime start,QDateTime end,RDNexusElementList &list);
    bool getFieldList(RDNexusFieldList &list);
    void getFieldMapList(RDNexusFieldMapList &fieldmaplist);
    QString fieldMap(QString rdname);
    void stationMapList(RDNexusStationMapList &list);
    bool stationMapForStation(const QString &station,RDNexusStationMap &map);
    bool stationMapForService(const QString &service,RDNexusStationMap &map);
    bool stationMapForGroup(const QString &group,RDNexusStationMap &map);
    bool importSong(RDNexusSongInfo &song);
    bool importSongs(RDNexusSongInfoList &songlist);
    bool songExists(int cutid,int songid) const;
    void songReplace(int cutid,int songid);
    bool getSongsByQuery(RDNexusSongInfoList &list);
    bool getSongInfo(int id,IdType type,RDNexusSongInfo &info);
    bool getSongInfo(RDNexusIdList &ids,IdType type,RDNexusSongInfoList &infolist);
    bool updateSong(RDNexusSongInfo &info);
    bool updateSongs(RDNexusSongInfoList &infolist);
    bool deleteSong(int songid,RDNexus::IdType type);
    bool deleteSong(const RDNexusSongInfo &info);
    bool deleteSongs(RDNexusIdList &idlist,RDNexus::IdType type);
    bool modifySchedule(RDNexusModifyScheduleList &list);
    bool reconcileElement(unsigned historyid,QString &mode,QString &airtime);
    void startHttpServer(int port=RDNEXUS_DEFAULT_PORT);
    void saveFieldMap(const RDNexusFieldMapList &fieldmaplist);
    void saveFieldMap(const QString &station,const RDNexusFieldMapList &fieldmaplist);
    RDHttpServer *httpServer();
    int indexOfHistoryId(int historyid,RDNexusElementList &list);
    bool exists(int id,RDNexus::IdType type);
    bool exists(int id,RDNexus::IdType type,const RDNexusSongInfoList &list);
    int exists(QString artist,QString title,const RDNexusSongInfoList &list);
    bool groupServices(const QString &group,QStringList &list);
    bool groupStations(const QString &group,RDNexusStationMapList &list);
    void saveStationMap(const RDNexusStationMap &station);

  signals:
    void publishMetaData(QString &station,RDNexusIdList &list);
    void publishSchedule(QString &station,RDNexusElementList &list);
    void ping(bool resp);

  private slots:
    void requestReceivedSlot(RDHttpRequest request);

  private:
    bool nexus_debug;
    QString nexus_server;
    int nexus_port;
    QString rd_service;
    QString nexus_station;
    int nexus_metadata_poll_secs;
    QList<RDNexusFieldMap> nexus_fieldmaplist;
    RDNexusFieldCartMap nexus_fieldcartmap;
    RDNexusFieldSongMap nexus_fieldsongmap;
    RDNexusStationMapList nexus_stationmaplist;
    RDHttpServer *nexus_http_server;
    CURL *nexus_curl;
    struct curl_slist *slist=NULL;
    QDomDocument resp_doc;
    QDomElement resp_element;
    QString nexus_response;
    bool resp_valid;
    QString resp_status;
    QString resp_command;
    QString resp_error;
    int resp_messageid;
    QString resp_station;
    void initializeCurl(int timeout=RDNEXUS_DEFAULT_TIMEOUT);
    void loadFieldMap();
    void loadStationMap();
    void prepareRequest(QDomDocument &doc,QString command);
    void addRequestAttribute(QDomDocument &doc,QString attribute,QString value);
    bool nexusRequest(QDomDocument &doc);
    bool nexusResponse();
    QString processGetAPIVersion();
    void processGetServerInfo(RDNexusServerInfo &info);
    void processGetStationInfo(RDNexusStationInfo &info);
    void processGetStations(RDNexusStationList &list);
    void processGetCategoryList(QList<RDNexusCategory> &list);
    int processPublishMetadata(RDNexusIdList &list);
    void processGetSchedule(RDNexusElementList &list);
    void processContents(QDomElement *element,RDNexusMap *map=NULL);
    void processContentsList(QDomElement *element,QString tagName,RDNexusMap *map=NULL,RDNexusMap *list=NULL);
    void processGetFieldList(QList<RDNexusField> &list);
    void processImportSongs(RDNexusIdList &idlist);
    void processGetSongsByQuery(RDNexusSongInfoList &infolist);
    void processGetSongInfo(RDNexusSongInfoList &infolist);
    void addSongTag(QDomDocument &d,QDomElement &e,QString rdname,QString value);
};


#endif  // RDNEXUS_H
