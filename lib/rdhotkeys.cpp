// rdhotkeys.cpp
//
// Abstract an RDHotKeys Configuration.
//
//   (C) Copyright 2002-2004,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdhotkeys.cpp,v 1.2.6.1 2012/12/13 22:33:44 cvs Exp $
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
#include <rdhotkeys.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDHotkeys::RDHotkeys(const QString &station,const QString &module)
{
  QString sql;
  RDSqlQuery *q;
  
  station_hotkeys=station;
  module_name=module;

  sql=QString().sprintf(
      "select STATION_NAME from RDHOTKEYS \
       where STATION_NAME=\"%s\" AND \
       MODULE_NAME=\"%s\"",
			(const char *)RDEscapeString(station_hotkeys),
			(const char *)RDEscapeString(module_name));

  q=new RDSqlQuery(sql);

  if(!q->first()) 
  {
      // Do any RDHOTKEY Module initializations for new objects here!

      if (strcmp((const char *)module_name,"airplay") ==0 ) 
      {
	  delete q;
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=1,\
            KEY_LABEL=\"Start Line 1\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;

          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=2,\
            KEY_LABEL=\"Stop Line 1\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
     
 
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=3,\
            KEY_LABEL=\"Pause Line 1\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
      
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=4,\
            KEY_LABEL=\"Start Line 2\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
          
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=5,\
            KEY_LABEL=\"Stop Line 2\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=6,\
            KEY_LABEL=\"Pause Line 2\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
        
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=7,\
            KEY_LABEL=\"Start Line 3\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=8,\
            KEY_LABEL=\"Stop Line 3\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=9,\
            KEY_LABEL=\"Pause Line 3\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=10,\
            KEY_LABEL=\"Start Line 4\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=11,\
            KEY_LABEL=\"Stop Line 4\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=12,\
            KEY_LABEL=\"Pause Line 4\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=13,\
            KEY_LABEL=\"Start Line 5\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=14,\
            KEY_LABEL=\"Stop Line 5\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=15,\
            KEY_LABEL=\"Pause Line 5\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=16,\
            KEY_LABEL=\"Start Line 6\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
            
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=17,\
            KEY_LABEL=\"Stop Line 6\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=18,\
            KEY_LABEL=\"Pause Line 6\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=19,\
            KEY_LABEL=\"Start Line 7\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=20,\
            KEY_LABEL=\"Stop Line 7\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=21,\
            KEY_LABEL=\"Pause Line 7\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=22,\
            KEY_LABEL=\"Add\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=23,\
            KEY_LABEL=\"Delete\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=24,\
            KEY_LABEL=\"Copy\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=25,\
            KEY_LABEL=\"Move\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=26,\
            KEY_LABEL=\"Sound Panel\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=27,\
            KEY_LABEL=\"Main Log\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=28,\
            KEY_LABEL=\"Aux Log 1\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
          delete q;
       
          sql=QString().sprintf("insert into RDHOTKEYS set STATION_NAME=\"%s\",\
            MODULE_NAME=\"airplay\",\
            KEY_ID=29,\
            KEY_LABEL=\"Aux Log 2\" ",
            (const char *)RDEscapeString(station_hotkeys));
          q=new RDSqlQuery(sql);
        }
    }
    delete q;
}


QString RDHotkeys::station() const
{
  return station_hotkeys;
}


QString RDHotkeys::GetRowLabel(const QString &station,const QString &module,const QString &value) const
{
  RDSqlQuery *q;
  QString sql; 
  QString hotkey_label;

  sql=QString().sprintf("SELECT KEY_LABEL FROM RDHOTKEYS WHERE STATION_NAME=\"%s\" \
                         AND MODULE_NAME=\"%s\" and KEY_VALUE = \"%s\"",
			(const char *)station,
			(const char *)module,
			(const char *)value);
  q=new RDSqlQuery(sql);

  if(!q->first()) {
      hotkey_label = QString("");
  }
  else {
      hotkey_label = QString().sprintf("%s",(const char *)q->value(0).toString());
  }

  delete q;
  return hotkey_label;
}

