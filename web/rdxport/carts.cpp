// carts.cpp
//
// Rivendell web service portal -- Cart services
//
//   (C) Copyright 2010-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdcgiapplication.h>
#include <rdformpost.h>
#include <rdweb.h>
#include <rdgroup.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdcart_search_text.h>

#include "rdxport.h"

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
  if(!rdcgi->user()->groupAuthorized(group_name)) {
    XmlExit("No such group",404);
  }
  group=new RDGroup(group_name);
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
  if(!rdcgi->user()->createCarts()) {
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
    printf("%s",(const char *)cart->xml(false,true));
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
    where=RDAllCartSearchText(filter,"",rdcgi->user()->name(),false);
  }
  else {
    sql=QString("select GROUP_NAME from USER_PERMS where ")+
      "(GROUP_NAME=\""+RDEscapeString(group_name)+"\")&&"+
      "(USER_NAME=\""+RDEscapeString(rdcgi->user()->name())+"\")";
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
    printf("%s",(const char *)cart->xml(include_cuts,true));
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
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
  printf("%s",(const char *)cart->xml(include_cuts,true));
  delete cart;
  printf("</cartList>\n");

  Exit(0);
}


void Xport::EditCart()
{
  QString where="";
  RDCart *cart;
  RDGroup *group;
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!rdcgi->user()->modifyCarts()) {
    XmlExit("Unauthorized",401);
  }
  if(xport_post->getValue("GROUP_NAME",&group_name)) {
    if(!rdcgi->user()->groupAuthorized(group_name)) {
      XmlExit("No such group",404);
    }
    group=new RDGroup(group_name);
    if(!group->exists()) {
      delete group;
      XmlExit("No such group",404);
    }
    if(group->enforceCartRange()) {
      if(((unsigned)cart_number<group->defaultLowCart())||
	 ((unsigned)cart_number>group->defaultHighCart())) {
	delete group;
	XmlExit("Invalid cart number for group",409);
      }
    }
    delete group;
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
  printf("%s",(const char *)cart->xml(include_cuts,true));
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!rdcgi->user()->deleteCarts()) {
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
  if(!cart->remove(NULL,NULL,rdcgi->config())) {
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!rdcgi->user()->editAudio()) {
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
    printf("%s",(const char *)cut->xml(true));
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }

  //
  // Process Request
  //
  sql=QString("select CUT_NAME from CUTS where ")+
    QString().sprintf("CART_NUMBER=%u ",cart_number)+
    "order by CUT_NAME";
  q=new RDSqlQuery(sql);
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cutList>\n");
  while(q->next()) {
    cut=new RDCut(q->value(0).toString());
    if(cut->exists()) {
      printf("%s",(const char *)cut->xml(true));
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
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
  printf("%s",(const char *)cut->xml(true));
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
  QDateTime start_datetime;
  bool use_start_datetime=false;
  QDateTime end_datetime;
  bool use_end_datetime=false;
  QTime start_daypart;
  bool use_start_daypart=false;
  QTime end_daypart;
  bool use_end_daypart=false;
  int talk_points[2];
  bool use_end_points[2]={false,false};
  int end_points[2];
  bool use_talk_points[2]={false,false};
  int segue_points[2];
  bool use_segue_points[2]={false,false};
  int hook_points[2];
  bool use_hook_points[2]={false,false};
  int fadeup_point;
  bool use_fadeup_point=false;
  int fadedown_point;
  bool use_fadedown_point=false;
  bool use_weight=false;
  int weight;
  bool ok=false;

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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!rdcgi->user()->editAudio()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Check Date/Time Values for Validity
  //
  if((use_start_datetime=xport_post->
      getValue("START_DATETIME",&start_datetime,&ok))) {
    if(!ok) {
      XmlExit("invalid START_DATETIME",400);
    }
  }
  if((use_end_datetime=xport_post->
      getValue("END_DATETIME",&end_datetime,&ok))) {
    if(!ok) {
      XmlExit("invalid END_DATETIME",400);
    }
  }
  if(use_start_datetime!=use_end_datetime) {
    XmlExit("both DATETIME values must be set together",400);
  }
  if(use_start_datetime&&(start_datetime>end_datetime)) {
    XmlExit("START_DATETIME is later than END_DATETIME",400);
  }

  if((use_start_daypart=xport_post->
      getValue("START_DAYPART",&start_daypart,&ok))) {
    if(!ok) {
      XmlExit("invalid START_DAYPART",400);
    }
  }
  if((use_end_daypart=xport_post->
      getValue("END_DAYPART",&end_daypart,&ok))) {
    if(!ok) {
      XmlExit("invalid END_DAYPART",400);
    }
  }
  if(use_start_daypart!=use_end_daypart) {
    XmlExit("both DAYPART values must be set together",400);
  }

  cut=new RDCut(cart_number,cut_number);
  if(!cut->exists()) {
    delete cut;
    XmlExit("No such cut",404);
  }

  //
  // Check pointers for validity
  //
  end_points[0]=cut->startPoint();
  end_points[1]=cut->endPoint();
  fadeup_point=cut->fadeupPoint();
  fadedown_point=cut->fadedownPoint();
  CheckPointerValidity(end_points,use_end_points,"",0);
  CheckPointerValidity(talk_points,use_talk_points,"TALK_",end_points[1]);
  CheckPointerValidity(segue_points,use_segue_points,"SEGUE_",end_points[1]);
  CheckPointerValidity(hook_points,use_hook_points,"HOOK_",end_points[1]);
  if((use_fadeup_point=xport_post->
      getValue("FADEUP_POINT",&fadeup_point,&ok))) {
    if(!ok) {
      XmlExit("invalid FADEUP_POINT",400);
    }
    if(fadeup_point>end_points[1]) {
      XmlExit("FADEUP_POINT exceeds length of cart",400);
    }
  }
  if((use_fadedown_point=xport_post->
      getValue("FADEDOWN_POINT",&fadedown_point,&ok))) {
    if(!ok) {
      XmlExit("invalid FADEDOWN_POINT",400);
    }
    if(fadeup_point>end_points[1]) {
      XmlExit("FADEDOWN_POINT exceeds length of cart",400);
    }
  }
  if(use_fadeup_point&&use_fadedown_point&&
     (fadeup_point>=0)&&(fadedown_point>=0)&&(fadeup_point>fadedown_point)) {
    XmlExit("FADEUP_POINT is greater than FADEDOWN_POINT",400);
  }

  //
  // Check Weight
  //
  if((use_weight=xport_post->getValue("WEIGHT",&weight,&ok))) {
    if((!ok)||(weight<0)) {
      XmlExit("invalid WEIGHT",400);
    }
  }

  //
  // Process Request
  //
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
  if(use_start_datetime) {
    cut->setStartDatetime(start_datetime,!start_datetime.isNull());
    length_changed=true;
    rotation_changed=true;
  }
  if(use_end_datetime) {
    cut->setEndDatetime(end_datetime,!end_datetime.isNull());
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
  if(use_start_daypart) {
    cut->setStartDaypart(start_daypart,!start_daypart.isNull());
    rotation_changed=true;
  }
  if(use_end_daypart) {
    cut->setEndDaypart(end_daypart,!end_daypart.isNull());
    rotation_changed=true;
  }
  if(use_weight) {
    cut->setWeight(weight);
    rotation_changed=true;
  }
  if(use_end_points[0]) {
    cut->setStartPoint(end_points[0]);
    length_changed=true;
  }
  if(use_end_points[1]) {
    cut->setEndPoint(end_points[1]);
    length_changed=true;
  }
  if(use_fadeup_point) {
    cut->setFadeupPoint(fadeup_point);
    length_changed=true;
  }
  if(use_fadedown_point) {
    cut->setFadedownPoint(fadedown_point);
    length_changed=true;
  }
  if(use_segue_points[0]) {
    cut->setSegueStartPoint(segue_points[0]);
    length_changed=true;
  }
  if(use_segue_points[1]) {
    cut->setSegueEndPoint(segue_points[1]);
    length_changed=true;
  }
  if(use_hook_points[0]) {
    cut->setHookStartPoint(hook_points[0]);
    length_changed=true;
  }
  if(use_hook_points[1]) {
    cut->setHookEndPoint(hook_points[1]);
    length_changed=true;
  }
  if(use_talk_points[0]) {
    cut->setTalkStartPoint(talk_points[0]);
    length_changed=true;
  }
  if(use_talk_points[1]) {
    cut->setTalkEndPoint(talk_points[1]);
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
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<cutList>\n");
  printf("%s",(const char *)cut->xml(true));
  printf("</cutList>\n");
  delete cut;

  Exit(0);
}


void Xport::CheckPointerValidity(int ptr_values[2],bool use_ptrs[2],
                                const QString &type,unsigned max_value)
{
  bool start_ok=false;
  bool end_ok=false;

  use_ptrs[0]=xport_post->getValue(type+"START_POINT",&ptr_values[0],&start_ok);
  use_ptrs[1]=xport_post->getValue(type+"END_POINT",&ptr_values[1],&end_ok);
  if((!use_ptrs[0])&&(!use_ptrs[1])) {
    return;
  }
  if(!start_ok) {
    XmlExit("invalid "+type+"START_POINT",400);
  }
  if(!end_ok) {
    XmlExit("invalid "+type+"END_POINT",400);
  }
  if(use_ptrs[0]!=use_ptrs[1]) {
    XmlExit("both "+type+"*_POINT values must be set together",400);
  }
  if(use_ptrs[0]) {
    if(((ptr_values[0]<0)&&(ptr_values[1]>=0))||
       ((ptr_values[0]>=0)&&(ptr_values[1]<0))) {
      XmlExit("inconsistent "+type+"*_POINT values",400);
    }
  }
  if(ptr_values[0]>=0) {
    if(ptr_values[0]>ptr_values[1]) {
      XmlExit(type+"START_POINT greater than "+type+"END_POINT",400);
    }
    if((max_value>0)&&((unsigned)ptr_values[1]>max_value)) {
      XmlExit(type+"END_POINT exceeds length of cut",400);
    }
  }
  else {
    if(max_value==0) {
      XmlExit("End markers cannot be removed",400);
    }
    else {
      ptr_values[0]=-1;
      ptr_values[1]=-1;
    }
  }
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
  if(!rdcgi->user()->cartAuthorized(cart_number)) {
    XmlExit("No such cart",404);
  }
  if(!rdcgi->user()->editAudio()) {
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
		      rdcgi->config())) {
    delete cart;
    XmlExit("No such cut",404);
  }
  delete cart;
  XmlExit("OK",200);
}
