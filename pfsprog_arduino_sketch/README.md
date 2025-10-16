<br>
pfs154_prog2.ino ist ein Pseudo-Arduino Sketch, welches innerhalb des Arduino-Frameworks
uebersetzt werden kann.<br>
<br>
Dieser Sketch ist ein an sich ein reines C-Programm (und hat von daher auch keine typische
Funktion "setup" oder "loop" eines Arduino-Sketches).<br>
<br>
Das Arduino-Framework kann jedoch (weil der zugrunde liegende Compiler ein AVR-GCC ist) auch
Dateien uebersetzen, die reinen C-Code darstellen.<br>
<br>
Hiervon wurde hier Gebrauch gemacht und so kann die Arduino-IDE diesen Sketch zum einen
kompilieren und (an sich wichtiger) das Kompilat auch zu einem Arduino-UNO transferieren.<br>
<br>
Dieses sollte fuer alle die hilfreich sein, die zwar eine Arduino-Installation auf dem
Computer haben, aber keine systemweite AVR-GCC Installation.
