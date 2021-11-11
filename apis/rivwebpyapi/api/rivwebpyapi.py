# rivwebpyapi.py
#
# Python3 binding for the Rivendell Web API
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

import sys

import datetime
from datetime import timedelta
import requests
from xml.sax.handler import ContentHandler
import xml.sax

class RivWebPyApi_ListHandler(ContentHandler):
    def __init__(self,base_tag,fields):
        self.__output=[]
        self.__record={}
        self.__base_tag=base_tag
        self.__fields=fields;
        self.__field=''

    def output(self):
        for d in self.__output:  # Convert to the appropriate types
            for f in self.__fields.keys():
                d[f]=d[f].strip()
                if(self.__fields[f]=='boolean'):
                    d[f]=self.__fromBoolean(d[f])
                elif(self.__fields[f]=='datetime'):
                    d[f]=self.__fromDatetime(d[f])
                elif(self.__fields[f]=='date'):
                    d[f]=self.__fromDate(d[f])
                elif(self.__fields[f]=='integer'):
                    d[f]=self.__fromInteger(d[f])
                elif(self.__fields[f]=='time'):
                    d[f]=self.__fromTime(d[f])
        return self.__output

    def startElement(self,tag,attrs):
        if(tag==self.__base_tag):  # Create new (empty) record
            self.__record={}
            for f in self.__fields.keys():
                self.__record[f]=''
        if('src' in attrs.keys()):
            tag=tag+attrs['src'].capitalize()
        self.__field=tag

    def endElement(self,tag):
        if(tag==self.__base_tag):  # Add completed record to output
            self.__output.append(self.__record)
            self.__field=''

    def characters(self,content):
        if(self.__field in self.__fields.keys()):  # Add content to field
            self.__record[self.__field]=self.__record[self.__field]+content

    def __fromBoolean(self,str):
        if(not str):
            return None
        if((str=='1')or(str.lower()=='true')):
            return True
        if((str=='0')or(str.lower()=='false')):
            return False
        raise ValueError('invalid boolean value')

    def __fromDate(self,str):
        if(not str):
            return None
        f0=str.split('-')
        if(len(f0)!=3):
            raise(ValueError('invalid date string'))
        return datetime.date(year=int(f0[0]),
                             month=int(f0[1]),
                             day=int(f0[2]));

    def __fromDatetime(self,str):
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

    def __fromInteger(self,str):
        if(not str):
            return None
        return int(str)

    def __fromTime(self,str):
        if(not str):
            return None

        #
        # Split time from timezone fields
        #
        timestr=''
        zonestr=''
        if('+' in str):
            f0=str.split('+')
            timestr=f0[0]
            zonestr=f0[1]
        elif('-' in str):
            f0=str.split('-')
            timestr=f0[0]
            zonestr=f0[1]
        else:
            timestr=str

        #
        # Calculate time part
        #
        time_parts=timestr.split(':')
        if(len(time_parts)!=3):
            raise ValueError('invalid time string')
        msecs=0
        if('.' in time_parts[2]):
            f0=time_parts[2].split('.')
            msecs=int(f0[1])
            time_parts[2]=f0[0]

        #
        # Calculate timezone offset
        #
        offset_minutes=0
        if(zonestr):
            offset_minutes=60*int(zonestr[0:2])+int(timestr[3:5])

        #
        # Put it all together
        #
        if(zonestr):
            offset=timedelta(minutes=offset_minutes)
            if('-' in str):
                offset=-offset
            return datetime.time(hour=int(time_parts[0]),
                                 minute=int(time_parts[1]),
                                 second=int(time_parts[2]),
                                 microsecond=1000*msecs,
                                 tzinfo=datetime.timezone(offset))
        return datetime.time(hour=int(time_parts[0]),
                             minute=int(time_parts[1]),
                             second=int(time_parts[2]),
                             microsecond=1000*msecs)

class rivwebpyapi(object):
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

    def ListCarts(self,group_name='',filter_string='',cart_type='all',
                  include_cuts=False):
        """
          Returns a list of Rivendell carts (dictionary)

          Takes the following arguments:

          group_name - Filter returns to only include carts belonging to
                       the specified group. Default is to not filter by
                       group. (string)

          filter_string - Filter returns to only include carts matching the
                          filter string. Default is not to filter by phrase.
                          (string)

          cart_type - Filter returns to only include carts matching the
                      specified type. Recognized values are 'all', 'audio'
                      or 'macro'. Default is 'all'. (string)

          include_cuts - Include cut information in the return. Default is
                         False. (boolean)
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '6',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'GROUP_NAME': str(group_name),
            'FILTER': filter_string,
            'TYPE': cart_type.lower(),
            'INCLUDE_CUTS': '0'
        }
        if(include_cuts):
            postdata['INCLUDE_CUTS']='1'

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
            'number': 'integer',
            'type': 'string',
            'groupName': 'string',
            'title': 'string',
            'artist': 'string',
            'album': 'string',
            'year': 'integer',
            'label': 'string',
            'client': 'string',
            'agency': 'string',
            'publisher': 'string',
            'composer': 'string',
            'conductor': 'string',
            'userDefined': 'string',
            'usageCode': 'integer',
            'forcedLength': 'string',
            'averageLength': 'string',
            'lengthDeviation': 'string',
            'averageSegueLength': 'string',
            'averageHookLength': 'string',
            'minimumTalkLength': 'string',
            'maximumTalkLength': 'string',
            'cutQuantity': 'integer',
            'lastCutPlayed': 'integer',
            'enforceLength': 'boolean',
            'asyncronous': 'boolean',
            'owner': 'string',
            'metadataDatetime': 'datetime',
            'songId': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='cart',fields=fields)
        print(r.text)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListGroup(self,group_name):
        """
          Returns a list of Rivendell groups (dictionary).

          Takes the following argument:

          group_name - Return the attributes of the specified group name.
                       (string)
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '5',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'GROUP_NAME': group_name
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
            'description': 'string',
            'defaultCartType': 'string',
            'defaultLowCart': 'integer',
            'defaultHighCart': 'integer',
            'cutShelfLife': 'integer',
            'defaultTitle': 'string',
            'enforceCartRange': 'boolean',
            'reportTfc': 'boolean',
            'reportMus': 'boolean',
            'color': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='group',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListGroups(self):
        """
          Returns a list of Rivendell groups (dictionary).
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '4',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password
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
            'description': 'string',
            'defaultCartType': 'string',
            'defaultLowCart': 'integer',
            'defaultHighCart': 'integer',
            'cutShelfLife': 'integer',
            'defaultTitle': 'string',
            'enforceCartRange': 'boolean',
            'reportTfc': 'boolean',
            'reportMus': 'boolean',
            'color': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='group',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListLog(self,log_name):
        """
          Returns a list of Rivendell logs (dictionary).

          Takes the following argument:

          log_name - Return the specified log. (string)
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '22',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'NAME': log_name
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
            'line': 'integer',
            'id': 'integer',
            'type': 'string',
            'cartType': 'string',
            'cartNumber': 'integer',
            'cutNumber': 'integer',
            'groupName': 'string',
            'groupColor': 'string',
            'title': 'string',
            'artist': 'string',
            'publisher': 'string',
            'composer': 'string',
            'album': 'string',
            'label': 'string',
            'year': 'integer',
            'client': 'string',
            'agency': 'string',
            'conductor': 'string',
            'userDefined': 'string',
            'usageCode': 'integer',
            'enforceLength': 'boolean',
            'forcedLength': 'string',
            'evergreen': 'boolean',
            'source': 'string',
            'timeType': 'string',
            'startTime': 'time',
            'transitionType': 'string',
            'cutQuantity': 'integer',
            'lastCutPlayed': 'integer',
            'markerComment': 'string',
            'markerLabel': 'string',
            'description': 'string',
            'isrc': 'string',
            'isci': 'string',
            'recordingMbId': 'string',
            'releaseMbId': 'string',
            'originUser': 'string',
            'originDateTime': 'datetime',
            'startPointCart': 'integer',
            'startPointLog': 'integer',
            'endPointCart': 'integer',
            'endPointLog': 'integer',
            'segueStartPointCart': 'integer',
            'segueStartPointLog': 'integer',
            'segueEndPointCart': 'integer',
            'segueEndPointLog': 'integer',
            'segueGain': 'integer',
            'fadeupPointCart': 'integer',
            'fadeupPointLog': 'integer',
            'fadeupGain': 'integer',
            'fadedownPointCart': 'integer',
            'fadedownPointLog': 'integer',
            'fadedownGain': 'integer',
            'duckUpGain': 'integer',
            'duckDownGain': 'integer',
            'talkStartPoint': 'integer',
            'talkEndPoint': 'integer',
            'hookMode': 'boolean',
            'hookStartPoint': 'integer',
            'hookEndPoint': 'integer',
            'eventLength': 'integer',
            'linkEventName': 'string',
            'linkStartTime': 'time',
            'linkStartSlop': 'integer',
            'linkEndSlop': 'integer',
            'linkId': 'integer',
            'linkEmbedded': 'boolean',
            'extStartTime': 'time',
            'extLength': 'integer',
            'extCartName': 'string',
            'extData': 'string',
            'extEventId': 'integer',
            'extAnncType': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='logLine',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

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
          Returns a list of Rivendell services (dictionary).

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
