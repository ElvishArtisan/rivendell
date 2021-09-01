// rdalsacard.cpp
//
// Abstract ALSA 'card' information
//
//   (C) Copyright 2019-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdalsacard.h"

RDAlsaCard::RDAlsaCard(snd_ctl_t *ctl,int index)
{
  snd_ctl_card_info_t *card_info;
  snd_pcm_info_t *pcm_info;
  int pcm=0;

  card_index=index;

  snd_ctl_card_info_malloc(&card_info);
  snd_pcm_info_malloc(&pcm_info);

  snd_ctl_card_info(ctl,card_info);
  card_id=QString(snd_ctl_card_info_get_id(card_info));
  card_driver=QString(snd_ctl_card_info_get_driver(card_info));
  card_name=QString(snd_ctl_card_info_get_name(card_info));
  card_long_name=QString(snd_ctl_card_info_get_longname(card_info));
  card_mixer_name=QString(snd_ctl_card_info_get_mixername(card_info));
  if(snd_ctl_pcm_info(ctl,pcm_info)==0) {
    pcm=0;
    while(pcm>=0) {
      card_pcm_names.push_back(snd_pcm_info_get_name(pcm_info)+
			       QString::asprintf("[%02d]",pcm+1));
      snd_ctl_pcm_next_device(ctl,&pcm);
      card_enableds.push_back(false);
    }
  }
  snd_pcm_info_free(pcm_info);
  snd_ctl_card_info_free(card_info);
}


int RDAlsaCard::index() const
{
  return card_index;
}


QString RDAlsaCard::id() const
{
  return card_id;
}


QString RDAlsaCard::driver() const
{
  return card_driver;
}


QString RDAlsaCard::name() const
{
  return card_name;
}


QString RDAlsaCard::longName() const
{
  return card_long_name;
}


QString RDAlsaCard::mixerName() const
{
  return card_long_name;
}


bool RDAlsaCard::isEnabled(int pcm_num) const
{
  return card_enableds.at(pcm_num);
}


void RDAlsaCard::setEnabled(int pcm_num,bool state)
{
  card_enableds[pcm_num]=state;
}


QString RDAlsaCard::dump() const
{
  QString ret=QString::asprintf("Card %d\n",index());

  ret+="  ID: "+id()+"\n";
  ret+="  Name: "+name()+"\n";
  ret+="  LongName: "+longName()+"\n";
  ret+="  MixerName: "+mixerName()+"\n";
  for(int i=0;i<pcmQuantity();i++) {
    ret+=QString::asprintf("    PCM[%d]: ",i)+pcmName(i)+"\n";
  }

  return ret;
}


int RDAlsaCard::pcmQuantity() const
{
  return card_pcm_names.size();
}


QString RDAlsaCard::pcmName(int n) const
{
  return card_pcm_names.at(n);
}
