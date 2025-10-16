/* ----------------------------------------------------------
                      charlie16_send.c

     Sendet 16-Bit hexadezimale Werte an den
     Charlie16_Plexing Empfaenger mit 2400 Bd.

     PC-Konsolenprogramm zum Testen der Funktionsfaehigkeit
     der charliegeplexten Anzeige


     17.12.2020   R. Seelig
   ---------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "minirs232.h"

#define baudrate   2400

#define my_usleep(anz)     ({ usleep(anz); fflush(stdout);} )


/* ---------------------------------------------------------------------------
                         existfile
     stellt fest, ob eine Datei vorhanden ist oder nicht
   --------------------------------------------------------------------------- */
char existfile(char *dname)
{
  FILE    *tdat;

  tdat= fopen(dname,"r");
  if (tdat== 0)
  {
    return 0;
  }
  else
  {
    fclose(tdat);
    return 1;
  }
}


/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(int argc, char **argv)
{

  char         portname[256];
  char         string[256];
  int          error_nr;
  unsigned int inp;
  int          i;
  uint16_t     w;

  if (argc == 3)
  {
    // Valueargument als Hexwert interpretieren
    // (wuerde man evtl. als htoi bezeichnen koennen);
    strcpy(string,argv[2]);
    sscanf(string, "%x", &i);
    w= i & 0xffff;

    strcpy(portname,argv[1]);
    error_nr= existfile(&portname[0]);
    if (!error_nr)
    {
      printf("\n   serialport not found: %s\n\n", portname);
      return 1;
    }

    error_nr= serial_init(&portname[0], baudrate);
    if (error_nr)
    {
      printf("\nerror: serialport %s\n\n",portname[0]);
      return 3;
    }

    printf("\n\n Port: %s | Value: %x \n\n", portname, w);
    serial_putchar(w >> 8);
    my_usleep(40000);
    serial_putchar(w & 0xff);
    serial_close();
  }
  else
  {
    printf("\n Syntax : charlie16_send port hexvalue");
    printf("\n Example: charlie16_send /dev/ttyUSB0 3C09\n\n");
  }
}

