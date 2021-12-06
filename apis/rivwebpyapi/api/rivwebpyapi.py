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
from urllib.parse import urlparse
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

class rivwebpyapi(object):
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

    def AddCart(self,group_name,cart_type,cart_number=0):
        """
          Add a new cart to the cart library. Returns the metadata of
          the newly created cart (list of dictionaries).

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
        print('ARGS: '+str(postdata))
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
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def AddCut(self,cart_number):
        """
          Add a new cut to an existing audio cart. Returns the metadata of
          the newly created cut (list of dictionaries).

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
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
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

    def AudioInfo(self,cart_number,cut_number):
        """
          Get information about an entry in the Rivendell audio store
          (list of dictionaries).

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

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

    def AudioStore(self):
        """
          Get information about the audio store (list of dictionaries).
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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'freeBytes': 'integer',
            'totalBytes': 'integer'
        }
        handler=RivWebPyApi_ListHandler(base_tag='audioStore',fields=fields)
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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

    def ListCart(self,cart_number,include_cuts=False):
        """
          Returns the metadata associated with a Rivendell cart
          (list of dictionaries).

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
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListCarts(self,group_name='',filter_string='',cart_type='all',
                  include_cuts=False):
        """
          Returns the metadata associated with a set of Rivendell carts
          (list of dictionaries)

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
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListCartSchedCodes(self,cart_number):
        """
          Returns the set of scheduler codes associated with a Rivendell
          cart (list of dictionaries)

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'code': 'string',
            'description': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='schedCode',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListCut(self,cart_number,cut_number):
        """
          Returns the metadata associated with a Rivendell cut
          (list of dictionaries).

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
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
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListCuts(self,cart_number):
        """
          Returns the metadata associated with all of the cuts in
          a Rivendell cart (list of dictionaries).

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
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
        handler=RivWebPyApi_ListHandler(base_tag='cut',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListGroup(self,group_name):
        """
          Returns a Rivendell group (list of dictionaries).

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
          Returns a list of all Rivendell groups (list of dictionaries).
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
          Returns the contents of a Rivendell log (list of dictionaries).

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
          Returns the metadata for a set of Rivendell logs
          (list of dictionaries).

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

    def ListSchedulerCodes(self):
        """
          Returns a list of all defined Rivendell schedule codes
          (list of dictionaries)
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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'code': 'string',
            'description': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='schedCode',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

    def ListServices(self,trackable):
        """
          Returns a set of Rivendell services (list of dictionaries).

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

    def ListSystemSettings(self):
        """
          Returns Rivendell system settings (list of dictionaries).
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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
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
        handler=RivWebPyApi_ListHandler(base_tag='systemSettings',fields=fields)
        xml.sax.parseString(r.text,handler)

        return handler.output()

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'ResponseCode': 'integer',
            'ErrorString': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='RDWebResult',fields=fields)

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()

        #
        # Generate the output dictionary
        #
        fields={
            'ResponseCode': 'integer',
            'ErrorString': 'string'
        }
        handler=RivWebPyApi_ListHandler(base_tag='RDWebResult',fields=fields)

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
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()
