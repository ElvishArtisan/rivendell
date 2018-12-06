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

class PyPADUpdate(object):
    def __init__(self,pad_data):
        self.__fields=pad_data;

    def __replace(self,wildcard,string):
        stype='now'
        if wildcard[1].isupper():
            stype='next'
        sfields={'a':'artist','b':'label','c':'client','d':'','e':'agency',
                 'f':'','g':'groupName','h':'length','i':'description',
                 'j':'cutNumber','k':'','l':'album','m': 'composer',
                 'n':'cartNumber','o':'outcue','p':'publisher','q': '',
                 'r':'conductor','s':'songId','t':'title','u':'userDefined',
                 'v':'','w':'','x':'','y':'year',
                 'z':''}
        sfield=sfields[wildcard[1].lower()]

        try:
            string=string.replace(wildcard,self.__fields['padUpdate'][stype][sfield])
        except TypeError:
            string=string.replace(wildcard,'')
        except KeyError:
            string=string.replace(wildcard,'')
        return string

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

    def padFields(self,string):
        """
           Takes an argument of a string containing one or more PAD wildcards,
           which it will resolve into the appropriate values. See the
           'Metadata Wildcards' section of the Rivendell Operations Guide
           for a list of recognized wildcards.
        """
        for i in range(65,68):
               string=self.__replace('%'+chr(i),string)
        for i in range(69,91):
               string=self.__replace('%'+chr(i),string)
        for i in range(97,100):
               string=self.__replace('%'+chr(i),string)
        for i in range(101,123):
               string=self.__replace('%'+chr(i),string)
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
                          

class PyPADReceiver(object):
    def __init__(self):
        self.__callback=None

    def __PyPAD_Process(self,pad):
        self.__callback(pad)

    def setCallback(self,cb):
        """
           Set the processing callback.
        """
        self.__callback=cb

    def start(self,hostname):
        """
           Connect to a Rivendell system and begin processing PAD events.
           Once started, a PyPAD object can be interacted with
           only within one of its callback methods.
           Takes the following argument:

           hostname: The hostname or IP address of the Rivendell system.
        """
        sock=socket.socket(socket.AF_INET)
        conn=sock.connect((hostname,34289))
        c=""
        line=""
        msg=""

        while 1<2:
            c=sock.recv(1)
            line+=c
            if c[0]=="\n":
                msg+=line
                if line=="\r\n":
                    self.__PyPAD_Process(PyPADUpdate(json.loads(msg)))
                    msg=""
                line=""


