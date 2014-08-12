// rdpam.h
//
// Authenticate a PAM name.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpam.h,v 1.2 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDPAM_H
#define RDPAM_H

#include <qstring.h>

class RDPam
{
 public:
  RDPam(const QString &pam_service);
  bool authenticate(const QString &username,const QString &token);

 private:
  void CleanupPam();
  QString system_pam_service;
  QString system_token;
  struct pam_response *system_pam_response;
  int system_pam_response_count;
  friend int RDPamCallback(int num_msg, const struct pam_message **msg,
			   struct pam_response **resp, void *appdata_ptr);
};


#endif  // RDPAM_H
