// test_pam.h
//
// Test PAM Authentication Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id:
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

#ifndef TEST_PAM_H
#define TEST_PAM_H

#include <list>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rdsettings.h>
#include <rdcmd_switch.cpp>

#define TEST_PAM_USAGE "[options]\n\nTest a PAM-based authentication service\n\n--service-name=<svc-name>\n     The name of the PAM service to test (as defined in /etc/pam.d/).\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);
};


#endif  // TEST_PAM_H
