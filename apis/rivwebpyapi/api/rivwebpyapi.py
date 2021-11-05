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

import requests
import xmltodict

class RivWebPyApi(object):
    """
       Create a 'RivWebPyApi' object for accessing the Web API.
    """

    def __init__(self,url,username,passwd):
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
        self.__connection_password=passwd

    def ListServices(self,trackable):
        """
          Returns a list of Rivendell services (Element).

          Takes one argument:

          trackable - Boolean. If True, return will include only services that
                      are configured to support voice tracking. If False,
                      all configured services will be included.
        """
        trackarg='0'
        if(trackable):
            trackarg='1'
        postdata={
            'COMMAND': '21',
            'LOGIN_NAME': self.__connection_username,
            'PASSWORD': self.__connection_password,
            'TRACKABLE': trackarg
        }
        r=requests.post(self.__connection_url,data=postdata)
        if(r.status_code!=requests.codes.ok):
            r.raise_for_status()
        return xmltodict.parse(r.text)
