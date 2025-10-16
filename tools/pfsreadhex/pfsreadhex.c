/* ------------------------------------------------------------
                            pfsreadhex.c


      Wertet Intel-Hex Datei fuer Padauk PFS154 / 172 /173
      aus und ermittelt den Speicherbedarf im Flashspeicher
      des Controllers.

      Compiler: GCC

      16.11.2020

      R. Seelig
   ------------------------------------------------------------ */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define mcuanz       6

struct mcudefs
{
  char name[20];
  int flashsize;
  int ramsize;
};

struct mcudefs mculib [mcuanz] =
{
  "PFS154",     0x1000,   128,
  "PFS172",     0x1000,   128,
  "PFS173",     0x1800,   256,
  "PMS152",     0x0a00,    80,
  "PMS154",     0x1000,   128,
  "PMS171",     0x0c00,    96
};

/* ----------------------------------------------------------
                              strdel

     loescht eine Anzahl n Zeichen aus dem String s ab der
     Position pos (inklusive des Zeichens an pos)
     Erstes Zeichen hat Postion (pos) = 1 (und nicht 0) !!!
   ----------------------------------------------------------*/
char *strdel(char *s, int pos, int n)
{
  pos--;
  if (pos> strlen(s)) return s;
  if ((pos+n) > strlen(s))
  {
    n= strlen(s)-pos;
  }
  memmove(s+pos, s+pos+n, strlen(s)-pos-n+1);
  return s;
}

/* ----------------------------------------------------------
                              strspos

     sucht den String srcstr nach dem Substring substr ab
     und liefert die erste Position an der der Substring
     enthalten ist (0 bei Nichtvorhandensein)
   ----------------------------------------------------------*/
int strspos(char *srcstr, char *substr)
// liefert 0 zurueck, wenn Teilstring nicht vorhanden
{
  char *p;
  p= strstr(srcstr, substr);
  if ((p-srcstr+1)> strlen(srcstr)) return 0;
  return (p-srcstr+1);
}

/* ----------------------------------------------------------
                              strcpos

     sucht den String srcstr nach dem Zeichen ch ab
     und liefert die erste Position an der das Zeichen
     enthalten ist (0 bei Nichtvorhandensein)
   ----------------------------------------------------------*/
int strcpos(char *srcstr, char ch)
{
  char *p;
  p= strchr(srcstr,ch);
  if ((p-srcstr+1)> strlen(srcstr)) return 0;
  return (p-srcstr+1);
}


/* ----------------------------------------------------------
                              strcopy

     kopiert aus dem String src ab der Position pos
     (inklusive) die Anzahl anz Zeichen in den String dest
   ----------------------------------------------------------*/
char *strcopy(char *src, char *dest, int pos, int anz)
{
  int l;

  l= strlen(src);
  if ((pos> l) || (!pos))
  {
    *dest= 0;
    return dest;
  }
  src= src+(pos-1);
  strncpy(dest, src, anz);
  if ( (l-pos+1) < anz) anz= l-pos+1;
  dest[anz]= 0;                         // im Zielstring das Endezeichen setzen
}

/* --------------------------------------------------
                        strtoupper

      konvertiert den String auf den der Zeiger
      zeigt von Kleinbuchstaben nach Grossbuchstaben
   -------------------------------------------------- */
void strtoupper(char *ptr)
{
  while(*ptr)
  {
    *ptr= toupper(*ptr);
    ptr++;
  }
}

/* --------------------------------------------------
                        existfile

     testet, ob eine Datei vorhanden ist oder nicht

     Uebergabe:
         0 : Datei ist nicht vorhanden
         1 : Datei ist vorhanden
   -------------------------------------------------- */
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

/* --------------------------------------------------
                       readhexfile

     liest Intel-Hexdatei und stellt die hoechste
     verwendete Seicheradresse fest.

     dname  :        zu lesende Datei auf Massenspeicher
   -------------------------------------------------- */
int readhexfile(char* dname)
{
  FILE     *tdat;
  char     tx[256], cpybuffer[256];
  int      zeil;
  int      b, b2, banz;
  uint8_t  codbyte, cb;
  int      fadress, highestadr;


  zeil= 0;
  tdat= fopen(dname,"r");
  if (tdat== 0)
  {
//    printf("\n\r%s: no such file\n\r",dname);
    return 1;
  }

  b= 0;
  highestadr= 0;
  do
  {
    fgets(tx,255,tdat);

    do                                 // CR entfernen
    {
      b= strcpos(tx,0x0d);
      if (b) strdel(tx,b,1);
    } while (b);
    do                                 // LF entfernen
    {
      b= strcpos(tx,0x0a);
      if (b) strdel(tx,b,1);
    } while (b);

//    printf("\n%s",tx);

    // Hier jede gelesene Zeile der Hexdatei interpretieren und die hoechste
    // und niedrigste Speicheradresse speichern
    if (tx[0]== ':')                   // korrekter Beginn einer Hexzeile ?
    {
      strcopy(tx,cpybuffer,8,2);
      if (!(strcmp(cpybuffer,"00")))   // wenn es normale Datenbytes sind...
      {                                // ... diese auswerten
        zeil++;

        strcopy(tx,cpybuffer,2,2);                // Anzahl Datenbytes in dieser Zeile
        banz= strtol(cpybuffer,0,16);             // hexstring to longint, Basis 16 (Hex)

        strcopy(tx,cpybuffer,4,4);                // Speicheradresse der Datenbytes dieser Zeile
        fadress= strtol(cpybuffer,0,16);

        for (b= 0; b< banz; b++)
        {
          codbyte= strtol(cpybuffer,0,16);
          fadress++;

          // die Configuration Words fuer hoechste Programmadresse ignorieren
          if (!( ((fadress >= 0x400e) && (fadress <= 0x4012))   ||
                 ((fadress >= 0x10010) && (fadress <= 0x10014))    ))
          {
            if (fadress> highestadr) highestadr= fadress;
          }
        }
      }
    }

  }while (!feof(tdat));
  fclose(tdat);

  return highestadr;
}

/* --------------------------------------------------
                      mapfile_parse

     sucht im Map-File die Stelle, an der angegeben
     ist, wieviel Ram-Speicher die Variable belegen
     und gibt diese Anzahl als Argument zurueck
   -------------------------------------------------- */
int mapfile_parse(char* dname)
{
  FILE     *tdat;
  char     tx[256], cpybuffer[256];
  int      b, rambytes;


  tdat= fopen(dname,"r");
  if (tdat== 0)
  {
    return 1;
  }
  do
  {
    fgets(tx,255,tdat);
    strcopy(&tx[0], &cpybuffer[0], 1, 4);

    // ist eine Zeile mit dem Anfang "Area" gefunden
    if (!(strcmp(&cpybuffer[0], "Area")))
    {
//      printf("%s\n", cpybuffer);
      fgets(tx,255,tdat);             // Zeile auf "Area" folgend sind "-" Zeichen => nicht auswerten
      fgets(tx,255, tdat);
      strcopy(&tx[0], &cpybuffer[0], 1, 4);

      // Zeile mit Ram-Bedarf gefunden
      if (!(strcmp(&cpybuffer[0], "DATA")))
      {
        b= strcpos(&tx[0],'.');
        strcopy(&tx[0], &cpybuffer[0], b-4, 4);
        rambytes= atoi(cpybuffer);
        fclose(tdat);
        return rambytes;
      }
    }

  }while (!feof(tdat));
  fclose(tdat);
  return 0x8000;
}


/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(int argc, char **argv)

{
  int        flashbytes, rambytes;
  char       b;
  char       filename[256];
  char       filename2[256];
  char       devicename[256];
  int        flashsize;
  int        ramsize;
  uint8_t    namelen;
  char       found;


  if (argc < 3)
  {
    printf("\n Syntax: pfsreadhex device projectfile");
    printf("\n\n Note: enter name of projectfile WITHOUT any extensions");
    printf("\n\n     Example: pfsreadhex PFS154 myprog");
    printf("\n\n This will analyse filenames myprog.ihx and myprog.map \n\n");
    return 1;
  }

  strcpy(filename,argv[2]);
  strcpy(filename2, filename);
  strcat(filename,".ihx");
  strcat(filename2,".map");
  strcpy(devicename,argv[1]);
  strtoupper(devicename);


  found= 0; b= 0;
  do
  {
    if (!(strcmp(devicename, mculib[b].name))) found= 1; else b++;
  } while(!found && (b< mcuanz));

  if (found)
  {
    flashsize= mculib[b].flashsize;
    ramsize= mculib[b].ramsize;
  }

  b= existfile(&filename[0]);
  if (!b)
  {
    printf("\n   No such file: %s\n\n", filename);
    return 2;
  }

  b= existfile(&filename2[0]);
  if (!b)
  {
    printf("\n    Note: no map-file: %s found \n\n",filename2);
  }

  rambytes= mapfile_parse(&filename2[0]);
  flashbytes= readhexfile(&filename[0]);

  if (found)
  {
    fprintf(stderr,"Device : %s => Flash-Size: %d words; Ram-Size: %d bytes\n\n",devicename, flashsize / 2, ramsize);
    fprintf(stderr,"Flash  : %d words used (%.1f%% full)\n",flashbytes / 2, (float)(flashbytes*100)/flashsize);
    if (b)
    {
      fprintf(stderr,"Ram    : %d bytes used (%.1f%% full)\n",rambytes, (float)(rambytes*100)/ramsize);
    }
  }
  else
  {
    fprintf(stderr,"Device : not known by pfsreadihx\n");
    fprintf(stderr,"Flash  : %d words used\n",flashbytes / 2);
    if (b)
    {
      fprintf(stderr,"Ram    : %d bytes used\n",rambytes);
    }
  }
  return 0;
}
