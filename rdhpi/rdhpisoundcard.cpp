//   rdhpisoundcard.cpp
//
//   The audio card subsystem for the HPI Library.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>
#include <unistd.h>

#include <qtimer.h>
#include <unistd.h>

#include <rdapplication.h>

#include "rdhpisoundcard.h"

RDHPISoundCard::RDHPISoundCard(RDConfig *config,QObject *parent)
  : QObject(parent)
{
  card_config=config;

  card_quantity=0;
  fade_type=RDHPISoundCard::Log;
  for(int i=0;i<HPI_MAX_ADAPTERS;i++) {
    card_index[i]=0;
    card_input_streams[i]=0;
    card_output_streams[i]=0;
    card_input_ports[i]=0;
    card_output_ports[i]=0;
    input_mux_type[i]=false;
    timescale_support[i]=false;
    for(int j=0;j<HPI_MAX_NODES;j++) {
      input_port_level[i][j]=false;
      output_port_level[i][j]=false;
      input_port_meter[i][j]=false;
      output_port_meter[i][j]=false;
      input_port_mux[i][j]=false;
      input_port_mux_type[i][j][0]=false;
      input_port_mux_type[i][j][1]=false;
      input_mux_index[i][j][0]=0;
      input_mux_index[i][j][1]=0;
      input_port_aesebu[i][j]=false;
      input_port_aesebu_error[i][j]=false;
      for(int k=0;k<HPI_MAX_STREAMS;k++) {
	input_stream_volume[i][k][j]=false;
	output_stream_volume[i][k][j]=false;
      }
      for(int k=0;k<HPI_MAX_NODES;k++) {
	passthrough_port_volume[i][j][k]=false;
      }
    }
    for(int j=0;j<HPI_MAX_STREAMS;j++) {
      input_stream_meter[i][j]=false;
      output_stream_meter[i][j]=false;
      input_port_mode[i][j]=false;
      output_stream_mode[i][j]=false;
      input_stream_vox[i][j]=false;
      input_stream_mux[i][j]=false;
    }
  }
  if(HPI_SubSysCreate()==NULL) {
    return;
  }
  HPIProbe();
}


RDHPISoundCard::~RDHPISoundCard()
{
  HPI_SubSysFree(NULL);
}


RDHPISoundCard::Driver RDHPISoundCard::driver() const
{
  return RDHPISoundCard::Hpi;
}


RDHPIInformation *RDHPISoundCard::hpiInformation(int card)
{
  return &(hpi_info[card]);
}


int RDHPISoundCard::getCardQuantity() const
{
  return card_quantity;
}


int RDHPISoundCard::getCardInputStreams(int card) const
{
  return card_input_streams[card];
}


int RDHPISoundCard::getCardOutputStreams(int card) const
{
  return card_output_streams[card];
}


int RDHPISoundCard::getCardInputPorts(int card) const
{
  return card_input_ports[card];
}


int RDHPISoundCard::getCardOutputPorts(int card) const
{
  return card_output_ports[card];
}


const void *RDHPISoundCard::getCardInfo(int card) const
{
  return &hpi_info[card];
}


QString RDHPISoundCard::getCardDescription(int card) const
{
  return card_description[card];
}


QString RDHPISoundCard::getInputStreamDescription(int card,int stream) const
{
  return input_stream_description[card][stream];
}


QString RDHPISoundCard::getOutputStreamDescription(int card,int stream) const
{
  return output_stream_description[card][stream];
}


QString RDHPISoundCard::getInputPortDescription(int card,int port) const
{
  return input_port_description[card][port];
}


QString RDHPISoundCard::getOutputPortDescription(int card,int port) const
{
  return output_port_description[card][port];
}


bool RDHPISoundCard::setClockSource(int card,RDHPISoundCard::ClockSource src)
{
  hpi_err_t hpi_err=0;

  switch(src) {
  case RDHPISoundCard::Internal:
    hpi_err=HPI_SampleClock_SetSource(NULL,
				      clock_source_control[card],
				      HPI_SAMPLECLOCK_SOURCE_LOCAL);
    break;

  case RDHPISoundCard::AesEbu:
  case RDHPISoundCard::SpDiff:
    hpi_err=LogHpi(HPI_SampleClock_SetSource(NULL,
					     clock_source_control[card],
					     HPI_SAMPLECLOCK_SOURCE_AESEBU_SYNC),__LINE__);
    break;

  case RDHPISoundCard::WordClock:
    hpi_err=LogHpi(HPI_SampleClock_SetSource(NULL,
					     clock_source_control[card],
					     HPI_SAMPLECLOCK_SOURCE_WORD),
		   __LINE__);
    break;
  }
  return hpi_err==0;
}


bool RDHPISoundCard::haveTimescaling(int card) const
{
  if(card>=HPI_MAX_ADAPTERS) {
    return false;
  }

  return timescale_support[card];
}


bool RDHPISoundCard::haveInputVolume(int card,int stream,int port) const
{
  if(card>=HPI_MAX_ADAPTERS||stream>=HPI_MAX_STREAMS||port>=HPI_MAX_NODES) {
    return false;
  }

  return input_stream_volume[card][stream][port];
}


bool RDHPISoundCard::haveOutputVolume(int card,int stream,int port) const
{
  if(card>=HPI_MAX_ADAPTERS||stream>=HPI_MAX_STREAMS||port>=HPI_MAX_NODES) {
    return false;
  }

  return output_stream_volume[card][stream][port];
}


bool RDHPISoundCard::haveInputLevel(int card,int port) const
{
  if(card>=HPI_MAX_ADAPTERS||port>=HPI_MAX_NODES) {
    return false;
  }

  return input_port_level[card][port];
}


bool RDHPISoundCard::haveOutputLevel(int card,int port) const
{
  if(card>=HPI_MAX_ADAPTERS||port>=HPI_MAX_NODES) {
    return false;
  }

  return output_port_level[card][port];
}


bool RDHPISoundCard::haveInputStreamVOX(int card,int stream) const
{
  if(card>=HPI_MAX_ADAPTERS||stream>=HPI_MAX_STREAMS) {
    return false;
  }

  return input_stream_vox[card][stream];
}


RDHPISoundCard::SourceNode RDHPISoundCard::getInputPortMux(int card,int port)
{
  uint16_t type;
  uint16_t index;

  LogHpi(HPI_Multiplexer_GetSource(NULL,input_mux_control[card][port],
				   &type,&index),__LINE__);
  return (RDHPISoundCard::SourceNode)type;
}


bool RDHPISoundCard::setInputPortMux(int card,int port,RDHPISoundCard::SourceNode node)
{
  switch(node) {
  case RDHPISoundCard::LineIn:
    if(HPI_Multiplexer_SetSource(NULL,
				 input_mux_control[card][port],node,0)!=0) {
      return false;
    }
    break;

  case RDHPISoundCard::AesEbuIn:
    if(LogHpi(HPI_Multiplexer_SetSource(NULL,
					input_mux_control[card][port],node,
					input_mux_index[card][port][1]),
	      __LINE__)!=0) {
      return false;
    }
    break;

  default:
    return false;
    break;
  }
  return true;
}


unsigned short RDHPISoundCard::getInputPortError(int card,int port)
{
  uint16_t error_word=0;

  if(input_port_aesebu[card][port]) {
    if(HPI_AESEBU_Receiver_GetErrorStatus(NULL,
					  input_port_aesebu_control[card][port],
					  &error_word)==0) {
      //
      // Do nothing
      //
      // Needed to suppress the goofy 'result unused' compiler warnings.
      //
    }
  }
  return error_word;
}


RDHPISoundCard::FadeProfile RDHPISoundCard::getFadeProfile() const
{
  return fade_type;
}


void RDHPISoundCard::setFadeProfile(RDHPISoundCard::FadeProfile profile)
{
  fade_type=profile;
  switch(fade_type) {
  case RDHPISoundCard::Linear:
    hpi_fade_type=HPI_VOLUME_AUTOFADE_LINEAR;
    break;

  case RDHPISoundCard::Log:
    hpi_fade_type=HPI_VOLUME_AUTOFADE_LOG;
    break;
  }
}


RDConfig *RDHPISoundCard::config() const
{
  return card_config;
}


bool RDHPISoundCard::haveInputStreamMeter(int card,int stream) const
{
  return input_stream_meter[card][stream];
}


bool RDHPISoundCard::haveInputPortMeter(int card,int port) const
{
  return input_stream_meter[card][port];
}


bool RDHPISoundCard::haveOutputStreamMeter(int card,int stream) const
{
  return output_stream_meter[card][stream];
}


bool RDHPISoundCard::haveOutputPortMeter(int card,int port) const
{
  return output_port_meter[card][port];
}


bool RDHPISoundCard::haveTuner(int card,int port) const
{
  return false;
}


void RDHPISoundCard::setTunerBand(int card,int port,
				 RDHPISoundCard::TunerBand band)
{
}


int RDHPISoundCard::tunerFrequency(int card,int port)
{
  return 0;
}


void RDHPISoundCard::setTunerFrequency(int card,int port,int freq)
{
}


bool RDHPISoundCard::tunerSubcarrier(int card,int port,
				    RDHPISoundCard::Subcarrier sub)
{
  return false;
}


int RDHPISoundCard::tunerLowFrequency(int card,int port,
				     RDHPISoundCard::TunerBand band)
{
  return 0;
}


int RDHPISoundCard::tunerHighFrequency(int card,int port,
				      RDHPISoundCard::TunerBand band)
{
  return 0;
}


bool RDHPISoundCard::inputStreamMeter(int card,int stream,short *level)
{
  if(card>=card_quantity) {
    return false;
  }
  if(stream>=card_input_streams[card]) {
    return false;
  }
  LogHpi(HPI_MeterGetPeak(NULL,
			  input_stream_meter_control[card][stream],level),
	 __LINE__);
  return true;
}


bool RDHPISoundCard::outputStreamMeter(int card,int stream,short *level)
{
  if(card>=card_quantity) {
    return false;
  }
  if(stream>=card_output_streams[card]) {
    return false;
  }
  LogHpi(HPI_MeterGetPeak(NULL,output_stream_meter_control[card][stream],
			  level),__LINE__);
  return true;
}


bool RDHPISoundCard::inputPortMeter(int card,int port,short *level)
{
  if(card>=card_quantity) {
    return false;
  }
  if(port>=card_input_ports[card]) {
    return false;
  }
  LogHpi(HPI_MeterGetPeak(NULL,input_port_meter_control[card][port],level),
	 __LINE__);
  return true;
}


bool RDHPISoundCard::outputPortMeter(int card,int port,short *level)
{
  if(card>=card_quantity) {
    return false;
  }
  if(port>=card_output_ports[card]) {
    return false;
  }
  LogHpi(HPI_MeterGetPeak(NULL,output_port_meter_control[card][port],level),
	 __LINE__);
  return true;
}


bool RDHPISoundCard::haveInputMode(int card,int port) const
{
  return input_port_mode[card][port];
}


bool RDHPISoundCard::haveOutputMode(int card,int stream) const
{
  return output_stream_mode[card][stream];
}


bool RDHPISoundCard::haveInputPortMux(int card,int port) const
{
  return input_port_mux[card][port];
}


bool RDHPISoundCard::queryInputPortMux(int card,int port,SourceNode node) const
{
  switch(node) {
  case RDHPISoundCard::LineIn:
    return input_port_mux_type[card][port][0];
    break;

  case RDHPISoundCard::AesEbuIn:
    return input_port_mux_type[card][port][1];
    break;

  default:
    return false;
    break;
  }
}


bool RDHPISoundCard::haveInputStreamMux(int card,int stream) const
{
  return input_stream_mux[card][stream];
}


int RDHPISoundCard::getInputVolume(int card,int stream,int port)
{
  short gain[2];

  LogHpi(HPI_VolumeGetGain(NULL,input_stream_volume_control[card][stream][port],
			   gain),__LINE__);
  return gain[0];
}


int RDHPISoundCard::getOutputVolume(int card,int stream,int port)
{
  short gain[2];

  LogHpi(HPI_VolumeGetGain(NULL,output_stream_volume_control[card][stream][port],gain),__LINE__);
  return gain[0];
}


int RDHPISoundCard::getInputLevel(int card,int port)
{
  short gain[2];

  LogHpi(HPI_VolumeGetGain(NULL,input_port_level_control[card][port],gain),
	 __LINE__);
  return gain[0];
}


int RDHPISoundCard::getOutputLevel(int card,int port)
{
  short gain[2];

  LogHpi(HPI_VolumeGetGain(NULL,output_port_level_control[card][port],gain),
	 __LINE__);
  return gain[0];
}



void RDHPISoundCard::setInputVolume(int card,int stream,int level)
{
  if(!haveInputVolume(card,stream,0)) {
    return;
  }
  short gain[2];
  gain[0]=level;
  gain[1]=level;
  LogHpi(HPI_VolumeSetGain(NULL,input_stream_volume_control[card][stream][0],
			   gain),__LINE__);
}


void RDHPISoundCard::setOutputVolume(int card,int stream,int port,int level)
{
  if(!haveOutputVolume(card,stream,port)) {
    return;
  }
  short gain[2];
  gain[0]=level;
  gain[1]=level;
  LogHpi(HPI_VolumeSetGain(NULL,
			   output_stream_volume_control[card][stream][port],
			   gain),__LINE__);
}



void RDHPISoundCard::fadeOutputVolume(int card,int stream,int port,
				     int level,int length)
{
  if(!haveOutputVolume(card,stream,port)) {
    return;
  }
  short gain[2];

  gain[0]=level;
  gain[1]=level;
  LogHpi(HPI_VolumeAutoFadeProfile(NULL,
			      output_stream_volume_control[card][stream][port],
				   gain,length,hpi_fade_type),__LINE__);
}


void RDHPISoundCard::setInputLevel(int card,int port,int level)
{
  short gain[HPI_MAX_CHANNELS];

  if(!haveInputLevel(card,port)) {
    return;
  }
  for(unsigned i=0;i<HPI_MAX_CHANNELS;i++) {
    gain[i]=level;
  }
  LogHpi(HPI_LevelSetGain(NULL,input_port_level_control[card][port],gain),
	 __LINE__);
}


void RDHPISoundCard::setOutputLevel(int card,int port,int level)
{
  short gain[HPI_MAX_CHANNELS];

  if(!haveOutputLevel(card,port)) {
    return;
  }
  for(unsigned i=0;i<HPI_MAX_CHANNELS;i++) {
    gain[i]=level;
  }
  LogHpi(HPI_LevelSetGain(NULL,output_port_level_control[card][port],gain),
	 __LINE__);
}


void RDHPISoundCard::setInputMode(int card,int port,
				 RDHPISoundCard::ChannelMode mode)
{
  if(!haveInputMode(card,port)) {
    return;
  }
  LogHpi(HPI_ChannelModeSet(NULL,input_port_mode_control[card][port],mode+1),
	 __LINE__);
}


void RDHPISoundCard::setOutputMode(int card,int stream,
				  RDHPISoundCard::ChannelMode mode)
{
  if(!haveOutputMode(card,stream)) {
    return;
  }
  LogHpi(HPI_ChannelModeSet(NULL,output_stream_mode_control[card][stream],
			    mode+1),__LINE__);
}


void RDHPISoundCard::setInputStreamVOX(int card,int stream,short gain)
{
  LogHpi(HPI_VoxSetThreshold(NULL,input_stream_vox_control[card][stream],gain),
	 __LINE__);
}


bool RDHPISoundCard::havePassthroughVolume(int card,int in_port,int out_port)
{
  return passthrough_port_volume[card][in_port][out_port];
}


bool RDHPISoundCard::setPassthroughVolume(int card,int in_port,int out_port,
					 int level)
{
  if(!passthrough_port_volume[card][in_port][out_port]) {
    return false;
  }
  short gain[2];
  gain[0]=level;
  gain[1]=level;
  LogHpi(HPI_VolumeSetGain(NULL,
		       passthrough_port_volume_control[card][in_port][out_port],
			   gain),__LINE__);
  return true;
}


void RDHPISoundCard::clock()
{
  uint16_t error_word;

  for(int i=0;i<card_quantity;i++) {
    for(int j=0;j<HPI_MAX_NODES;j++) {
      if(input_port_aesebu[i][j]) {
	error_word=getInputPortError(i,j);
	if(error_word!=input_port_aesebu_error[i][j]) {
	  input_port_aesebu_error[i][j]=error_word;
	  emit inputPortError(i,j);
	}
      }
    }
  }
}


void RDHPISoundCard::HPIProbe()
{
  uint16_t hpi_adapter_list[HPI_MAX_ADAPTERS];
  uint32_t dummy_serial;
  uint32_t dummy_hpi;
  uint16_t dummy_version;
  uint16_t dummy_type;
  uint16_t l;
  uint16_t type;
  uint16_t index;
  QString str;

  hpi_fade_type=HPI_VOLUME_AUTOFADE_LOG;
#if HPI_VER < 0x00030600
  LogHpi(HPI_SubSysGetVersion(NULL,&dummy_hpi),__LINE__);
  LogHpi(HPI_SubSysFindAdapters(NULL,(uint16_t *)&card_quantity,
				hpi_adapter_list,HPI_MAX_ADAPTERS),__LINE__);  
#else
  LogHpi(HPI_SubSysGetVersionEx(NULL,&dummy_hpi),__LINE__);
  LogHpi(HPI_SubSysGetNumAdapters(NULL,&card_quantity),__LINE__);
#endif  // HPI_VER
  for(int i=0;i<card_quantity;i++) {
#if HPI_VER < 0x00030600
    card_index[i]=i;
#else
    LogHpi(HPI_SubSysGetAdapter(NULL,i,card_index+i,hpi_adapter_list+i),
	   __LINE__);
#endif  // HPI_VER
    if((hpi_adapter_list[i]&0xF000)==0x6000) { 
      timescale_support[i]=true;
    }
    else {
      timescale_support[i]=false;
    }
    switch(hpi_adapter_list[i]) {
    case 0x5111:
    case 0x5211:
      input_mux_type[i]=true;
      break;

    default:
      input_mux_type[i]=false;
      break;
    }
    card_input_ports[i]=0;
    card_output_ports[i]=0;
    card_description[i]=QString().sprintf("AudioScience %04X [%d]",
					  hpi_adapter_list[i],i+1);
    LogHpi(HPI_AdapterOpen(NULL,card_index[i]),__LINE__);
    LogHpi(HPI_AdapterGetInfo(NULL,card_index[i],
			      &card_output_streams[i],
			      &card_input_streams[i],
			      &dummy_version,(uint32_t *)&dummy_serial,
			      &dummy_type),__LINE__);
    hpi_info[i].setSerialNumber(dummy_serial);
    hpi_info[i].setHpiVersion(dummy_hpi);
    hpi_info[i].setDspMajorVersion((dummy_version>>13)&7);
    hpi_info[i].setDspMinorVersion((dummy_version>>7)&63);
    hpi_info[i].setPcbVersion((char)(((dummy_version>>3)&7)+'A'));
    hpi_info[i].setAssemblyVersion(dummy_version&7);
    LogHpi(HPI_AdapterClose(NULL,card_index[i]),__LINE__);
    str=QString(tr("Input Stream"));
    for(int j=0;j<card_input_streams[i];j++) {
      input_stream_description[i][j]=
	QString().sprintf("%s - %s %d",card_description[i].toUtf8().constData(),
			  str.toUtf8().constData(),j+1);
    }
    str=QString(tr("Output Stream"));
    for(int j=0;j<card_output_streams[i];j++) {
      output_stream_description[i][j]=
	QString().sprintf("%s - %s %d",
			  card_description[i].toUtf8().constData(),
			  str.toUtf8().constData(),j+1);
    }
  }

  //
  // Mixer Initialization
  //
  for(int i=0;i<card_quantity;i++) {
    LogHpi(HPI_MixerOpen(NULL,card_index[i],&hpi_mixer[i]),__LINE__);

    //
    // Get Input Ports
    //
    str=QString(tr("Input Port"));
    for(int k=0;k<HPI_MAX_NODES;k++) {    
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],
			     0,0,
			     HPI_DESTNODE_ISTREAM,k,
			     HPI_CONTROL_MULTIPLEXER,
			     &input_stream_volume_control[i][0][k])==0) {
	card_input_ports[i]++;
	input_port_description[i][k]=
	  QString().sprintf("%s - %s %d",
			    card_description[i].toUtf8().constData(),
			    str.toUtf8().constData(),card_input_ports[i]);
      }

      //
      // Get Input Mode Controls
      //
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],0,0,
			     HPI_DESTNODE_ISTREAM,k,
			     HPI_CONTROL_CHANNEL_MODE,
			     &input_port_mode_control[i][k])==0) {
	input_port_mode[i][k]=true;
      }
    }

    //
    // Get Output Ports
    //
    str=QString(tr("Output Port"));
    for(int k=0;k<HPI_MAX_NODES;k++) {
      if((HPI_MixerGetControl(NULL,hpi_mixer[i],
			      HPI_SOURCENODE_OSTREAM,0,
			      HPI_DESTNODE_LINEOUT,k,
			      HPI_CONTROL_VOLUME,
			      &output_stream_volume_control[i][0][k])==0)||
	 (HPI_MixerGetControl(NULL,hpi_mixer[i],
			      HPI_SOURCENODE_OSTREAM,0,
			      HPI_DESTNODE_AESEBU_OUT,k,
			      HPI_CONTROL_VOLUME,
			      &output_stream_volume_control[i][0][k])==0)) {
	output_stream_volume[i][0][k]=true;
	card_output_ports[i]++;
	output_port_description[i][k]=
	  QString().sprintf("%s - %s %d",
			    card_description[i].toUtf8().constData(),
			    str.toUtf8().constData(),card_output_ports[i]);
      }
    }
    LogHpi(HPI_MixerGetControl(NULL,hpi_mixer[i],
			       HPI_SOURCENODE_CLOCK_SOURCE,0,
			       0,0,
			       HPI_CONTROL_SAMPLECLOCK,
			       &clock_source_control[i]),__LINE__);
    for(int j=0;j<card_input_streams[i];j++) {
      if(LogHpi(HPI_MixerGetControl(NULL,hpi_mixer[i],  // VOX Controls
				    0,0,
				    HPI_DESTNODE_ISTREAM,j,
				    HPI_CONTROL_VOX,
				    &input_stream_vox_control[i][j]),
		__LINE__)==0) {
	input_stream_vox[i][j]=true;
      }
      else {
	input_stream_vox[i][j]=false;
      }

      if(input_mux_type[i]) {
	if(LogHpi(HPI_MixerGetControl(NULL,hpi_mixer[i],  // MUX Controls
				      0,0,
				      HPI_DESTNODE_ISTREAM,j,
				      HPI_CONTROL_MULTIPLEXER,
				      &input_mux_control[i][j]),__LINE__)==0) {
	  input_stream_mux[i][j]=true;
	  l=0;
	  input_port_mux_type[i][j][0]=false;
	  input_port_mux_type[i][j][1]=false;
	  while(LogHpi(HPI_Multiplexer_QuerySource(NULL,
						   input_mux_control[i][j],
						   l++,&type,&index),
		       __LINE__)==0) {
	    switch(type) {
	    case HPI_SOURCENODE_LINEIN:
	      input_port_mux_type[i][j][0]=true;
	      input_mux_index[i][j][0]=index;
	      break;

	    case HPI_SOURCENODE_AESEBU_IN:
	      input_port_mux_type[i][j][1]=true;
	      input_mux_index[i][j][1]=index;
	      break;
	    }
	  }
	}
	else {
	  input_stream_mux[i][j]=false;
	}
      }
    }
    for(int j=0;j<card_output_streams[i];j++) {
      for(int k=0;k<HPI_MAX_NODES;k++) {
	if(HPI_MixerGetControl(NULL,hpi_mixer[i],
			       HPI_SOURCENODE_LINEIN,j,
			       HPI_DESTNODE_ISTREAM,k,
			       HPI_CONTROL_VOLUME,
			       &input_stream_volume_control[i][j][k])==0) {
	  input_stream_volume[i][j][k]=true;
	}
	else {
	  input_stream_volume[i][j][k]=false;
	}
	if((HPI_MixerGetControl(NULL,hpi_mixer[i],
				HPI_SOURCENODE_OSTREAM,j,
				HPI_DESTNODE_LINEOUT,k,
				HPI_CONTROL_VOLUME,
				&output_stream_volume_control[i][j][k])==0)||
	   (HPI_MixerGetControl(NULL,hpi_mixer[i],
				HPI_SOURCENODE_OSTREAM,j,
				HPI_DESTNODE_AESEBU_OUT,k,
				HPI_CONTROL_VOLUME,
				&output_stream_volume_control[i][j][k])==0)) {
	  output_stream_volume[i][j][k]=true;
	}
	else {
	  output_stream_volume[i][j][k]=false;
	}
      }
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],
			     0,0,
			     HPI_DESTNODE_ISTREAM,j,
			     HPI_CONTROL_METER,
			     &input_stream_meter_control[i][j])==0) {
	input_stream_meter[i][j]=true;
      }
      else {
	input_stream_meter[i][j]=false;
      }
      if(LogHpi(HPI_MixerGetControl(NULL,hpi_mixer[i],
				    HPI_SOURCENODE_OSTREAM,j,
				    0,0,
				    HPI_CONTROL_METER,
				    &output_stream_meter_control[i][j]),
		__LINE__)==0) {
	output_stream_meter[i][j]=true;
      }
      else {
	output_stream_meter[i][j]=false;
      }
    }
    for(int j=0;j<HPI_MAX_NODES;j++) {
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],  // Input Level Controls
			     HPI_SOURCENODE_LINEIN,j,
			     0,0,
			     HPI_CONTROL_LEVEL,
			     &input_port_level_control[i][j])==0) {
	input_port_level[i][j]=true;
      }
      else {
	input_port_level[i][j]=false;
      }
      if((HPI_MixerGetControl(NULL,hpi_mixer[i],  // Output Level Controls
			      0,0,
			      HPI_DESTNODE_LINEOUT,j,
			      HPI_CONTROL_LEVEL,
			      &output_port_level_control[i][j])==0)||
	 (HPI_MixerGetControl(NULL,hpi_mixer[i],
			      0,0,
			      HPI_DESTNODE_AESEBU_OUT,j,
			      HPI_CONTROL_LEVEL,
			      &output_port_level_control[i][j])==0)) {
	output_port_level[i][j]=true;
      }
      else {
	output_port_level[i][j]=false;
      }
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],  // Input Port Meter
			     HPI_SOURCENODE_LINEIN,j,
			     0,0,
			     HPI_CONTROL_METER,
			     &input_port_meter_control[i][j])==0) {
	input_port_meter[i][j]=true;
      }
      else {
	input_port_meter[i][j]=false;
      }
      if((HPI_MixerGetControl(NULL,hpi_mixer[i],  // Output Port Meter
			     0,0,
			     HPI_DESTNODE_LINEOUT,j,
			     HPI_CONTROL_METER,
			     &output_port_meter_control[i][j])==0)||
	 (HPI_MixerGetControl(NULL,hpi_mixer[i],
			      0,0,
			      HPI_DESTNODE_AESEBU_OUT,j,
			      HPI_CONTROL_METER,
			      &output_port_meter_control[i][j])==0)) {
	output_port_meter[i][j]=true;
      }
      else {
	output_port_meter[i][j]=false;
      }
      if(HPI_MixerGetControl(NULL,hpi_mixer[i],  // Input Port AES/EBU
			     HPI_SOURCENODE_AESEBU_IN,j,
			     0,0,
			     HPI_CONTROL_AESEBU_RECEIVER,
			     &input_port_aesebu_control[i][j])==0) {
	input_port_aesebu[i][j]=true;
      }
      else {
	input_port_aesebu[i][j]=false;
      }
      if(!input_mux_type[i]) {
	if(HPI_MixerGetControl(NULL,hpi_mixer[i],  // Input Port Mux
			       HPI_SOURCENODE_LINEIN,j,
			       0,0,
			       HPI_CONTROL_MULTIPLEXER,
			       &input_mux_control[i][j])==0) {
	  input_port_mux[i][j]=true;
	  l=0;
	  input_port_mux_type[i][j][0]=false;
	  input_port_mux_type[i][j][1]=false;
	  while(HPI_Multiplexer_QuerySource(NULL,input_mux_control[i][j],
					    l++,&type,&index)==0) {
	    switch(type) {
	    case HPI_SOURCENODE_LINEIN:
	      input_port_mux_type[i][j][0]=true;
	      input_mux_index[i][j][0]=index;
	      break;

	    case HPI_SOURCENODE_AESEBU_IN:
	      input_port_mux_type[i][j][1]=true;
	      input_mux_index[i][j][1]=index;
	      break;
	    }
	  }
	}
	else {
	  input_port_mux[i][j]=false;
	}
      }
    }

    //
    // Get The Passthroughs
    //
    for(int j=0;j<HPI_MAX_NODES;j++) {
      for(int k=0;k<HPI_MAX_NODES;k++) {
	if((HPI_MixerGetControl(NULL,hpi_mixer[i],
				HPI_SOURCENODE_LINEIN,j,
				HPI_DESTNODE_LINEOUT,k,
				HPI_CONTROL_VOLUME,
				&passthrough_port_volume_control[i][j][k])==0)||
	   (HPI_MixerGetControl(NULL,hpi_mixer[i],
				HPI_SOURCENODE_LINEIN,j,
				HPI_DESTNODE_AESEBU_OUT,k,
				HPI_CONTROL_VOLUME,
				&passthrough_port_volume_control[i][j][k])==0)) {
	  passthrough_port_volume[i][j][k]=true;
	}
	else {
	  passthrough_port_volume[i][j][k]=false;
	}
      }
    }
  }
  clock_timer=new QTimer(this);
  connect(clock_timer,SIGNAL(timeout()),this,SLOT(clock()));
  clock_timer->start(METER_INTERVAL);
}


hpi_err_t RDHPISoundCard::LogHpi(hpi_err_t err,int lineno)
{
  char err_txt[200];

  if(err!=0) {
    HPI_GetErrorText(err,err_txt);
    RDApplication::syslog(card_config,LOG_WARNING,
			  "HPI Error: %s, %s line %d",err_txt,__FILE__,lineno);
  }
  return err;
}
