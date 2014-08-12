// rdmacro.cpp
//
// A container class for a Rivendell Macro Language Command
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmacro.cpp,v 1.37.4.2.2.1 2014/05/22 19:37:44 cvs Exp $
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

#include <ctype.h>

#include <rdmacro.h>

RDMacro::RDMacro()
{
  clear();
}


RDMacro::Role RDMacro::role() const
{
  return rml_role;
}


void RDMacro::setRole(RDMacro::Role role)
{
  rml_role=role;
}


RDMacro::Command RDMacro::command() const
{
  return rml_cmd;
}


void RDMacro::setCommand(RDMacro::Command cmd)
{
  rml_cmd=cmd;
}


QHostAddress RDMacro::address() const
{
  return rml_addr;
}


void RDMacro::setAddress(QHostAddress addr)
{
  rml_addr=addr;
}


Q_UINT16 RDMacro::port() const
{
  return rml_port;
}


void RDMacro::setPort(Q_UINT16 port)
{
  rml_port=port;
}


bool RDMacro::echoRequested() const
{
  return rml_echo_requested;
}


void RDMacro::setEchoRequested(bool state)
{
  rml_echo_requested=state;
}


QVariant RDMacro::arg(int n) const
{
  if(n>=RD_RML_MAX_ARGS) {
    return QVariant();
  }
  return rml_arg[n];
}


void RDMacro::setArg(int n,QVariant arg)
{
  if(n>=RD_RML_MAX_ARGS) {
    return;
  }
  rml_arg[n]=arg;
}


int RDMacro::argQuantity() const
{
  return rml_arg_quantity;
}


void RDMacro::acknowledge(bool state)
{
  rml_role=RDMacro::Reply;
  if(rml_arg_quantity<RD_RML_MAX_ARGS) {
    if(state) {
      rml_arg[rml_arg_quantity]=QString("+");
      rml_arg_quantity++;
    }
    else {
      rml_arg[rml_arg_quantity]=QString("-");
      rml_arg_quantity++;
    }
  }
}


void RDMacro::setArgQuantity(int n)
{
  rml_arg_quantity=n;
}


QString RDMacro::rollupArgs(int n)
{
  QString str;
  for(int i=n;i<rml_arg_quantity;i++) {
    str=(str+" "+rml_arg[i].toString());
  }
  return str.right(str.length()-1);
}


bool RDMacro::parseString(const char *str,int n)
{
  int argnum=0;
  int argptr=0;
  char arg[RD_RML_MAX_LENGTH];
  bool quoted=false;
  bool escaped=false;

  if(n<3) {
    return false;
  }

  //
  // Command Mneumonic
  //
  rml_cmd=(RDMacro::Command)((str[0]<<8)+str[1]);
  switch(rml_cmd) {
      case RDMacro::AL:
      case RDMacro::BO:
      case RDMacro::CC:
      case RDMacro::CL:
      case RDMacro::DB:
      case RDMacro::DL:
      case RDMacro::DP:
      case RDMacro::DS:
      case RDMacro::DX:
      case RDMacro::EX:
      case RDMacro::FS:
      case RDMacro::GE:
      case RDMacro::GI:
      case RDMacro::GO:
      case RDMacro::JC:
      case RDMacro::JD:
      case RDMacro::LB:
      case RDMacro::LC:
      case RDMacro::LL:
      case RDMacro::LO:
      case RDMacro::MB:
      case RDMacro::MD:
      case RDMacro::MN:
      case RDMacro::MT:
      case RDMacro::NN:
      case RDMacro::PB:
      case RDMacro::PC:
      case RDMacro::PE:
      case RDMacro::PL:
      case RDMacro::PM:
      case RDMacro::PN:
      case RDMacro::PP:
      case RDMacro::PS:
      case RDMacro::PT:
      case RDMacro::PU:
      case RDMacro::PW:
      case RDMacro::PX:
      case RDMacro::RL:
      case RDMacro::RS:
      case RDMacro::RR:
      case RDMacro::RN:
      case RDMacro::SN:
      case RDMacro::ST:
      case RDMacro::SA:
      case RDMacro::SC:
      case RDMacro::SD:
      case RDMacro::SG:
      case RDMacro::SI:
      case RDMacro::SO:
      case RDMacro::SP:
      case RDMacro::SR:
      case RDMacro::SL:
      case RDMacro::SX:
      case RDMacro::SY:
      case RDMacro::SZ:
      case RDMacro::TA:
      case RDMacro::UO:
      case RDMacro::PD:
	break;
	
      default:
	return false;
  }
  if(str[2]=='!') {
    rml_arg_quantity=0;
    return true;
  }
  if(!isblank(str[2])) {
    rml_arg_quantity=0;
    rml_cmd=RDMacro::NN;
    return false;
  }

  //
  // Arguments
  //
  for(int i=3;i<n;i++) {
    if(quoted||escaped) {
      arg[argptr]=str[i];
      if(quoted&&((arg[argptr]==0x22)||(arg[argptr]==0x27))) {
	quoted=false;
      }
      if(escaped) {
	escaped=false;
      }
      argptr++;
    }
    else {
      switch(str[i]) {
	case ' ':
	  arg[argptr]=0;
	  rml_arg[argnum++]=QVariant(QString(arg));
	  argptr=0;
	  break;
	  
	case '!':
	  arg[argptr]=0;
	  rml_arg[argnum++]=QVariant(QString(arg));
	  rml_arg_quantity=argnum;
	  return true;
	  
	case '\\':
	  escaped=true;
	  break;

	default:
	  arg[argptr]=str[i];
	  if((arg[argptr]==0x22)||(arg[argptr]==0x27)) {
	    quoted=true;
	  }
	  argptr++;
	  break;
      }
    }
    if((argnum>=RD_RML_MAX_ARGS)||(argptr>=RD_RML_MAX_LENGTH)) {
      rml_arg_quantity=0;
      rml_cmd=RDMacro::NN;
      for(int j=0;j<RD_RML_MAX_ARGS;j++) {
	rml_arg[j].clear();
      }
      return false;
    }
  }
  rml_arg_quantity=0;
  rml_cmd=RDMacro::NN;
  for(int i=0;i<RD_RML_MAX_ARGS;i++) {
    rml_arg[i].clear();
  }
  return false;
}


int RDMacro::generateString(char *str,int n) const
{
  int len=3;

  for(int i=0;i<rml_arg_quantity;i++) {
    len+=(rml_arg[i].toString().length()+1);
  }
  if(len>=n) {
    return -1;
  }
  sprintf(str,"%c%c",rml_cmd>>8,rml_cmd&0xFF);
  for(int i=0;i<rml_arg_quantity;i++) {
    strcat(str," ");
    strcat(str,(const char *)rml_arg[i].toString());
  }
  strcat(str,"!");
  return len;
}


unsigned RDMacro::length() const
{
  switch(rml_cmd) {
      case RDMacro::SP:
	if(rml_arg_quantity==1) {
	  return rml_arg[0].toUInt();
	}
	return 0;

      default:
	return 0;
  }
  return 0;
}


void RDMacro::clear()
{
  rml_role=RDMacro::Invalid;
  rml_cmd=RDMacro::NN;
  rml_addr=QHostAddress();
  rml_port=RD_RML_NOECHO_PORT;
  rml_echo_requested=false;
  for(int i=0;i<RD_RML_MAX_ARGS;i++) {
    rml_arg[i].clear();
  }
  rml_arg_quantity=0;
}
