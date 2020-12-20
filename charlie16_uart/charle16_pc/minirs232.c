/* ---------------------  minirs232.c  ---------------------

     Minilibrary zur Verwendung EINER einzelnen RS-232
     - gloabale einzelnes Handle (deshalb nur eine
       Schnittstelle moeglich)
     - Sicherung des Konsolenstatus in globale Variable

     18.04.2016  R. Seelig
   --------------------------------------------------------- */


#include "minirs232.h"


// ----------------- Globale Variable ---------------

int               myhandle = 0;                       // Streamhandle fuer die ser. Schnittstelle
struct termios    consave;


/* ---------------------------------------------------------
                 set_interface_attribs
     initialisiert serielle Schnittstelle ueber Linux-
     funktionen.
     Protokoll ist 8 Datenbits, 1 Stopbit

     speed => Baudrate
     paritiy => Paritaetsbit

   --------------------------------------------------------- */
int set_interface_attribs (int speed, int parity)
{
  struct termios tty;

  memset (&tty, 0, sizeof tty);
  if (tcgetattr (myhandle, &tty) != 0)
  {
    printf ("Error: %d from tcgetattr", errno);
    return -1;
  }

  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars

  tty.c_iflag &= ~IGNBRK;                         // disable break processing

  tty.c_lflag = 0;                                // no signaling chars, no echo,
                                                  // no canonical processing
  tty.c_oflag = 0;                                // no remapping, no delays
  tty.c_cc[VMIN]  = 0;                            // read doesn't block
  tty.c_cc[VTIME] = 5;                            // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls,
                                                  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);              // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr (myhandle, TCSANOW, &tty) != 0)
  {
    printf ("Error: %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

/* ---------------------------------------------------------
                        set_blocking
   --------------------------------------------------------- */
void set_blocking (int should_block)
{
  struct termios tty;

  memset (&tty, 0, sizeof tty);
  if (tcgetattr (myhandle, &tty) != 0)
  {
    printf ("Error: %d from tggetattr", errno);
    return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;                               // 0.5 seconds read timeout

  if (tcsetattr (myhandle, TCSANOW, &tty) != 0)
    printf ("Error: %d Attribute settings", errno);
}

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

char serial_init(char *portname, int baudr)
{
  int     baudrlist[20] = {0, 50, 75, 110, 134, 150, 200, 300, 600,     \
                           1200, 1800, 2400, 4800, 9600, 19200, 38400,  \
                           57600, 115200, 230400, -1 };
  char    sttycall[512];
  int     i;
  struct  termios tp;

  if (myhandle != 0) return -4;
  sttycall[0]= 0;
  strcat(sttycall,"stty -F ");
  strcat(sttycall,portname);
  strcat(sttycall," hupcl cooked > /dev/NULL");

  for (i= 0; ((i< 19) && (baudr != baudrlist[i])); i++);       // sucht Eintrag in der zulaessigen Baudratenliste
  if (i== 19)                                                  // unzulaessige Baudrate
  {
    printf ("Error: %d illegal baudrate", baudr);
    return -2;
  }
  if (i> 15) { i+= 4081; }                                     // Baudraten > 38400 haben Initialisierungsnummer ab 4097
                                                               // ansonsten Baudratennummer 1..15
  baudr= i;
  system(sttycall);


  if (tcgetattr(STDIN_FILENO, &tp) == -1)                      // Fehler in tcgetattr
  {
    printf("Error in: tcsetattr");
    return -1;
  }

  consave = tp;                                                // Konsolenparameter sichern

  tp.c_lflag &= ~ECHO;                                         // Konsolenecho ausschalten :  ECHO off
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)
  {
    printf("Error in: tcsetattr");
    return -1;
  }

  myhandle = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NDELAY);
  if (myhandle < 0)
  {
    printf (" Error %d\n\r Portname %s:  %s\n\r", errno, portname, strerror (errno));
    return -3;
  }

  set_interface_attribs (baudr, 0);                 // set speed to xxxxx bps, 8n1 (no parity) : Speed is NOT the baudrate,
                                                               //                                           it is the "speed-index" of LINUX
  set_blocking (0);                                 // set no blocking

  return 0;
}

/* ---------------------------------------------------------
                    serial_getchar_no_wait
     liest ein Zeichen von der Schnittstelle ein, wartet
     aber nicht darauf

     Rueckgabe:
        > 0  : gelesenes Zeichen
        == 0 : es ist kein Zeichen eingetrofen
   --------------------------------------------------------- */
char serial_getchar_no_wait(void)
{

  int   b;
  int   anz;

  anz = read (myhandle, &b, 1);
  if (anz> 0) return (char) b; else return 0;
}

/* ---------------------------------------------------------
                      serial_getchar
     liest ein Zeichen von der Schnittstelle ein

     Rueckgabe:
        > 0  : gelesenes Zeichen
        == 0 : es ist kein Zeichen eingetrofen
   --------------------------------------------------------- */
char serial_getchar(void)
{
  char   b;

  do
  {
    b = serial_getchar_no_wait();
  } while(!b);
  return b;
}

/* ---------------------------------------------------------
                      serial_putchar
     sendet Zeichen auf der seriellen Schnittstelle
   --------------------------------------------------------- */
void serial_putchar(char ch)
{
  write (myhandle, &ch, 1);
}

/* ---------------------------------------------------------
                      serial_close
     gibt serielle Schnittstelle wieder frei
   --------------------------------------------------------- */
int serial_close(void)
{
  myhandle= 0;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &consave) == -1)           // alte Konsolenparameter wieder herstellen (Echo wieder einschalten)
    return -1;
  close(myhandle);
}
