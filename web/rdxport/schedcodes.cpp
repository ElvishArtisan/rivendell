// schedcodes.h
//
// Rivendell web service portal
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <qstringlist.h>

#include "rdcart.h"
#include "rddb.h"
#include "rdschedcode.h"
#include "rdxport.h"

void Xport::ListSchedCodes()
{
  QString sql;

  //
  // Generate Scheduler Code List
  //
  sql=QString("select CODE from SCHED_CODES order by CODE");
  RDSqlQuery q(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<schedCodeList>\n");
  while(q.next()) {
    RDSchedCode schedcode(q.value(0).toString());
    printf("%s",(const char *)schedcode.xml().utf8());
  }
  printf("</schedCodeList>\n");

  Exit(0);
}


void Xport::AssignSchedCode()
{
  int cart_number;
  QString sched_code;
  QStringList codes;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  if(!xport_post->getValue("CODE",&sched_code)) {
    XmlExit("Missing CODE",400);
  }

  if(!RDCart::exists(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("Forbidden",403);
  }

  //
  // Process Request
  //
  RDCart cart(cart_number);
  RDSchedCode code(sched_code);
  if(!code.exists()) {
    XmlExit("No such scheduler code",404);
  }
  codes=cart.schedCodesList();
  for(unsigned i=0;i<codes.size();i++) {
    if(codes[i]==sched_code) {
      XmlExit("OK",200);
    }
  }
  cart.addSchedCode(sched_code);
  XmlExit("OK",200);
}


void Xport::UnassignSchedCode()
{
  int cart_number;
  QString sched_code;
  QStringList codes;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  if(!xport_post->getValue("CODE",&sched_code)) {
    XmlExit("Missing CODE",400);
  }

  if(!RDCart::exists(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("Forbidden",403);
  }

  //
  // Process Request
  //
  RDCart cart(cart_number);
  RDSchedCode code(sched_code);
  if(!code.exists()) {
    XmlExit("No such scheduler code",404);
  }
  cart.removeSchedCode(sched_code);
  XmlExit("OK",200);
}


void Xport::ListCartSchedCodes()
{
  int cart_number;
  QStringList codes;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }

  if(!RDCart::exists(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("Forbidden",403);
  }
  //
  // Generate Scheduler Code List
  //
  RDCart cart(cart_number);
  codes=cart.schedCodesList();

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<schedCodeList>\n");
  for(unsigned i=0;i<codes.size();i++) {
    RDSchedCode schedcode(codes[i]);
    printf("%s",(const char *)schedcode.xml().utf8());
  }
  printf("</schedCodeList>\n");

  Exit(0);
}
