{ ----------------------------------------------------------
                           pfsprog.pas

    Hostprogramm zum arduinobasierenden PFS Programmer.
    Mikrcocontrollerunterstuetzung fuer:

       - PFS154

    11.12.2020    R. Seelig
  ---------------------------------------------------------- }
program pfsprog;

uses
  sysutils,synaser, crt;

const
  dtime_hi        = 83;
  dtime_send      = 200;

var

  sport       : string;
  sparity     : char;
  sdbit,ssbit : byte;
  sbaud       : longint;
  ser         : tblockserial;
  portname    : string;

  tdat        : text;
  filename    : string;

  nowait      : boolean;
  withpbar    : boolean;

type
  mcumem = array[0..4096] of byte;

var
  flashmem    : mcumem;                        // in diesem Array wird ein Speicherabbild
                                               // wie es spaeter im Target geflasht ist

  maxadr      : word;

  blksize     : word = 500;
  blkanz      : byte;

{ ----------------------------------------------------------
                           word2hex

    konvertiert ein 16-Bit Wert in einen hexadezimalen
    String
  ---------------------------------------------------------- }
function word2hex(val16 : word) : string;
var
  w  : word;
  ub : byte;
  hs : string[6];
begin
  w:= val16;
  ub:= (w shr 12);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= chr(ub);

  w:= val16;
  ub:= (w shr 8);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  w:= val16;
  ub:= (w shr 4);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  w:= val16;
  ub:= w and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  word2hex:= hs;

end;

{ ----------------------------------------------------------
                           byte2hex

    konvertiert ein 8-Bit Wert in einen hexadezimalen
    String
  ---------------------------------------------------------- }
function byte2hex(val8 : word) : string;
var
  b  : word;
  ub : byte;
  hs : string[4];
begin

  b:= val8;
  ub:= (b shr 4);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= chr(ub);

  b:= val8;
  ub:= b and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  hs:= hs+chr(ub);

  byte2hex:= hs;

end;

{ ----------------------------------------------------------
                         txpbarscala

    schreibt die Skala fuer eine Textmode - Progressbar
    betitelt mit writing / write
  ---------------------------------------------------------- }
procedure txpbarscala(pbaranzahl : byte);
var
  b, b2, b3       : byte;

begin
  write(' Writing|');
  b2:= pbaranzahl;
  for b:= 0 to b2 do
  begin
    b3:= b mod 10;
    if (b3= 0) then
    begin
      write('|');
    end else
    begin
      write('-');
    end;
  end;
end;

{ ----------------------------------------------------------
                         txpbarscala2

    schreibt die Skala fuer eine Textmode - Progressbar
    betitelt mit reading / read
  ---------------------------------------------------------- }
procedure txpbarscala2(pbaranzahl : byte);
var
  b, b2, b3       : byte;

begin
  write('Reading |');
  b2:= pbaranzahl;
  for b:= 0 to b2 do
  begin
    b3:= b mod 10;
    if (b3= 0) then
    begin
      write('|');
    end else
    begin
      write('-');
    end;
  end;
end;

{ ----------------------------------------------------------
                           txpbar

    schreibt die Progressbar (Fortschrittsbalken) im
    Textmode.

    Uebergabe:
       startzeit  : Zeitstempel
       maxwert    : Anzahl Werte, der 100% entsprechen
       value      : Anzahl abgearbeiteter Werte
       ch         : Zeichen durch das die Progressbar
                    dargestellt wird
       pbaranzahl : Zeichenbreite der Progressbar
  ---------------------------------------------------------- }
procedure txpbar(startzeit : comp; maxwert : word; value : word; ch : char; pbaranzahl : byte);
var
  r          : real;
  zanz,b     : byte;
  ts         : ttimestamp;
  lz2        : comp;
  prozent    : word;
  newstep    : word;
  xpos       : word;

begin
  newstep:= maxwert div pbaranzahl;
  if newstep= 0 then newstep:= 1;
  if (((value mod newstep)= 0) and (value> 0)) then
  begin
    write(chr(13));
    write(chr(27),'[',9,'C');
    r:= pbaranzahl;
    r:= (r / maxwert);
    zanz:= trunc(r * value);
    for b:= 0 to zanz do write(ch);

    xpos:= pbaranzahl + 1 + 9;
    write(chr(13));
    write(chr(27),'[',xpos,'C');

    prozent:= (value*100) div maxwert;
    ts:= DateTimeToTimeStamp(Now);
    lz2:= TimeStampToMSecs(ts);
    write (' ',prozent,'% ',((lz2 - startzeit) / 1000):3:2,'s  ');
  end;
end;

{ ----------------------------------------------------------
                           txpbar100

    schreibt die Progressbar (Fortschrittsbalken) im
    Textmode fuer 100% (Vorgang abgeschlossen)

    Uebergabe:
       startzeit  : Zeitstempel
       pbaranzahl : Zeichenbreite der Progressbar
  ---------------------------------------------------------- }
procedure txpbar100(startzeit : comp; pbaranzahl : byte);
var
  ts              : ttimestamp;
  lz2             : comp;
  prozent         : word;
  xpos            : word;
begin
  xpos:= pbaranzahl + 1 + 9;
  write(chr(13));
  write(chr(27),'[',xpos,'C');

  prozent:= 100;
  ts:= DateTimeToTimeStamp(Now);
  lz2:= TimeStampToMSecs(ts);
  write (' ',prozent,'% ',((lz2 - startzeit) / 1000):3:2,'s  ');
end;

{ -------------------------------------------------------------
                         ConvertHex2Mem

   Interpretiert eine Zeile einer Intel-Hexdatei.

   Zeile wird gelesen und die Speicheradresse des Targets
   festgestellt. Datenbytes werden in ein Speicherarray
   flashmem eingetragen, das dem Flashspeicher des Targets
   entspricht.

   Speicherarchitektor entspricht Lo-Endian (Lo-Wert des
   Word-Datums zuerst auf den Speicher)

   highestadr enthaelt die hoechste Speicherstelle, an der
   ein Wert geschrieben wurde.

   Funktionsrueckgabe:
      true wenn gueltiger Hexstring, false wenn ungueltig

  ------------------------------------------------------------- }
function ConvertHex2Mem(var flashmem : mcumem; src : string; var highestadr : word) : byte;
var
  fadress, bytelen  : word;
  anz, err, p       : word;
  hs                : string;
  ub                : byte;
  wb1, wb2          : byte;
  wanz              : word;

begin
  wanz:= 0;

  if (copy(src,1,1)= ':') then                       // Einleitung einer Zeile ":"
  begin
    if (copy(src,8,2)= '00') then                    // Position fuer "normale" Datenbytes
    begin
      hs:= '$'+copy(src,2,2);                        // Position fuer die Anzahl der Datenbytes
      val(hs,bytelen,err);
      bytelen:= bytelen;
      hs:= '$'+copy(src,4,2);                        // Position Hi-Byte Adresse
      val(hs,fadress,err);
      fadress:= fadress * 256;
      hs:= '$'+copy(src,6,2);                        // Position Lo-Byte Adresse
      val(hs,ub,err);
      fadress:= fadress+ub;
      highestadr:= fadress;

      p:= 10;                                        // p ist Index auf das erste Datenbyte (an Pos. 10)
      hs:= word2hex(fadress);
      highestadr:= highestadr + bytelen;             // hoechste Speicheradresse der Zeile

      for anz:= 1 to (bytelen div 2) do
      begin
        val('$'+copy(src, p+2,2), wb1, err);
        val('$'+copy(src, p,2), wb2, err);

        flashmem[fadress+((anz-1)*2)]:= wb1;
        flashmem[fadress+((anz-1)*2)+1]:= wb2;

        hs:= hs+' '+copy(src, p+2 ,2)+copy(src, p, 2);  // eine Mnemonic im PFS-Flash ist 14 Bit breit
                                                        // und benoetigt im Hexfile 2 Byte (1 Word) mit
                                                        // Reihenfolge Lo-Byte, Hi-Byte
        p:= p+4;
        inc(wanz);
      end;

    end else
    begin
      if (copy(src,8,2)= '01') then                  // Endekennung Hex-File
      begin
        ConvertHex2Mem:= 3;
        exit;
      end else
      begin
        ConvertHex2Mem:= 4;
        exit;
      end;
    end;
  end else
  begin
    ConvertHex2Mem:= 1;
    exit;
  end;
  ConvertHex2Mem:= 0;
end;


{ -------------------------------------------------------------
                         ConvertHexFile

   Konvertiert eine Intel-Hexdatei in ein Speicherabbild
   des Zielkcontrollers
  ------------------------------------------------------------- }
procedure ConvertHexFile(datnam : string);
var
  ts        : ttimestamp;
  lz, lz2   : comp;
  src,dest  : string;
  hs        : string;
  recstring : string;
  b         : byte;
  flags     : byte;
  err       : integer;
  bytezlen  : byte;
  fanz      : word;
  wadd      : word;
  worcnt    : word;
  i         : word;
  w1, w2    : word;

begin
  for i:= 0 to 4095 do flashmem[i]:= $ff;             // virtuellen Speicher des MCU loeschen

  assign(tdat,datnam);
  {$i-} reset(tdat); {$i+}
  if (ioresult <> 0) then
  begin
    writeln('Error: file ',datnam,' not found');
    halt;
  end;
  // Anzahl zu flashender Words (Locations) feststellen
  fanz:= 0;
  while(not(eof(tdat))) do
  begin
    readln(tdat,src);
//    writeln(src);
    if (src[1]= ':') then
    begin
      hs:= '$'+copy(src,2,2);                        // Position fuer die Anzahl der Datenbytes
      val(hs,bytezlen,err);
      fanz:= fanz+bytezlen;
    end;
  end;

  fanz:= fanz div 2;
  reset(tdat);

  maxadr:= 0;
  while(not(eof(tdat))) do
  begin
    readln(tdat,src);

    flags:= converthex2mem(flashmem, src, wadd);
    if (wadd> maxadr) then maxadr:= wadd;
//    writeln('     wadd:     ', wadd, '   maxadr   ', maxadr);

  end;
  close(tdat);
end;

{ ----------------------------------------------------------
                           clr_inpuffer

    loescht den Eingangspuffer der seriellen Schnittstelle
  ---------------------------------------------------------- }
procedure clr_inpuffer;
var
  ch     : char;
begin

  while (ser.waitingdataex> 0) do
  begin
    ch:= chr(ser.recvbyte(1));
  end;

end;

{ -------------------------------------------------------------
                         uart_getstring

    liest einen String von der seriellen Schnittstelle ein
  ------------------------------------------------------------- }
function uart_getstring(var hs : string; termchar : byte) : byte;
var
  errcnt : word;
  ch     : char;
begin
  hs:= '';
  errcnt:= 0;

  repeat
    ch:= chr(ser.recvbyte(1));
    inc(errcnt);
    if (ch<> chr(0)) and (ch<> chr(10)) and (ch<>chr(13)) then
    begin
      hs:= hs+ch;
      errcnt:= 0;
    end;
  until ((ch= chr(termchar)) or (errcnt= 500));
  if (errcnt= 500) then uart_getstring:= 1 else uart_getstring:= 0;
end;

{ -------------------------------------------------------------
                         uart_sendword16

    sendet einen 16-Bit Wert auf der seriellen Schnittstelle
  ------------------------------------------------------------- }
procedure uart_sendword16(val16 : word);
var
  w  : word;
  ch : char;
  ub : byte;
begin

  w:= val16;
  ub:= (w shr 12);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  ser.sendbyte(ub);

  w:= val16;
  ub:= (w shr 8);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  ser.sendbyte(ub);

  w:= val16;
  ub:= (w shr 4);
  ub:= ub and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  ser.sendbyte(ub);

  w:= val16;
  ub:= w and $0f;
  if (ub< 10) then ub:= ub+ 48 else ub:= ub+55;                  // umwandeln nach Hex-Ziffer
  ser.sendbyte(ub);
  sleep(dtime_send);
end;


{ ---------------------------------------------------------------
                           Main - Program
  --------------------------------------------------------------- }
var
  recstring    : string;
  b            : byte;
  ch           : char;
  runmcu       : boolean;
  err, errcnt  : word;
  dummyw       : word;
  w1, w2,
  cnt, i, mcx  : word;
  ts           : ttimestamp;
  lz, lz2      : comp;

begin
  runmcu:= false;
  if (paramcount= 2) then
    if (paramstr(1) = 'run') then
      runmcu:= true;
  if ((paramcount< 3) and not(runmcu)) then
  begin
    writeln('Syntax:');
    writeln('pfsprog action port filename [nowait]'); writeln();
    writeln('  action    : wr    = upload (write) file to mcu');
    writeln('              txwr  = upload (write) file to mcu (with bargraph)');
//    writeln('              rd  = read (download) MCU to file');
    writeln('              run   = run the microcontroller');
    writeln('              stop  = stop a running program');
    writeln('  port      : port were this adapter is connected to');
    writeln('  filename  : file to be uploaded');
    writeln('  nowait    : optional parameter: don'+chr(39)+'delay programstart');
    writeln('              You can use this option if you have');
    writeln('              a programmer WITHOUT a bootloader');
    writeln();
    writeln('  Example   : pfsprog txwr /dev/ttyUSB0 helloworld.ihx nowait');
    writeln();
    writeln();
    halt;
  end;


  filename:= paramstr(3);

  portname:= paramstr(2);
  if (paramcount= 4) then
    if (paramstr(4)= 'nowait') then
      nowait:= true
    else
      nowait:= false;

  if (paramstr(1) = 'run') and (paramstr(3) = 'nowait') then nowait:= true;
  if (paramstr(1) = 'stop') and (paramstr(3) = 'nowait') then nowait:= true;

  sbaud:= 115200;
  sdbit:= 8;
  ssbit:= 1;
  sparity:= 'N';
  sport:= portname;

  ser:= tblockserial.create;
  ser.raiseExcept:= false;
  ser.linuxlock:= false;
  ser.connect(sport);
  ser.config(sbaud,sdbit,sparity,ssbit,false,false);

  writeln(' waiting for programmer...');
  if (nowait= false) then 
  begin
    sleep(2200);
  end else
  begin
    sleep(200);
  end;  
  writeln;

  clr_inpuffer;
  case paramstr(1) of
    'run' : begin
              ser.sendbyte(ord('R'));
              writeln('PFS - Controller is running...');
              writeln;
            end;
    'stop' : begin
              ser.sendbyte(ord('r'));
              writeln('PFS - Controller stopped...');
              writeln;
            end;

    'wr','txwr' :
            begin
              if paramstr(1)= 'txwr' then
                withpbar:= true else withpbar:= false;

              converthexfile(filename);

              clr_inpuffer;

              ser.sendbyte(10);
              // eventuelle "Reste" im seriellen Puffer
              recstring:= ser.recvTerminated(200, chr(13));
              ser.sendbyte(ord('P'));
              err:= uart_getstring(recstring, 13);
              if err> 0 then
              begin
                writeln('communication error...');
                ser.free;
                halt;
              end else
              begin
                if recstring= '0x0AA1' then
                begin
                  writeln(' ID: 0x0aa1 found, PFS154 present...');
                end else
                begin
                  writeln('Unknown ID: ', recstring);
                  writeln('Programm terminated...');
                  ser.free;
                  halt;
                end;
              end;
              writeln(' Words to flash: ', maxadr div 2);
              writeln;
              uart_sendword16(maxadr);
              err:= uart_getstring(recstring, 13);
              if err> 0 then
              begin
                writeln('communication error...');
                ser.free;
                halt;
              end;
              recstring:= '$'+recstring;
              val(recstring, dummyw, err);
              if (dummyw<> maxadr) then
              begin
                writeln('wrong answer from programmer, expected: ',maxadr, ' but get: ',dummyw);
                writeln('Programm terminated...');
                halt;
              end;

             ts:= DateTimeToTimeStamp(Now);
             lz:= TimeStampToMSecs(ts);

              // Anzahl Datenbytebloecke ermitteln
              blkanz:= maxadr div blksize;
              if ((maxadr mod blksize)<> 0) then blkanz:= blkanz+1;

              if withpbar= true then txpbarscala(50);

              mcx:= 0;                        // Memory counter
              for i:= 1 to blkanz do
              begin
                for cnt:= 1 to blksize do
                begin
                  b:= flashmem[mcx];
                  inc(mcx);
                  ser.sendbyte(b);
                end;
                sleep(dtime_send);

                ch:= chr(ser.recvbyte(1));
                if (ch<> 'x') then
                begin
                  writeln('communication error...');
                  ser.free;
                  halt;
                end;

                // Progressbar
                if withpbar= true then txpbar(lz,blkanz, i, '#', 50);

              end;

              if withpbar= true then
              begin
                txpbar100(lz,50);
                writeln;
                writeln;
                writeln (' All is done...');
              end else
              begin
                ts:= DateTimeToTimeStamp(Now);
                lz2:= TimeStampToMSecs(ts);
                writeln (' All is done, used time: ',((lz2 - lz) / 1000):3:2,'s  ');
              end;
              writeln('');
            end;
{*
    'rd'  : begin
            end;
*}
  end;

  ser.free;
end.
