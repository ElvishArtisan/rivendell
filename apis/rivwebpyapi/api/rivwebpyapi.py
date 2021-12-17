# rivwebpyapi.py
#
# Python3 binding for the Rivendell Web API
#
#   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import datetime
import sys
from datetime import timedelta
from urllib.parse import urlparse
from xml.sax.handler import ContentHandler
import xml.sax

import requests

#
# Audio Formats
#
AUDIO_FORMAT_PCM16_WAV=0
AUDIO_FORMAT_PCM24_WAV=7
AUDIO_FORMAT_MPEG_L2_WAV=6
AUDIO_FORMAT_MPEG_L2_NATIVE=2
AUDIO_FORMAT_MPEG_L3_NATIVE=3
AUDIO_FORMAT_FLAC=4
AUDIO_FORMAT_OGG_VORBIS=5

#
# Data Schemas
#
CART_FIELDS={
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

CUT_FIELDS={
    'cutName': 'string',
    'cartNumber': 'integer',
    'cutNumber': 'integer',
    'evergreen': 'boolean',
    'description': 'string',
    'outcue': 'string',
    'isrc': 'string',
    'isci': 'string',
    'recordingMbId': 'string',
    'releaseMbId': 'string',
    'length': 'integer',
    'originDatetime': 'datetime',
    'startDatetime': 'datetime',
    'endDatetime': 'datetime',
    'sun': 'boolean',
    'mon': 'boolean',
    'tue': 'boolean',
    'wed': 'boolean',
    'thu': 'boolean',
    'fri': 'boolean',
    'sat': 'boolean',
    'startDaypart': 'time',
    'endDaypart': 'time',
    'originName': 'string',
    'originLoginName': 'string',
    'sourceHostname': 'string',
    'weight': 'integer',
    'lastPlayDatetime': 'datetime',
    'playCounter': 'integer',
    'codingFormat': 'integer',
    'sampleRate': 'integer',
    'bitRate': 'integer',
    'channels': 'integer',
    'playGain': 'integer',
    'startPoint': 'integer',
    'endPoint': 'integer',
    'fadeupPoint': 'integer',
    'fadedownPoint': 'integer',
    'segueStartPoint': 'integer',
    'segueEndPoint': 'integer',
    'segueGain': 'integer',
    'hookStartPoint': 'integer',
    'hookEndPoint': 'integer',
    'talkStartPoint': 'integer',
    'talkEndPoint': 'integer'
}

GROUP_FIELDS={
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

LOG_FIELDS={
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

LOGLINE_FIELDS={
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
    'graceTime': 'integer',
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
    'linkLength': 'integer',
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

LOG_LOCK_FIELDS={
    'result': 'boolean',
    'logName': 'string',
    'lockGuid': 'string',
    'address': 'string',
    'lockTimeout': 'integer'
}

SCHEDULER_CODE_FIELDS={
    'code': 'string',
    'description': 'string'
}

SERVICE_FIELDS={
    'name': 'string',
    'description': 'string'
}

SYSTEM_SETTINGS_FIELDS={
    'realmName': 'string',
    'sampleRate': 'integer',
    'duplicateTitles': 'boolean',
    'fixDuplicateTitles': 'boolean',
    'maxPostLength': 'integer',
    'isciXreferencePath': 'string',
    'tempCartGroup': 'string',
    'longDateFormat': 'string',
    'shortDateFormat': 'string',
    'showTwelveHourTime': 'boolean'
}

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
                if(d[f]!=None):
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
                self.__record[f]=None
        else:
            if(tag in self.__fields.keys()):
                self.__record[tag]=''
        if('src' in attrs.keys()):
            tag=tag+attrs['src'].capitalize()
            self.__record[tag]=''
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

class RivWebPyError(Exception):
    def __init__(self,resp_body,resp_code):
        self.errorString=resp_body
        self.responseCode=resp_code

class RivendellType(object):
    def __init__(self,schema,values):
        self.__schema=schema
        if(len(values)==0):
            self.__values={}
            for key in schema:
                self.__values[key]=None
        else:
            self.__values=values

    def schema(self):
        return self.__schema

    def values(self):
        return self.__values

    def setValues(self,values):
        for key in values:
            if(key in self.__values.keys()):
                self.__values[key]=values[key]

class Cart(RivendellType):
    def __init__(self,values={}):
        super().__init__(CART_FIELDS,values)

class Cut(RivendellType):
    def __init__(self,values={}):
        super().__init__(CUT_FIELDS,values)

class Group(RivendellType):
    def __init__(self,values={}):
        super().__init__(GROUP_FIELDS,values)

class Log(RivendellType):
    def __init__(self,values={}):
        super().__init__(LOG_FIELDS,values)

class LogLine(RivendellType):
    def __init__(self,values={}):
        super().__init__(LOGLINE_FIELDS,values)

class LogLock(RivendellType):
    def __init__(self,values={}):
        super().__init__(LOG_LOCK_FIELDS,values)

class SchedulerCode(RivendellType):
    def __init__(self,values={}):
        super().__init__(SCHEDULER_CODE_FIELDS,values)

class Service(RivendellType):
    def __init__(self,values={}):
        super().__init__(SERVICE_FIELDS,values)

class SystemSettings(RivendellType):
    def __init__(self,values={}):
        super().__init__(SYSTEM_SETTINGS_FIELDS,values)

class Site(object):
    """
       Create a 'RivWebPyApi' object for accessing the Web API.
    """

    def __init__(self,url,username,password):
        """
           Initialize a WebAPI connection to a Rivendell instance.

           Takes three arguments:

           url - The URL of the service. Typically of the form
                'http://rivendell.example.com' (string).

           username - The Rivendell Username to connect as (string).

           password - The password for the specified Rivendell Username
                      (string).
        """
        #
        # Normalize the URL
        #
        parsed_url=urlparse(url)
        parsed_url=parsed_url._replace(path='/rd-bin/rdxport.cgi',
                                       params='',
                                       query='',
                                       fragment='')
        self.__connection_url=parsed_url.geturl()
        self.__connection_username=username
        self.__connection_password=password
        self.__session=requests.Session()

    def __throwError(self,response):
        if(response.headers['content-type']=='application/xml'):
            fields={
                'ResponseCode': 'integer',
                'ErrorString': 'string'
            }
            handler=RivWebPyApi_ListHandler(base_tag='RDWebResult',fields=fields)
            xml.sax.parseString(response.text,handler)
            output=handler.output();
            if((len(output)==1)and('ErrorString' in output[0])):
                raise RivWebPyError(resp_body=output[0]['ErrorString'],
                                    resp_code=response.status_code)
        response.raise_for_status()

    def AddCart(self,group_name,cart_type,cart_number=0):
        """
          Add a new cart to the cart library. Returns the metadata of
          the newly created cart (rivwebpyapi.Cart object).

          Takes the following arguments:

          group_name - The name of the group to which the created cart
                       will belong (string).

          cart_type - The type of cart to be created. Valid values are
                      'audio' and 'macro' (string).

          cart_number - The number of the desired cart, in the range
                        1 - 999999. Optional. If no cart number
                        is specified, the next available cart number for
                        the specified group will be used (integer).
        """

        if((cart_number<0)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cart_type!='audio')and(cart_type!='macro')):
            raise ValueError('invalid cart type')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '12',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'GROUP_NAME': str(group_name),
            'TYPE': str(cart_type)
        }
        if(cart_number>0):
            postdata['CART_NUMBER']=str(cart_number)

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cart',fields=CART_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cart(handler.output()[0])

    def AddCut(self,cart_number):
        """
          Add a new cut to an existing audio cart. Returns the metadata of
          the newly created cut (rivwebpyapi.Cut object).

          Takes the following argument:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '10',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=CUT_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cut(handler.output()[0])

    def AddLog(self,service_name,log_name):
        """
          Create a new log.

          Takes the following arguments:

          service-name - The name of the service to which the new log will
                         belong (string).

          log-name - The name of the log to be created (string).
        """

        if(not service_name):
            raise ValueError('invalid service name')
        if(not log_name):
            raise ValueError('invalid log name')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '29',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'SERVICE_NAME': str(service_name),
            'LOG_NAME': str(log_name)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def AssignSchedCode(self,cart_number,sched_code):
        """
          Assign a schedule code to a cart.

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          sched_code - An existing schedule code (integer).
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if(not sched_code):
            raise ValueError('invalid schedule code')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '25',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CODE': sched_code
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def AudioInfo(self,cart_number,cut_number):
        """
          Get information about an entry in the Rivendell audio store
          (dictionary).

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '19',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        fields={
            'cartNumber': 'integer',
            'cutNumber': 'integer',
            'format': 'integer',
            'channels': 'integer',
            'sampleRate': 'integer',
            'bitRate': 'integer',
            'frames': 'integer',
            'length': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='audioInfo',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()[0]

    def AudioStore(self):
        """
          Get information about the audio store (dictionary).
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '23',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        fields={
            'freeBytes': 'integer',
            'totalBytes': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='audioStore',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()[0]

    def DeleteAudio(self,cart_number,cut_number):
        """
          Delete PCM/MPEG data from the audio store.

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '3',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        fields={
            'cartNumber': 'integer',
            'cutNumber': 'integer',
            'format': 'integer',
            'channels': 'integer',
            'sampleRate': 'integer',
            'bitRate': 'integer',
            'frames': 'integer',
            'length': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='audioInfo',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def DeleteLog(self,log_name):
        """
          Delete an existing log.

          Takes the following argument:

          log_name - The name of the log to be deleted (string).
        """

        if(not log_name):
            raise ValueError('invalid log name')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '30',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'LOG_NAME': log_name
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def EditCart(self,cart_number,values):
        """
          Write metadata changes for an existing cart into the database.

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999.

          values - A rivwebpyapi.Cart object containing the desired edits.
        """

        if((cart_number<0)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '14',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number)
        }
        postnames={
            'groupName': 'GROUP_NAME',
            'title': 'TITLE',
            'artist': 'ARTIST',
            'album': 'ALBUM',
            'year': 'YEAR',
            'label': 'LABEL',
            'client': 'CLIENT',
            'agency': 'AGENCY',
            'publisher': 'PUBLISHER',
            'composer': 'COMPOSER',
            'conductor': 'CONDUCTOR',
            'userDefined': 'USER_DEFINED',
            'usageCode': 'USAGE_CODE',
            'forcedLength': 'FORCED_LENGTH',
            'enforceLength': 'ENFORCE_LENGTH',
            'asyncronous': 'ASYNCHRONOUS',
            'owner': 'OWNER'
        }
        for key in values:
            if((values[key]!=None)and(key in postnames.keys())):
                postdata[postnames[key]]=values[key]

        #
        # Execute
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cart',fields=CART_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cart(handler.output()[0])

    def EditCut(self,cart_number,cut_number,values):
        """
          Write metadata changes for an existing cut into the database.

          Takes the following arguments:

          cart_number - The number of the cart containing the cut to be
                        edited, in the range 1 - 999999.

          cut_number - The number of the cut to be edited, in the range
                       1 - 999 (integer)

          values - A rivwebpyapi.Cut object containing the desired edits.
        """

        if((cart_number<0)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '15',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }
        postnames={
            'evergreen': 'EVERGREEN',
            'description': 'DESCRIPTION',
            'outcue': 'OUTCUE',
            'isrc': 'ISRC',
            'isci': 'ISCI',
            'startDatetime': 'START_DATETIME',
            'endDatetime': 'END_DATETIME',
            'sun': 'SUN',
            'mon': 'MON',
            'tue': 'TUE',
            'wed': 'WED',
            'thu': 'THU',
            'fri': 'FRI',
            'sat': 'SAT',
            'startDaypart': 'START_DAYPART',
            'endDaypart': 'END_DAYPART',
            'weight': 'WEIGHT',
            'startPoint': 'START_POINT',
            'endPoint': 'END_POINT',
            'fadeupPoint': 'FADEUP_POINT',
            'fadedownPoint': 'FADEDOWN_POINT',
            'segueStartPoint': 'SEGUE_START_POINT',
            'segueEndPoint': 'SEGUE_END_POINT',
            'hookStartPoint': 'HOOK_START_POINT',
            'hookEndPoint': 'HOOK_END_POINT',
            'talkStartPoint': 'TALK_START_POINT',
            'talkEndPoint': 'TALK_END_POINT'
        }
        for key in values:
            if((values[key]!=None)and(key in postnames.keys())):
                postdata[postnames[key]]=values[key]

        #
        # Execute
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=CUT_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cut(handler.output()[0])

    def Export(self,filename,cart_number,cut_number,audio_format,channels,
               sample_rate,bit_rate,quality,start_point,end_point,
               normalization_level,enable_metadata):
        """
          Export audio data from the audio store (file).

          Takes the following arguments:

          filename - Location to save the exported audio file (string).

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)

          audio_format - Format of the exported audio (AUDIO_FORMAT)

          channels - Channel count of the exported audio (integer)

          sample_rate - Sample rate of the exported audio (integer, 
                        samples/sec)

          bit_rate - Bit rate of the export audio [MPEG formats only]
                     (integer, bits/sec)

          quality - Quality [OggVorbis only] (integer, -1 - 10)

          start_point - Start point of audio export (integer, mS from
                        absolute start of stored audio)

          end_point - End point of audio export (integer, mS from
                      absolute start of stored audio)

          normalization_level - Normalization level, 0 = no normalization
                                (integer, dBFS)

          enable_metadata - Encode metadata into exported file (boolean)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '1',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number),
            'FORMAT': str(audio_format),
            'CHANNELS': str(channels),
            'SAMPLE_RATE': str(sample_rate),
            'BIT_RATE': str(bit_rate),
            'QUALITY': str(quality),
            'START_POINT': str(start_point),
            'END_POINT': str(end_point),
            'NORMALIZATION_LEVEL': str(normalization_level)
        }
        if(enable_metadata):
            postdata['ENABLE_METADATA']='1'
        else:
            postdata['ENABLE_METADATA']='0'

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata,stream=True)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)
        with open(filename,'wb') as handle:
            for block in r.iter_content(1024):
                handle.write(block)

    def ExportPeaks(self,cart_number,cut_number):
        """
          Export audio peak data from the audio store (bytes).

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '16',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the peak data
        #
        r=self.__session.post(self.__connection_url,data=postdata,stream=True)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)
        ret=bytes()
        for block in r.iter_content(chunk_size=1024):
            ret=ret+block
        return ret

    def GetPodcast(self,cast_id,filename):
        """
          Get posted podcast audio from the Rivendell audio store.

          Takes the following arguments:

          cast_id - ID of the owning podcast item (integer).

          filename - Destination filename (string).
        """

        if(cast_id<0):
            raise ValueError('invalid podcast ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '37',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(cast_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata,stream=True)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)
        with open(filename,'wb') as handle:
            for block in r.iter_content(1024):
                handle.write(block)

    def Import(self,filename,use_metadata,cart_number=0,cut_number=0,
               channels=2,normalization_level=0,autotrim_level=0,
               group_name='',title=''):
        """
          Import audio into the audio store

          Takes the following arguments:

          filename - Name of the file containing audio to be imported
                     (string).

          cart_number - The number of the desired cart, in the range
                        0 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        0 - 999 (integer)

          use_metadata - Apply file embedded metadata to the destination
                         cart/cut (boolean).

          channels - Use specified number of channels in the destination
                     cut. Valid values are 1 or 2 (integer).

          normalization_level - Normalization level, 0 = no normalization
                                (integer, dBFS)

          autotrim_level - Autotrim level, 0 = no autotrim (integer, dBFS)

          group_name - If both the 'cart-number' and 'cut-number' parameters
                       are set to 0 and a valid 'group_name' parameter passed,
                       the system will attempt to create a new cart/cut in
                       the specified group.

          title - The value to set as the title of the target cart. This
                  will override any value found in in-file metadata
                  (string).
        """

        if(not filename):
            raise ValueError('missing filename')
        if(group_name):
            if((cart_number!=0) or (cut_number!=0)):
                raise RuntimeError('cart/cut numbers cannot be specified when using the "group_name" parameter')
        else:
            if((cart_number<1)or(cart_number>999999)):
                raise ValueError('invalid cart number')
            if((cut_number<1)or(cut_number>999)):
                raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '2',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number),
            'CHANNELS': str(channels),
            'NORMALIZATION_LEVEL': str(normalization_level),
            'AUTOTRIM_LEVEL': str(autotrim_level),
            'TITLE': str(title),
            'USE_METADATA': '0'
        }
        if(use_metadata):
            postdata['USE_METADATA']='1'
        if(group_name):
            postdata['CREATE']='1'
            postdata['GROUP_NAME']=group_name
        files={'FILENAME': open(filename,'rb')}

        #
        # Execute
        #
        r=self.__session.post(self.__connection_url,data=postdata,files=files)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Process Response
        #
        fields={
            'ResponseCode': 'integer',
            'ErrorString': 'string',
            'CartNumber': 'integer',
            'CutNumber': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='RDWebResult',fields=fields)
        xml.sax.parseString(r.text,handler)
        output=handler.output();
        if(len(output)==1):
            return output[0]

    def ListCart(self,cart_number,include_cuts=False):
        """
          Returns the metadata associated with a Rivendell cart
          (rivwebpyapi.Cart object).

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          include_cuts - Include cut information in the return. Default is
                         False. (boolean)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '7',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'INCLUDE_CUTS': '0'
        }
        if(include_cuts):
            postdata['INCLUDE_CUTS']='1'

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cart',fields=CART_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cart(handler.output()[0])

    def ListCarts(self,group_name='',filter_string='',cart_type='all',
                  include_cuts=False):
        """
          Returns the metadata associated with a set of Rivendell carts
          (list of rivwebpyapi.Cart objects)

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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output
        #
        handler=RivWebPyApi_ListHandler(base_tag='cart',fields=CART_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(Cart(item))

        return ret

    def ListCartSchedCodes(self,cart_number):
        """
          Returns the set of scheduler codes associated with a Rivendell
          cart (list of rivwebpyapi.SchedulerCode objects).

          Takes the following arguments:

          cart_number - The number of the cart for the desired schedule codes,
                        in the range
                        1 - 999999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '27',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        handler=RivWebPyApi_ListHandler(base_tag='schedCode',
                                        fields=SCHEDULER_CODE_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(SchedulerCode(item))

        return ret

    def ListCut(self,cart_number,cut_number):
        """
          Returns the metadata associated with a Rivendell cut
          (rivwebpyapi.Cut object).

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '8',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=CUT_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Cut(handler.output()[0])

    def ListCuts(self,cart_number):
        """
          Returns the metadata associated with all of the cuts in
          a Rivendell cart (list of rivwebpyapi.Cut objects).

          Takes the following argument:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '9',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=CUT_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(Cut(item))
        return ret

    def ListGroup(self,group_name):
        """
          Returns a Rivendell group (rivwebpyapi.Group object).

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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='group',fields=GROUP_FIELDS)
        xml.sax.parseString(r.text,handler)

        return Group(handler.output()[0])

    def ListGroups(self):
        """
          Returns a list of all Rivendell groups
          (list of rivwebpyapi.Group objects).
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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='group',fields=GROUP_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(Group(item))

        return ret

    def ListLog(self,log_name):
        """
          Returns the contents of a Rivendell log
          (list of rivwebpyapi.LogLine objects).

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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='logLine',
                                        fields=LOGLINE_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(LogLine(item))
        return ret

    def ListLogs(self,service_name='',log_name='',trackable=False,
                 filter_string='',recent=False):
        """
          Returns the metadata for a set of Rivendell logs
          (list of rivwebpyapi.Log objects).

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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='log',fields=LOG_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(Log(item))
        return ret

    def ListSchedulerCodes(self):
        """
          Returns a list of all defined Rivendell schedule codes
          (list of rivwebpyapi.SchedulerCode objects).
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '24',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='schedCode',
                                        fields=SCHEDULER_CODE_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(SchedulerCode(item))

        return ret

    def ListServices(self,trackable):
        """
          Returns a set of Rivendell services
          (list of rivwebpyapi.Service objects).

          Takes one argument:

          trackable - Boolean. If True, return will include only services that
                      are configured to support voice tracking. If False,
                      all configured services for which the user has access
                      will be included.
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
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='service',
                                        fields=SERVICE_FIELDS)
        xml.sax.parseString(r.text,handler)
        out=handler.output()
        ret=[]
        for item in out:
            ret.append(Service(item))

        return ret

    def ListSystemSettings(self):
        """
          Returns Rivendell system settings (rivwebpyapi.SystemSettings object).
        """

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '33',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        handler=RivWebPyApi_ListHandler(base_tag='systemSettings',
                                        fields=SYSTEM_SETTINGS_FIELDS)
        xml.sax.parseString(r.text,handler)

        return SystemSettings(handler.output()[0])

    def LockLog(self,log_name,operation,guid):
        """
          Manage the lock for the specified log (rivwebpyapi.LogLock object).

          Takes the following argument:

          log_name - The name of the log for which to create the write lock
                     (string)

          operation - The operation to perform. Valid values are 'CREATE',
                      'UPDATE' and 'CLEAR'.

          guid - The Globally Unique IDentifier for an existing log.
                 Need only for the 'UPDATE' and 'CLEAR' operations (string).
        """

        if(not log_name):
            raise ValueError('invalid log name')
        if((operation!='CREATE')and(operation!='UPDATE')and
           (operation!='CLEAR')):
            raise ValueError('invalid lock operation')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '34',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'LOG_NAME': str(log_name),
            'OPERATION': str(operation),
            'LOCK_GUID': str(guid)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        fields={
            'result': 'boolean',
            'logName': 'string',
            'lockGuid': 'string',
            'address': 'string',
            'lockTimeout': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='logLock',fields=fields)
        xml.sax.parseString(r.text,handler)

        return LogLock(handler.output()[0])

    def PostImage(self,image_id):
        """
          Upload a podcast image to the remote archive.

          Takes the following argument:

          image_id - ID of the image (integer).
        """

        if(image_id<0):
            raise ValueError('invalid image ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '44',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(image_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def PostPodcast(self,cast_id):
        """
          Upload podcast audio from the Rivendell audio store to the
          remote archive.

          Takes the following argument:

          cast_id - ID of the owning podcast item (integer).
        """

        if(cast_id<0):
            raise ValueError('invalid podcast ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '40',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(cast_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def PostRss(self,feed_id):
        """
          Upload podcast RSS XML to the remote archive.

          Takes the following argument:

          feed_id - ID of the owning podcast feed (integer).
        """

        if(feed_id<0):
            raise ValueError('invalid feed ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '42',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(feed_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def Rehash(self,cart_number,cut_number):
        """
          Update the SHA1 hash entry in the database from the contents of
          the audio store.

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
        """
        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '32',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def RemoveCart(self,cart_number):
        """
          Remove a cart from the cart library.

          Takes the following argument:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '13',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def RemoveCut(self,cart_number,cut_number):
        """
          Remove an existing cut from an audio cart.

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the cut to remove, in the range
                        1 - 999 (integer)
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '11',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def RemoveImage(self,image_id):
        """
          Upload a podcast image to the remote archive.

          Takes the following argument:

          image_id - ID of the image (integer).
        """

        if(image_id<0):
            raise ValueError('invalid image ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '45',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(image_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def RemovePodcast(self,cast_id):
        """
          Delete podcast audio from the remote archive.

          Takes the following argument:

          cast_id - ID of the owning podcast item (integer).
        """

        if(cast_id<0):
            raise ValueError('invalid podcast ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '41',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(cast_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def RemoveRss(self,feed_id):
        """
          Delete podcast RSS XML from the remote archive.

          Takes the following argument:

          feed_id - ID of the owning podcast feed (integer).
        """

        if(feed_id<0):
            raise ValueError('invalid feed ID')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '43',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(feed_id)
        }

        #
        # Fetch the audio data
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def SaveLog(self,log_name,header_values,line_values,guid=None):
        """
          Save a log into the database. If the specified log does not
          exist, it will be created automatically.

          Takes the following arguments:

          log_name - The log name to save to (string).

          header_values - Log header data (rivwebpyapi.Log object).

          line_values - Log line data (List of rivwebpyapi.LogLine objects).

          guid - The lock ID, provided by rivwebpyapi.LockLog() (string).
        """

        if(not log_name):
            raise ValueError('invalid log name')

        #
        # Build the WebAPI header arguments
        #
        postdata={
            'COMMAND': '28',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'LOG_NAME': str(log_name),
            'LINE_QUANTITY': str(len(line_values)),
            'SERVICE_NAME': header_values.values()['serviceName'],
            'DESCRIPTION': header_values.values()['description'],
            'PURGE_DATE': header_values.values()['purgeDate']
        }
        if(header_values.values()['autoRefresh']):
            postdata['AUTO_REFRESH']=1
        else:
            postdata['AUTO_REFRESH']=0
        if(header_values.values()['startDate']==None):
            postdata['START_DATE']=''
        else:
            postdata['START_DATE']=header_values.values()['startDate']
        if(header_values.values()['endDate']==None):
            postdata['END_DATE']=''
        else:
            postdata['END_DATE']=header_values.values()['endDate']
        if(header_values.values()['purgeDate']==None):
            postdata['PURGE_DATE']=''
        else:
            postdata['PURGE_DATE']=header_values.values()['purgeDate']
        if(guid):
            postdata['LOCK_GUID']=guid

        #
        # Log Lines
        #
        line_postnames={
            'id': 'ID',
            'type': 'TYPE',
            'cartNumber': 'CART_NUMBER',
            'timeType': 'TIME_TYPE',
            'startTime': 'START_TIME',
            'graceTime': 'GRACE_TIME',
            'transitionType': 'TRANS_TYPE',
            'originUser': 'ORIGIN_USER',
            'originDateTime': 'ORIGIN_DATETIME',
            'markerComment': 'COMMENT',
            'markerLabel': 'LABEL',
            'startPointLog': 'START_POINT',
            'endPointLog': 'END_POINT',
            'segueStartPointLog': 'SEGUE_START_POINT',
            'segueEndPointLog': 'SEGUE_END_POINT',
            'fadeupPointLog': 'FADEUP_POINT',
            'fadeupGain': 'FADEUP_GAIN',
            'fadedownPointLog': 'FADEDOWN_POINT',
            'fadedownGain': 'FADEDOWN_GAIN',
            'duckUpGain': 'DUCK_UP_GAIN',
            'duckDownGain': 'DUCK_DOWN_GAIN',
            'eventLength': 'EVENT_LENGTH',
            'linkLength': 'LINK_LENGTH',
            'linkEventName': 'LINK_EVENT_NAME',
            'linkLength': 'LINK_LENGTH',
            'linkStartTime': 'LINK_START_TIME',
            'linkStartSlop': 'LINK_START_SLOP',
            'linkEndSlop': 'LINK_END_SLOP',
            'linkId': 'LINK_ID',
            'linkEmbedded': 'LINK_EMBEDDED',
            'extStartTime': 'EXT_START_TIME',
            'extLength': 'EXT_LENGTH',
            'extCartName': 'EXT_CART_NAME',
            'extData': 'EXT_DATA',
            'extEventId': 'EXT_EVENT_ID',
            'extAnncType': 'EXT_ANNC_TYPE'
        }
        line=0
        for ll in line_values:
            for key in ll.values():
                if(key in line_postnames.keys()):
                    if(ll.values()[key]==None):
                        postdata['LINE'+str(line)+'_'+line_postnames[key]]=''
                    else:
                        postdata['LINE'+str(line)+'_'+line_postnames[key]]=ll.values()[key]
            line=line+1

        #
        # Execute
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def SavePodcast(self,cast_id,filename):
        """
          Save posted podcast audio to the Rivendell audio store.

          Takes the following arguments:

          filename - Name of the file containing audio to be stored (string).

          cast_id - ID of the owning podcast item (integer).
        """

        if(not filename):
            raise ValueError('missing filename')
        if(cast_id<0):
            raise ValueError('invalid podcast id')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '38',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'ID': str(cast_id)
        }
        files={'FILENAME': open(filename,'rb')}

        #
        # Execute
        #
        r=self.__session.post(self.__connection_url,data=postdata,files=files)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

    def TrimAudio(self,cart_number,cut_number,trim_level):
        """
          Return time pointers to the first and last instance of a
          particular level in a cut (dictionary).

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          cut_number - The number of the desired cut, in the range
                        1 - 999 (integer)
          trim_level - Level in 1/100 dB, relative to Rivendell reference
                       level [-16 dBFS] (integer).
        """
        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if((cut_number<1)or(cut_number>999)):
            raise ValueError('invalid cut number')
        if(trim_level>1600):
            raise ValueError('invalid trim level')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '17',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CUT_NUMBER': str(cut_number),
            'TRIM_LEVEL': str(trim_level)
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)

        #
        # Generate the output dictionary
        #
        fields={
            'cartNumber': 'integer',
            'cutNumber': 'integer',
            'trimLevel': 'integer',
            'startTrimPoint': 'integer',
            'endTrimPoint': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='trimPoint',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()[0]

    def UnassignSchedCode(self,cart_number,sched_code):
        """
          Unassign a schedule code from a cart.

          Takes the following arguments:

          cart_number - The number of the desired cart, in the range
                        1 - 999999 (integer)

          sched_code - An existing schedule code (integer).
        """

        if((cart_number<1)or(cart_number>999999)):
            raise ValueError('invalid cart number')
        if(not sched_code):
            raise ValueError('invalid schedule code')

        #
        # Build the WebAPI arguments
        #
        postdata={
            'COMMAND': '26',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'CART_NUMBER': str(cart_number),
            'CODE': sched_code
        }

        #
        # Fetch the XML
        #
        r=self.__session.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            self.__throwError(response=r)
