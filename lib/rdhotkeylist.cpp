// rdhotkeylist.cpp
//
// An Abstract of the rdhotkeylist
//
//   (C) Copyright 2002-2004,2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdhotkeylist.cpp,v 1.2.6.1 2012/12/13 22:33:44 cvs Exp $
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

#include<vector>

#include <stdlib.h>

#include<rdhotkeylist.h>

RDHotKeyList::RDHotKeyList( )
{

    hotkeylist.resize(0);
    BuildKeyList();
}

RDHotKeyList::~RDHotKeyList()
{
    hotkeylist.clear();
}

void RDHotKeyList::BuildKeyList( )
{
    char qtpath[256];
    char keyfile[256];
    char line[256];
    FILE *hkeyfile;

    char enumstring[11] = "enum Key {";
    QString KeyString;
    QString KeyHex;
    QString Filestring;
    keyList CurKey;
    char *p = {0};

    if(getenv("QTDIR")==NULL) {
        return;
    }
    strcpy(qtpath, (const char *) getenv("QTDIR"));
    strcpy(keyfile,qtpath);
    strcat(keyfile,"/include/qnamespace.h");
    hkeyfile = fopen(keyfile,"r");
    if (hkeyfile==NULL) {
        return;
    }
    if (fgets(line,256,hkeyfile) !=NULL) {
        while ((p = strstr(line,enumstring))== NULL ) {
            if (fgets(line,256,hkeyfile) == NULL) {
                break;
            }
        }
        if (p != NULL) {
            while ( ( fgets(line,256,hkeyfile) != NULL) && (!(strstr(line,"}")) ) ) {
                QString buf = cleanStrings(QString().sprintf("%s",line));
                int acomment = buf.find("//");
                int eqsign = buf.find("=");
                if ((eqsign != -1) && (acomment != 0) ) {
                    KeyString = buf.left((eqsign ));
                    KeyString = KeyString.mid(4);   // Remove 'Key_'
                    int comma = buf.find(",");
                    if (comma != -1)  {
                        KeyHex = buf.mid((eqsign + 1),
                                             (comma - eqsign)-1 );
                    }
                    else {
                        int comment = buf.find("//");
                        if (comment != -1) {
                            KeyHex = buf.mid( (eqsign+1),
                                                 (comment - eqsign) );
                        }
                        else {
                            KeyHex = buf.mid( (eqsign+1),
                                                 (buf.length() - eqsign) );
                        }
                    }
                    bool ok;
                    int hotkey_int = KeyHex.toInt(&ok,16);         //Convert to decimal
                    if (ok)  {
                        CurKey.decvalue = hotkey_int;
                        CurKey.stringvalue = KeyString;
                        hotkeylist.push_back(CurKey) ;
                    }
                }
            }
        }
    }
    fclose(hkeyfile);
}

QString RDHotKeyList::cleanStrings( const QString sent)
{
    QString cleanstring;
    for (unsigned i=0 ; i<sent.length(); i++) {
        switch(((const char *)sent)[i]) {
            case '\n':
                break;
            case '\t':
                break;
            case ' ':
                break;
            default:
                cleanstring+=((const char *)sent)[i];
                break;
        }
    }
    return cleanstring;
}

QString RDHotKeyList::GetKeyCode(int number)
{
     for (unsigned i = 0; i < hotkeylist.size(); i++)  {
         if ((hotkeylist.at(i).decvalue) == number){
             return hotkeylist.at(i).stringvalue;
         }
      }
      return (QString(""));
}

