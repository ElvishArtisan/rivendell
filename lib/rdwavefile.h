//   rdwavefile.h
//
//   A class for handling Microsoft WAV files.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdwavefile.h,v 1.10.6.2 2014/01/15 19:56:32 cvs Exp $
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

#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif  // WIN32
#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qfile.h>

#ifdef HAVE_VORBIS
#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>
#endif  // HAVE_VORBIS

#include <rdwavedata.h>
#include <rdringbuffer.h>
#include <rdsettings.h>

using namespace std;


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


/**
 * @short A class for handling Microsoft WAV files.
 * @author Fred Gleason <fredg@wava.com>
 *
 * RDWaveFile provides an abstraction of a Microsoft RIFF-based WAV file.
 * In addition to 'FMT' and 'DATA' chunks, chunk types of particular 
 * interest to broadcast applications are supported, including those 
 * specified by the Broadcast Wave File specification (EBU Tech Document
 * 3285, with suppliments) and the CartChunk specification (currently 
 * proposed to become AES standard AES-46).
 **/   
class RDWaveFile
{
 public:
  enum Encoding {Raw=0,Signed16Int=1,Signed32Float=2};
  enum Format {Pcm8=0,Pcm16=1,Float32=2,MpegL1=3,MpegL2=4,MpegL3=5,
	       DolbyAc2=6,DolbyAc3=7,Vorbis=8};
  enum Type {Unknown=0,Wave=1,Mpeg=2,Ogg=3,Atx=4,Tmc=5,Flac=6,Ambos=7,
	     Aiff=8};
  enum MpegID {NonMpeg=0,Mpeg1=1,Mpeg2=2};

  /**
   * Create an RDWaveFile object.
   * @param file_name The name of the WAV file to load into the object.
   **/
  RDWaveFile(QString file_name="");

   /**
    * Destroy an RDWaveFile object.
    **/
  ~RDWaveFile();

  /**
   * Get the file type.
   **/
  RDWaveFile::Type type() const;

  /**
   * Assign a WAV file name.
   * @param file_name The WAV filename to assign.
   **/
  void nameWave(QString file_name);

  /**
   * Open the WAV file for recording.  If the file already exists, it's
   * prior contents will be overwritten.  If not, a new file will be
   * created.  
   *
   * It is important that all desired meta-data chunks be enabled (using
   * the set<name>Chunk() family of methods) before invoking createWav().
   * Once the WAV file is created, new meta-data chunks cannot be added or 
   * removed, although the contents of existing ones can be updated.  
   * At a minimum, the 'FMT' chunk will need to have it's parameters
   * (sample rate, channels, etc) defined, or this method will return an 
   * error.
   *
   * Returns true if WAV file was created successfully, otherwise false.
   **/
   bool createWave(RDWaveData *data=NULL);

  /**
   * Open the WAV file for playback.  A WAV file name must first have 
   * been assigned, either in the constructor or by means of nameWav().
   *
   * Setting 'allow_broken=true' will permit even patently-broken files
   * (like the raw files from an AMR-100 receiver, that have no FMT chunk)
   * to be opened to allow the metadata to be read.
   **/
   bool openWave(RDWaveData *data=NULL);

  /**
   * Close the WAV file.  Any pending DATA chunk or meta-data writes
   * will be applied, and DATA and FACT chunk size structures updated
   * accordingly.
   **/
   void closeWave(int samples=-1);

  /**
   * Reset the record pointer on the WAV file to zero.  Erases any audio
   * data previously recorded, but otherwise leaves things (format settings,
   * meta-data) as they were.
   **/
   void resetWave();

  /**
   * Returns true if the WAV file contains a FMT chunk, otherwise false.
   **/
   bool getFormatChunk() const;

  /**
   * Returns true if the WAV file contains a FACT chunk, otherwise false.
   **/
   bool getFactChunk() const;

  /**
   * Returns the length of the audio in samples.
   **/
   unsigned getSampleLength() const;

  /**
   * Returns the length of the audio in seconds.
   **/
   unsigned getTimeLength() const;

  /**
   * Returns the length of the audio in milliseconds.
   **/
   unsigned getExtTimeLength() const;

  /**
   * Returns true if the WAV file contains a DATA chunk, otherwise false.
   **/
   bool getDataChunk() const;

  /**
   * Returns the length of the contents of the DATA chunk, in bytes.
   **/
   unsigned getDataLength() const;

  /**
   * Returns the current encoding type.
   **/
   RDWaveFile::Encoding encoding() const;

  /**
   * Set the encoding type.
   * @param code The encoding to use.
   **/
   void setEncoding(RDWaveFile::Encoding code);

  /**
   * Read a block of data from the DATA chunk, using the current 
   * encoding type.
   * @param buf The buffer in which to place the data.
   * @param count The maximum number of bytes to transfer.
   * Returns the number of bytes read.
   **/
   int readWave(void *buf,int count);

  /**
   * Write a block of data to the DATA chunk.
   * @param buf The buffer from which to take the data.
   * @param count The number of bytes to transfer.
   * Returns the number of bytes written.
   **/
   int writeWave(void *buf,int count);

  /**
   * Set the DATA chunk file pointer.
   * @param offset The value to which to set the pointer.  The exact meaning
   * depends upon the value of the 'whence' parameters, below:
   * @param whence If SEEK_SET, set the file pointer to the value given
   * in 'offset.  If SEEK_CUR, set the pointer to the value given in 'offset',
   * plus the current value.
   * Returns If successful, return the current pointer position, relative to
   * the start of the DATA chunk, otherwise -1.
   **/
   int seekWave(int offset,int whence);

   void getSettings(RDSettings *settings);
   void setSettings(const RDSettings *settings);

  /**
   * Returns true if energy data is available.
   **/
   bool hasEnergy();

  /**
   * Returns the size of the energy data, or zero if none available.
   **/
   unsigned energySize();

  /**
   * Returns energy data.  For stereo files, this will be 
   * interleaved LEFT>>RIGHT>>LEFT>>... .
   * @param frame The frame to reference.
   **/
   unsigned short energy(unsigned frame);

  /**
   * Read a block of energy data.
   * @param buf The buffer in which to place the data.
   * @param count The maximum number of bytes to transfer.
   * Returns the number of bytes read.
   **/
   int readEnergy(unsigned short buf[],int count);

  /**
   * Find the first instance of energy at or above the specified level.
   * @param level The level, in dbFS * 100.
   * Returns: The location in samples from file start, or -1 to indicate
   * failure.
   **/
   int startTrim(int level);

  /**
   * Find the last instance of energy at or above the specified level.
   * @param level The level, in dbFS * 100.
   * Returns: The location in samples from file start, or -1 to indicate
   * failure.
   **/
   int endTrim(int level);

  /**
   * Returns the filename of the WAV file.
   **/
   QString getName() const;

  /**
   * Returns the FormatTag of the WAV file, as defined in the 'FMT' chunk.
   * Values currently understood by RDWaveFile are:
   * WAVE_FORMAT_PCM  Linear PCM Data
   * WAVE_FORMAT_IEEE_FLOAT IEEE Floating Point Data
   * WAVE_FORMAT_MPEG MPEG-1 Encoded Data
   **/
   unsigned short getFormatTag() const;

  /**
   * Set the format tag in the FMT chunk for a WAV file to be recorded.
   * Values currently understood by RDWaveFile are:
   * WAVE_FORMAT_PCM  Linear PCM Data
   * WAVE_FORMAT_MPEG MPEG-1 Encoded Data
   * @param format The encoding format.
   **/
   void setFormatTag(unsigned short format);

  /**
   * Returns the number of audio channels recorded in the WAV file, as 
   * represented by the 'FMT chunk.
   **/
   unsigned short getChannels() const;

  /**
   * Sets the number of channels in the FMT chunk for a WAV file to be 
   * recorded.  Currently supported values are '1' or '2'.
   * @param chan Number of channels.
   **/
   void setChannels(unsigned short chan);

  /**
   * Returns the sampling rate of the audio recorded in the WAV file, as 
   * represented by the 'FMT chunk, in samples per second.
   **/
   unsigned getSamplesPerSec() const;

  /**
   * Sets the sampling rate in the FMT chunk for a WAV file to be recorded.
   * @param rate The sampling rate in samples per second.
   **/
   void setSamplesPerSec(unsigned rate);

  /**
   * Returns the average data rate of the audio recorded in the WAV file, as 
   * represented by the 'FMT chunk, in bytes per second.
   **/
   unsigned getAvgBytesPerSec() const;

  /**
   * Returns the 'atomic' sample size of the audio recorded in the WAV file,
   * as represented by the 'FMT' chunk.  In the case of a non-homogenous or
   * bit-padded MPEG file, this will be set to '1'.
   **/
   unsigned short getBlockAlign() const;

  /**
   * Returns the bits per audio sample for PCM files, otherwise a '0'.
   **/
   unsigned short getBitsPerSample() const;

  /**
   * Set the number of bits per sample for PCM files in the FMT chunk
   * for a WAV file to be recorded.  For MPEG files, this should be set
   * to '0'.
   * @param bits Number of bits per audio sample, or '0' for an MPEG file.
   **/
   void setBitsPerSample(unsigned short bits);

  /**
   * Returns the MPEG-1 Layer (1, 2 or 3) for an MPEG-1 file, otherwise a '0'.
   **/
   unsigned short getHeadLayer() const;

  /**
   * Set the MPEG-1 Layer for MPEG files in the FMT chunk for a WAV file
   * to be recorded.  Layers 1, 2 and 3 are supported.  For PCM files,
   * this should be set to '0'.
   * @param layer MPEG-1 encoding layer, or '0' for a PCM file.
   **/
   void setHeadLayer(unsigned short layer);

  /**
   * Returns the bit rate for an MPEG-1 file in bits per second, otherwise 
   * a '0'.
   **/
   unsigned getHeadBitRate() const;

  /**
   * Set the bit rate for an MPEG-1 file in the FMT chunk for a WAV file
   * to be recorded.  This needs to be one of the "official" rates as
   * defined in the MPEG-1 standard.  Valid rates are:
   * LAYER 1:  32000, 64000, 96000,128000,160000,192000,224000,
   *          256000,288000,320000,352000,384000,416000,448000
   *
   * LAYER 2:  32000, 48000, 56000, 64000, 80000, 96000,112000,
   *          128000,160000,192000,224000,256000,320000,384000
   *
   * LAYER 3:  32000, 40000, 48000, 56000, 64000, 80000, 96000,
   *          112000,128000,160000,192000,224000,256000,320000
   *
   * @param rate MPEG bitrate, in bits per second, or '0' for a PCM file.
   **/
   void setHeadBitRate(unsigned rate);

  /**
   * Returns the MPEG-1 Mode for an MPEG-1 file, otherwise a '0'.  Possible
   * modes are:
   * ACM_MPEG_STEREO          Stereo
   * ACM_MPEG_JOINTSTEREO     Joint Stereo
   * ACM_MPEG_DUALCHANNEL     Dual Channel
   * ACM_MPEG_SINGLECHANNEL   Mono
   **/
   unsigned short getHeadMode() const;

  /**
   * Set the MPEG-1 mode for an MPEG file in the FMT chunk for a WAV
   * file to be recorded.  This value is a series of flags bitwise-OR'd
   * together.  The corresponding flag should be set if one or more
   * MPEG-1 frames in that mode are present in the audio data.
   *
   * Valid mode flags are:
   * ACM_MPEG_STEREO           Stereo
   * ACM_MPEG_JOINTSTEREO      Joint Stereo
   * ACM_MPEG_DUALCHANNEL      Dual Channel
   * ACM_MPEG_SINGLECHANNEL    Mono
   *
   * @param mode The bitwise-OR'd mode flags for MPEG files, or a '0' for PCM
   * files.
   **/
   void setHeadMode(unsigned short mode);

  /**
   * Returns the extended mode flags for an MPEG-1 file, otherwise a '0'.
   **/
   unsigned getHeadModeExt() const;

  /**
   * Returns the request preemphasis for an MPEG-1 file, otherwise a '0'.
   **/
   unsigned getHeadEmphasis() const;

  /**
   * Returns the MPEG-1 flags for an MPEG-1 file, otherwise a '0'.
   **/
   unsigned short getHeadFlags() const;

  /**
   * Set the MPEG-1 flags for an MPEG file.  All appropriate flags should
   * be bitwise-OR'd together.  Valid flags include:
   * ACM_MPEG_PRIVATEBIT       Private Bit
   * ACM_MPEG_COPYRIGHT        Copyright Bit
   * ACM_MPEG_ORIGINALHOME     Original Home Bit
   * ACM_MPEG_PROTECTIONBIT    Protection Bit
   * ACM_MPEG_ID_MPEG1         MPEG-1 ID Bit
   *
   * @param flags The MPEG-1 flags to set.
   **/
   void setHeadFlags(unsigned short flags);

  /**
   * Not currently implemented.
   **/
   unsigned long getPTS() const;

  /**
   * Returns true if the WAV file contains a CART chunk, otherwise false.
   **/
   bool getCartChunk() const;

  /**
   * Enable a CartChunk structure.  The default is to not create a 
   * CartChunk structure in the WAV file to be recorded.  For more 
   * information on CartChunk and it's capabilities, see 
   * http://www.cartchunk.org/.
   * @param state true = Enable cart chunk structure, false = disable
   **/
   void setCartChunk(bool state);

  /**
   * Returns the version of the CartChunk standard used in the WAV file.
   **/
   unsigned getCartVersion() const;

  /**
   * Returns the contents of the TITLE field in the WAV file's CART chunk.
   **/
   QString getCartTitle() const;

  /**
   * Set the TITLE field on the WAV file's CartChunk.
   * @param string The value for the TITLE field.
   **/
   void setCartTitle(QString string);

  /**
   * Returns the contents of the ARTIST field in the WAV file's CART chunk.
   **/
   QString getCartArtist() const;

  /**
   * Set the ARTIST field on the WAV file's CartChunk.
   * @param string The value for the ARTIST field.
   **/
   void setCartArtist(QString string);

  /**
   * Returns the contents of the CutID field in the WAV file's CART chunk.
   **/
   QString getCartCutID() const;

  /**
   * Set the CutID field on the WAV file's CartChunk.
   * @param string The value for the CutID field.
   **/
   void setCartCutID(QString string);

  /**
   * Returns the contents of the ClientID field in the WAV file's CART chunk.
   **/
   QString getCartClientID() const;

  /**
   * Set the ClientID field on the WAV file's CartChunk.
   * @param string The value for the ClientID field.
   **/
   void setCartClientID(QString string);

  /**
   * Returns the contents of the CATEGORY field in the WAV file's CART chunk.
   **/
   QString getCartCategory() const;

  /**
   * Set the Category field on the WAV file's CartChunk.
   * @param string The value for the Category field.
   **/
   void setCartCategory(QString string);

  /**
   * Returns the contents of the CLASSIFICATION field in the WAV file's 
   * CART chunk.
   **/
   QString getCartClassification() const;

  /**
   * Set the Classification field on the WAV file's CartChunk.
   * @param string The value for the Cclassification field.
   **/
   void setCartClassification(QString string);

  /**
   * Returns the contents of the OUTCUE field in the WAV file's CART chunk.
   **/
   QString getCartOutCue() const;

  /**
   * Set the OutCue field on the WAV file's CartChunk.
   * @param string The value for the OutCue field.
   **/
   void setCartOutCue(QString string);

  /**
   * Returns the contents of the STARTDATE field in the WAV file's CART chunk.
   **/
   QDate getCartStartDate() const;

  /**
   * Set the StartDate field on the WAV file's CartChunk.
   * @param date The value for the StartDate field.
   **/
   void setCartStartDate(QDate date);

  /**
   * Returns the contents of the STARTTIME field in the WAV file's CART chunk.
   **/
   QTime getCartStartTime() const;

  /**
   * Set the StartTime field on the WAV file's CartChunk.
   * @param time The value for the StartTime field.
   **/
   void setCartStartTime(QTime time);

  /**
   * Returns the contents of the ENDDATE field in the WAV file's CART chunk.
   **/
   QDate getCartEndDate() const;

  /**
   * Set the EndDate field on the WAV file's CartChunk.
   * @param date The value for the EndDate field.
   **/
   void setCartEndDate(QDate date);

  /**
   * Returns the contents of the ENDTIME field in the WAV file's CART chunk.
   **/
   QTime getCartEndTime() const;

  /**
   * Set the EndTime field on the WAV file's CartChunk.
   * @param time The value for the EndTime field.
   **/
   void setCartEndTime(QTime time);

  /**
   * Returns the contents of the PRODUCER APPLICATION ID field in the WAV 
   * file's CART chunk.
   **/
   QString getCartProducerAppID() const;

  /**
   * Returns the contents of the PRODUCER APPLICATION VERSION field in the 
   * WAV file's CART chunk.
   **/
   QString getCartProducerAppVer() const;

  /**
   * Returns the contents of the USERDEF field in the WAV file's CART chunk.
   **/
   QString getCartUserDef() const;

  /**
   * Set the UserDef field on the WAV file's CartChunk.
   * @param string The value for the UserRef field.
   **/
   void setCartUserDef(QString string);

  /**
   * Returns the contents of the LEVELREF field in the WAV file's CART chunk.
   **/
   unsigned getCartLevelRef() const;

  /**
   * Set the LevelRef field on the WAV file's CartChunk.
   * @param level The value for the LevelRef field.
   **/
   void setCartLevelRef(unsigned level);

  /**
   * Retrieve the label for one of the eight CartChunk timers.
   * @param index The index number of the timer to access (0 - 7).
   * Returns the label of the corresponding timer.
   **/
   QString getCartTimerLabel(int index) const;

  /**
   * Set the label for one of the eight CartChunk timers.
   * @param index The index number of the timer to access (0 - 7).
   * @param label the label of the corresponding timer.
   **/
   void setCartTimerLabel(int index,QString label);

  /**
   * Retrieve the sample value for one of the eight CartChunk timers.
   * @param index The index number of the timer to access (0 - 7).
   * Returns the sample value of the corresponding timer.
   **/
   unsigned getCartTimerSample(int index) const;

  /**
   * Set the sample value for one of the eight CartChunk timers.
   * @param index The index number of the timer to access (0 - 7).
   * @param sample the sample value of the corresponding timer.
   **/
   void setCartTimerSample(int index,unsigned sample);

  /**
   * Returns the contents of the URL field in the WAV file's CART chunk.
   **/
   QString getCartURL() const;

  /**
   * Set the URL field on the WAV file's CartChunk.
   * @param string The value for the URL field.
   **/
   void setCartURL(QString string);

  /**
   * Returns the contents of the TAGTEXT field in the WAV file's CART chunk.
   **/
   QString getCartTagText() const;

  /**
   * Returns true if the WAV file contains a BEXT chunk, otherwise false.
   **/
   bool getBextChunk() const;

  /**
   * Enable a BWF Bext structure.  The default is to not create a 
   * Bext structure in the WAV file to be recorded.  For more 
   * information on the BWF format and it's capabilities, see 
   * http://www.sr.se/utveckling/tu/bwf/.
   * @param state true = Enable Bext structure, false = disable
   **/
   void setBextChunk(bool state);

  /**
   * Returns the contents of the DESCRIPTION field in the WAV file's BEXT 
   * chunk.
   **/
   QString getBextDescription() const;

  /**
   * Set the Description field on the WAV file's Bext structure.
   * @param string The value for the Description field.
   **/
   void setBextDescription(QString string);

  /**
   * Returns the contents of the ORIGINATOR field in the WAV file's BEXT 
   * chunk.
   **/
   QString getBextOriginator() const;

  /**
   * Set the Originator field on the WAV file's Bext structure.
   * @param string The value for the Originator field.
   **/
   void setBextOriginator(QString string);

  /**
   * Returns the contents of the ORIGINATOR REFERENCE field in the WAV 
   * file's BEXT chunk.
   **/
   QString getBextOriginatorRef() const;

  /**
   * Set the Originator Reference field on the WAV file's Bext structure.
   * @param string The value for the Originator Reference field.
   **/
   void setBextOriginatorRef(QString string);

  /**
   * Returns the contents of the ORIGINATION DATE field in the WAV file's BEXT 
   * chunk.
   **/
   QDate getBextOriginationDate() const;

  /**
   * Set the Origination Date field on the WAV file's Bext structure.
   * @param date The value for the Origination Date field.
   **/
   void setBextOriginationDate(QDate date);

  /**
   * Returns the contents of the ORIGINATION TIME field in the WAV file's BEXT 
   * chunk.
   **/
   QTime getBextOriginationTime() const;

  /**
   * Set the Origination Time field on the WAV file's Bext structure.
   * @param time The value for the Origination Time field.
   **/
   void setBextOriginationTime(QTime time);

  /**
   * Returns the lower 32 bits of the BWF Time Reference field.
   **/
   unsigned getBextTimeReferenceLow() const;

  /**
   * Sets the lower 32 bits of the BWF Time Reference field.
   * @param sample The sample value for the lower 32 bits
   **/
   void setBextTimeReferenceLow(unsigned sample);

  /**
   * Returns the upper 32 bits of the BWF Time Reference field.
   **/
   unsigned getBextTimeReferenceHigh() const;

  /**
   * Sets the upper 32 bits of the BWF Time Reference field.
   * @param sample The sample value for the upper 32 bits
   **/
   void setBextTimeReferenceHigh(unsigned sample);

  /**
   * Returns the version of the BWF standard used by the WAV file.
   **/
   unsigned short getBextVersion() const;

  /**
   * Gets the SMPTE Unique Material Identifier (UMD) in the WAV file's
   * BEXT chunk.  See SMPTE Standard 330M-2000 for more info.
   * @param buf A 64 byte long buffer in which to place the UMD
   **/
   void getBextUMD(unsigned char *buf) const;

  /**
   * Sets the SMPTE Unique Material Identifier (UMD) in the WAV file's
   * BEXT chunk.  See SMPTE Standard 330M-2000 for more info.
   * @param buf A 64 byte long buffer containing the UMD
   **/
   void setBextUMD(unsigned char *buf);

  /**
   * Returns the contents of the CODING HISTORY field from the WAV files'
   * BEXT chunk.
   **/
   QString getBextCodingHistory() const;

  /**
   * Set the Coding History field on the WAV file's Bext structure.
   * NOTE: This can only be set prior to calling createWave()!  If
   * called after createWave(), it will be silently ignored.
   * @param string The value for the Coding History field.
   **/
   void setBextCodingHistory(QString string);

  /**
   * Returns true if the WAV file contains a MEXT chunk, otherwise false.
   **/
   bool getMextChunk() const;

  /**
   * Enable a BWF Mext structure.  The default is to not create a 
   * Mext structure in the WAV file to be recorded.  For more 
   * information on the BWF format and it's capabilities, see 
   * http://www.sr.se/utveckling/tu/bwf/.
   * @param state true = Enable Mext structure, false = disable
   **/
   void setMextChunk(bool state);

  /**
   * Returns true if file is tagged as containing homogenous data, otherwise 
   * false.
   **/
   bool getMextHomogenous() const;

  /** 
   * Returns true if the padding bit is used (and hence the MPEG frame size
   * can vary between frames), otherwise false.
   **/
   bool getMextPaddingUsed() const;

   /**
    * Returns true if the file contains frames with the padding bits all 
    * set to '0' and a sample rate of 22.025 or 44.1 kHz.  According to
    * MPEG-1, such files are deprecated (because the overall bit rate is
    * not one of the standard MPEG rates).
    **/
   bool getMextHackedBitRate() const;

  /**
   * Returns true if the file contains free format frames, otherwise false.
   **/
   bool getMextFreeFormat() const;

  /**
   * Returns the number of bytes in a frame for homogenous data, otherwise
   * zero.  If the padding bit is used, this will be the frame size for 
   * frames with the padding bit set to '0'.  For frames with the padding
   * bit set to '1', the frame size will be four bytes longer for Layer 1
   * and one byte longer for Layer 2 and Layer 3.
   **/
   int getMextFrameSize() const;

  /**
   * Returns the minimal number of known bytes for ancillary data in the
   * full sound file.  The value is relative from the end of the audio frame.
   **/
   int getMextAncillaryLength() const;

  /**
   * Returns true if the ancillary data contains left or mono channel energy 
   * information, otherise false.
   **/
   bool getMextLeftEnergyPresent() const;

  /**
   * Returns true if the ancillary data contains right channel energy
   * information, otherwise false.
   **/
   bool getMextRightEnergyPresent() const;

  /**
   * Returns true if the ancillary data contains private data, otherwise
   * false.
   **/
   bool getMextPrivateDataPresent() const;

  /**
   * Set the BWF Mext structure in the WAV file to be recorded as containing
   * homogenous or non-homogenous MPEG-1 data.
   * @param state True = Data is homogenous, false = data is non-homogenous
   **/
   void setMextHomogenous(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * the status of the MPEG-1 padding bit.
   * @param state True = padding bit is used, false = padding bit is ignored
   **/
   void setMextPaddingUsed(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to indicate frames with
   * padding bits all set to '0' and a sample rate of 22.025 or 44.1 kHz.
   * According to MPEG-1, such files are deprecated (because the overall 
   * bit rate is not one of the standard MPEG rates).
   * @param state True = hacked bit rate used, false = proper bit rate used
   **/
   void setMextHackedBitRate(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * free format MPEG-1 frame.
   * @param state True = free format frames, false = constant bit rate frames
   **/
   void setMextFreeFormat(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * the number of bytes in a frame for homogenous data, otherwise
   * zero.  If the padding bit is used, this will be the frame size for 
   * frames with the padding bit set to '0'.  For frames with the padding
   * bit set to '1', the frame size will be four bytes longer for Layer 1
   * and one byte longer for Layer 2 and Layer 3.
   **/
   void setMextFrameSize(int size);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * the minimal number of known bytes for ancillary data in the
   * full sound file.  The value is relative from the end of the audio frame.
   **/
   void setMextAncillaryLength(int length);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * if the ancillary data contains left or mono channel energy 
   * information.
   * @param state True = left energy data is present, false = is not present
   **/
   void setMextLeftEnergyPresent(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * if the ancillary data contains rightchannel energy 
   * information.
   * @param state True = right energy data is present, false = is not present
   **/
   void setMextRightEnergyPresent(bool state);

  /**
   * Set the BWF Mext structure in the WAV file to be recorded to indicate 
   * if the ancillary data contains private data.
   * @param state True = private data present, false = no private data
   **/
   void setMextPrivateDataPresent(bool state);

  /**
   * Returns true if the WAV file contains a LEVL chunk, otherwise false.
   **/
   bool getLevlChunk() const;

  /**
   * Enable a BWF Levl structure.  The default is to not create a 
   * Levl structure in the WAV file to be recorded.  For more 
   * information on the BWF format and it's capabilities, see 
   * http://www.sr.se/utveckling/tu/bwf/.
   * @param state true = Enable Levl structure, false = disable
   **/
   void setLevlChunk(bool state);

  /**
   * Get the version of the LEVL chunk data.
   **/
   int getLevlVersion() const;

  /**
   * Set the version of the LEVL chunk data.
   * @param ver The version number to set.
   **/
   void setLevlVersion(unsigned ver);

   /**
    * Get the number of audio samples per peak value of the LEVL chunk.
    **/
   int getLevlBlockSize() const;

  /**
   * Set the block size of the LEVL chunk data.
   * @param ver The block size to set.
   **/
   void setLevlBlockSize(unsigned size);

   /**
    * Get the number of channels of peak values.
    **/
   int getLevlChannels() const;

   /**
    * Get the 'peak-of-peaks' value.
    **/
   unsigned short getLevlPeak() const;

   /**
    * Get the timestamp of the LEVL chunk.
    **/
   QDateTime getLevlTimestamp() const;

   /**
    * Set the encoding quality (for OggVorbis only)
    **/
   void setEncodeQuality(float qual);

   /**
    * Get serial number (for OggVorbis only)
    **/
   int getSerialNumber() const;

   /**
    * Set serial number (for OggVorbis only)
    **/
   void setSerialNumber(int serial);

  /**
   * Returns true if the WAV file contains a SCOT chunk, otherwise false.
   **/
   bool getScotChunk() const;

   bool getAIR1Chunk() const;

   double getNormalizeLevel() const;
   void setNormalizeLevel(double level);

  private:
   RDWaveFile::Type GetType(int fd);
   bool IsWav(int fd);
   bool IsMpeg(int fd);
   bool IsOgg(int fd);
   bool IsAtx(int fd);
   bool IsTmc(int fd);
   bool IsFlac(int fd);
   bool IsAiff(int fd);
   off_t FindChunk(int fd,const char *chunk_name,unsigned *chunk_size,
		   bool big_end=false);
   bool GetChunk(int fd,const char *chunk_name,unsigned *chunk_size,
		 unsigned char *chunk,size_t size,bool big_end=false);
   void WriteChunk(int fd,const char *cname,unsigned char *buf,unsigned size,
		   bool big_end=false);
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
   bool MakeCart();
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
   void ValidateMetadata();
#ifdef HAVE_VORBIS
   int WriteOggPage(ogg_page *page);
#endif  // HAVE_VORBIS
   int WriteOggBuffer(char *buf,int size);
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
   unsigned long pts;              // The MPEG PTS
   RDWaveFile::MpegID mpeg_id;              
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
   vector<unsigned short> energy_data;
   bool energy_loaded;
   unsigned energy_ptr;
   int wave_id;
   RDWaveFile::Type wave_type;

   unsigned char *cook_buffer;
   int cook_buffer_size;
   RDWaveFile::Encoding cook_encoding;
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

