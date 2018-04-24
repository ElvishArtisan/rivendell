// rdhotkeylist.h
//
// Abstract a Rivendell HotKey List from QT Key Library entries
//
//  (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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
//   written by Todd Baker   bakert@rfa.org

#ifndef RDHOTKEYLIST_H
#define RDHOTKEYLIST_H

#include <vector>
#include <qstring.h>
#include <qfile.h>

struct keyList   {
    int decvalue ;
    QString stringvalue;
};

class RDHotKeyList
{
    
public:
    RDHotKeyList( );
   ~RDHotKeyList( ) ;
    void BuildKeyList( );
    QString GetKeyCode(int);
private:
    std::vector<keyList> hotkeylist; 
    QString cleanStrings(const QString cleanstring);
};

#endif

