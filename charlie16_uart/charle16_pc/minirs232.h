/* ---------------------  minirs232.h  ---------------------

     Minilibrary zur Verwendung EINER einzelnen RS-232
     - gloabale einzelnes Handle (deshalb nur eine
       Schnittstelle moeglich)
     - Sicherung des Konsolenstatus in globale Variable

     18.04.2016  R. Seelig
   --------------------------------------------------------- */

#ifndef in_minirs
  #define in_minirs

  #include <fcntl.h>
  #include <stdio.h>
  #include <string.h>
  #include <errno.h>
  #include <termios.h>
  #include <unistd.h>
  #include <stdlib.h>


  // ----------------- Globale Variable ---------------

  extern int               myhandle;                       // Streamhandle fuer die ser. Schnittstelle
  extern struct termios    consave;


  /* ---------------------------------------------------------
                         serial_init

       initialisiert serielle Schnittstelle

       *portname => serielles device
                    (z.Bsp.:  /dev/ttyUSB0
                              /dev/ttyS0

       baurd     => Baudrate

     Rueckgabe:
          0 : fehlerfrei
         -1 : Fehler in tcsetattr
         -2 : nicht unterstuetzte Baudrate
         -3 : serielle Schnittstelle nicht vorhanden
     --------------------------------------------------------- */
  char serial_init(char *portname, int baudr);

/* ---------------------------------------------------------
                    serial_getchar_no_wait
     liest ein Zeichen von der Schnittstelle ein, wartet
     aber nicht darauf

     Rueckgabe:
        > 0  : gelesenes Zeichen
        == 0 : es ist kein Zeichen eingetrofen
   --------------------------------------------------------- */
char serial_getchar_no_wait(void);

  /* ---------------------------------------------------------
                        serial_getchar
       liest ein Zeichen von der Schnittstelle ein

       Rueckgabe:
          > 0  : gelesenes Zeichen
          == 0 : es ist kein Zeichen eingetrofen
     --------------------------------------------------------- */
  char serial_getchar(void);

  /* ---------------------------------------------------------
                        serial_putchar
       sendet Zeichen auf der seriellen Schnittstelle
     --------------------------------------------------------- */
  void serial_putchar(char ch);

  /* ---------------------------------------------------------
                        serial_close
       gibt serielle Schnittstelle wieder frei
     --------------------------------------------------------- */
  int serial_close(void);

#endif
