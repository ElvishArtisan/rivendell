// rduser.cpp
//
// Abstract a Rivendell User.
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>

#include <rdconf.h>
#include <rdpam.h>
#include <rduser.h>
#include <rddb.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDUser::RDUser(const QString &name)
{
  user_name=name;
}


RDUser::RDUser()
{
  user_name="";
}


QString RDUser::name() const
{
  return user_name;
}


void RDUser::setName(const QString &name)
{
  user_name=name;
}


bool RDUser::exists() const
{
  return RDDoesRowExist("USERS","LOGIN_NAME",user_name);
}


bool RDUser::authenticated(bool webuser) const
{
  RDSqlQuery *q;
  QString sql;

  if(localAuthentication()) {
    sql=QString("select ")+
      "LOGIN_NAME,"+
      "ENABLE_WEB "+
      "from USERS where "+
      "LOGIN_NAME=\""+RDEscapeString(user_name)+"\" && "+
      "PASSWORD=\""+RDEscapeString(user_password)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      bool ret=RDBool(q->value(1).toString())||
	((!RDBool(q->value(1).toString()))&&(!webuser));
      delete q;
      return ret;
    }
    delete q;
  }
  else {
    bool ret=false;
    RDPam *pam=new RDPam(pamService());
    ret=pam->authenticate(user_name,user_password);
    delete pam;
    return ret;
  }

  return false;
}


bool RDUser::checkPassword(const QString &password,bool webuser)
{
  user_password=password;
  return authenticated(webuser);
}


QString RDUser::password() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"PASSWORD").toString();
}


void RDUser::setPassword(const QString &password)
{
  user_password=password;
  SetRow("PASSWORD",password);
}


bool RDUser::enableWeb() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,"ENABLE_WEB").
	       toString());
}


void RDUser::setEnableWeb(bool state) const
{
  SetRow("ENABLE_WEB",RDYesNo(state));
}


bool RDUser::localAuthentication() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,"LOCAL_AUTH").
	       toString());
}


void RDUser::setLocalAuthentication(bool state) const
{
  SetRow("LOCAL_AUTH",RDYesNo(state));
}


QString RDUser::pamService() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"PAM_SERVICE").toString();
}


void RDUser::setPamService(const QString &str) const
{
  SetRow("PAM_SERVICE",str);
}


QString RDUser::fullName() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"FULL_NAME").toString();
}


void RDUser::setFullName(const QString &name) const
{
  SetRow("FULL_NAME",name);
}


QString RDUser::emailAddress() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"EMAIL_ADDRESS").
    toString();
}


void RDUser::setEmailAddress(const QString &str) const
{
  SetRow("EMAIL_ADDRESS",str);
}


QString RDUser::emailContact() const
{
  QString ret;

  QString sql=QString("select ")+
    "EMAIL_ADDRESS,"+  // 00
    "FULL_NAME "+      // 01
    "from USERS where "+
    "LOGIN_NAME=\""+RDEscapeString(user_name)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDUser::emailContact(q->value(0).toString(),q->value(1).toString());
  }
  delete q;

  return ret;
}


QString RDUser::description() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"DESCRIPTION").toString();
}


void RDUser::setDescription(const QString &desc) const
{
  SetRow("DESCRIPTION",desc);
}


QString RDUser::phone() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"PHONE_NUMBER").
    toString();
}


void RDUser::setPhone(const QString &phone) const
{
  SetRow("PHONE_NUMBER",phone);
}


int RDUser::webapiAuthTimeout() const
{
  return RDGetSqlValue("USERS","LOGIN_NAME",user_name,"WEBAPI_AUTH_TIMEOUT").
    toInt();
}


void RDUser::setWebapiAuthTimeout(int sec) const
{
  SetRow("WEBAPI_AUTH_TIMEOUT",sec);
}


bool RDUser::adminConfig() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "ADMIN_CONFIG_PRIV").toString());
}


void RDUser::setAdminConfig(bool priv) const
{
  SetRow("ADMIN_CONFIG_PRIV",priv);
}


bool RDUser::createCarts() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "CREATE_CARTS_PRIV").toString());
}


void RDUser::setCreateCarts(bool priv) const
{
  SetRow("CREATE_CARTS_PRIV",priv);
}


bool RDUser::deleteCarts() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "DELETE_CARTS_PRIV").toString());
}


void RDUser::setDeleteCarts(bool priv) const
{
  SetRow("DELETE_CARTS_PRIV",priv);
}


bool RDUser::modifyCarts() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "MODIFY_CARTS_PRIV").toString());
}


void RDUser::setModifyCarts(bool priv) const
{
  SetRow("MODIFY_CARTS_PRIV",priv);
}


bool RDUser::editAudio() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "EDIT_AUDIO_PRIV").toString());
}


void RDUser::setEditAudio(bool priv) const
{
  SetRow("EDIT_AUDIO_PRIV",priv);
}


bool RDUser::webgetLogin() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "WEBGET_LOGIN_PRIV").toString());
}


void RDUser::setWebgetLogin(bool priv) const
{
  SetRow("WEBGET_LOGIN_PRIV",priv);
}


bool RDUser::createLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "CREATE_LOG_PRIV").toString());
}


void RDUser::setCreateLog(bool priv) const
{
  SetRow("CREATE_LOG_PRIV",priv);
}


bool RDUser::deleteLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "DELETE_LOG_PRIV").toString());
}


void RDUser::setDeleteLog(bool priv) const
{
  SetRow("DELETE_LOG_PRIV",priv);
}


bool RDUser::deleteRec() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "DELETE_REC_PRIV").toString());
}


void RDUser::setDeleteRec(bool priv) const
{
  SetRow("DELETE_REC_PRIV",priv);
}


bool RDUser::playoutLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "PLAYOUT_LOG_PRIV").toString());
}


void RDUser::setPlayoutLog(bool priv) const
{
  SetRow("PLAYOUT_LOG_PRIV",priv);
}


bool RDUser::arrangeLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "ARRANGE_LOG_PRIV").toString());
}


void RDUser::setArrangeLog(bool priv) const
{
  SetRow("ARRANGE_LOG_PRIV",priv);
}


bool RDUser::addtoLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "ADDTO_LOG_PRIV").toString());
}


void RDUser::setAddtoLog(bool priv) const
{
  SetRow("ADDTO_LOG_PRIV",priv);
}


bool RDUser::removefromLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "REMOVEFROM_LOG_PRIV").toString());
}


bool RDUser::configPanels() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "CONFIG_PANELS_PRIV").toString());
}


void RDUser::setConfigPanels(bool priv) const
{
  SetRow("CONFIG_PANELS_PRIV",priv);
}


void RDUser::setRemovefromLog(bool priv) const
{
  SetRow("REMOVEFROM_LOG_PRIV",priv);
}


bool RDUser::voicetrackLog() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "VOICETRACK_LOG_PRIV").toString());
}


void RDUser::setVoicetrackLog(bool priv) const
{
  SetRow("VOICETRACK_LOG_PRIV",priv);
}


bool RDUser::modifyTemplate() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "MODIFY_TEMPLATE_PRIV").toString());
}


void RDUser::setModifyTemplate(bool priv) const
{
  SetRow("MODIFY_TEMPLATE_PRIV",priv);
}


bool RDUser::editCatches() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "EDIT_CATCHES_PRIV").toString());
}


void RDUser::setEditCatches(bool priv) const
{
  SetRow("EDIT_CATCHES_PRIV",priv);
}


bool RDUser::addPodcast() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "ADD_PODCAST_PRIV").toString());
}


void RDUser::setAddPodcast(bool priv) const
{
  SetRow("ADD_PODCAST_PRIV",priv);
}


bool RDUser::editPodcast() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "EDIT_PODCAST_PRIV").toString());
}


void RDUser::setEditPodcast(bool priv) const
{
  SetRow("EDIT_PODCAST_PRIV",priv);
}


bool RDUser::deletePodcast() const
{
  return RDBool(RDGetSqlValue("USERS","LOGIN_NAME",user_name,
			    "DELETE_PODCAST_PRIV").toString());
}


void RDUser::setDeletePodcast(bool priv) const
{
  SetRow("DELETE_PODCAST_PRIV",priv);
}


bool RDUser::groupAuthorized(const QString &group_name)
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select GROUP_NAME from USER_PERMS where ")+
    "(USER_NAME=\""+RDEscapeString(user_name)+"\")&&"+
    "(GROUP_NAME=\""+RDEscapeString(group_name)+"\")";
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;
  return ret;
}


QStringList RDUser::groups() const
{
  QString sql;
  RDSqlQuery *q;
  QStringList ret;

  sql=QString("select GROUP_NAME from USER_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(user_name)+"\" order by GROUP_NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ret.push_back(q->value(0).toString());
  }
  delete q;

  return ret;
}


bool RDUser::cartAuthorized(unsigned cartnum) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select CART.NUMBER from CART ")+
    "left join USER_PERMS "+
    "on CART.GROUP_NAME=USER_PERMS.GROUP_NAME where "+
    "(USER_PERMS.USER_NAME=\""+RDEscapeString(user_name)+"\")&&"+
    QString().sprintf("(CART.NUMBER=%u)",cartnum);
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;
  return ret;
}


bool RDUser::feedAuthorized(const QString &keyname)
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select ID from FEED_PERMS where ")+
    "(USER_NAME=\""+RDEscapeString(user_name)+"\")&&"+
    "(KEY_NAME=\""+RDEscapeString(keyname)+"\")";
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;
  return ret;
}


QString RDUser::serviceCheckDefault(QString serv) const
{
  bool match_flag = false;
  QStringList services_list = services();

  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end(); ++it ) {
    if (serv.compare(*it) == 0) {
      match_flag = true;
      break;
    }
  }

  if (match_flag)
    return serv;
  else
    return QString("");
}


QStringList RDUser::services() const
{
  RDSqlQuery *q;
  QString sql;
  QStringList services_list;

  if (adminConfig()) {
    sql=QString().sprintf("SELECT NAME FROM SERVICES" );
  } 
  else {
    sql=QString("select distinct ")+
      "AUDIO_PERMS.SERVICE_NAME "+
      "from USER_PERMS left join AUDIO_PERMS "+
      "on USER_PERMS.GROUP_NAME=AUDIO_PERMS.GROUP_NAME where "+
      "USER_PERMS.USER_NAME=\""+RDEscapeString(user_name)+"\"";
  }
  
  q=new RDSqlQuery(sql);
  while (q->next() ) {
    services_list.append( q->value(0).toString() );
  }
  delete q;

  return services_list;
}


bool RDUser::emailIsValid(const QString &addr)
{
  QStringList f0=addr.split("@",QString::KeepEmptyParts);

  if(f0.size()!=2) {
    return false;
  }
  QStringList f1=f0.last().split(".");
  if(f1.size()<2) {
    return false;
  }
  return true;
}


QString RDUser::emailContact(const QString &addr,const QString &fullname)
{
  QString ret;

  if(RDUser::emailIsValid(addr)) {
    ret=addr;
    if(!fullname.isEmpty()) {
      ret+=" ("+fullname+")";
    }
  }

  return ret;
}


void RDUser::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update USERS set ")+
    param+"=\""+RDEscapeString(value)+"\" where "+
    "LOGIN_NAME=\""+RDEscapeString(user_name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDUser::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update USERS set ")+
    param+QString().sprintf("=%d where ",value)+
    "LOGIN_NAME=\""+user_name+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDUser::SetRow(const QString &param,bool value) const
{
  SetRow(param,RDYesNo(value));
}
