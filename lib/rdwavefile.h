//   rdwavefile.h
//
//   A class for handling audio files.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDWAVEFILE_H
#define RDWAVEFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>

#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QString>

#ifdef HAVE_VORBIS
#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>
#endif  // HAVE_VORBIS

#include <rdmp4.h>
#include <rdringbuffer.h>
#include <rdsettings.h>
#include <rdwavedata.h>

//
// Number of timers allowed in the CartChunk structure.
// The official number is '8'.
//
#define MAX_TIMERS 8

//
// Define this if we should guesstimate the overall sample count
// of a MPEG file with a missing or zeroed 'fact' chunk.  (Technically,
// such a file is invalid, but there are lots of them floating
// around out there nonetheless.)  Otherwise, return '0' for the
// sample count.
//
#define MPEG_FACT_FUDGE

//
// The default mode bits for new WAV files.
//
#define WAVE_MODE_BITS 0644

/*
 * Chunk Sizes
 */
#define FMT_CHUNK_SIZE 40
#define FACT_CHUNK_SIZE 4
#define CART_CHUNK_SIZE 2048
#define BEXT_CHUNK_SIZE 602
#define MEXT_CHUNK_SIZE 12
#define LEVL_CHUNK_SIZE 128
#define SCOT_CHUNK_SIZE 424
#define AV10_CHUNK_SIZE 512
#define AIR1_CHUNK_SIZE 2048
#define COMM_CHUNK_SIZE 18
#define RDXL_CHUNK_SIZE 4

//
// Maximum Header Size for ATX Files
#define MAX_ATX_HEADER_SIZE 512

//
// The size of the MPEG Ring Buffer
//
#define MPEG_BUFFER_SIZE 32768

//
// Default Values
//
#define DEFAULT_LEVL_FORMAT 2
#define DEFAULT_LEVL_POINTS 1
#define DEFAULT_LEVL_BLOCK_SIZE 1152

class RDWaveFile
{
 public:
  enum Format {Pcm8=0,Pcm16=1,Float32=2,MpegL1=3,MpegL2=4,MpegL3=5,
  	       DolbyAc2=6,DolbyAc3=7,Vorbis=8,Pcm24=9};
  enum Type {Unknown=0,Wave=1,Mpeg=2,Ogg=3,Atx=4,Tmc=5,Flac=6,Ambos=7,
	     Aiff=8,M4A=9};
  RDWaveFile(QString file_name="");
  ~RDWaveFile();
  RDWaveFile::Type type() const;
  void nameWave(QString file_name);
  bool createWave(RDWaveData *data=NULL,unsigned ptr_offset=0);
  bool openWave(RDWaveData *data=NULL);
  void closeWave(int samples=-1);
  void resetWave();
  bool getFormatChunk() const;
  bool getFactChunk() const;
  unsigned getSampleLength() const;
  unsigned getTimeLength() const;
  unsigned getExtTimeLength() const;
  bool getDataChunk() const;
  unsigned getDataLength() const;
  int readWave(void *buf,int count);
  int writeWave(void *buf,int count);
  int seekWave(int offset,int whence);
  void getSettings(RDSettings *settings);
  void setSettings(const RDSettings *settings);
  bool hasEnergy();
  unsigned energySize();
  unsigned short energy(unsigned frame);
  int readEnergy(unsigned short buf[],int count);
  int startTrim(int level);
  int endTrim(int level);
  QString getName() const;
  unsigned short getFormatTag() const;
  void setFormatTag(unsigned short format);
  unsigned short getChannels() const;
  void setChannels(unsigned short chan);
  unsigned getSamplesPerSec() const;
  void setSamplesPerSec(unsigned rate);
  unsigned getAvgBytesPerSec() const;
  unsigned short getBlockAlign() const;
  unsigned short getBitsPerSample() const;
  void setBitsPerSample(unsigned short bits);
  unsigned short getHeadLayer() const;
  void setHeadLayer(unsigned short layer);
  unsigned getHeadBitRate() const;
  void setHeadBitRate(unsigned rate);
  unsigned short getHeadMode() const;
  void setHeadMode(unsigned short mode);
  unsigned getHeadModeExt() const;
  unsigned getHeadEmphasis() const;
  unsigned short getHeadFlags() const;
  void setHeadFlags(unsigned short flags);
  bool getCartChunk() const;
  void setCartChunk(bool state);
  unsigned getCartVersion() const;
  QString getCartTitle() const;
  void setCartTitle(QString string);
  QString getCartArtist() const;
  void setCartArtist(QString string);
  QString getCartCutID() const;
  void setCartCutID(QString string);
  QString getCartClientID() const;
  void setCartClientID(QString string);
  QString getCartCategory() const;
  void setCartCategory(QString string);
  QString getCartClassification() const;
  void setCartClassification(QString string);
  QString getCartOutCue() const;
  void setCartOutCue(QString string);
  QDate getCartStartDate() const;
  void setCartStartDate(QDate date);
  QTime getCartStartTime() const;
  void setCartStartTime(QTime time);
  QDate getCartEndDate() const;
  void setCartEndDate(QDate date);
  QTime getCartEndTime() const;
  void setCartEndTime(QTime time);
  QString getCartProducerAppID() const;
  QString getCartProducerAppVer() const;
  QString getCartUserDef() const;
  void setCartUserDef(QString string);
  unsigned getCartLevelRef() const;
  void setCartLevelRef(unsigned level);
  QString getCartTimerLabel(int index) const;
  unsigned getCartTimerSample(int index) const;
  QString getCartURL() const;
  void setCartURL(QString string);
  QString getCartTagText() const;
  bool getBextChunk() const;
  void setBextChunk(bool state);
  QString getBextDescription() const;
  void setBextDescription(QString string);
  QString getBextOriginator() const;
  void setBextOriginator(QString string);
  QString getBextOriginatorRef() const;
  void setBextOriginatorRef(QString string);
  QDate getBextOriginationDate() const;
  void setBextOriginationDate(QDate date);
  QTime getBextOriginationTime() const;
  void setBextOriginationTime(QTime time);
  unsigned getBextTimeReferenceLow() const;
  void setBextTimeReferenceLow(unsigned sample);
  unsigned getBextTimeReferenceHigh() const;
  void setBextTimeReferenceHigh(unsigned sample);
  unsigned short getBextVersion() const;
  void getBextUMD(unsigned char *buf) const;
  void setBextUMD(unsigned char *buf);
  QString getBextCodingHistory() const;
  void setBextCodingHistory(QString string);
  bool getMextChunk() const;
  void setMextChunk(bool state);
  bool getMextHomogenous() const;
  bool getMextPaddingUsed() const;
  bool getMextHackedBitRate() const;
  bool getMextFreeFormat() const;
  int getMextFrameSize() const;
  int getMextAncillaryLength() const;
  bool getMextLeftEnergyPresent() const;
  bool getMextRightEnergyPresent() const;
  bool getMextPrivateDataPresent() const;
  void setMextHomogenous(bool state);
  void setMextPaddingUsed(bool state);
  void setMextHackedBitRate(bool state);
  void setMextFreeFormat(bool state);
  void setMextFrameSize(int size);
  void setMextAncillaryLength(int length);
  void setMextLeftEnergyPresent(bool state);
  void setMextRightEnergyPresent(bool state);
  void setMextPrivateDataPresent(bool state);
  bool getLevlChunk() const;
  void setLevlChunk(bool state);
  int getLevlVersion() const;
  void setLevlVersion(unsigned ver);
  int getLevlBlockSize() const;
  void setLevlBlockSize(unsigned size);
  int getLevlChannels() const;
  unsigned short getLevlPeak() const;
  QDateTime getLevlTimestamp() const;
  void setEncodeQuality(float qual);
  int getSerialNumber() const;
  void setSerialNumber(int serial);
  bool getScotChunk() const;
  bool getAIR1Chunk() const;
  bool getRdxlChunk() const;
  QString getRdxlContents() const;
  void setRdxlContents(const QString &xml);
  double getNormalizeLevel() const;
  void setNormalizeLevel(double level);
  static QString formatText(Format fmt);
  static QString typeText(Type type);

  private:
   RDWaveFile::Type GetType(int fd);
   bool IsWav(int fd);
   bool IsMpeg(int fd);
   bool IsOgg(int fd);
   bool IsAtx(int fd);
   bool IsTmc(int fd);
   bool IsFlac(int fd);
   bool IsAiff(int fd);
   bool IsM4A(int fd);
   off_t FindChunk(int fd,const char *chunk_name,unsigned *chunk_size,
		   bool big_end=false);
   bool GetChunk(int fd,const char *chunk_name,unsigned *chunk_size,
		 unsigned char *chunk,size_t size,bool big_end=false);
   void WriteChunk(int fd,const char *cname,unsigned char *buf,unsigned size,
		   bool big_end=false);
   void WriteChunk(int fd,const char *cname,const QString &contents);
   bool GetFmt(int fd);
   bool GetFact(int fd);
   bool GetCart(int fd);
   bool GetBext(int fd);
   bool GetMext(int fd);
   bool GetLevl(int fd);
   bool GetList(int fd);
   bool GetScot(int fd);
   bool GetAv10(int fd);
   bool GetAir1(int fd);
   bool GetRdxl(int fd);
   bool GetComm(int fd);
   bool ReadListElement(unsigned char *buffer,unsigned *offset,unsigned size);
   bool ReadTmcMetadata(int fd);
   void ReadTmcTag(const QString tag,const QString value);
   bool GetLine(int fd,char *buffer,int max_len);
   void ReadId3Metadata();
   bool GetMpegHeader(int fd,int offset);
   int GetAtxOffset(int fd);
   bool GetFlacStreamInfo();
   void ReadFlacMetadata();
   bool MakeFmt();
   bool MakeCart(unsigned ptr_offset);
   bool MakeBext();
   bool MakeMext();
   bool MakeLevl();
   void WriteDword(unsigned char *,unsigned,unsigned);
   void WriteSword(unsigned char *,unsigned,unsigned short);
   unsigned ReadDword(unsigned char *,unsigned);
   unsigned short ReadSword(unsigned char *,unsigned);
   void GetEnergy();
   unsigned LoadEnergy();
   bool ReadNormalizeLevel(QString wave_file_name);
   bool ReadEnergyFile(QString wave_file_name);
   void GrowAlloc(size_t size);
#ifdef HAVE_VORBIS
   int WriteOggPage(ogg_page *page);
#endif  // HAVE_VORBIS
   int WriteOggBuffer(char *buf,int size);
   unsigned FrameOffset(int msecs) const;
   int CheckExitCode(const QString &msg,int exit_code);
   QString wave_file_name;
   QFile wave_file;
   RDWaveData *wave_data;
   bool recordable;                // Allow DATA chunk writes?
   unsigned time_length;           // Audio length in secs
   unsigned ext_time_length;       // Audio length in msec
   bool format_chunk;              // Does 'fmt ' chunk exist?
   bool comm_chunk;                // Does 'COMM' chunk exist?
   unsigned char comm_chunk_data[COMM_CHUNK_SIZE];
   unsigned short format_tag;      // Encoding Format
   unsigned short channels;        // Number of channels
   unsigned samples_per_sec;       // Samples/sec/channel
   unsigned avg_bytes_per_sec;     // Average bytes/sec overall
   unsigned short block_align;     // Data block size
   unsigned short bits_per_sample; // Bits per mono sample (PCM only)
   unsigned short cb_size;         // Number of bytes of extended data 
   unsigned short head_layer;      // The MPEG audio layer 
   unsigned head_bit_rate;         // MPEG bit rate, in byte/sec
   unsigned short head_mode;       // MPEG stream mode
   unsigned head_mode_ext;         // Extra mode parameters (for joint stereo)
   unsigned head_emphasis;         // De-emphasis
   unsigned short head_flags;      // MPEG header flags
   unsigned ptr_offset_msecs;
   int mpeg_frame_size;
   bool id3v1_tag;
   bool id3v2_tag[2];
   unsigned id3v2_offset[2];
   unsigned char fmt_chunk_data[FMT_CHUNK_SIZE];
   int fmt_size;                   // Size of FMT chunk
   bool fact_chunk;                // Does 'fact' chunk exist?
   unsigned sample_length;         // Audio length in samples
   unsigned char fact_chunk_data[FACT_CHUNK_SIZE];
   bool data_chunk;                // Does 'data' chunk exist?
   int data_start;                 // Start position of WAV data
   unsigned data_length;           // Length of raw audio data
   bool cart_chunk;                   // Does 'cart' chunk exist?
   unsigned cart_version;             // CartChunk Version field
   QString cart_title;                // CartChunk Title field
   QString cart_artist;               // CartChunk Artist field
   QString cart_cut_id;               // CartChunk CutID field
   QString cart_client_id;            // CartChunk ClientID field
   QString cart_category;             // CartChunk Category field
   QString cart_classification;       // CartChunk Classification field
   QString cart_out_cue;              // CartChunk OutCue Field
   QDate cart_start_date;             // CartChunk StartDate field
   QTime cart_start_time;             // CartChunk StartTime field
   QDate cart_end_date;               // CartChunk EndDate field
   QTime cart_end_time;               // CartChunk EndTime field
   QString cart_producer_app_id;      // CartChunk ProducerAppID field
   QString cart_producer_app_ver;     // CartChunk ProducerAppVersion field
   QString cart_user_def;             // CartChunk UserDef field
   unsigned cart_level_ref;           // CartChunk dwLevelReference field
   QString cart_timer_label[MAX_TIMERS];   // CartChunk CartTimer labels
   unsigned cart_timer_sample[MAX_TIMERS]; // CarChunk CartTimer samples
   QString cart_url;                  // CartChunk URL field
   QString cart_tag_text;             // CartChunk TagText field
   unsigned char cart_chunk_data[CART_CHUNK_SIZE];

   bool bext_chunk;                   // Does the chunk exist?
   QString bext_description;          // BWF Description of sound sequence
   QString bext_originator;           // BWF Name of originator
   QString bext_originator_ref;       // BWF Reference of the originator
   QDate bext_origination_date;       // BWF Origination date
   QTime bext_origination_time;       // BWF Origination time
   unsigned bext_time_reference_low;  // BWF Sample count since midnight, low
   unsigned bext_time_reference_high; // BWF Sample count since midnight, high
   unsigned short bext_version;       // BWF Version of the BWF
   unsigned char bext_umid[64];       // BWF SMPTE UMD
   QString bext_coding_history;       // BWF Coding History
   unsigned char bext_chunk_data[BEXT_CHUNK_SIZE];
   unsigned char *bext_coding_data;
   unsigned bext_coding_size;

   bool mext_chunk;                   // Does the chunk exist?
   bool mext_homogenous;              // Is the data homogenous?
   bool mext_padding_used;            // Is the padding bit used?
   bool mext_rate_hacked;             // Is padding not used for a 22 or 44 sr?
   bool mext_free_format;             // Is it free format?
   int mext_frame_size;               // Size of MPEG frame, n/c padding
   int mext_anc_length;               // Ancillary data length
   bool mext_left_energy;             // Does anc data contain left/mono energy
   bool mext_right_energy;            // Does anc data contain right energy?
   bool mext_ancillary_private;       // Does anc data contain private data?
   unsigned char mext_chunk_data[MEXT_CHUNK_SIZE];
   bool has_energy;                   // Can we produce energy data?

   unsigned char levl_chunk_data[LEVL_CHUNK_SIZE];
   unsigned levl_size;                // Size of LEVL chunk
   bool levl_chunk;                   // Does LEVL chunk exist?
   int levl_version;                  // Version
   int levl_format;                   // Peak Data Format
   int levl_points;                   // Points per Peak
   int levl_block_size;               // Frames per Peak Value
   int levl_channels;                 // Channels
   unsigned levl_frames;              // Total Peaks
   unsigned levl_peak_offset;         // Pointer to peak-of-peaks
   unsigned short levl_peak_value;    // Value of peak-of-peaks
   unsigned levl_block_offset;        // Offset to start of peaks
   QDateTime levl_timestamp;          // Timestamp
   unsigned short levl_block_ptr;
   unsigned levl_istate;
   short levl_accum;

   QString cutString(char *,unsigned,unsigned);
   QDate cutDate(char *,unsigned);
   QTime cutTime(char *,unsigned);
   std::vector<unsigned short> energy_data;
   bool energy_loaded;
   unsigned energy_ptr;
   int wave_id;
   RDWaveFile::Type wave_type;

   unsigned char *cook_buffer;
   int cook_buffer_size;
   float encode_quality;
   int serial_number;
   int atx_offset;
   bool scot_chunk;
   unsigned char scot_chunk_data[SCOT_CHUNK_SIZE];
   QString scot_title;
   QString scot_artist;
   QString scot_etc;
   int scot_year;
   QString scot_cut_number;
   int scot_intro_length;
   int scot_eom_length;
   QDate scot_start_date;
   QTime scot_start_time;
   QDate scot_end_date;
   QTime scot_end_time;

   bool AIR1_chunk;
   unsigned char AIR1_chunk_data[AIR1_CHUNK_SIZE];

   bool rdxl_chunk;
   QString rdxl_contents;

   double normalize_level; 
   
   bool av10_chunk;
   unsigned char av10_chunk_data[AV10_CHUNK_SIZE];
   
#ifdef HAVE_VORBIS
   OggVorbis_File vorbis_file;
   vorbis_info vorbis_inf;
   vorbis_block vorbis_blk;
   vorbis_dsp_state vorbis_dsp;
   ogg_stream_state ogg_stream;
   ogg_page ogg_pg;
   ogg_packet ogg_pack;
#endif  // HAVE_VORBIS
#ifdef HAVE_MP4_LIBS
   DLMP4 dlmp4;
#endif

};


/*
 * Cart Chunk Stuff
 */
#define CART_VERSION "0101"
#define CART_DEFAULT_END_YEAR 2099
#define CART_DEFAULT_END_MONTH 12
#define CART_DEFAULT_END_DAY 31
#define CART_DEFAULT_END_HOUR 23
#define CART_DEFAULT_END_MINUTE 59
#define CART_DEFAULT_END_SECOND 59
#define CART_DEFAULT_LEVEL_REF 0x8000


/*
 * BWF Stuff
 */
#define BWF_VERSION 1


/* 
 * WAVE Format Categories 
 * (From RFC2361)
 */
#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_ADPCM 0x0002
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_VSELP 0x0004
#define WAVE_FORMAT_IBM_CVSD 0x0005
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_OKI_ADPCM 0x0010
#define WAVE_FORMAT_DVI_ADPCM 0x0011
#define WAVE_FORMAT_MEDIASPACE_ADPCM 0x0012
#define WAVE_FORMAT_SIERRA_ADPCM 0x0013
#define WAVE_FORMAT_G723_ADPCM 0x0014
#define WAVE_FORMAT_DIGISTD 0x0015
#define WAVE_FORMAT_DIGIFIX 0x0016
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM 0x0017
#define WAVE_FORMAT_MEDIAVISION_ADPCM 0x0018
#define WAVE_FORMAT_CU_CODEC 0x0019
#define WAVE_FORMAT_YAMAHA_ADPCM 0x0020
#define WAVE_FORMAT_SONARC 0x0021
#define WAVE_FORMAT_DSPGROUP_TRUESPEECH 0x0022
#define WAVE_FORMAT_ECHOSC1 0x0023
#define WAVE_FORMAT_AUDIOFILE_AF36 0x0024
#define WAVE_FORMAT_APTX 0x0025
#define WAVE_FORMAT_AUDIOFILE_AF10 0x0026
#define WAVE_FORMAT_PROSODY_1612 0x0027
#define WAVE_FORMAT_LRC 0x0028
#define WAVE_FORMAT_DOLBY_AC2 0x0030
#define WAVE_FORMAT_GSM610 0x0031
#define WAVE_FORMAT_MSNAUDIO 0x0032
#define WAVE_FORMAT_ANTEX_ADPCME 0x0033
#define WAVE_FORMAT_CONTROL_RES_VQLPC 0x0034
#define WAVE_FORMAT_DIGIREAL 0x0035
#define WAVE_FORMAT_DIGIADPCM 0x0036
#define WAVE_FORMAT_CONTROL_RES_CR10 0x0037
#define WAVE_FORMAT_NMS_VBXADPCM 0x0038
#define WAVE_FORMAT_ROLAND_RDAC 0x0039
#define WAVE_FORMAT_ECHOSC3 0x003A
#define WAVE_FORMAT_ROCKWELL_ADPCM 0x003B
#define WAVE_FORMAT_ROCKWELL_DIGITALK 0x003C
#define WAVE_FORMAT_XEBEC 0x003D
#define WAVE_FORMAT_G721_ADPCM 0x0040
#define WAVE_FORMAT_G728_CELP 0x0041
#define WAVE_FORMAT_MSG723 0x0042
#define WAVE_FORMAT_MPEG 0x0050
#define WAVE_FORMAT_RT24 0x0052
#define WAVE_FORMAT_PAC 0x0053
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#define WAVE_FORMAT_LUCENT_G723 0x0059
#define WAVE_FORMAT_CIRRUS 0x0060
#define WAVE_FORMAT_ESPCM 0x0061
#define WAVE_FORMAT_VOXWARE 0x0062
#define WAVE_FORMAT_CANOPUS_ATRAC 0x0063
#define WAVE_FORMAT_G726_ADPCM 0x0064
#define WAVE_FORMAT_G722_ADPCM 0x0065
#define WAVE_FORMAT_DSAT 0x0066
#define WAVE_FORMAT_DSAT_DISPLAY 0x0067
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED 0x0069
#define WAVE_FORMAT_VOXWARE_AC8 0x0070
#define WAVE_FORMAT_VOXWARE_AC10 0x0071
#define WAVE_FORMAT_VOXWARE_AC16 0x0072
#define WAVE_FORMAT_VOXWARE_AC20 0x0073
#define WAVE_FORMAT_VOXWARE_RT24 0x0074
#define WAVE_FORMAT_VOXWARE_RT29 0x0075
#define WAVE_FORMAT_VOXWARE_RT29HW 0x0076
#define WAVE_FORMAT_VOXWARE_VR12 0x0077
#define WAVE_FORMAT_VOXWARE_VR18 0x0078
#define WAVE_FORMAT_VOXWARE_TQ40 0x0079
#define WAVE_FORMAT_SOFTSOUND 0x0080
#define WAVE_FORMAT_VOXWARE_TQ60 0x0081
#define WAVE_FORMAT_MSRT24 0x0082
#define WAVE_FORMAT_G729A 0x0083
#define WAVE_FORMAT_MVI_MV12 0x0084
#define WAVE_FORMAT_DF_G726 0x0085
#define WAVE_FORMAT_DF_GSM610 0x0086
#define WAVE_FORMAT_ISIAUDIO 0x0088
#define WAVE_FORMAT_ONLIVE 0x0089
#define WAVE_FORMAT_SBC24 0x0091
#define WAVE_FORMAT_DOLBY_AC3_SPDIF 0x0092
#define WAVE_FORMAT_ZYXEL_ADPCM 0x0097
#define WAVE_FORMAT_PHILIPS_LPCBB 0x0098
#define WAVE_FORMAT_PACKED 0x0099
#define WAVE_FORMAT_RHETOREX_ADPCM 0x0100
#define WAVE_FORMAT_IRAT 0x0101
#define WAVE_FORMAT_VIVO_G723 0x0111
#define WAVE_FORMAT_VIVO_SIREN 0x0112
#define WAVE_FORMAT_DIGITAL_G723 0x0123
#define WAVE_FORMAT_CREATIVE_ADPCM 0x0200
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8 0x0202
#define WAVE_FORMAT_ CREATIVE_FASTSPEECH10 0x0203
#define WAVE_FORMAT_QUARTERDECK 0x0220
#define WAVE_FORMAT_FM_TOWNS_SND 0x0300
#define WAVE_FORMAT_BTV_DIGITAL 0x0400
#define WAVE_FORMAT_VME_VMPCM 0x0680
#define WAVE_FORMAT_OLIGSM 0x1000
#define WAVE_FORMAT_OLIADPCM 0x1001
#define WAVE_FORMAT_OLICELP 0x1002
#define WAVE_FORMAT_OLISBC 0x1003
#define WAVE_FORMAT_OLIOPR 0x1004
#define WAVE_FORMAT_LH_CODEC 0x1100
#define WAVE_FORMAT_NORRIS 0x1400
#define WAVE_FORMAT_ISIAUDIO2 0x1401
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS 0x1500
#define WAVE_FORMAT_DVM 0x2000

/*
 * OggVorbis/FLAC Formats
 *
 * FIXME: These are made up out of thin air -- if a real registered number 
 *        exists, we should use that instead.
 */
#define WAVE_FORMAT_VORBIS 0xFFFF
#define WAVE_FORMAT_FLAC 0xFFFE
#define WAVE_FORMAT_M4A 0xFFFD

/*
 * Proprietary Format Categories
 * (Not supported)
 */
#define MS_FORMAT_ADPCM 0x0002
#define ITU_FORMAT_G711_ALAW 0x0006
#define ITU_FORMAT_G711_MLAW 0x0007
#define IMA_FORMAT_ADPCM 0x0011
#define ITU_FORMAT_G723_ADPCM 0x0016
#define GSM_FORMAT_610 0x0031
#define ITU_FORMAT_G721_ADPCM 0x0040
#define IBM_FORMAT_MULAW 0x0101
#define IBM_FORMAT_ALAW 0x0102
#define IBM_FORMAT_ADPCM 0x0103

/*
 * MPEG Defines
 *
 * fwHeadLayer Flags
 */
#define ACM_MPEG_LAYER1 0x0001
#define ACM_MPEG_LAYER2 0x0002
#define ACM_MPEG_LAYER3 0x0004

/*
 * fwHeadMode Flags
 */
#define ACM_MPEG_STEREO 0x0001
#define ACM_MPEG_JOINTSTEREO 0x0002
#define ACM_MPEG_DUALCHANNEL 0x0004
#define ACM_MPEG_SINGLECHANNEL 0x0008

/*
 * fwHeadFlags Flags
 */
#define ACM_MPEG_PRIVATEBIT 0x0001
#define ACM_MPEG_COPYRIGHT 0x0002
#define ACM_MPEG_ORIGINALHOME 0x0004
#define ACM_MPEG_PROTECTIONBIT 0x0008
#define ACM_MPEG_ID_MPEG1 0x0010


#endif  // RDWAVEFILE_H

