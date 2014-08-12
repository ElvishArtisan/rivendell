// rdalsa.cpp
//
// Abstract an ALSA configuration. 
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdalsa.cpp,v 1.2 2010/07/29 19:32:39 cvs Exp $
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
#include <stdio.h>
#include <alsa/asoundlib.h>

#include <qstringlist.h>

#include <rdalsa.h>

RDAlsa::RDAlsa()
{
  clear();
}


unsigned RDAlsa::cards() const
{
  return card_ids.size();
}


QString RDAlsa::cardId(unsigned cardnum) const
{
  if(cardnum>=card_ids.size()) {
    return QString("[invalid card]");
  }
  return card_ids[cardnum];
}


QString RDAlsa::cardDriver(unsigned cardnum) const
{
  if(cardnum>=card_drivers.size()) {
    return QString("[invalid card]");
  }
  return card_drivers[cardnum];
}


QString RDAlsa::cardName(unsigned cardnum) const
{
  if(cardnum>=card_names.size()) {
    return QString("[invalid card]");
  }
  return card_names[cardnum];
}


QString RDAlsa::cardLongName(unsigned cardnum) const
{
  if(cardnum>=card_long_names.size()) {
    return QString("[invalid card]");
  }
  return card_long_names[cardnum];
}


QString RDAlsa::cardMixerName(unsigned cardnum) const
{
  if(cardnum>=card_mixer_names.size()) {
    return QString("[invalid card]");
  }
  return card_mixer_names[cardnum];
}


int RDAlsa::pcmDevices(unsigned cardnum) const
{
  if(cardnum>=card_pcm_names.size()) {
    return -1;
  }
  return card_pcm_names[cardnum].size();
}


QString RDAlsa::pcmName(unsigned cardnum,unsigned pcm) const
{
  if(cardnum>=card_pcm_names.size()) {
    return QString("[invalid pcm device]");
  }
  if(pcm>=card_pcm_names[cardnum].size()) {
    return QString("[invalid pcm device]");
  }
  return card_pcm_names[cardnum][pcm];
}


int RDAlsa::rivendellCard(int slot) const
{
  return card_rivendell_cards[slot];
}


void RDAlsa::setRivendellCard(int slot,int cardnum)
{
  if(slot>=RD_MAX_CARDS) {
    return;
  }
  card_rivendell_cards[slot]=cardnum;
}


int RDAlsa::rivendellDevice(int slot) const
{
  return card_rivendell_devices[slot];
}


void RDAlsa::setRivendellDevice(int slot,int devnum)
{
  if(slot>=RD_MAX_CARDS) {
    return;
  }
  card_rivendell_devices[slot]=devnum;
}


bool RDAlsa::load(const QString &filename)
{
  LoadSystemConfig();
  return LoadAsoundConfig(filename);
}


bool RDAlsa::save(const QString &filename)
{
  return SaveAsoundConfig(filename);
}


void RDAlsa::clear()
{
  card_ids.clear();
  card_drivers.clear();
  card_names.clear();
  card_long_names.clear();
  card_mixer_names.clear();
  card_pcm_names.clear();
  for(unsigned i=0;i<RD_MAX_CARDS;i++) {
    card_rivendell_cards[i]=-1;
    card_rivendell_devices[i]=-1;
  }
};


void RDAlsa::LoadSystemConfig()
{
  snd_ctl_t *snd_ctl=NULL;
  snd_ctl_card_info_t *card_info;
  snd_pcm_info_t *pcm_info;
  int card=0;
  int pcm=0;

  snd_ctl_card_info_malloc(&card_info);
  snd_pcm_info_malloc(&pcm_info);
  while(snd_ctl_open(&snd_ctl,QString().sprintf("hw:%d",card),0)>=0) {
    snd_ctl_card_info(snd_ctl,card_info);
    card_ids.push_back(snd_ctl_card_info_get_id(card_info));
    card_drivers.push_back(snd_ctl_card_info_get_driver(card_info));
    card_names.push_back(snd_ctl_card_info_get_name(card_info));
    card_long_names.push_back(snd_ctl_card_info_get_longname(card_info));
    card_mixer_names.push_back(snd_ctl_card_info_get_mixername(card_info));
    std::vector<QString> pcms;
    if(snd_ctl_pcm_info(snd_ctl,pcm_info)==0) {
      pcm=0;
      while(pcm>=0) {
	pcms.push_back(QString().sprintf("%s [%u]",
		      (const char *)snd_pcm_info_get_name(pcm_info),pcm+1));
	snd_ctl_pcm_next_device(snd_ctl,&pcm);
      }
    }
    card_pcm_names.push_back(pcms);
    snd_ctl_close(snd_ctl);
    card++;
  }
}


bool RDAlsa::LoadAsoundConfig(const QString &filename)
{
  FILE *f=NULL;
  char line[1024];
  int istate=0;
  int port=0;
  int card=0;
  int device=0;
  QStringList list;

  if((f=fopen(filename,"r"))==NULL) {
    return false;
  }
  while(fgets(line,1024,f)!=NULL) {
    QString str=line;
    str.replace("\n","");
    if((str!=START_MARKER)&&(str!=END_MARKER)) {
      switch(istate) {
	case 0:
	  if(str.left(6)=="pcm.rd") {
	    port=str.mid(6,1).toInt();
	    istate=1;
	  }
	  else {
	    if(str.left(6)=="ctl.rd") {
	      istate=10;
	    }
	    else {
	      card_other_lines.push_back(str+"\n");
	    }
	  }
	  break;

	case 1:
	  list=list.split(" ",str);
	  if(list[0]=="}") {
	    if((port>=0)&&(port<RD_MAX_CARDS)) {
	      card_rivendell_cards[port]=card;
	      card_rivendell_devices[port]=device;
	    }
	    card=0;
	    device=0;
	    istate=0;
	  }
	  else {
	    if(list.size()==2) {
	      if(list[0]=="card") {
		card=list[1].toInt();
	      }
	      if(list[0]=="device") {
		device=list[1].toInt();
	      }
	    }
	  }
	  break;

	case 10:
	  if(str.left(1)=="}") {
	    istate=0;
	  }
	  break;
      }
    }
  }
  fclose(f);
  return true;
}


bool RDAlsa::SaveAsoundConfig(const QString &filename)
{
  FILE *f=NULL;

  if((f=fopen(filename,"w"))==NULL) {
    return false;
  }
  for(unsigned i=0;i<card_other_lines.size();i++) {
    fprintf(f,card_other_lines[i]);
  }

  fprintf(f,"%s\n",START_MARKER);
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if((card_rivendell_cards[i]>=0)&&(card_rivendell_devices[i]>=0)) {
      fprintf(f,"pcm.rd%d {\n",i);
      fprintf(f,"  type hw\n");
      fprintf(f,"  card %d\n",card_rivendell_cards[i]);
      fprintf(f,"  device %d\n",card_rivendell_devices[i]);
      fprintf(f,"}\n");
      fprintf(f,"ctl.rd%d {\n",i);
      fprintf(f,"  type hw\n");
      fprintf(f,"  card %d\n",card_rivendell_cards[i]);
      fprintf(f,"}\n");
    }
  }
  fprintf(f,"%s\n",END_MARKER);

  fclose(f);
  return true;
}
