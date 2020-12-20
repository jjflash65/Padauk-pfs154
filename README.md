Hier war urspruenglich geplant, Schaltplan, Layout und Software fuer einen
Arduino UNO basierenden Programmer fuer den PFS154 Mikrocontroller von
Padauk zu veroeffentlich. Daraus entwickelt hat sich bei den Fragen nach
einer Toolchain und Beispielprogrammen eher ein komplettes Setup fier den
Chip. Grundsaetzlich:<br>
<br>
- alle Beispiele sind Makefile-Projekte<br>
- die Toolchain und alle hier enthaltenen Binaries sind ausschliesslich<br>
Binaries fuer ein 64-Bit Linuxsystem, alle Programme sind Kommandozeilenprogramme<br>
<br>
Die Toolchain ist eine reduzierter SDCC Compiler, bei dem alle Programmteile
die nicht fuer das Programmieren eines Padauks Controllers benoetigt werden,
entfernt wurden. Die Pfade zur Toolchain sind in allen Makefiles zu relativen
Pfaden eingerichtet, das heisst, dass dieses Repository in ein beliebiges
Verzeichnis entpackt werden kann und von dort aus sofort ohne weitere
Installation lauffaehig ist.<br>
<br>
- in ein Beispielverzeichnis wechseln<br>
- Aufruf: make clean<br>
- Aufruf: make <br>
- Aufruf: make flash<br>
<br>
##Programmer
<br>
Nachdem mit dem "Easy-PDK-Programmer" ein wirklicher toller Programmer fuer die
PFS-Mikrocontroller von Padauk geschaffen wurde (herzlichen Dank an
js_12345678_55AA aus dem EEVblog), wollte ich einen Programmer haben, der
(nicht nur fuer mich) wesentlich einfacher aufzubauen ist (einen
Easy-PDK-Programmer Clone hatte ich bereits aufgebaut, aber eben wie der
originale mit einem STM32F072. Dieser ist derzeit nur relativ teuer erhaetlich
Stand 12.12.2020).

Google-Ergebnisse geben da derzeit nur 2 Projekte aus, die nicht auf dem oben
genannten Programmer basieren.

Beide habe ich ausprobiert (eines ist ein Projekt von socram8888 und eines vom
mikrocontroller.net Mitglied CPLDCPU.. auch hier ein Dankeschoen). Beide
dieser Projekte basieren auf einem ATMega / Arduino. Das Projekt von socram
habe ich gar nicht an's Laufen gebracht, das von CPLDCPU / Tim nur mit sehr
grossen Einschraenkungen,  aber er hatte es in seiner Beschreibung ja auch schon
gesagt gehabt: sein Projekt ist "hoch unfertig" und kann als Vorlage /
Ideengeber dienen, einen eigenen Programmer zu entwickeln.

Grundsaetzlich hapert es bei beiden AVR basierenden Programmern daran, dass
die Spannungen die zum Flashen benoetigt werden relativ kritisch in ihrer
Konstanz und in ihrer Genauigkeit sind. Auch mein erster Ansatz, mittels eines
PWM-Signals an einer geschalteten Spule die Programmierspannung zu erzeugen
hatte nicht den gewuenschten Erfolg gebracht.

Der Ansatz des Easy-PDK-Programmers ist hier der bessere / richtige Weg:
Eine konstante Spannung erzeugen, die ein paar Volt oberhalb der Spannung liegt,
die beim Flashen benoetigt wird. Zu allem Unglueck (und da hapert es dann bei
Tim's Entwurf) kann nach meinen Erkenntnissen - ein PFS-Controller nur korrekt
geflasht werden, wenn 2 Spannungen variabel eingestellt werden koennen: Die
Spannung an V_dd und die Programmierspannung V_pp an PA5.

Beim hier vorgestellten Programmer wird nun die Programmierspannung nicht mit
einer PWM erzeugt, sondern mit dem ultra billigen (und sehr alten) Schaltregler
MC34063. Wie beim "Easy-PDK-Programmer" auch werden 2 Operationsverstaerker in
einem DIP-8 Gehaeuse (wieder ultra billig, wieder sehr alt: LM358) vom AVR an
deren Eingaengen mit einer analogen Spannung gespeist, die durch PWM generiert
und durch ein passives Tiefpass mit einer Grenzfrequenz von ca. 72 Hz in eine
halbwegs ertraegliche Gleichspannung gefiltert wird. Der LM358 wird durch die
Ausgangsspannung des Schaltreglers versorgt.

Meine eigene Zielvorgabe beim Entwurf des Programmers war, eine Schaltung zu
erstellen, die von einem ambitionierten Elektroniker / Hobbybastler in
wirklicher "do it yourself" Manier mit absolut gaengigen Standardbauteilen
aufgebaut werden kann. LM358 und MC34063 sind aeuserst preiswert und hoch
verfuegbar, ansonsten werden nur Widerstaende, Kondensatoren und eine Spule
verwendet.

Aus diesem Grunde wurde auch ein Arduino UNO als Controller gewaehlt, weil dieser
eine sehr grosse Verbreitung hat und wohl auch im Besitz solcher Elektroniker
ist, die normalerweise die Nase bei dem Wort "Arduino" ruempfen (selbst mag
ich die Hardwareteile, das Framework der Arduino-Sketche mag ich jedoch weniger:
ich schreibe mir meine "Libraries" lieber selbst).

Die Firmware selbst ist daher auch nicht als Arduino-Sketch programmiert,
sondern ist ein "normales" C-File, welches mit AVR-GCC kompiliert werden kann.
Die Firmware wurde auch auf ATMega88 und Atmega168 getestet und funktioniert
auf beiden Controllern.

--------------------------------------------------------------------------------

Damit mit dem Arduino basierenden Programmer gearbeitet werden kann, habe ich
in die Zip-Datei ein komplettes Setup gepackt, welches ein Programm fuer einen
PFS154 erstellen und mit dem Programmer der Chip geflasht werden kann.

Vorraussetzung fuer dieses Setup ist:

- ein 64-Bit Linux System (bei Bedarf kann ich auch ein Setup fuer ein
  32-Bit System erstellen, auf meinen Rechnern hier laeuft der Programmer auch
  darauf sehr gut)

- ein im System installierter SDCC ab der Version 4.0.3

- AVR-GCC zum Compilieren der Firmware des Programmers (wenn die Firmware des
  Programmers neu uebersetzt werden soll)

- AVRDUDE zum Flashen der Firmware

Das Hostprogramm ist in FreePascal geschrieben (einfach aus dem Grund, weil ich
vor Ur-Zeiten in Pascal / Delphi Routinen zum Umgang mit Intel-Hex Files und
deren Uebertragung geschrieben habe und ich diese eben weiterverwende). Sollte
jemand am Hostprogramm Modifikationen vornehmen wollen, ist ein im System
installierter FreePascal Compiler ab Version 2.6.4 notwendig.

Die Installation der im ZIP-File enthaltenen Programm ist einfach: Das Archiv
in einen gewuenschten Ordner entpacken. Alle benoetigten Programme (inklusive
eines uebersetzten Hostprogramms fuer den Easy-PDK-Programmer) haben relative Pfade.

Jedes Beispielprogramm liegt in einem Unterverzeichnis und jedes ist ein
"Makefile-Projekt".

Ein Programm fuer den PFS154 wird uebersetzt durch einen Aufruf:

                               make

Der Chip wird mit

                               make flash

beschrieben.

Da Makefiles nicht jedermans Sache ist, habe ich die Handhabung der Makefiles
deutlich vereinfacht. Es besteht aus einem Teil, in dem Angaben ueber den
Controller, die Frequenz, zusaetzlich einzubindende Quelldateien etc. gemacht
werden muessen:

## Beispiel Makefile fuer das Blinkprogramm:
<br>
<br>
PROJECT       = blink<br>
MCU           = PFS154<br>
MEMORG        = pdk14 <br>
F_CPU         = 8000000<br>
FACTORYCAL    = 1<br>
<br>
==> hier alle zusaetzlichen Softwaremodule angegeben
<br>
SRCS          = ../src/delay.rel<br>
SRCS         += ../src/"eine-weitere-Datei.rel"<br>
<br>
INC_DIR       = -I./ -I../include<br>
<br>
=> benutzbare Programmer:<br>
=>  1 : easypdkprogrammer  ==> serielle Portangabe kann frei bleiben<br>
=>  2 : pfsprog            ==> benoetigt serielle Portangabe<br>
<br>
PROGRAMMER    = 2<br>
SERPORT       = /dev/ttyUSB0<br>
CH340RESET    = 1<br>
<br>
include ../makefile.mk<br>
<br>
<br>
Das eigentliche Compilieren, Linken und Flashen geschieht in der eingebunden
Datei makefile.mk und diese muss / sollte nicht veraendert werden.

Bei "PROJECT" wird die Datei angegeben, die die "main"-Funktion beinhaltet.
Die Angabe ist OHNE eine Dateierweiterung anzugeben, im Beispiel benoetigt es
also eine Datei "blink.c".

Zusaetzliche einzubindende "Softwaremodule" (.c / .h - Kombinationen) sind bei SRCS
anzugeben, sie werden standardmaessig im Verzeichnis ../src abgelegt. Sollen
mehrere Quelldateien uebersetzt und hinzugelinkt werden, muss jede weitere
Angabe fuer die Makefile-Variable SRCS mit += erfolgen (die die Variable SRCS um
genau diesen Eintrag dann erweitert).

Im obigen Fall bedarf es einer Datei namens "delay.c" die das Makefile in eine
Objektdatei mit der Endung .rel (hier dann delay.rel) compilieren wird.

Eine besondere Beachtung verlang die Angabe unter "FACTORYCAL". Der
"Easy-PDK-Programmer" ist in der Lage beim Flashen eine Kalibrierung des
Systemtakts des Controllers vorzunehmen, das kann der Arduino basierende
Programmer NICHT. Soll das Projekt mit dem "Easy-PDK-Programmer" geflasht
werden, wird eine Zuweisung "FACTORYCAL = 0" beim Flashen eine Kalibrierung
vornehmen. Bei "FACTORYCAL = 1" wird keine Kalibrierung vorgenommen und ist die
Wahl fuer den Arduino basierenden Programmer.

Wird im Makefile Programmer 2 gewaehlt (Arduino basierend) ist zwingend eine
Angabe erforderlich,  unter welchem Port der Arduino angeschlossen ist. Arduino
Clone mit einer CH340 USB2UART Bridge sind unter /dev/ttyUSBx ansprechbar,
originale Arduino UNO's sind unter /dev/ttyACMx ansprechbar.

Im Umgang mit dem CH340 Chip hat sich bei genau einem meiner Rechner ein Problem
gezeigt (ein AMD Ryzen5): Er kann den CH340-Chip nur genau ein einziges mal
ansprechen, danach ist der virtuelle serielle Port blockiert (ich habe bis
heute nicht herausgefunden warum). Um nicht jedesmal den USB-Stecker zu ziehen
und wieder einzustecken habe ich ein kleines Tool geschrieben, das den CH340
zuruecksetzt.

Sollte dieses Phaenomen auftreten, kann im Makefile die Zuweisung

                            CH340RESET = 1

gemacht werden, der CH340 wird dann vor dem Zugriff geresetet und das Flashen
sollte kein Problem sein.


--------------------------------------------------------------------------------
Inbetriebnahme des Programmers
--------------------------------------------------------------------------------

Im Verzeichnis ./tools/pfsprog/firmware befindet sich die Betriebssoftware des
Programmers. Soll eine Neuuebersetzung des Sourcecodes erfolgen, ist ein:

                                     make

aufzurufen. Dies ist jedoch fuer einen Arduino Uno NICHT notwendig, da in
diesem Ordner auch ein bereits kompiliertes Hex-File enthalten ist.

Wenn es sich beim Arduino um einen Clone handelt (Schnittstelle /dev/ttyUSB0),
ist nichts weiter zu tun als:

                                     make flash

aufzurufen, der Arduino wird geflasht. Bei einem originalen UNO muss mittels
Texteditor in diesem Makefile der Port unter dem der Arduino angesprochen wird
auf /dev/ttyACM0 geÃƒÂ¤ndert werden.

Um die Funktion des Shields zu ueberpruefen (kein muss, aber sehr empfohlen),
kann nun das Shield auf den UNO ohne eingesteckten PFS154 Controller
aufgesteckt und der Jumper JP1 gesteckt werden. Nach einem Reset
arbeitet die Firmware nun nicht als Programmer sondern sie kann die Spannungen,
die an den PFS-Controller waehrend des Flashens gefuehrt wird kalibrieren.
Notwendig war das bisher in keinem Fall, aber es eignet sich wie gesagt sehr
dazu, die Funktion der Spannungseinstellung zu ueberpruefen.

Arduino mit aufgestecktem Shield und gestecktem JP1 anschlissen und ein
Terminalprogramm mit der Einstellung 8 Datenbits, keine Paritaet, 1 Stopbit
und 115200 Baud starten. Anschliessend einen Reset am Arduino durchfuehren.

Der Nutzer wird nun aufgefordert, erst die Spannung an PA5 (V_pp) des
PFS-Controllers zu messen und diese Spannung in Millivolt einzugeben. Ist die
Eingabe getaetigt, sollte an diesem Pin sofort 8V anliegen. Das gleiche Vorgehen
nun fuer den Pin fuer V_dd durchfuehren, hier sollte an diesem Pin sofort 6V
anliegen.

Ist dieses der Fall, so ist der Programmer korrekt aufgebaut. Die Kalibrierwerte
werden im internen EEPROM gespeichert.

Da der Arduino bei jedem Zugriff ÃƒÂ¼ber die Schnittstelle einen Reset durchfuehrt,
durchlaeuft der Arduino eben jedem Zugriff jedesmal seinen Bootloader, er
verweilt darin ca. 2 Sekunden. Um dieses zu unterbinden kann der Jumper JP2
gesteckt werden, er blockiert hier einen Reset ueer die serielle Schnittstelle.

Hinweis: soll der Arduino neu geflasht werden muss dieser Jumper wieder entfernt
werden, weil ansonsten ein Reset blockiert ist und der Bootloader nicht gestartet
werden kann.


--------------------------------------------------------------------------------
Bedienung des Hostprograms pfsprog als Kommandozeilenprogramm
--------------------------------------------------------------------------------


pfsprog action port filename [nowait]

  action    : wr    = flasht einen PFS154 Controller mit der
                      Intel-Hexdatei "filename"
              txwr  = dasselbe wie "wr" mit dem Unterschied, dass beim Flashen
                      ein Fortschrittsbalken (Progressbar) angezeigt wird
              run   = startet das Programm im Controller
              stop  = stopt ein gestartetes Programm

  port      : Anschluss, unter dem der Programmer angesprochen werden kann

  filename  : Intel-Hex Datei, die geflasht werden soll

  nowait    : Optionaler Parameter. Normalerweise verbindet sich pfsprog mit dem
              Programmer und wartet dann 2,2 Sekunden (das ist die Zeit, die der
              Bootloader fuer sich in Anspruch nimmt) und beginnt erst dann mit
              der Datenuebertragung. nowait ueberspringt die Wartezeit und beginnt
              sofort mit der Uebertragung. Diese Option kann gewaehlt werden,
              wenn ein AVR-Controller ohne Bootloader geflasht wurde oder der
              Reset (mittels JP2) blockiert ist

Beispiel   : pfsprog txwr /dev/ttyUSB0 blink.ihx nowait


--------------------------------------------------------------------------------
Was der Arduino basierende Programmer (noch) nicht kann:
--------------------------------------------------------------------------------

- er kann keinen anderen Controller als einen PFS154 flashen. Lt. EEVblog sollte
  bspw. Ein PFS173 dasselbe Flashprotokoll haben, mit dem Unterschied, dass die
  Datenbitbreite 15 statt 14 Bits betraegt und eine hoehere Programmierspannung
  von Noeten ist. Versuche hierzu waren bisher leider erfolglos. Die hoehere
  Programmierspannung konnte problemlos eingestellt werden. Vielleicht hat hierzu
  jemand einen Tip fuer mich welche Gegebenheiten bei einem PFS173 zu beachten sind

- Kalibrierung des Systemtakts kann nicht vorgenommen werden, so dass man bedenken
  muss, dass es beim Takt zu einer Abweichung lt. Datenblatt von +- 2% kommen kann.



Viel Spass mit dem Arduino basierenden PFS154 Flashershield

R. Seelig im Corona-Dezember 2020


