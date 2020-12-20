/* --------------------------------------------------------------------------
                                    ch340reset.c

     zuruecksetzen eines CH340 USB-Chips (USB2RS232). Nur der erste gefundene
     Chip wird zurueckgesetzt.

     Ausgangsprogramm ist eine Version von lusb (siehe angehaengten
     Kommentar).

     Aenderungen zum Originalen lusb:

       Programm- Projectname geaendert nach ch340reset
       geaendertes Makefile

       device.c

         - eingefuegte Funktion: get_buspathname
                                 ( gibt den Pfad zurueck unter dem der CH340
                                   vom System angemeldet wurde )
           Prototyp in ch340reset.h

         - originale Funktion: print_usb_devices
                               ( es werden Information zu Startpoint und
                                 Endpoint ausgeblendet )

     Durchfuehren eines USB-Resets fuer den ersten angeschlossen CH340

     09.01.2018  R. Seelig

   --------------------------------------------------------------------------
                            Originalkommentar lusb
   -------------------------------------------------------------------------- */
/*
 *
 *
 * Copyright (C) 2009 Kay Sievers <kay.sievers@vrfy.org>
 * Copyright (C) 2009 Greg Kroah-Hartman <greg@kroah.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */

//   --------------------------------------------------------------------------


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <syslog.h>
#include <fcntl.h>
#include <poll.h>
#include <limits.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/ioctl.h>

#include <linux/usbdevice_fs.h>


#define LIBUDEV_I_KNOW_THE_API_IS_SUBJECT_TO_CHANGE

#include <libudev.h>
#include "list.h"
#include "usb.h"
#include "ch340reset.h"


static LIST_HEAD(usb_devices);
struct udev *udev;

void *robust_malloc(size_t size)
{
  void *data;

  data = malloc(size);
  if (data == NULL) exit(1);
  memset(data, 0, size);
  return data;
}

char *get_dev_string(struct udev_device *device, const char *name)
{
  const char *value;

  value = udev_device_get_sysattr_value(device, name);
  if (value != NULL) return strdup(value);
  return NULL;
}


/* --------------------------------------------------------------------------
                                     M-A-I-N
   -------------------------------------------------------------------------- */
int main(void)
{
  char   buspathname[256];
  int    fd;
  int    rc;


  struct udev_enumerate *enumerate;
  struct udev_list_entry *list_entry;

  // libudev context
  udev = udev_new();

  // prepare a device scan
  enumerate = udev_enumerate_new(udev);

  // filter for usb devices
  udev_enumerate_add_match_subsystem(enumerate, "usb");

  // retrieve the list
  udev_enumerate_scan_devices(enumerate);

  // print devices
  udev_list_entry_foreach(list_entry, udev_enumerate_get_list_entry(enumerate))
  {
    struct udev_device *device;

    device = udev_device_new_from_syspath(udev_enumerate_get_udev(enumerate),
                              udev_list_entry_get_name(list_entry));

    if (device == NULL) continue;
    if (strcmp("usb_device", udev_device_get_devtype(device)) == 0)
      create_usb_device(device);

    udev_device_unref(device);
  }
  udev_enumerate_unref(enumerate);

  udev_unref(udev);
  sort_usb_devices();

  buspathname[0]= 0;
//  print_usb_devices();


  // VID:PID 7392:7811 ist ID Edimax WLAN-Adapter
  // get_buspathname("1a86", "7523", &buspathname[0]);

  // VID:PID 413c:2106 ist ID Tastatur
  // get_buspathname("1a86", "7523", &buspathname[0]);

  // VID:PID 1a86:7523 ist ID des CH340 Chips
  get_buspathname("1a86", "7523", &buspathname[0]);
  free_usb_devices();

  // Reset des CH340 (ueber den Device Pathnamen (Bsp.: /dev/bus/usb/001/013)
  if (buspathname[0])
  {
    fd = open(buspathname, O_WRONLY);

    // Fehler beim Oeffnen des Device (sollte nicht vorkommen, da das
    // Device zuvor bestimmt wurde
    if (fd < 0) { printf("\nopening error\n"); return 1; }

    printf("\nResetting CH340... ");
    rc = ioctl(fd, USBDEVFS_RESET, 0);

    // Fehler im ioctl
    if (rc < 0) { printf("\nioctl error\n"); return 1; }
    printf("Done\n");

    close(fd);
  }

  return 0;
}
