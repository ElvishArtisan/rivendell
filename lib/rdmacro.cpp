// rdmacro.cpp
//
// A container class for a Rivendell Macro Language Command
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdint.h>

#include "rdmacro.h"

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


QString RDMacro::arg(int n) const
{
  return rml_args[n];
}


int RDMacro::addArg(const QString &arg)
{
  rml_args.push_back(arg);
  return rml_args.size()-1;
}


int RDMacro::addArg(int arg)
{
  rml_args.push_back(QString().sprintf("%d",arg));
  return rml_args.size()-1;
}


int RDMacro::addArg(unsigned arg)
{
  rml_args.push_back(QString().sprintf("%u",arg));
  return rml_args.size()-1;
}


void RDMacro::setArg(int n,const QString &arg)
{
  rml_args[n]=arg;
}


void RDMacro::setArg(int n,int arg)
{
  rml_args[n]=QString().sprintf("%d",arg);
}


void RDMacro::setArg(int n,unsigned arg)
{
  rml_args[n]=QString().sprintf("%u",arg);
}


int RDMacro::argQuantity() const
{
  return rml_args.size();
}


void RDMacro::acknowledge(bool state)
{
  rml_role=RDMacro::Reply;
  if(state) {
    rml_args.push_back("+");
  }
  else {
    rml_args.push_back("-");
  }
}


QString RDMacro::rollupArgs(int n)
{
  return rml_args.join(" ");
}


QString RDMacro::toString() const
{
  QString ret=QChar(((uint16_t)rml_cmd)>>8);
  ret+=QChar(0xFF&((uint16_t)rml_cmd));
  ret+=" ";
  ret+=rml_args.join(" ");
  ret+="!";

  return ret;
}


unsigned RDMacro::length() const
{
  if((rml_cmd==RDMacro::SP)&&(rml_args.size()==1)) {
    return rml_args[0].toUInt();
  }
  return 0;
}


bool RDMacro::isNull() const
{
  return rml_cmd==RDMacro::NN;
}


void RDMacro::clear()
{
  rml_role=RDMacro::Invalid;
  rml_addr=QHostAddress();
  rml_port=RD_RML_NOECHO_PORT;
  rml_echo_requested=false;
  rml_cmd=RDMacro::NN;
  rml_args.clear();
}


RDMacro RDMacro::fromString(const QString &str,RDMacro::Role role)
{
  RDMacro ret;
  RDMacro::Command cmd=RDMacro::NN;

  ret.setRole(role);

  //
  // Check for bang
  //
  QString str2=str.stripWhiteSpace();
  if(str2.right(1)!="!") {
    ret.setCommand(RDMacro::NN);
    return ret;
  }

  //
  // Get Command
  //
  QStringList f0=f0.split(" ",str2.left(str2.length()-1).stripWhiteSpace());  
  if(f0[0].length()!=2) {
    ret.setCommand(RDMacro::NN);
    return ret;
  }
  cmd=(RDMacro::Command)((f0[0].at(0).latin1()<<8)+f0[0].at(1).latin1());
  switch(cmd) {
  case RDMacro::AG:
  case RDMacro::AL:
  case RDMacro::BO:
  case RDMacro::CC:
  case RDMacro::CE:
  case RDMacro::CL:
  case RDMacro::CP:
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
    ret.setCommand(cmd);
    break;
	
  default:
    ret.setCommand(RDMacro::NN);
    return ret;
  }

  //
  // Get Arguments
  //
  for(int i=1;i<f0.size();i++) {
    ret.addArg(f0[i]);
  }

  return ret;
}
