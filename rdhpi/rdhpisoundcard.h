//   rdhpisoundcard.h
//
//   Sound card subsystem for the HPI Library.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdhpisoundcard.h,v 1.5.6.3 2012/08/07 15:48:04 cvs Exp $
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
//

#ifndef RDHPISOUNDCARD_H
#define RDHPISOUNDCARD_H

#define METER_INTERVAL 20

#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>

#include <rdhpiinformation.h>

#include <asihpi/hpi.h>
#ifndef HPI_VER
#include <asihpi/hpi_version.h>
#endif

#if HPI_VER < 0x40411
typedef uint16_t hpi_err_t;
typedef uint32_t hpi_handle_t;
#endif

/*
 * Definitions
 */
#define RDHPISOUNDCARD_HPI_MAX_LEVEL 2400
#define RDHPISOUNDCARD_HPI_MIN_LEVEL 0


class RDHPISoundCard : public QObject
{
  Q_OBJECT;

 public:
  enum FadeProfile {Linear=0,Log=1};
  enum Channel {Left=0,Right=1};
  enum ChannelMode {Normal=0,      // HPI_CHANNEL_MODE_NORMAL-1
		    Swap=1,        // HPI_CHANNEL_MODE_SWAP-1
		    LeftOnly=2,    // HPI_CHANNEL_MODE_LEFT_TO_STEREO-1
		    RightOnly=3};  // HPI_CHANNEL_MODE_RIGHT_TO_STEREO-1
  enum DeviceClass {RecordDevice=0,PlayDevice=1};
  enum Driver {Alsa=0,Hpi=1,Jack=2};
  enum ClockSource {Internal=0,AesEbu=1,SpDiff=2,WordClock=4};
  enum SourceNode {SourceBase=100, // HPI_SOURCENODE_BASE
		   OStream=101,    // HPI_SOURCENODE_OSTREAM
		   LineIn=102,     // HPI_SOURCENODE_LINEIN
		   AesEbuIn=103,   // HPI_SOURCENODE_AESEBU_IN
		   Tuner=104,      // HPI_SOURCENODE_TUNER
		   RfIn=105,       // HPI_SOURCENODE_RF
		   Clock=106,      // HPI_SOURCENODE_CLOCK_SOURCE
		   Raw=107,        // HPI_SOURCENODE_RAW_BITSTREAM
		   Mic=108         // HPI_SOURCENODE_MICROPHONE
  };
  
  /**
   * Mixer Destination Nodes
   **/
  enum DestNode {DestBase=200,     // HPI_DESTNODE_BASE
		 IStream=201,      // HPI_DESTNODE_ISTREAM
		 LineOut=202,      // HPI_DESTNODE_LINEOUT
		 AesEbuOut=203,    // HPI_DESTNODE_AESEBU_OUT
		 RfOut=204,        // HPI_DESTNODE_RF
		 Speaker=205       // HPI_DESTNODE_SPEAKER
  };
  
  enum TunerBand {Fm=0,FmStereo=1,Am=2,Tv=3};
  
  enum Subcarrier {Mpx=0,Rds=1};
  RDHPISoundCard(QObject *parent=0,const char *name=0);
  ~RDHPISoundCard();
  Driver driver() const;
  RDHPIInformation *hpiInformation(int card);
  int getCardQuantity() const;
  int getCardInputStreams(int card) const;
  int getCardOutputStreams(int card) const;
  int getCardInputPorts(int card) const;
  int getCardOutputPorts(int card) const;
  const void *getCardInfo(int card) const;
  QString getCardDescription(int card) const;
  QString getInputStreamDescription(int card,int stream) const;
  QString getOutputStreamDescription(int card,int stream) const;
  QString getInputPortDescription(int card,int port) const;
  QString getOutputPortDescription(int card,int port) const;
  bool setClockSource(int card,RDHPISoundCard::ClockSource src);
  bool haveTimescaling(int card) const;
  bool haveInputVolume(int card,int stream,int port) const;
  bool haveOutputVolume(int card,int stream,int port) const;
  bool haveInputLevel(int card,int port) const;
  bool haveOutputLevel(int card,int port) const;
  bool haveInputStreamMeter(int card,int stream) const;
  bool haveOutputStreamMeter(int card,int stream) const;
  bool haveInputPortMeter(int card,int port) const;
  bool haveOutputPortMeter(int card,int port) const;
  bool haveTuner(int card,int port) const;
  RDHPISoundCard::TunerBand tunerBand(int card,int port);
  void setTunerBand(int card,int port,RDHPISoundCard::TunerBand band);
  int tunerFrequency(int card,int port);
  void setTunerFrequency(int card,int port,int freq);
  bool tunerSubcarrier(int card,int port,RDHPISoundCard::Subcarrier sub);
  int tunerLowFrequency(int card,int port,RDHPISoundCard::TunerBand band);
  int tunerHighFrequency(int card,int port,RDHPISoundCard::TunerBand band);
  bool inputStreamMeter(int card,int stream,short *level);
  bool outputStreamMeter(int card,int stream,short *level);
  bool inputPortMeter(int card,int port,short *level);
  bool outputPortMeter(int card,int port,short *level);
  bool haveInputMode(int card,int port) const;
  bool haveOutputMode(int card,int stream) const;
  bool haveInputStreamVOX(int card,int stream) const;
  bool haveInputPortMux(int card,int port) const;
  bool queryInputPortMux(int card,int port,SourceNode node) const;
  bool haveInputStreamMux(int card,int stream) const;
  int getInputVolume(int card,int stream,int port);
  int getOutputVolume(int card,int stream,int port);
  int getInputLevel(int card,int port);
  int getOutputLevel(int card,int port);
  RDHPISoundCard::SourceNode getInputPortMux(int card,int port);
  bool setInputPortMux(int card,int port,RDHPISoundCard::SourceNode node);
  RDHPISoundCard::FadeProfile getFadeProfile() const;
  unsigned short getInputPortError(int card,int port);
  void setFadeProfile(RDHPISoundCard::FadeProfile profile);
  
 signals:
  void inputPortError(int card,int port);
  void leftInputStreamMeter(int card,int stream,int level);
  void leftOutputStreamMeter(int card,int stream,int level);
  void rightInputStreamMeter(int card,int stream,int level);
  void rightOutputStreamMeter(int card,int stream,int level);
  void leftInputPortMeter(int card,int port,int level);
  void leftOutputPortMeter(int card,int port,int level);
  void rightInputPortMeter(int card,int port,int level);
  void rightOutputPortMeter(int card,int port,int level);
  void inputMode(int card,int port,RDHPISoundCard::ChannelMode mode);
  void outputMode(int card,int stream,RDHPISoundCard::ChannelMode mode);
  void tunerSubcarrierChanged(RDHPISoundCard::Subcarrier car,bool state);
  
 public slots:
  void setInputVolume(int card,int stream,int level);
  void setOutputVolume(int card,int stream,int port,int level);
  void fadeOutputVolume(int card,int stream,int port,int level,int length);
  void setInputLevel(int card,int port,int level);
  void setOutputLevel(int card,int port,int level);
  void setInputMode(int card,int port,RDHPISoundCard::ChannelMode mode);
  void setOutputMode(int card,int stream,RDHPISoundCard::ChannelMode mode);
  void setInputStreamVOX(int card,int stream,short gain);
  bool havePassthroughVolume(int card,int in_port,int out_port);
  bool setPassthroughVolume(int card,int in_port,int out_port,int level);
  
 private slots:
  void clock();
  
 private:
  void HPIProbe();
  hpi_err_t LogHpi(hpi_err_t err);
  uint16_t card_input_streams[HPI_MAX_ADAPTERS];
  uint16_t card_output_streams[HPI_MAX_ADAPTERS];
  uint16_t card_input_ports[HPI_MAX_ADAPTERS];
  uint16_t card_output_ports[HPI_MAX_ADAPTERS];
  QString card_description[HPI_MAX_ADAPTERS];
  QString input_stream_description[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  QString output_stream_description[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  QString input_port_description[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  QString output_port_description[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool input_stream_volume[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS][HPI_MAX_NODES];
  bool output_stream_volume[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS][HPI_MAX_NODES];
  bool input_port_level[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool output_port_level[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool input_stream_meter[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool output_stream_meter[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool input_port_meter[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool output_port_meter[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool input_port_mode[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool output_stream_mode[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool input_stream_vox[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool input_port_mux[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool input_port_mux_type[HPI_MAX_ADAPTERS][HPI_MAX_NODES][2];
  bool passthrough_port_volume[HPI_MAX_ADAPTERS][HPI_MAX_NODES][HPI_MAX_NODES];
  uint16_t input_mux_index[HPI_MAX_ADAPTERS][HPI_MAX_NODES][2];
  bool input_stream_mux[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool input_port_aesebu[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  uint16_t input_port_aesebu_error[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  bool timescale_support[HPI_MAX_ADAPTERS];
  int card_quantity;
  QTimer *clock_timer;
  RDHPISoundCard::FadeProfile fade_type;
  uint32_t card_index[HPI_MAX_ADAPTERS];
  hpi_handle_t hpi_mixer[HPI_MAX_ADAPTERS];
  hpi_handle_t clock_source_control[HPI_MAX_ADAPTERS];
  hpi_handle_t input_stream_volume_control[HPI_MAX_ADAPTERS]
    [HPI_MAX_STREAMS][HPI_MAX_NODES];
  hpi_handle_t output_stream_volume_control[HPI_MAX_ADAPTERS]
    [HPI_MAX_STREAMS][HPI_MAX_NODES];
  hpi_handle_t input_port_level_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  hpi_handle_t output_port_level_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  hpi_handle_t input_stream_meter_control[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  hpi_handle_t output_stream_meter_control[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  hpi_handle_t input_port_meter_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  hpi_handle_t output_port_meter_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  hpi_handle_t input_port_mode_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  hpi_handle_t output_stream_mode_control[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  hpi_handle_t input_stream_vox_control[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  bool input_mux_type[HPI_MAX_ADAPTERS];
  hpi_handle_t input_mux_control[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS];
  hpi_handle_t passthrough_port_volume_control[HPI_MAX_ADAPTERS]
    [HPI_MAX_NODES][HPI_MAX_NODES];
  hpi_handle_t input_port_aesebu_control[HPI_MAX_ADAPTERS][HPI_MAX_NODES];
  uint16_t hpi_fade_type;
  short 
    input_stream_meter_peak[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS][HPI_MAX_CHANNELS];
  short 
     output_stream_meter_peak[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS][HPI_MAX_CHANNELS];
   short input_port_meter_peak[HPI_MAX_ADAPTERS][HPI_MAX_NODES][HPI_MAX_CHANNELS];
   short output_port_meter_peak[HPI_MAX_ADAPTERS][HPI_MAX_NODES][HPI_MAX_CHANNELS];
   RDHPIInformation hpi_info[HPI_MAX_ADAPTERS];
};


#endif  // RDHPISOUNDCARD_H
