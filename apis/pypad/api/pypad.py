# pypad.py
#
# PAD processor for Rivendell
#
#   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

import os.path
import configparser
import datetime
import MySQLdb
import signal
import selectors
import socket
import sys
import syslog
import json
from urllib.parse import quote

#
# Enumerated Constants (sort of)
#
# Escape types
#
ESCAPE_NONE=0
ESCAPE_XML=1
ESCAPE_URL=2
ESCAPE_JSON=3

#
# PAD Types
#
TYPE_NOW='now'
TYPE_NEXT='next'

#
# Field Names
#
FIELD_START_DATETIME='startDateTime'
FIELD_LINE_NUMBER='lineNumber'
FIELD_LINE_ID='lineId'
FIELD_CART_NUMBER='cartNumber'
FIELD_CART_TYPE='cartType'
FIELD_CUT_NUMBER='cutNumber'
FIELD_LENGTH='length'
FIELD_YEAR='year'
FIELD_GROUP_NAME='groupName'
FIELD_TITLE='title'
FIELD_ARTIST='artist'
FIELD_PUBLISHER='publisher'
FIELD_COMPOSER='composer'
FIELD_ALBUM='album'
FIELD_LABEL='label'
FIELD_CLIENT='client'
FIELD_AGENCY='agency'
FIELD_CONDUCTOR='conductor'
FIELD_USER_DEFINED='userDefined'
FIELD_SONG_ID='songId'
FIELD_OUTCUE='outcue'
FIELD_DESCRIPTION='description'
FIELD_ISRC='isrc'
FIELD_ISCI='isci'
FIELD_EXTERNAL_EVENT_ID='externalEventId'
FIELD_EXTERNAL_DATA='externalData'
FIELD_EXTERNAL_ANNC_TYPE='externalAnncType'

#
# Default TCP port for connecting to Rivendell's PAD service
#
PAD_TCP_PORT=34289

class Update(object):
    def __init__(self,pad_data,config,rd_config):
        self.__fields=pad_data
        self.__config=config
        self.__rd_config=rd_config

    def __fromIso8601(self,string):
        try:
            return datetime.datetime.strptime(string.strip()[:19],'%Y-%m-%dT%H:%M:%S')
        except AttributeError:
            return ''

    def __escapeXml(self,string):
        string=string.replace("&","&amp;")
        string=string.replace("<","&lt;")
        string=string.replace(">","&gt;")
        string=string.replace("'","&apos;")
        string=string.replace("\"","&quot;")
        return string

    def __escapeWeb(self,string):
        string=quote(string)
        return string

    def __escapeJson(self,string):
        string=string.replace("\\","\\\\")
        string=string.replace("\"","\\\"")
        string=string.replace("/","\\/")
        string=string.replace("\b","\\b")
        string=string.replace("\f","\\f")
        string=string.replace("\n","\\n")
        string=string.replace("\r","\\r")
        string=string.replace("\t","\\t")
        return string

    def __replaceWildcard(self,wildcard,sfield,stype,string,esc):
        try:
            if isinstance(self.__fields['padUpdate'][stype][sfield],str):
                string=string.replace('%'+wildcard,self.escape(self.__fields['padUpdate'][stype][sfield],esc))
            else:
                if self.__fields['padUpdate'][stype][sfield] is None:
                    string=string.replace('%'+wildcard,'')
                else :
                    string=string.replace('%'+wildcard,str(self.__fields['padUpdate'][stype][sfield]))
        except TypeError:
            string=string.replace('%'+wildcard,'')
        except KeyError:
            string=string.replace('%'+wildcard,'')
        return string

    def __replaceWildcardPair(self,wildcard,sfield,string,esc):
        string=self.__replaceWildcard(wildcard,sfield,'now',string,esc);
        string=self.__replaceWildcard(wildcard[0].upper()+wildcard[1:],sfield,'next',string,esc);
        return string;

    def __findDatetimePattern(self,pos,wildcard,string):
        start=string.find('%'+wildcard+'(',pos)
        if start>=0:
            end=string.find(")",start+3)
            if end>0:
                return (end+2,string[start:end+1])
        return None

    def __replaceDatetimePattern(self,string,pattern):
        stype='now'
        if pattern[1]=='D':
            stype='next'
        try:
            dt=self.__fromIso8601(self.__fields['padUpdate'][stype]['startDateTime'])
        except TypeError:
            string=string.replace(pattern,'')
            return string

        dt_pattern=pattern[3:-1]

        try:
            #
            # Process Times
            #
            miltime=(dt_pattern.find('ap')<0)and(dt_pattern.find('AP')<0)
            if not miltime:
                if dt.hour<13:
                    dt_pattern=dt_pattern.replace('ap','am')
                    dt_pattern=dt_pattern.replace('AP','AM')
                else:
                    dt_pattern=dt_pattern.replace('ap','pm')
                    dt_pattern=dt_pattern.replace('AP','PM')
            if miltime:
                dt_pattern=dt_pattern.replace('hh',dt.strftime('%H'))
                dt_pattern=dt_pattern.replace('h',str(dt.hour))
            else:
                dt_pattern=dt_pattern.replace('hh',dt.strftime('%I'))
                hour=dt.hour
                if hour==0:
                    hour=12
                dt_pattern=dt_pattern.replace('h',str(hour))

            dt_pattern=dt_pattern.replace('mm',dt.strftime('%M'))
            dt_pattern=dt_pattern.replace('m',str(dt.minute))

            dt_pattern=dt_pattern.replace('ss',dt.strftime('%S'))
            dt_pattern=dt_pattern.replace('s',str(dt.second))

            #
            # Process Dates
            #
            dt_pattern=dt_pattern.replace('MMMM',dt.strftime('%B'))
            dt_pattern=dt_pattern.replace('MMM',dt.strftime('%b'))
            dt_pattern=dt_pattern.replace('MM',dt.strftime('%m'))
            dt_pattern=dt_pattern.replace('M',str(dt.month))

            dt_pattern=dt_pattern.replace('dddd',dt.strftime('%A'))
            dt_pattern=dt_pattern.replace('ddd',dt.strftime('%a'))
            dt_pattern=dt_pattern.replace('dd',dt.strftime('%d'))
            dt_pattern=dt_pattern.replace('d',str(dt.day))

            dt_pattern=dt_pattern.replace('yyyy',dt.strftime('%Y'))
            dt_pattern=dt_pattern.replace('yy',dt.strftime('%y'))

        except AttributeError:
            string=string.replace(pattern,'')
            return string

        string=string.replace(pattern,dt_pattern)
        return string

    def __replaceDatetimePair(self,string,wildcard):
        pos=0
        pattern=(0,'')
        while(pattern!=None):
            pattern=self.__findDatetimePattern(pattern[0],wildcard,string)
            if pattern!=None:
                string=self.__replaceDatetimePattern(string,pattern[1])
        return string

    def config(self):
        """
           If a valid configuration file was set in
           'pypad.Receiver::setConfigFile()', this will return a
           parserconfig object created from it. If no configuration file
           was specified, returns None.
        """
        return self.__config

    def dateTimeString(self):
        """
           Returns the date-time of the update in ISO 8601 format (string).
        """
        return self.__fields['padUpdate']['dateTime']

    def dateTime(self):
        """
           Returns the date-time of the PAD update (datetime)
        """
        return self.__fromIso8601(self.__fields['padUpdate']['dateTime'])

    def escape(self,string,esc):
        """
           Returns an 'escaped' version of the specified string.

           Takes two arguments:

           string - The string to be processed.

           esc - The type of escaping to be applied. The following values
                 are valid:
                 pypad.ESCAPE_JSON - Escape for use in JSON string values
                                     (as per ECMA-404)
                 pypad.ESCAPE_NONE - String is passed through unchanged
                 pypad.ESCAPE_URL - Escape for use in URLs
                                    (as per RFC 2396)
                 pypad.ESCAPE_XML - Escape for use in XML
                                    (as per XML-v1.0)
        """
        if(esc==0):
            return string
        if(esc==1):
            return self.__escapeXml(string)
        if(esc==2):
            return self.__escapeWeb(string)
        if(esc==3):
            return self.__escapeJson(string)
        raise ValueError('invalid esc value')

    def hostName(self):
        """
           Returns the host name of the machine whence this PAD update
           originated (string).
        """
        return self.__fields['padUpdate']['hostName']

    def shortHostName(self):
        """
           Returns the short host name of the machine whence this PAD update
           originated (string).
        """
        return self.__fields['padUpdate']['shortHostName']

    def machine(self):
        """
           Returns the log machine number to which this update pertains
           (integer).
        """
        return self.__fields['padUpdate']['machine']

    def mode(self):
        """
           Returns the operating mode of the host log machine to which
           this update pertains (string).
        """
        return self.__fields['padUpdate']['mode']

    def onairFlag(self):
        """
           Returns the state of the on-air flag (boolean).
        """
        return self.__fields['padUpdate']['onairFlag']

    def hasService(self):
        """
           Indicates if service information is included with this update
           (boolean).
        """
        try:
            return self.__fields['padUpdate']['service']!=None
        except TypeError:
           return False;
        
    def rivendellConfig(self):
        """
           Returns a parserconfig object containing the contents of the
           current rd.conf(5) file.
        """
        return self.__rd_config

    def serviceName(self):
        """
           Returns the name of the service associated with this update (string).
        """
        return self.__fields['padUpdate']['service']['name']

    def serviceDescription(self):
        """
           Returns the description of the service associated with this update
           (string). Not to be confused with the 'description' field for a
           cut!
        """
        return self.__fields['padUpdate']['service']['description']

    def serviceProgramCode(self):
        """
           Returns the Program Code of the service associated with this update
           (string).
        """
        return self.__fields['padUpdate']['service']['programCode']

    def hasLog(self):
        """
           Indicates if log information is included with this update
           (boolean).
        """
        try:
            return self.__fields['padUpdate']['log']!=None
        except TypeError:
            return False;
        
    def logName(self):
        """
           Returns the name of the log associated with this update (string).
        """
        return self.__fields['padUpdate']['log']['name']

    def resolvePadFields(self,string,esc):
        """
           Takes two arguments:

           string - A string containing one or more PAD wildcards, which it
                    will resolve into the appropriate values. See the
                    'Metadata Wildcards' section of the Rivendell Operations
                    Guide for a list of recognized wildcards.

           esc - Character escaping to be applied to the PAD fields.
                 See the documentation for the 'escape()' method for valid
                 field values.
        """
        #
        # MAINTAINER'S NOTE: These mappings must be kept in sync with
        #                    those of the 'RDLogLine::resolveWildcards()'
        #                    method in 'lib/rdlog_line.cpp' as well as
        #                    the 'RunPattern()' and 'VerifyPattern()' methods
        #                    in 'utils/rdimport/rdimport.cpp'.
        #
        string=self.__replaceWildcardPair('a','artist',string,esc)
        string=self.__replaceWildcardPair('b','label',string,esc)
        string=self.__replaceWildcardPair('c','client',string,esc)
        string=self.__replaceDatetimePair(string,'d') # %d(<dt>) Handler
        string=self.__replaceDatetimePair(string,'D') # %D(<dt>) Handler
        string=self.__replaceWildcardPair('e','agency',string,esc)
        #string=self.__replaceWildcardPair('f',sfield,string,esc) # Unassigned
        string=self.__replaceWildcardPair('g','groupName',string,esc)
        string=self.__replaceWildcardPair('h','length',string,esc)
        string=self.__replaceWildcardPair('i','description',string,esc)
        string=self.__replaceWildcardPair('j','cutNumber',string,esc)
        # %k - Assigned for use for the Start Time for rdimport(1)
        string=self.__replaceWildcardPair('l','album',string,esc)
        string=self.__replaceWildcardPair('m','composer',string,esc)
        string=self.__replaceWildcardPair('n','cartNumber',string,esc)
        string=self.__replaceWildcardPair('o','outcue',string,esc)
        string=self.__replaceWildcardPair('p','publisher',string,esc)
        # %q - Assigned for use for the Start Date for rdimport(1)
        string=self.__replaceWildcardPair('r','conductor',string,esc)
        string=self.__replaceWildcardPair('s','songId',string,esc)
        string=self.__replaceWildcardPair('t','title',string,esc)
        string=self.__replaceWildcardPair('u','userDefined',string,esc)
        secs=self.__replaceWildcard('v','length','now','%v',ESCAPE_NONE) # Length, rounded down
        if(secs==''):
            string=string.replace('%v','0')
        else:
            string=string.replace('%v',str(int(secs)//1000))
        secs=self.__replaceWildcard('V','length','next','%V',ESCAPE_NONE)
        if(secs==''):
            string=string.replace('%V','0')
        else:
            string=string.replace('%V',str(int(secs)//1000))
        string=self.__replaceWildcardPair('wc','isci',string,esc)
        string=self.__replaceWildcardPair('wi','isrc',string,esc)
        string=self.__replaceWildcardPair('wm','recordingMbId',string,esc)
        string=self.__replaceWildcardPair('wr','releaseMbId',string,esc)
        string=self.__replaceWildcardPair('x','lineId',string,esc) # Log Line ID
        string=self.__replaceWildcardPair('y','year',string,esc)
        string=self.__replaceWildcardPair('z','lineNumber',string,esc) # Log Line #
        string=string.replace('\\b','\b')
        string=string.replace('\\f','\f')
        string=string.replace('\\n','\n')
        string=string.replace('\\r','\r')
        string=string.replace('\\t','\t')
        return string

    def hasPadType(self,pad_type):
        """
           Indicates if this update includes the specified PAD type

           Takes one argument:

           pad_type - The type of PAD value. Valid values are:
                      pypad.TYPE_NOW - Now playing data
                      pypad.TYPE_NEXT - Next to play data
        """
        try:
            return self.__fields['padUpdate'][pad_type]!=None
        except TypeError:
            return False;

    def startDateTimeString(self,pad_type):
        """
           Returns the start date-time of the specified PAD type in ISO 8601
           format (string).

           Takes one argument:

           pad_type - The type of PAD value. Valid values are:
                      pypad.TYPE_NOW - Now playing data
                      pypad.TYPE_NEXT - Next to play data
        """
        return self.__fields['padUpdate'][pad_type]['startDateTime']

    def startDateTime(self,pad_type):
        """
           Returns the start datetime of the specified PAD type

           Takes one argument:

           pad_type - The type of PAD value. Valid values are:
                      pypad.TYPE_NOW - Now playing data
                      pypad.TYPE_NEXT - Next to play data
        """
        try:
            return self.__fromIso8601(self.__fields['padUpdate'][pad_type]['startDateTime'])
        except AttributeError:
            return None

    def padField(self,pad_type,pad_field):
        """
           Returns the raw value of the specified PAD field.

           Takes two arguments:

           pad_type - The type of PAD value. Valid values are:
                      pypad.TYPE_NOW - Now playing data
                      pypad.TYPE_NEXT - Next to play data

           pad_field - The specific field. Valid values are:
                       pypad.FIELD_AGENCY - The 'Agency' field (string)
                       pypad.FIELD_ALBUM - The 'Album' field (string)
                       pypad.FIELD_ARTIST - The 'Artist' field (string)
                       pypad.FIELD_CART_NUMBER - The 'Cart Number' field
                                                 (integer)
                       pypad.FIELD_CART_TYPE - 'The 'Cart Type' field (string)
                       pypad.FIELD_CLIENT - The 'Client' field (string)
                       pypad.FIELD_COMPOSER - The 'Composer' field (string)
                       pypad.FIELD_CONDUCTOR - The 'Conductor' field (string)
                       pypad.FIELD_CUT_NUMER - The 'Cut Number' field (integer)
                       pypad.FIELD_DESCRIPTION - The cut 'Description' field
                                                 (string). Not to be confused
                                                 with the service description!
                       pypad.FIELD_EXTERNAL_ANNC_TYPE - The 'EXT_ANNC_TYPE'
                                                        field (string)
                       pypad.FIELD_EXTERNAL_DATA - The 'EXT_DATA' field
                                                   (string)
                       pypad.FIELD_EXTERNAL_EVENT_ID - The 'EXT_EVENT_ID'
                                                       field (string)
                       pypad.FIELD_GROUP_NAME - The 'GROUP_NAME' field (string)
                       pypad.FIELD_ISRC - The 'ISRC' field (string)
                       pypad.FIELD_ISCI - The 'ISCI' field (string)
                       pypad.FIELD_LABEL - The 'Label' field (string)
                       pypad.FIELD_LENGTH - The 'Length' field (integer)
                       pypad.FIELD_LINE_ID - The log line ID (integer)
                       pypad.FIELD_LINE_NUMBER - The log line number (integer)
                       pypad.FIELD_OUTCUE - The 'Outcue' field (string)
                       pypad.FIELD_PUBLISHER - The 'Publisher' field (string)
                       pypad.FIELD_SONG_ID - The 'Song ID' field (string)
                       pypad.FIELD_START_DATETIME - The 'Start DateTime field
                                                    (string)
                       pypad.FIELD_TITLE - The 'Title' field (string)
                       pypad.FIELD_USER_DEFINED - 'The 'User Defined' field
                                                   (string)
                       pypad.FIELD_YEAR - The 'Year' field (integer)
        """
        return self.__fields['padUpdate'][pad_type][pad_field]

    def resolveFilepath(self,string,dt):
        """
           Returns a string with any Rivendell Filepath wildcards resolved
           (See Appdendix C of the Rivendell Operations Guide for a list).

           Takes two arguments:
 
           string - The string to resolve.

           dt - A Python 'datetime' object to use for the resolution.
        """
        ret=''
        upper_case=False
        initial_case=False
        offset=0
        i=0

        while i<len(string):
            field=''
            offset=0;
            if string[i]!='%':
                ret+=string[i]
            else:
                i=i+1
                offset=offset+1
                if string[i]=='^':
                    upper_case=True
                    i=i+1
                    offset=offset+1
                else:
                    upper_case=False

                if string[i]=='$':
                    initial_case=True
                    i=i+1
                    offset=offset+1
                else:
                    initial_case=False

                found=False
                if string[i]=='a':   # Abbreviated weekday name
                    field=dt.strftime('%a').lower()
                    found=True

                if string[i]=='A':   # Full weekday name
                    field=dt.strftime('%A').lower()
                    found=True

                if (string[i]=='b') or (string[i]=='h'): # Abrev. month Name
                    field=dt.strftime('%b').lower()
                    found=True

                if string[i]=='B':  # Full month name
                    field=dt.strftime('%B').lower()
                    found=True

                if string[i]=='C':  # Century
                    field=dt.strftime('%C').lower()
                    found=True

                if string[i]=='d':  # Day (01 - 31)
                    field='%02d' % dt.day
                    found=True

                if string[i]=='D':  # Date (mm-dd-yy)
                    field=dt.strftime('%m-%d-%y')
                    found=True

                if string[i]=='e':  # Day, padded ( 1 - 31)
                    field='%2d' % dt.day
                    found=True

                if string[i]=='E':  # Day, unpadded (1 - 31)
                    field='%d' % dt.day
                    found=True

                if string[i]=='F':  # Date (yyyy-mm-dd)
                    field=dt.strftime('%F')
                    found=True
                    
                if string[i]=='g':  # Two digit year number (as per ISO 8601)
                    field=dt.strftime('%g').lower()
                    found=True

                if string[i]=='G':  # Four digit year number (as per ISO 8601)
                    field=dt.strftime('%G').lower()
                    found=True

                if string[i]=='H':  # Hour, zero padded, 24 hour
                    field=dt.strftime('%H').lower()
                    found=True

                if string[i]=='I':  # Hour, zero padded, 12 hour
                    field=dt.strftime('%I').lower()
                    found=True

                if string[i]=='i':  # Hour, space padded, 12 hour
                    hour=dt.hour
                    if hour>12:
                        hour=hour-12
                    if hour==0:
                        hour=12
                    field='%2d' % hour
                    found=True

                if string[i]=='J':  # Hour, unpadded, 12 hour
                    hour=dt.hour
                    if hour>12:
                        hour=hour-12
                    if hour==0:
                        hour=12
                    field=str(hour)
                    found=True

                if string[i]=='j':  # Day of year
                    field=dt.strftime('%j')
                    found=True
                
                if string[i]=='k':  # Hour, space padded, 24 hour
                    field=dt.strftime('%k')
                    found=True

                if string[i]=='M':  # Minute, zero padded
                    field=dt.strftime('%M')
                    found=True

                if string[i]=='m':  # Month (01 - 12)
                    field=dt.strftime('%m')
                    found=True

                if string[i]=='p':  # AM/PM string
                    field=dt.strftime('%p')
                    found=True

                if string[i]=='r':  # Rivendell host name
                    field=self.hostName()
                    found=True

                if string[i]=='R':  # Rivendell short host name
                    field=self.shortHostName()
                    found=True

                if string[i]=='S':  # Second (SS)
                    field=dt.strftime('%S')
                    found=True

                if string[i]=='s':  # Rivendell service name
                    if self.hasService():
                        field=self.serviceName()
                    else:
                        field=''
                    found=True

                if string[i]=='u':  # Day of week (numeric, 1..7, 1=Monday)
                    field=dt.strftime('%u')
                    found=True

                if (string[i]=='V') or (string[i]=='W'): # Week # (as per ISO 8601)
                    field=dt.strftime('%V')
                    found=True
    
                if string[i]=='w':  # Day of week (numeric, 0..6, 0=Sunday)
                    field=dt.strftime('%w')
                    found=True

                if string[i]=='y':  # Year (yy)
                    field=dt.strftime('%y')
                    found=True

                if string[i]=='Y':  # Year (yyyy)
                    field=dt.strftime('%Y')
                    found=True

                if string[i]=='%':
                    field='%'
                    found=True

                if not found:  # No recognized wildcard, rollback!
                    i=-offset
                    field=string[i]

            if upper_case:
                field=field.upper();
            if initial_case:
                field=field[0].upper()+field[1::]
            ret+=field
            upper_case=False
            initial_case=False
            i=i+1

        return ret

    def shouldBeProcessed(self,section):
        """
           Reads the Log Selection and SendNullUpdate parameters of the
           config and returns a boolean to indicate whether or not this
           update should be processed (boolean).

           Takes one argument:

           section - The '[<section>]' of the INI configuration from which
                     to take the parameters.
        """
        result=True
        if self.__config.has_section(section):
            if self.__config.has_option(section,'ProcessNullUpdates'):
                if self.__config.get(section,'ProcessNullUpdates')=='0':
                    result=result and True
                if self.__config.get(section,'ProcessNullUpdates')=='1':
                    result=result and self.hasPadType(pypad.TYPE_NOW)
                if self.__config.get(section,'ProcessNullUpdates')=='2':
                    result=result and self.hasPadType(pypad.TYPE_NEXT)
                if self.__config.get(section,'ProcessNullUpdates')=='3':
                    result=result and self.hasPadType(pypad.TYPE_NOW) and self.hasPadType(pypad.TYPE_NEXT)
            else:
                result=result and True

            log_dict={1: 'MasterLog',2: 'Aux1Log',3: 'Aux2Log',
                      101: 'VLog101',102: 'VLog102',103: 'VLog103',104: 'VLog104',
                      105: 'VLog105',106: 'VLog106',107: 'VLog107',108: 'VLog108',
                      109: 'VLog109',110: 'VLog110',111: 'VLog111',112: 'VLog112',
                      113: 'VLog113',114: 'VLog114',115: 'VLog115',116: 'VLog116',
                      117: 'VLog117',118: 'VLog118',119: 'VLog119',120: 'VLog120'}
            option=log_dict[self.machine()]
            if self.__config.has_option(section,option):
                if self.__config.get(section,option).lower()=='yes':
                    result=result and True
                else:
                    if self.__config.get(section,option).lower()=='no':
                        result=result and False
                    else:
                        if self.__config.get(section,option).lower()=='onair':
                            result=result and self.onairFlag()
                        else:
                            result=result and False
            else:
                result=result and False
        else:
            result=result and False
        #print('machine(): '+str(self.machine()))
        #print('result: '+str(result))
        return result


    def syslog(self,priority,msg):
        """
           Send a message to the syslog.
        """
        if((priority&248)==0):
            priority=priority|(int(self.__rd_config.get('Identity','SyslogFacility',fallback=syslog.LOG_USER))<<3)
        syslog.syslog(priority,msg)

        

class Receiver(object):
    def __init__(self):
        self.__pad_callback=None
        self.__timer_callback=None
        self.__timer_interval=None
        self.__config_parser=None

    def __pypad_Process(self,pad):
        self.__pad_callback(pad)

    def __pypad_TimerProcess(self,config):
        self.__timer_callback(config)

    def __getDbCredentials(self):
        config=configparser.ConfigParser()
        config.readfp(open('/etc/rd.conf'))
        return (config.get('mySQL','Loginname'),config.get('mySQL','Password'),
                config.get('mySQL','Hostname'),config.get('mySQL','Database'))

    def __openDb(self):
        creds=self.__getDbCredentials()
        try:
            return MySQLdb.connect(user=creds[0],passwd=creds[1],
                                   host=creds[2],database=creds[3],
                                   charset='utf8mb4')
        except TypeError:
            return MySQLdb.connect(user=creds[0],password=creds[1],
                                   host=creds[2],database=creds[3],
                                   charset='utf8mb4')

    def setPadCallback(self,callback):
        """
           Set the processing callback.
        """
        self.__pad_callback=callback

    def setTimerCallback(self,interval,callback):
        """
           Set the timer callback.

           Takes two arguments:

           interval - The interval (in seconds) between callback invocations.

           callback - The function to call when the timer interval expires. The
                      function should take one argument, which will be a
                      configparser object if setConfigFile() was called prior
                      to start(), otherwise None.
        """
        self.__timer_interval=interval
        self.__timer_callback=callback

    def setConfigFile(self,filename):
        """
           Set a file whence to get configuration information. If set,
           the 'pypad.Update::config()' method will return a parserconfig
           object created from the specified file. The file must be in INI
           format.

           A special case is if the supplied filename string begins with
           the '$' character. If so, the remainder of the string is assumed
           to be an unsigned integer ID that is used to retrieve the
           configuration from the 'PYPAD_INSTANCES' table in the database
           pointed to by '/etc/rd.conf'.

           Returns a configparser object created from the specified
           configuration.
        """
        if filename[0]=='$':  # Get the config from the DB
            db=self.__openDb()
            cursor=db.cursor()
            cursor.execute('select CONFIG from PYPAD_INSTANCES where ID='+
                           filename[1::])
            config=cursor.fetchone()
            self.__config_parser=configparser.ConfigParser(interpolation=None)
            self.__config_parser.read_string(config[0])
            db.close()

        else:   # Get the config from a file
            fp=open(filename)
            self.__config_parser=configparser.ConfigParser(interpolation=None)
            self.__config_parser.read_file(fp)
            fp.close()

        return self.__config_parser


    def start(self,hostname,port):
        """
           Connect to a Rivendell system and begin processing PAD events.
           Once started, a pypad object can be interacted with
           only within its callback method.

           Takes the following arguments:

           hostname - The hostname or IP address of the Rivendell system.

           port - The TCP port to connect to. For most cases, just use
                  'pypad.PAD_TCP_PORT'.
        """
        # So we exit cleanly when shutdown by rdpadengined(8)
        signal.signal(signal.SIGTERM,SigHandler)

        # Open rd.conf(5)
        rd_config=configparser.ConfigParser(interpolation=None)
        rd_config.readfp(open('/etc/rd.conf'))

        # Open the syslog
        pypad_name=sys.argv[0].split('/')[-1]
        syslog.openlog(pypad_name,logoption=syslog.LOG_PID,facility=int(rd_config.get('Identity','SyslogFacility',fallback=syslog.LOG_USER)))

        # Connect to the PAD feed
        sock=socket.socket(socket.AF_INET)
        conn=sock.connect((hostname,port))
        timeout=None
        if self.__timer_interval!=None:
            timeout=self.__timer_interval
            deadline=datetime.datetime.now()+datetime.timedelta(seconds=timeout)
        sel=selectors.DefaultSelector()
        sel.register(sock,selectors.EVENT_READ)
        c=bytes()
        line=bytes()
        msg=""

        while 1<2:
            if len(sel.select(timeout))==0:
                now=datetime.datetime.now()
                if now>=deadline:
                    timeout=self.__timer_interval
                    deadline=now+datetime.timedelta(seconds=timeout)
                    self.__pypad_TimerProcess(self.__config_parser)
                else:
                    timeout=(deadline-now).total_seconds()
            else:
                c=sock.recv(1)
                line+=c
                if c[0]==10:
                    linebytes=line.decode('utf-8','replace')
                    msg+=linebytes
                    if linebytes=='\r\n':
                        self.__pypad_Process(Update(json.loads(msg),self.__config_parser,rd_config))
                        msg=""
                    line=bytes()
                if self.__timer_interval!=None:
                    timeout=(deadline-datetime.datetime.now()).total_seconds()


def SigHandler(signo,stack):
    sys.exit(0)
