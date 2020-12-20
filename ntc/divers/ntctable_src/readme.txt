ntc_maketable
---------------------------------------------------------------------------------

ntc_maketable ist ein Konsolenprogramm, das fuer einen gegebenen NTC Widerstand
mit gegebenem B-Wert eine Stuetztabelle und eine C-Funktion zum Lesen dieser
Stuetztabelle erzeugt.

Die "Auswerteschaltung" besteht aus einem Spannungsteiler mit einem Pullup-
widerstand und dem NTC. Der Knotenpunkt wird dem ADC-Pin des Controllers zu-
gefuehrt.

 Syntax:
    -r value     | R25 Widerstandswert NTC
    -R value     | Popupwiderstand gegen Referenzspannung
    -b value     | Betawert NTC (B 25/85)
    -A value     | ADC-Aufloesung in Bit
    -h           | diese Anzeige (Help)
 optional:
    -a           | Sourcedatei fuer AVR-Conroller
    -l           | Lookup-Table mit 32 Punkten (statt 16)


Beispiel:

Bestehender Spannungsteiler aus 10k Ohm Pullup-Widerstand und 10k Ohm NTC mit
einem B-Wert von 3950. Es soll eine Lookuptabelle fuer ARM Controller (mit
12-Bit Aufloesung) erstellt werden:

ntc_maketable -r 10000 -R 10000 -b 3950 -A 12 -l


12.11.2019   R. Seelig

