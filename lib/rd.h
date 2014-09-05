// rd.h
//
// System-Wide Values for Rivendell
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rd.h,v 1.194.6.14.2.1 2014/05/22 14:30:44 cvs Exp $
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

#ifndef RD_H
#define RD_H

/*
 * Default Configuration File
 */
#define RD_CONF_FILE "/etc/rd.conf"
#define RD_WIN_CONF_FILE "rd.ini"

/*
 * Default ALSA asoundrc file
 */
#define RD_ASOUNDRC_FILE "/etc/asound.conf"

/*
 * PID File Locations
 */
#define RD_PID_DIR "/var/run/rivendell"
#define RD_CAED_PID "/var/run/rivendell/caed.pid"
#define RD_RIPCD_PID "/var/run/rivendell/ripcd.pid"
#define RD_RDCATCHD_PID "/var/run/rivendell/rdcatchd.pid"

/*
 * Where audio gets stored
 */
#define RD_AUDIO_ROOT "/var/snd"

/*
 * Audio File Extension
 */
#define RD_AUDIO_EXTENSION "wav"

/*
 * Allow Nonstandard Bitrates?
 */
#define RD_ALLOW_NONSTANDARD_RATES false

/*
 * Default mySQL Settings
 */
#define DEFAULT_MYSQL_HOSTNAME "localhost"
#define DEFAULT_MYSQL_DATABASE "Rivendell"
#define DEFAULT_MYSQL_USERNAME "rduser"
#define DEFAULT_MYSQL_PASSWORD "letmein"
#define DEFAULT_MYSQL_DRIVER "QMYSQL3"
#define DEFAULT_MYSQL_HEARTBEAT_INTERVAL 360
#define MYSQL_BUILTIN_DATABASE "mysql"
#define POSTGRESQL_BUILTIN_DATABASE "template1"

/*
 * Default Logging Settings
 */
#define DEFAULT_LOG_FACILITY "syslog"
#define DEFAULT_LOG_CORE_DUMP_DIRECTORY "/"
#define DEFAULT_LOG_PATTERN "%n-%Y%M%d.log"

/*
 * ALSA Settings
 */
#define RD_ALSA_DEFAULT_PERIOD_QUANTITY 4
#define RD_ALSA_DEFAULT_PERIOD_SIZE 1024
#define RD_ALSA_FADE_INTERVAL 100
#define RD_ALSA_SAMPLE_RATE_TOLERANCE 100

/*
 * Date Limits
 */
#define RD_MAX_YEAR 8000

/*
 * JACK Settings
 */
#define RD_JACK_FADE_INTERVAL 100

/*
 * RIPCD TCP Port
 */
#define RIPCD_TCP_PORT 5006

/*
 * CAED TCP Port
 */
#define CAED_TCP_PORT 5005

/*
 * RdCatchd TCP Port
 */
#define RDCATCHD_TCP_PORT 6006

/*
 * Minimum event ID for dynamic (RML-controlled) recordings
 */
#define RDCATCHD_DYNAMIC_BASE_ID 1000000000

/*
 * Default Local GPIO Device
 */
#define RD_DEFAULT_GPIO_DEVICE "/dev/gpio0"

/*
 * Max number of possible GPIO lines (GPI OR GPO)
 */
#define MAX_GPIO_PINS 32768

/*
 * Max number of possible workstations
 */
#define RD_MAX_STATIONS 64

/*
 * Max number of possible audio adapters
 */
#define RD_MAX_CARDS 8

/*
 * Max number of possible audio streams/card/type
 */
#define RD_MAX_STREAMS 16

/*
 * Max number of possible audio ports/card/type
 */
#define RD_MAX_PORTS 8

/*
 * Max number of possible TTYs
 */
#define MAX_TTYS 8

/*
 * Max number of netcatcher decks per workstation
 */
#define MAX_DECKS 8

/*
 * Max number of inputs or outputs in a switcher
 */
#define MAX_ENDPOINTS 1024

/*
 * Max number of attached switcher matrices per workstation
 */
#define MAX_MATRICES 8

/*
 * Max number of macro timers
 */
#define RD_MAX_MACRO_TIMERS 16

/*
 * Reference Analog Level at 0 dBFS (1/100 dB)
 * (thus, -16 dBFS = +4 dBu)
 */
#define RD_BASE_ANALOG 1600 

/*
 * Maximum number of SoundPanels of a given type
 */
#define MAX_PANELS 50

/*
 * Maximum Cart Number
 */
#define RD_MAX_CART_NUMBER 999999

/*
 * Maximum Cut Number
 */
#define RD_MAX_CUT_NUMBER 999

/*
 * In negative 1/100ths of a dB
 */
#define REFERENCE_LEVEL 1600

/*
 * Typomatic Rate Interval, in mS
 */
#define TYPO_RATE_1 300
#define TYPO_RATE_2 100

/*
 * ALSA Values
 */
#define ALSA_PLAY_PCM_DEVICE "rdp"
#define ALSA_RECORD_PCM_DEVICE "rdr"

/*
 * Default Administrative User
 */
#define RDA_LOGIN_NAME "admin"
#define RDA_PASSWORD ""
#define RDA_FULLNAME "Rivendell Administrator"
#define RDA_DESCRIPTION "Default Administrator Account"

/*
 * Default System User
 */
#define RD_USER_LOGIN_NAME "user"
#define RD_USER_PASSWORD ""
#define RD_USER_FULL_NAME "Rivendell User"
#define RD_USER_DESCRIPTION "Default User Account"

/*
 * Default System Identities
 */
#define RD_DEFAULT_AUDIO_OWNER "user"
#define RD_DEFAULT_AUDIO_GROUP "users"
#define RD_DEFAULT_LABEL "Default Configuration"

/*
 * Default Audio Store Settings
 */
#define RD_DEFAULT_AUDIO_STORE_MOUNT_OPTIONS "defaults"

/*
 * Default RDSelect Configuration Directory
 */
#define RD_DEFAULT_RDSELECT_DIR "/etc/rivendell.d"

/*
 * Default Workstation
 */
#define RD_STATION_NAME "DEFAULT"
#define RD_STATION_DESCRIPTION "Default Workstation"

/*
 * Default Program Service
 */
#define RD_SERVICE_NAME "Production"
#define RD_SERVICE_DESCRIPTION "Default Audio Service"

/*
 * RDSelect Label
 * (Used for the RDSelect entry in RDAdmin->ManageHosts)
 */
#define RD_RDSELECT_LABEL "[RDSelect]"

/*
 * Default Maximum POST Length (bytes)
 */
#define RD_DEFAULT_MAX_POST_LENGTH 10000000

/*
 * Pause Time for Starting Daemons (secs)
 */
#define RD_DAEMON_PAUSE_TIME 1

/*
 * Location of the proc filesystem
 */
#define RD_PROC_DIR "/proc"

/*
 * Location for Lock Files
 */
#define RD_LOCKFILE_DIR "/var/lock"

/*
 * Rivendell Macro Language (RML)
 */
#define RD_RML_ECHO_PORT 5858
#define RD_RML_NOECHO_PORT 5859
#define RD_RML_REPLY_PORT 5860
#define RD_RML_MAX_ARGS 100
#define RD_RML_MAX_LENGTH 2048

/*
 * Maximum Permissible Time Offset (+/- mS)
 */
#define RD_MAX_TIME_OFFSET 10000

/*
 * The file to save geometry state in
 */
#define RD_GEOMETRY_FILE ".rivendell"

/*
 * Meter Update Interval (msecs)
 */
#define RD_METER_UPDATE_INTERVAL 20

/*
 * The metering block shared memory key
 */
#define RD_METER_SHM_KEY 0x00005005

/*
 * Maximum length of import log file line
 */
#define RD_MAX_IMPORT_LINE_LENGTH 1024

/*
 * The minimum fader gain at the end of a segue transition
 * (in hundreths of a dB)
 */
#define RD_FADE_DEPTH -3000

/*
 * The mixer 'off' level for full muting
 */
#define RD_MUTE_DEPTH -10000

/*
 * The fade profile (for HPI adapters only)
 */
#define RD_FADE_TYPE RDHPISoundCard::Log

/*
 * This defines the maximum frequency (in Hz) of the broadcast audio
 * channel being fed by Rivendell.  It is used for things like calculating
 * how expensive an algorithm is needed for sample rate conversion.
 *
 * For modern FM facilities running MPX stereo, 15000 Hz is a reasonable
 * value.
 */
#define RD_MAX_BANDPASS 15000

/*
 * The limits on timescaling
 */
#define RD_TIMESCALE_MIN 0.833
#define RD_TIMESCALE_MAX 1.250
#define RD_TIMESCALE_DIVISOR 100000.0

/*
 * Max number of default services
 */
#define RD_MAX_DEFAULT_SERVICES 32

/*
 * Ripper Settings
 */
#define RIPPER_BAR_INTERVAL 500
#define RIPPER_TEMP_WAV "rdlibrary_rip.wav"
#define RIPPER_TEMP_PEAK "rdlibrary_rip.dat"
#define RIPPER_CDDB_USER "rdlibrary"
#define RIPPER_MAX_SECTORS 20

/*
 * CAE Values
 */
#define CAE_MAX_ARGS 10
#define CAE_MAX_LENGTH 256
#define CAE_POLL_INTERVAL 50
#define CAE_MAX_CONNECTIONS 128

/*
 * Default Sample Rate
 */
#define RD_DEFAULT_SAMPLE_RATE 48000

/*
 * Default Channels
 */
#define RD_DEFAULT_CHANNELS 2

/*
 * Marker Colors
 */
#define RD_SEGUE_MARKER_COLOR Qt::cyan
#define RD_TALK_MARKER_COLOR Qt::blue
#define RD_START_END_MARKER_COLOR Qt::red
#define RD_FADE_MARKER_COLOR Qt::yellow
#define RD_HOOK_MARKER_COLOR Qt::magenta

/*
 * Cart Status Colors
 */
#define RD_CART_ERROR_COLOR Qt::red
#define RD_CART_CONDITIONAL_COLOR Qt::yellow
#define RD_CART_FUTURE_COLOR "#00FFFF"
#define RD_CART_EVERGREEN_COLOR "#008000"
#define RD_CART_INVALID_SERVICE_COLOR Qt::magenta

/*
 * RDAirPlay Colors
 */
#define RDPANEL_SETUP_FLASH_COLOR Qt::blue
#define RDPANEL_RESET_FLASH_COLOR Qt::blue
#define RDPANEL_PLAY_BACKGROUND_COLOR Qt::red
#define RDPANEL_PAUSED_BACKGROUND_COLOR Qt::cyan
#define BUTTON_STOPPED_BACKGROUND_COLOR Qt::green
#define BUTTON_PLAY_BACKGROUND_COLOR Qt::red
#define BUTTON_PLAY_TEXT_COLOR Qt::black
#define BUTTON_PAUSE_BACKGROUND_COLOR Qt::cyan
#define BUTTON_PAUSE_TEXT_COLOR Qt::black
#define BUTTON_FROM_BACKGROUND_COLOR Qt::magenta
#define BUTTON_FROM_TEXT_COLOR Qt::black
#define BUTTON_TO_BACKGROUND_COLOR Qt::yellow
#define BUTTON_TO_TEXT_COLOR Qt::black
#define BUTTON_DISABLED_BACKGROUND_COLOR Qt::darkGray
#define BUTTON_DISABLED_TEXT_COLOR Qt::white
#define BUTTON_ERROR_BACKGROUND_COLOR Qt::red
#define BUTTON_ERROR_TEXT_COLOR Qt::black

/*
 * RDAirPlay Log Machines
 */
#define RDAIRPLAY_LOG_QUANTITY 3

/*
 * Cue Editor Colors
 */
#define RD_CUEEDITOR_KNOB_COLOR blue
#define RD_CUEEDITOR_PLAY_MARKER black
#define RD_CUEEDITOR_START_MARKER red
#define RD_CUEEDITOR_BUTTON_FLASH_PERIOD 200
#define RD_CUEEDITOR_AUDITION_PREROLL 5000

/*
 * Log Colors
 */
#define RD_CUSTOM_TRANSITION_COLOR Qt::blue

/*
 * Default Text Editors
 */
#define RD_LINUX_EDITOR "xterm -e vi"
#define RD_WIN32_EDITOR "notepad"

/*
 * System-wide Maintenance Interval (mS)
 */
#define RD_MAINT_MIN_INTERVAL 900000
#define RD_MAINT_MAX_INTERVAL 3600000

/*
 * Audio File Filter for QFileDialog
 */
#define RD_AUDIO_FILE_FILTER "Sound Files (*.mp* *.MP* *.wav *.WAV *.ogg *.OGG *.flac *.FLAC *.atx *.ATX *.tmc *.TMC *.aif* *.AIF*)\nAIFF Files (*.aif* *.AIF*)\nATX Files (*.atx *.ATX)\nMPEG Files (*.mp* *.MP*)\nOggVorbis Files (*.ogg *.OGG)\nFLAC Files (*.flac *.FLAC)\nTM Century GoldDrive Files (*.tmc *.TMC)\nWAV Files (*.wav *.WAV)\nAll Files (*.*)"

/*
 * Image File Filter for QFileDialog
 */
#define RD_IMAGE_FILE_FILTER "Image Files (*.png *.bmp *.xbm *.xpm *.pbm *.pgm *.ppm *.jpg *.mng *.gif *.PNG *.BMP *.XBM *.XPM *.PBM *.PGM *.PPM *.JPG *.MNG *.GIF)\nAll Files (*.*)"

/*
 * Loadable Module Filter for QFileDialog
 */
#define RD_MODULE_FILE_FILTER "Rivendell Loadable Modules (*.rlm)\nAll Files (*.*)"

/*
 * Web Interface Settings
 */
#define RD_WEB_BACKGROUND_COLOR "#FFFFFF"
#define RD_WEB_LINE_COLOR1 "#E0E0E0"
#define RD_WEB_LINE_COLOR2 "#F0F0F0"

/*
 * LiveWire Values
 */
#define RD_LIVEWIRE_DEFAULT_TCP_PORT 93
#define RD_LIVEWIRE_DEFAULT_STREAM_PORT 5004
#define RD_LIVEWIRE_DEFAULT_CHANNELS 2
#define RD_LIVEWIRE_MAX_CMD_LENGTH 1024
#define RD_LIVEWIRE_GPIO_BUNDLE_SIZE 5
#define RD_LIVEWIRE_DEFAULT_LOAD RDLiveWireDestination::LoadHighZ
#define RD_LIVEWIRE_MAX_SOURCE 0x7FFF
#define RD_LIVEWIRE_GPIO_MCAST_ADDR "239.192.255.4"
#define RD_LIVEWIRE_GPIO_SEND_PORT 2055
#define RD_LIVEWIRE_GPIO_RECV_PORT 2060
#define RD_LIVEWIRE_GPIO_PULSE_WIDTH 100

/*
 * Number of Carts to Include in a 'Limited' Search
 */
#define RD_LIMITED_CART_SEARCH_QUANTITY 100

/*
 * Default title for a new cart
 */
#define RD_DEFAULT_CART_TITLE "[new cart]"

/*
 * Timeout for libcurl (secs)
 */
#define RD_CURL_TIMEOUT 1200

/*
 * Anonymous FTP Credentials
 */
#define RD_ANON_FTP_USERNAME "anonymous"
#define RD_ANON_FTP_PASSWORD "rivendell"

/*
 * Status Monitor Height
 */
#define RDMONITOR_HEIGHT 30

/*
 * RDAirPlay Colors
 */
#define RDPANEL_SETUP_FLASH_COLOR Qt::blue
#define RDPANEL_RESET_FLASH_COLOR Qt::blue
#define RDPANEL_PLAY_BACKGROUND_COLOR Qt::red
#define RDPANEL_PAUSED_BACKGROUND_COLOR Qt::cyan
#define BUTTON_STOPPED_BACKGROUND_COLOR Qt::green
#define BUTTON_PLAY_BACKGROUND_COLOR Qt::red
#define BUTTON_PLAY_TEXT_COLOR Qt::black
#define BUTTON_PAUSE_BACKGROUND_COLOR Qt::cyan
#define BUTTON_PAUSE_TEXT_COLOR Qt::black
#define BUTTON_FROM_BACKGROUND_COLOR Qt::magenta
#define BUTTON_FROM_TEXT_COLOR Qt::black
#define BUTTON_TO_BACKGROUND_COLOR Qt::yellow
#define BUTTON_TO_TEXT_COLOR Qt::black
#define BUTTON_DISABLED_BACKGROUND_COLOR Qt::darkGray
#define BUTTON_DISABLED_TEXT_COLOR Qt::white
#define BUTTON_ERROR_BACKGROUND_COLOR Qt::red
#define BUTTON_ERROR_TEXT_COLOR Qt::black

#define LOG_SCHEDULED_COLOR Qt::white
#define LOG_PLAYING_COLOR Qt::green
#define LOG_PAUSED_COLOR Qt::cyan
#define LOG_NEXT_COLOR "#CCFFCC"
#define LOG_FINISHED_COLOR Qt::gray
#define LOG_EVERGREEN_COLOR "#008000"
#define LOG_ERROR_COLOR Qt::red
#define LOG_RELATIVE_TEXT_COLOR Qt::black
#define LOG_HARDTIME_TEXT_COLOR Qt::blue

/*
 * Cart Label Box Colors
 */
#define BAR_UNCHANGED_TRANSITION_COLOR Qt::green
#define BAR_UNCHANGED_STOPPING_COLOR Qt::red
#define BAR_CHANGED_TRANSITION_COLOR Qt::yellow
#define BAR_CHANGED_STOPPING_COLOR Qt::blue
#define LABELBOX_BACKGROUND_COLOR Qt::white
#define LABELBOX_MISSING_COLOR Qt::red
#define LABELBOX_MARKER_COLOR Qt::cyan
#define LABELBOX_CHAIN_COLOR Qt::magenta
#define LABELBOX_EVERGREEN_COLOR "#008000"
#define LABELBOX_TIMESCALE_COLOR Qt::green

/*
 * RDCartSlots Settings
 */
#define RDCARTSLOTS_MAX_ROWS 16
#define RDCARTSLOTS_MAX_COLUMNS 4

/*
 * Custom MIME Types (for Drag and Drop)
 */
#define RDMIMETYPE_CART "application/rivendell-cart"


#endif  // RD_H
