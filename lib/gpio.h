// gpio.h
//
// A device driver for MeasurementComputing GPIO cards.
//
// Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: gpio.h,v 1.1 2007/09/18 11:20:00 fredg Exp $
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

#ifndef GPIO_H
#define GPIO_H

#include <linux/ioctl.h>

/*
 * For non-Automake only
 */
#define GPIO_DEV_MAJOR 125

/*
 * Modes
 */
#define GPIO_MODE_AUTO 0
#define GPIO_MODE_INPUT 1
#define GPIO_MODE_OUTPUT 2

/*
 * Device Capability Flags
 */
#define GPIO_CAP_FILTER 0x00000001
#define GPIO_CAP_MODE 0x00000002


struct gpio_info {
  char name[48];     // Model/Description of board
  int mode;          // Board Mode (PCI-DIO24 only)
  int inputs;        // Number of input lines
  int outputs;       // Number of output lines
  int samples;       // Number of analog sample channels
  int depth;         // Number of sample bits per analog channel
  unsigned caps;     // Device capabilities
};

struct gpio_mask {
  unsigned mask[4];
};

struct gpio_line {
  int line;          // The number of the output line (0 - 127)
  int state;         // 0 = Off, 1 = On
};

struct gpio_sample {
  int line;          // The number of the input line (0 - 127)
  int gain;          // Channel gain, value 0 - 3
  unsigned sample;   // The sample value
};

/*
 * IOCTLS
 */
#define GPIO_IOCTL_MAGIC 0xfd
#ifdef KERNEL_2_4
#define GPIO_GETINFO _IOR(GPIO_IOCTL_MAGIC,1,sizeof(struct gpio_info))
#define GPIO_SETMODE _IOW(GPIO_IOCTL_MAGIC,2,sizeof(unsigned))
#define GPIO_GET_INPUTS _IOR(GPIO_IOCTL_MAGIC,3,sizeof(struct gpio_mask))
#define GPIO_GET_OUTPUTS _IOR(GPIO_IOCTL_MAGIC,4,sizeof(struct gpio_mask))
#define GPIO_GET_FILTERS _IOR(GPIO_IOCTL_MAGIC,5,sizeof(struct gpio_mask))
#define GPIO_SET_OUTPUT _IOW(GPIO_IOCTL_MAGIC,6,sizeof(struct gpio_line))
#define GPIO_SET_FILTER _IOW(GPIO_IOCTL_MAGIC,7,sizeof(struct gpio_line))
#define GPIO_SET_OUTPUTS _IOW(GPIO_IOCTL_MAGIC,8,sizeof(struct gpio_mask))
#define GPIO_SET_FILTERS _IOW(GPIO_IOCTL_MAGIC,9,sizeof(struct gpio_mask))
#define GPIO_GET_SAMPLE _IOWR(GPIO_IOCTL_MAGIC,10,sizeof(struct gpio_sample))
#else
#define GPIO_GETINFO _IOR(GPIO_IOCTL_MAGIC,1,struct gpio_info)
#define GPIO_SETMODE _IOW(GPIO_IOCTL_MAGIC,2,unsigned)
#define GPIO_GET_INPUTS _IOR(GPIO_IOCTL_MAGIC,3,struct gpio_mask)
#define GPIO_GET_OUTPUTS _IOR(GPIO_IOCTL_MAGIC,4,struct gpio_mask)
#define GPIO_GET_FILTERS _IOR(GPIO_IOCTL_MAGIC,5,struct gpio_mask)
#define GPIO_SET_OUTPUT _IOW(GPIO_IOCTL_MAGIC,6,struct gpio_line)
#define GPIO_SET_FILTER _IOW(GPIO_IOCTL_MAGIC,7,struct gpio_line)
#define GPIO_SET_OUTPUTS _IOW(GPIO_IOCTL_MAGIC,8,struct gpio_mask)
#define GPIO_SET_FILTERS _IOW(GPIO_IOCTL_MAGIC,9,struct gpio_mask)
#define GPIO_GET_SAMPLE _IOWR(GPIO_IOCTL_MAGIC,10,struct gpio_sample)
#endif  // KERNEL_2_4

#ifdef __KERNEL__

#include <linux/autoconf.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/pci.h>
#include <linux/ioport.h>

#ifndef CONFIG_PCI
#error "No PCI Support Present"
#endif

/*
 * Driver Defines
 */
#define GPIO_MAX_CARDS 8
#define GPIO_CBOARDS_VENDOR_ID 0x1307
#define GPIO_PCI_DIO24_DEVICE_ID 0x0028
#define GPIO_PCI_PDISO8_DEVICE_ID 0x000C
#define GPIO_PCI_PDISO16_DEVICE_ID 0x000D
#define GPIO_PCI_DAS1000_DEVICE_ID 0x004C


#endif  // __KERNEL__

#endif  // GPIO_H
