// rdalsacard.cpp
//
// Abstract ALSA 'card' information
//
//   (C) Copyright 2019-2026 Fred Gleason <fredg@paravelsystems.com>
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

#include <QObject>

#include <rdapplication.h>

#include "rdalsacard.h"

RDAlsaCard::RDAlsaCard(snd_ctl_t *ctl,int index)
{
  card_index=index;

  snd_ctl_card_info_t *card_info=NULL;
  snd_pcm_info_t *pcm_info=NULL;
  int pcm=0;
  int slot_quantity=0;

  snd_ctl_card_info_malloc(&card_info);
  snd_pcm_info_malloc(&pcm_info);

  snd_ctl_card_info(ctl,card_info);
  card_id=QString(snd_ctl_card_info_get_id(card_info));
  card_driver=QString(snd_ctl_card_info_get_driver(card_info));
  card_name=QString(snd_ctl_card_info_get_name(card_info));
  card_pretty_name=card_name;
  card_long_name=QString(snd_ctl_card_info_get_longname(card_info));
  card_pretty_long_name=card_long_name;
  card_mixer_name=QString(snd_ctl_card_info_get_mixername(card_info));
  card_pretty_mixer_name=card_mixer_name;
  if(card_driver=="Axia") {
    slot_quantity=AxiaSlotQuantity();
  }
  if(snd_ctl_pcm_info(ctl,pcm_info)==0) {
    pcm=-1;
    do {
      snd_ctl_pcm_next_device(ctl,&pcm);
      QString pcm_name=snd_pcm_info_get_name(pcm_info);
      if(card_driver=="Axia") {
	if(card_pcm_names.size()<slot_quantity) {
	  pcm_name=QString::asprintf("Livewire Driver, Slot %d",1+pcm);
	  card_pcm_names.push_back(pcm_name);
	  card_enableds.push_back(false);
	}
      }
      else {
	if(card_pcm_names.size()==0) {
	  card_pcm_names.push_back(pcm_name);
	  card_enableds.push_back(false);
	}
      }
    } while(pcm>=0);
  }
  card_max_channels_per_pcm=rda->config()->alsaChannelsPerPcm();
  card_period_frames=-1;
  card_period_quantity=-1;

  //
  // Apply Specific Device Hints
  //
  if(card_name=="Loopback") {  // Fix the opaque name assigned by Wheatstone
    card_pretty_name.replace("Loopback","WheatNet");
    card_pretty_long_name.replace("Loopback","WheatNet");
    card_pretty_mixer_name.replace("Loopback","WheatNet");
    if(card_max_channels_per_pcm<0) {
      card_max_channels_per_pcm=2;
    }
  }
  if(card_id=="lw") {  // AoIP driver
    card_id=QString::asprintf("lw32_%d",index);
    card_driver="LWSound";
    card_pretty_name+=QString::asprintf(", slot %d",1+index);
    card_pretty_long_name+=QString::asprintf(", slot %d",1+index);
    card_pretty_mixer_name=QObject::tr("[none]");
    card_max_channels_per_pcm=2;
    card_period_frames=240;
    card_period_quantity=4;
    card_pcm_names.push_back(card_pretty_name);
    card_enableds.push_back(false);
  }
  snd_pcm_info_free(pcm_info);
  snd_ctl_card_info_free(card_info);

  //  printf("[index:%d]: %s\n",index,dump().toUtf8().constData());
}


RDAlsaCard::RDAlsaCard(const QString &id,int index)
{
  card_id=id;
  card_index=index;

  card_driver=QObject::tr("ALSA Driver");
  card_name=card_id;
  card_pretty_name=card_name;
  card_long_name=QObject::tr("ALSA Device")+" "+card_id;
  card_pretty_long_name=card_long_name;
  card_mixer_name=QObject::tr("[none]");
  card_pretty_mixer_name=card_mixer_name;
  card_max_channels_per_pcm=rda->config()->alsaChannelsPerPcm();
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


QString RDAlsaCard::prettyName() const
{
  return card_pretty_name;
}


QString RDAlsaCard::longName() const
{
  return card_long_name;
}


QString RDAlsaCard::prettyLongName() const
{
  return card_pretty_long_name;
}


QString RDAlsaCard::mixerName() const
{
  return card_long_name;
}


QString RDAlsaCard::prettyMixerName() const
{
  return card_pretty_mixer_name;
}


int RDAlsaCard::pcmQuantity() const
{
  return card_pcm_names.size();
}


QString RDAlsaCard::pcmName(int pcm_num) const
{
  return card_pcm_names.at(pcm_num);
}


bool RDAlsaCard::isEnabled(int pcm_num) const
{
  return card_enableds.at(pcm_num);
}


void RDAlsaCard::setEnabled(int pcm_num,bool state)
{
  card_enableds[pcm_num]=state;
}


int RDAlsaCard::maxChannelsPerPcm() const
{
  return card_max_channels_per_pcm;
}


int RDAlsaCard::periodFrames() const
{
  return card_period_frames;
}


void RDAlsaCard::setPeriodFrames(int frames)
{
  card_period_frames=frames;
}


int RDAlsaCard::periodQuantity() const
{
  return card_period_quantity;
}


void RDAlsaCard::setPeriodQuantity(int n)
{
  card_period_quantity=n;
}


QString RDAlsaCard::dump() const
{
  QString ret=QString::asprintf("Card %d\n",index());

  ret+="  ID: "+id()+"\n";
  ret+="  Name: "+name()+"\n";
  ret+="  Pretty Name: "+prettyName()+"\n";
  ret+="  Long Name: "+longName()+"\n";
  ret+="  Pretty Long Name: "+prettyLongName()+"\n";
  ret+="  Driver: "+driver()+"\n";
  ret+="  Mixer Name: "+mixerName()+"\n";
  ret+="  Pretty Mixer Name: "+prettyMixerName()+"\n";
  if(maxChannelsPerPcm()<0) {
    ret+="  Max Channels Per PCM: [default]\n";
  }
  else {
    ret+=QString::asprintf("  Max Channels Per PCM: %d\n",maxChannelsPerPcm());
  }
  if(periodFrames()<0) {
    ret+="  Period Frames: [default]\n";
  }
  else {
    ret+=QString::asprintf("  Period Frames: %d\n",periodFrames());
  }
  if(periodQuantity()<0) {
    ret+="  Period Quantity: [default]\n";
  }
  else {
    ret+=QString::asprintf("  Period Quantity: %d\n",periodQuantity());
  }

  return ret;
}


int RDAlsaCard::AxiaSlotQuantity() const
{
  FILE *f=NULL;
  char line[256];
  int ret=0;

  if((f=fopen("/etc/axia/license","r"))!=NULL) {
    while(fgets(line,255,f)!=NULL) {
      QString str=QString::fromUtf8(line).trimmed();
      QStringList f0=str.split("=");
      if((f0.size()==2)&&(f0.first()=="SlotQuantity")) {
	ret=f0.last().toInt();
	break;
      }
    }
    fclose(f);
  }

  return ret;
}
