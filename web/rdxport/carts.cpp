// carts.cpp
//
// Rivendell web service portal -- Cart services
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: carts.cpp,v 1.8.2.2.2.1 2014/03/19 22:13:01 cvs Exp $
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rduser.h>
#include <rdgroup.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdcart_search_text.h>

#include <rdxport.h>

void Xport::AddCart()
{
  RDCart *cart;
  RDGroup *group;
  QString group_name;
  QString type;
  RDCart::Type cart_type=RDCart::All;
  int cart_number=0;

  //
  // Verify Post
  //
  if(!xport_post->getValue("GROUP_NAME",&group_name)) {
    XmlExit("Missing GROUP_NAME",400);
  }
  if(!xport_post->getValue("TYPE",&type)) {
    XmlExit("Missing TYPE",400);
  }
  if(type.lower()=="audio") {
    cart_type=RDCart::Audio;
  }
  else {
    if(type.lower()=="macro") {
      cart_type=RDCart::Macro;
    }
    else {
      XmlExit("Invalid TYPE",400);
    }
  }
  xport_post->getValue("CART_NUMBER",&cart_number);

  //
  // Verify User Perms
  //
  if(!xport_user->groupAuthorized(group_name)) {
    XmlExit("No such group",404);
  }
  group=new RDGroup(group_name,this);
  if(cart_number==0) {
    if((cart_number=group->nextFreeCart())==0) {
      delete group;
      XmlExit("No free carts in group",500);
    }
  }
  if(!group->cartNumberValid(cart_number)) {
    delete group;
    XmlExit("Cart number out of range for group",401);
  }
  delete group;
  if(!xport_user->createCarts()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  if(cart->exists()) {
    delete cart;
    XmlExit("Cart already exists",403);
  }
  if(!cart->create(group_name,cart_type)) {
    delete cart;
    XmlExit("Unable to create cart",500);
  }
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cartAdd>\n");
  if(cart->exists()) {
    printf("%s",(const char *)cart->xml(false));
  }
  delete cart;
  printf("</cartAdd>\n");

  Exit(0);
}


void Xport::ListCarts()
{
  QString sql;
  RDSqlQuery *q;
  QString where="";
  RDCart *cart;
  QString group_name;
  QString filter;
  int include_cuts;
  RDCart::Type cart_type=RDCart::All;
  QString type;

  //
  // Verify Post
  //
  xport_post->getValue("GROUP_NAME",&group_name);
  xport_post->getValue("FILTER",&filter);
  xport_post->getValue("INCLUDE_CUTS",&include_cuts);
  xport_post->getValue("TYPE",&type);
  if(type.lower()=="audio") {
    cart_type=RDCart::Audio;
  }
  if(type.lower()=="macro") {
    cart_type=RDCart::Macro;
  }

  //
  // Generate Cart List
  //
  if(group_name.isEmpty()||(group_name==tr("ALL"))) {
    where=RDAllCartSearchText(filter,"ALL",xport_user->name(),false);
  }
  else {
    sql=QString().
      sprintf("select GROUP_NAME from USER_PERMS \
               where (GROUP_NAME=\"%s\")&&(USER_NAME=\"%s\")",
	      (const char *)RDEscapeString(group_name),
	      (const char *)RDEscapeString(xport_user->name()));
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      delete q;
      XmlExit("No such group",404);
    }
    where=RDCartSearchText(filter,group_name,"",false);
  }
  if(cart_type!=RDCart::All) {
    where+=QString().sprintf("&&(TYPE=%u)",cart_type);
  }
  sql="select NUMBER from CART where "+where+"order by NUMBER";
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cartList>\n");
  while(q->next()) {
    cart=new RDCart(q->value(0).toUInt());
    printf("%s",(const char *)cart->xml(include_cuts));
    delete cart;
  }
  printf("</cartList>\n");
  
  delete q;
  Exit(0);
}


void Xport::ListCart()

{
  QString where="";
  RDCart *cart;
  int cart_number;
  int include_cuts;
  QString value;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  xport_post->getValue("INCLUDE_CUTS",&include_cuts);

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cartList>\n");
  cart=new RDCart(cart_number);
  printf("%s",(const char *)cart->xml(include_cuts));
  delete cart;
  printf("</cartList>\n");

  Exit(0);
}


void Xport::EditCart()
{
  QString where="";
  RDCart *cart;
  int cart_number;
  int include_cuts=0;
  QString group_name;
  QString value;
  int number;
  bool ok;
  int line;
  QString macro;
  bool length_changed=false;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  xport_post->getValue("INCLUDE_CUTS",&include_cuts);

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->modifyCarts()) {
    XmlExit("Unauthorized",401);
  }
  if(xport_post->getValue("GROUP_NAME",&group_name)) {
    if(!xport_user->groupAuthorized(group_name)) {
      XmlExit("No such group",404);
    }
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  if(!cart->exists()) {
    delete cart;
    XmlExit("No such cart",404);
  }
  if(xport_post->getValue("FORCED_LENGTH",&value)) {
    number=RDSetTimeLength(value);
    if(cart->type()==RDCart::Macro) {
      delete cart;
      XmlExit("Unsupported operation for cart type",403);
    }
    if(!cart->validateLengths(number)) {
      delete cart;
      XmlExit("Forced length out of range",403);
    }
  }
  switch(cart->type()) {
  case RDCart::Audio:
    break;

  case RDCart::Macro:
    line=0;
    while(xport_post->getValue(QString().sprintf("MACRO%d",line++),&value)) {
      value.stripWhiteSpace();
      if(value.right(1)!="!") {
	delete cart;
	XmlExit("Invalid macro data",400);
      }
      macro+=value;
    }
    cart->setMacros(macro);
    break;

  case RDCart::All:
    break;
  }
  if(!group_name.isEmpty()) {
    cart->setGroupName(group_name);
  }
  if(xport_post->getValue("TITLE",&value)) {
    cart->setTitle(value);
  }
  if(xport_post->getValue("ARTIST",&value)) {
    cart->setArtist(value);
  }
  if(xport_post->getValue("ALBUM",&value)) {
    cart->setAlbum(value);
  }
  if(xport_post->getValue("YEAR",&value)) {
    number=value.toInt(&ok);
    if((ok)&&(number>0)) {
      cart->setYear(number);
    }
  }
  if(xport_post->getValue("LABEL",&value)) {
    cart->setLabel(value);
  }
  if(xport_post->getValue("CLIENT",&value)) {
    cart->setClient(value);
  }
  if(xport_post->getValue("AGENCY",&value)) {
    cart->setAgency(value);
  }
  if(xport_post->getValue("PUBLISHER",&value)) {
    cart->setPublisher(value);
  }
  if(xport_post->getValue("COMPOSER",&value)) {
    cart->setComposer(value);
  }
  if(xport_post->getValue("USER_DEFINED",&value)) {
    cart->setUserDefined(value);
  }
  if(xport_post->getValue("USAGE_CODE",&value)) {
    number=value.toInt(&ok);
    if((ok)&&(number>0)) {
      cart->setUsageCode((RDCart::UsageCode)number);
    }
  }
  if(xport_post->getValue("ENFORCE_LENGTH",&value)) {
    number=value.toInt(&ok);
    if((ok)&&(number>=0)&&(number<2)) {
      cart->setEnforceLength(number);
      length_changed=true;
    }
  }
  if(xport_post->getValue("FORCED_LENGTH",&value)) {
    cart->setForcedLength(RDSetTimeLength(value));
    length_changed=true;
  }
  if(xport_post->getValue("ASYNCRONOUS",&value)) { 
    number=value.toInt(&ok);
    if((ok)&&(number>=0)&&(number<2)) {
      cart->setAsyncronous(number);
      length_changed=true;
    }
  }
  if(xport_post->getValue("OWNER",&value)) {
    cart->setOwner(value);
  }
  if(xport_post->getValue("NOTES",&value)) {
    cart->setNotes(value);
  }
  if(xport_post->getValue("SCHED_CODES",&value)) {
    cart->setSchedCodes(value);
  }
  if(length_changed) {
    cart->updateLength();
  }

  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cartList>\n");
  printf("%s",(const char *)cart->xml(include_cuts));
  delete cart;
  printf("</cartList>\n");

  Exit(0);
}


void Xport::RemoveCart()
{
  RDCart *cart;
  int cart_number;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->deleteCarts()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  if(!cart->exists()) {
    delete cart;
    XmlExit("No such cart",404);
  }
  if(!cart->remove(NULL,NULL,xport_config)) {
    delete cart;
    XmlExit("Unable to delete cart",500);
  }
  delete cart;
  XmlExit("OK",200);
}


void Xport::AddCut()
{
  RDCart *cart;
  RDCut *cut;
  int cart_number;
  int cut_number;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->editAudio()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  if(!cart->exists()) {
    delete cart;
    XmlExit("No such cart",404);
  }
  if((cut_number=cart->addCut(0,0,2))<0) {
    delete cart;
    XmlExit("No new cuts available",500);
  }
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cutAdd>\n");
  cut=new RDCut(cart_number,cut_number);
  if(cut->exists()) {
    printf("%s",(const char *)cut->xml());
  }
  delete cut;
  delete cart;
  printf("</cutAdd>\n");

  Exit(0);
}


void Xport::ListCuts()
{
  RDCut *cut;
  int cart_number;
  QString sql;
  RDSqlQuery *q;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Process Request
  //
  sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u \
                         order by CUT_NAME",
			cart_number);
  q=new RDSqlQuery(sql);
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cutList>\n");
  while(q->next()) {
    cut=new RDCut(q->value(0).toString());
    if(cut->exists()) {
      printf("%s",(const char *)cut->xml());
    }
    delete cut;
  }
  delete q;
  printf("</cutList>\n");

  Exit(0);
}


void Xport::ListCut()
{
  RDCut *cut;
  int cart_number;
  int cut_number;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  if(!xport_post->getValue("CUT_NUMBER",&cut_number)) {
    XmlExit("Missing CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Process Request
  //
  cut=new RDCut(cart_number,cut_number);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404);
  }
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cutList>\n");
  printf("%s",(const char *)cut->xml());
  printf("</cutList>\n");
  delete cut;

  Exit(0);
}


void Xport::EditCut()
{
  RDCut *cut;
  int cart_number;
  int cut_number;
  QString str;
  int num;
  QDateTime datetime;
  QTime time;
  bool rotation_changed=false;
  bool length_changed=false;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  if(!xport_post->getValue("CUT_NUMBER",&cut_number)) {
    XmlExit("Missing CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->editAudio()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Process Request
  //
  cut=new RDCut(cart_number,cut_number);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404);
  }
  if(xport_post->getValue("EVERGREEN",&num)) {
    cut->setEvergreen(num);
    rotation_changed=true;
  }
  if(xport_post->getValue("DESCRIPTION",&str)) {
    cut->setDescription(str);
  }
  if(xport_post->getValue("OUTCUE",&str)) {
    cut->setOutcue(str);
  }
  if(xport_post->getValue("ISRC",&str)) {
    cut->setIsrc(str);
  }
  if(xport_post->getValue("ISCI",&str)) {
    cut->setIsci(str);
  }
  if(xport_post->getValue("START_DATETIME",&datetime)) {
    cut->setStartDatetime(datetime,!datetime.isNull());
    length_changed=true;
    rotation_changed=true;
  }
  if(xport_post->getValue("END_DATETIME",&datetime)) {
    cut->setEndDatetime(datetime,!datetime.isNull());
    length_changed=true;
    rotation_changed=true;
  }
  if(xport_post->getValue("MON",&num)) {
    cut->setWeekPart(1,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("TUE",&num)) {
    cut->setWeekPart(2,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("WED",&num)) {
    cut->setWeekPart(3,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("THU",&num)) {
    cut->setWeekPart(4,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("FRI",&num)) {
    cut->setWeekPart(5,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("SAT",&num)) {
    cut->setWeekPart(6,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("SUN",&num)) {
    cut->setWeekPart(7,num);
    rotation_changed=true;
  }
  if(xport_post->getValue("START_DAYPART",&time)) {
    cut->setStartDaypart(time,!time.isNull());
    rotation_changed=true;
  }
  if(xport_post->getValue("END_DAYPART",&time)) {
    cut->setEndDaypart(time,!time.isNull());
    rotation_changed=true;
  }
  if(xport_post->getValue("WEIGHT",&num)) {
    cut->setWeight(num);
    rotation_changed=true;
  }
  if(xport_post->getValue("START_POINT",&num)) {
    cut->setStartPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("END_POINT",&num)) {
    cut->setEndPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("FADEUP_POINT",&num)) {
    cut->setFadeupPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("FADEDOWN_POINT",&num)) {
    cut->setFadedownPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("SEGUE_START_POINT",&num)) {
    cut->setSegueStartPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("SEGUE_END_POINT",&num)) {
    cut->setSegueEndPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("HOOK_START_POINT",&num)) {
    cut->setHookStartPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("HOOK_END_POINT",&num)) {
    cut->setHookEndPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("TALK_START_POINT",&num)) {
    cut->setTalkStartPoint(num);
    length_changed=true;
  }
  if(xport_post->getValue("TALK_END_POINT",&num)) {
    cut->setTalkEndPoint(num);
    length_changed=true;
  }
  if(length_changed||rotation_changed) {
    RDCart *cart=new RDCart(cut->cartNumber());
    if(length_changed) {
      cart->updateLength();
    }
    if(rotation_changed) {
      cart->resetRotation();
    }
    delete cart;
  }
  delete cut;
  XmlExit("OK",200);
}


void Xport::RemoveCut()
{
  RDCart *cart;
  int cart_number;
  int cut_number;

  //
  // Verify Post
  //
  if(!xport_post->getValue("CART_NUMBER",&cart_number)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  if(!xport_post->getValue("CUT_NUMBER",&cut_number)) {
    XmlExit("Missing CUT_NUMBER",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->editAudio()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Process Request
  //
  cart=new RDCart(cart_number);
  if(!cart->exists()) {
    delete cart;
    XmlExit("No such cart",404);
  }
  if(!cart->removeCut(NULL,NULL,RDCut::cutName(cart_number,cut_number),
		      xport_config)) {
    delete cart;
    XmlExit("No such cut",404);
  }
  delete cart;
  XmlExit("OK",200);
}
