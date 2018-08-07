// schedcodes.h
//
// Rivendell web service portal
//
//   (C) Copyright 2015-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdcart.h>
#include <rddb.h>
#include <rdschedcode.h>

#include "rdxport.h"

void Xport::ListSchedCodes()
{
  QString sql;
  RDSqlQuery *q;
  RDSchedCode *schedcode;

  //
  // Generate Scheduler Code List
  //
  sql=QString("select CODE from SCHED_CODES order by CODE");
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<schedCodeList>\n");
  while(q->next()) {
    schedcode=new RDSchedCode(q->value(0).toString());
    printf("%s",(const char *)schedcode->xml().utf8());
    delete schedcode;
  }
  printf("</schedCodeList>\n");

  delete q;
  Exit(0);
}


void Xport::AssignSchedCode()
{
  int cart_number;
  QString sched_code;
  QStringList codes;
  RDCart *cart=NULL;
  RDSchedCode *code;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400,"schedcodes.cpp",LINE_NUMBER);
  }
  if(!xport_post->getValue("CODE",&sched_code)) {
    XmlExit("Missing CODE",400,"schedcodes.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(!rda->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404,"schedcodes.cpp",LINE_NUMBER);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  code=new RDSchedCode(sched_code);
  if(!code->exists()) {
    XmlExit("No such scheduler code",404,"schedcodes.cpp",LINE_NUMBER);
  }
  codes=cart->schedCodesList();
  for(int i=0;i<codes.size();i++) {
    if(codes[i]==sched_code) {
      delete cart;
      XmlExit("OK",200,"schedcodes.cpp",LINE_NUMBER);
    }
  }
  cart->addSchedCode(sched_code);
  XmlExit("OK",200,"schedcodes.cpp",LINE_NUMBER);
}


void Xport::UnassignSchedCode()
{
  int cart_number;
  QString sched_code;
  QStringList codes;
  RDCart *cart=NULL;
  RDSchedCode *code;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400,"schedcodes.cpp",LINE_NUMBER);
  }
  if(!xport_post->getValue("CODE",&sched_code)) {
    XmlExit("Missing CODE",400,"schedcodes.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(!rda->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404,"schedcodes.cpp",LINE_NUMBER);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  code=new RDSchedCode(sched_code);
  if(!code->exists()) {
    XmlExit("No such scheduler code",404,"schedcodes.cpp",LINE_NUMBER);
  }
  cart->removeSchedCode(sched_code);
  delete cart;
  delete code;
  XmlExit("OK",200,"schedcodes.cpp",LINE_NUMBER);
}


void Xport::ListCartSchedCodes()
{
  int cart_number;
  RDCart *cart;
  QStringList codes;
  RDSchedCode *schedcode;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400,"schedcodes.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(!rda->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404,"schedcodes.cpp",LINE_NUMBER);
  }
  //
  // Generate Scheduler Code List
  //
  cart=new RDCart(cart_number);
  codes=cart->schedCodesList();

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<schedCodeList>\n");
  for(int i=0;i<codes.size();i++) {
    schedcode=new RDSchedCode(codes[i]);
    printf("%s",(const char *)schedcode->xml().utf8());
    delete schedcode;
  }
  printf("</schedCodeList>\n");

  Exit(0);
}
