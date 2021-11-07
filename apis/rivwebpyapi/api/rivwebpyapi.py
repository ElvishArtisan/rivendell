# pypad.py
#
# Python binding for the Rivendell Web API
#
#   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import datetime
from datetime import timedelta
import requests
from xml.sax.handler import ContentHandler
import xml.sax

class RivWebPyApi_ListHandler(ContentHandler):
    def __init__(self,base_tag,fields):
        self.__output=[]
        self.__svc={}
        self.__base_tag=base_tag
        self.__fields=fields;
        self.__field=''

    def output(self):
        for d in self.__output:  # Convert to the appropriate types
            for f in self.__fields.keys():
                d[f]=d[f].strip()
                if(self.__fields[f]=='boolean'):
                    d[f]=d[f]!='0'
                elif(self.__fields[f]=='datetime'):
                    d[f]=self.__parseDatetime(d[f])
                elif(self.__fields[f]=='date'):
                    d[f]=self.__parseDate(d[f])
                elif(self.__fields[f]=='integer'):
                    d[f]=int(d[f])
        return self.__output

    def startElement(self,tag,attrs):
        if(tag==self.__base_tag):  # Create new (empty) record
            self.__svc={}
            for f in self.__fields.keys():
                self.__svc[f]=''
        self.__field=tag

    def endElement(self,tag):
        if(tag==self.__base_tag):  # Add completed record to output
            self.__output.append(self.__svc)
            self.__field=''

    def characters(self,content):
        if(self.__field in self.__fields.keys()):  # Add content to field
            self.__svc[self.__field]=self.__svc[self.__field]+content

    def __parseDatetime(self,str):
        if(not str):
            return None
        f0=str.split('T')
        if(len(f0)!=2):
            raise ValueError('invalid datetime string')
        date_parts=f0[0].split('-')

        if(len(date_parts)!=3):
            raise ValueError('invalid datetime string')

        time_parts=f0[1][0:7].split(':')
        if(len(time_parts)!=3):
            raise ValueError('invalid datetime string')
        offset_minutes=60*int(f0[1][9:11])+int(f0[1][12:14])
        offset=timedelta(minutes=offset_minutes)
        if(f0[1][8:9]=='-'):
            offset=-offset
        return datetime.datetime(year=int(date_parts[0]),
                                 month=int(date_parts[1]),
                                 day=int(date_parts[2]),
                                 hour=int(time_parts[0]),
                                 minute=int(time_parts[1]),
                                 second=int(time_parts[2]),
                                 tzinfo=datetime.timezone(offset));

    def __parseDate(self,str):
        if(not str):
            return None
        f0=str.split('-')
        if(len(f0)!=3):
            raise(ValueError('invalid date string'))
        return datetime.date(year=int(f0[0]),
                             month=int(f0[1]),
                             day=int(f0[2]));

class RivWebPyApi(object):
    """
       Create a 'RivWebPyApi' object for accessing the Web API.
    """

    def __init__(self,url,username,password):
        """
           Initialize a WebAPI connection to a Rivendell instance.

           Takes three arguments:

           url - The URL of the service. Typically of the form
                'http://rivendell.example.com/rd-bin/rdxport.cgi' (string).

           username - The Rivendell Username to connect as (string).

           password - The password for the specified Rivendell Username
                      (string).
        """
        self.__connection_url=url
        self.__connection_username=username
        self.__connection_password=password

    def ListLogs(self,service_name='',log_name='',trackable=False,
                 filter_string='',recent=False):
        """
          Returns a list of Rivendell logs (dictionary).

          Takes the following arguments:

          service_name - Return only logs belonging to the specified service.
                         Default is to return all logs. (string)

          log_name - Return only the specified log. Default is to return
                     all logs. (string)

          trackable - If True, return will include only logs that
                      contain one or more voice track slots. If False,
                      all logs will be included. (boolean)

          filter_string - Return only logs that contain the filter_string
                          in the Name, Description or Service fields.
                          Default is to return all logs. (string)

          recent - Return the 14 most recently created logs. (boolean)
        """

        #
        # Build the WebAPI arguments
        #
        trackarg='0'
        if(trackable):
            trackarg='1'
        recentarg='0'
        if(recent):
            recentarg='1'
        postdata={
            'COMMAND': '20',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'SERVICE_NAME': service_name,
            'LOG_NAME': log_name,
            'TRACKABLE': trackarg,
            'FILTER': filter_string,
            'RECENT': recentarg
        }

        #
        # Fetch the XML
        #
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'name': 'string',
            'serviceName': 'string',
            'description': 'string',
            'originUserName': 'string',
            'originDatetime': 'datetime',
            'linkDatetime': 'datetime',
            'modifiedDatetime': 'datetime',
            'purgeDate': 'date',
            'autoRefresh': 'boolean',
            'startDate': 'date',
            'endDate': 'date',
            'scheduledTracks': 'integer',
            'completedTracks': 'integer',
            'musicLinks': 'integer',
            'musicLinked': 'boolean',
            'trafficLinks': 'integer',
            'trafficLinked': 'boolean'
        }
        handler=RivWebPyApi_ListHandler(base_tag='log',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListServices(self,trackable):
        """
          Returns a list of Rivendell services (Element).

          Takes one argument:

          trackable - Boolean. If True, return will include only services that
                      are configured to support voice tracking. If False,
                      all configured services will be included.
        """

        #
        # Build the WebAPI arguments
        #
        trackarg='0'
        if(trackable):
            trackarg='1'
        postdata={
            'COMMAND': '21',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'TRACKABLE': trackarg
        }

        #
        # Fetch the XML
        #
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'name': 'string',
            'description': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='service',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()
