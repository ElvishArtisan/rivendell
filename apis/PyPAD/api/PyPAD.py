#!/usr/bin/python

# PyPAD.py
#
# PAD processor for Rivendell
#
#   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

import socket
import json

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
# Default TCP port for connecting to Rivendell's PAD service
#
PAD_TCP_PORT=34289

class Update(object):
    def __init__(self,pad_data):
        self.__fields=pad_data;

    def __escapeXml(self,string):
        string=string.replace("&","&amp;")
        string=string.replace("<","&lt;")
        string=string.replace(">","&gt;")
        string=string.replace("'","&apos;")
        string=string.replace("\"","&quot;")
        return string

    def __escapeWeb(self,string):
        string=string.replace("%","%25")
        string=string.replace(" ","%20")
        string=string.replace("<","%3C")
        string=string.replace(">","%3E")
        string=string.replace("#","%23")
        string=string.replace("\"","%22")
        string=string.replace("{","%7B")
        string=string.replace("}","%7D")
        string=string.replace("|","%7C")
        string=string.replace("\\","%5C")
        string=string.replace("^","%5E")
        string=string.replace("[","%5B")
        string=string.replace("]","%5D")
        string=string.replace("`","%60")
        string=string.replace("\a","%07")
        string=string.replace("\b","%08")
        string=string.replace("\f","%0C")
        string=string.replace("\n","%0A")
        string=string.replace("\r","%0D")
        string=string.replace("\t","%09")
        string=string.replace("\v","%0B")
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

    def __escape(self,string,escaping):
        if(escaping==0):
            return string
        if(escaping==1):
            return self.__escapeXml(string)
        if(escaping==2):
            return self.__escapeWeb(string)
        if(escaping==3):
            return self.__escapeJson(string)
        raise ValueError('invalid escaping value')

    def __replaceWildcard(self,wildcard,sfield,stype,string,escaping):
        try:
            if isinstance(self.__fields['padUpdate'][stype][sfield],unicode):
                string=string.replace('%'+wildcard,self.__escape(self.__fields['padUpdate'][stype][sfield],escaping))
            else:
                string=string.replace('%'+wildcard,str(self.__fields['padUpdate'][stype][sfield]))
        except TypeError:
            string=string.replace('%'+wildcard,'')
        except KeyError:
            string=string.replace('%'+wildcard,'')
        return string

    def __replaceWildcardPair(self,wildcard,sfield,string,escaping):
        string=self.__replaceWildcard(wildcard,sfield,'now',string,escaping);
        string=self.__replaceWildcard(wildcard.upper(),sfield,'next',string,escaping);
        return string;

    def dateTime(self):
        """
           Returns the date-time stamp of the update in RFC-822 format (string).
        """
        return self.__fields['padUpdate']['dateTime']

    def logMachine(self):
        """
           Returns the log machine number to which this update pertains
           (integer).
        """
        return self.__fields['padUpdate']['logMachine']

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
        
    def serviceName(self):
        """
           Returns the name of the service associated with this update (string).
        """
        return self.__fields['padUpdate']['service']['name']

    def serviceDescription(self):
        """
           Returns the description of the service associated with this update
           (string).
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

    def padFields(self,string,escaping):
        """
           Takes two arguments:

           string - A string containing one or more PAD wildcards, which it
                    will resolve into the appropriate values. See the
                    'Metadata Wildcards' section of the Rivendell Operations
                    Guide for a list of recognized wildcards.

           escaping - Character escaping to be applied to the PAD fields.
                      Must be one of the following:

                      PyPAD.ESCAPE_NONE - No escaping
                      PyPAD.ESCAPE_XML - "XML" escaping: Escape reserved
                                          characters as per XML-v1.0
                      PyPAD.ESCAPE_URL - "URL" escaping: Escape reserved
                                          characters as per RFC 2396
                                          Section 2.4
                      PyPAD.ESCAPE_JSON - "JSON" escaping: Escape reserved
                                          characters as per ECMA-404.
        """
        string=self.__replaceWildcardPair('a','artist',string,escaping)
        string=self.__replaceWildcardPair('b','label',string,escaping)
        string=self.__replaceWildcardPair('c','client',string,escaping)
        # DateTime
        #string=self.__replaceWildcardPair('d',sfield,string,escaping)
        string=self.__replaceWildcardPair('e','agency',string,escaping)
        # Unassigned
        #string=self.__replaceWildcardPair('f',sfield,string,escaping) # Unassigned
        string=self.__replaceWildcardPair('g','groupName',string,escaping)
        string=self.__replaceWildcardPair('h','length',string,escaping)
        string=self.__replaceWildcardPair('i','description',string,escaping)
        string=self.__replaceWildcardPair('j','cutNumber',string,escaping)
        #string=self.__replaceWildcardPair('k',sfield,string,escaping) # Start time for rdimport
        string=self.__replaceWildcardPair('l','album',string,escaping)
        string=self.__replaceWildcardPair('m','composer',string,escaping)
        string=self.__replaceWildcardPair('n','cartNumber',string,escaping)
        string=self.__replaceWildcardPair('o','outcue',string,escaping)
        string=self.__replaceWildcardPair('p','publisher',string,escaping)
        #string=self.__replaceWildcardPair('q',sfield,string,escaping) # Start date for rdimport
        string=self.__replaceWildcardPair('r','conductor',string,escaping)
        string=self.__replaceWildcardPair('s','songId',string,escaping)
        string=self.__replaceWildcardPair('t','title',string,escaping)
        string=self.__replaceWildcardPair('u','userDefined',string,escaping)
        #string=self.__replaceWildcardPair('v',sfield,string,escaping) # Length, rounded down
        #string=self.__replaceWildcardPair('w',sfield,string,escaping) # Unassigned
        #string=self.__replaceWildcardPair('x',sfield,string,escaping) # Unassigned
        string=self.__replaceWildcardPair('y','year',string,escaping)
        #string=self.__replaceWildcardPair('z',sfield,string,escaping) # Unassigned
        string=string.replace('\\b','\b')
        string=string.replace('\\f','\f')
        string=string.replace('\\n','\n')
        string=string.replace('\\r','\r')
        string=string.replace('\\t','\t')
        return string

    def hasNowPad(self):
        """
           Indicates if this update include 'Now' playing PAD.
        """
        try:
            return self.__fields['padUpdate']['now']!=None
        except TypeError:
            return False;
                          
    def hasNextPad(self):
        """
           Indicates if this update include 'Next' playing PAD.
        """
        try:
            return self.__fields['padUpdate']['next']!=None
        except TypeError:
            return False;
                          

class Receiver(object):
    def __init__(self):
        self.__callback=None

    def __PyPAD_Process(self,pad):
        self.__callback(pad)

    def setCallback(self,cb):
        """
           Set the processing callback.
        """
        self.__callback=cb

    def start(self,hostname,port):
        """
           Connect to a Rivendell system and begin processing PAD events.
           Once started, a PyPAD object can be interacted with
           only within one of its callback methods.
           Takes the following arguments:

           hostname - The hostname or IP address of the Rivendell system.

           port - The TCP port to connect to. For most cases, just use
                  'PyPAD.PAD_TCP_PORT'.
        """
        sock=socket.socket(socket.AF_INET)
        conn=sock.connect((hostname,port))
        c=""
        line=""
        msg=""

        while 1<2:
            c=sock.recv(1)
            line+=c
            if c[0]=="\n":
                msg+=line
                if line=="\r\n":
                    self.__PyPAD_Process(Update(json.loads(msg)))
                    msg=""
                line=""


