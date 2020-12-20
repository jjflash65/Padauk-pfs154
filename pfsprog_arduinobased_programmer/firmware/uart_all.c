/* ----------------------------------------------------------
                           uart_all.c

     Softwaremodul fuer serielle Schnittstelle am AVR-MCU

     Unterstuetzte Controller:

        Controller mit Hardware-UART

        - ATmega8
        - ATmega8515
        - ATmega48 - ATmega328
        - ATtiny2313, ATtiny4313

        Controller mit USI-Serial

        - ATtiny24 - ATtiny84
        - ATtiny25 - ATtiny85


        Anschlussbelegungen:

                MCU            PORT    Funk.1   Funk.2
        ---------------------------------------------------
        ATtiny25 - ATtiny85    PB1      TxD       Do   (6)
                               PB0      RxD       Di   (5)
        ---------------------------------------------------
        ATtiny24 - ATtiny84    PA5      TxD       Do   (8)
                               PA6      RxD       Di   (7)
        ---------------------------------------------------
        ATtiny2313 / 4313      PD1      TxD            (3)
                               PD0      RxD            (2)
        ---------------------------------------------------
        28 pol. ATmega (8, 48, 88, 168, 328)
        ---------------------------------------------------
                               PD1      TxD            (3)
                               PD0      RxD            (2)
        ---------------------------------------------------


     16.2.2019     R. Seelig

   ---------------------------------------------------------- */


#include "uart_all.h"

#define echo_enable    0


/* ##########################################################
         Liste der unterstuetzten AVR-Controller
   ########################################################## */

#if defined __AVR_ATtiny4313  || __AVR_ATtiny2313__ ||                                              \
            __AVR_ATtiny25__  || __AVR_ATtiny45__   || __AVR_ATtiny85__  ||                         \
            __AVR_ATtiny24__  || __AVR_ATtiny44__   || __AVR_ATtiny84__  ||                         \
            __AVR_ATmega48__  || __AVR_ATmega88__   || __AVR_ATmega168__ || __AVR_ATmega328P__ ||   \
            __AVR_ATmega328__ ||                                                                    \
            __AVR_ATmega8__   || __AVR_ATmega8515__

  /* --------------------------------------------------------
       Registernamen von ATmega8, 8515, ATtiny2313 und 4313
       anpassen
     -------------------------------------------------------- */
  #if defined __AVR_ATmega8__    || __AVR_ATmega8515__  ||                                            \
              __AVR_ATtiny2313__ || __AVR_ATtiny4313__

    #define UBRR0       UBRR
    #define UBRR0L      UBRRL
    #define UBRR0H      UBRRH
    #define UCSR0A      UCSRA
    #define UCSR0B      UCSRB
    #define UCSR0C      UCSRC
    #define U2X0        U2X
    #define RXEN0       RXEN
    #define TXEN0       TXEN
    #define UCSZ00      UCSZ0
    #define UCSZ01      UCSZ1
    #define UDRE0       UDRE
    #define UDR0        UDR
    #define RXC0        RXC

  #endif

#else

  #error Given MCU is not supported

#endif

/* ##########################################################
          MCU mit USI-Serial (ATtinyx4, ATtinyx5)
   ########################################################## */
#if defined  __AVR_ATtiny25__ || __AVR_ATtiny45__  || __AVR_ATtiny85__ ||            \
             __AVR_ATtiny24__ || __AVR_ATtiny44__  || __AVR_ATtiny84__

  /* -----------------------------------------------------------------------
         Anschlusspins des USI Interface fuer ATtinyx4
     ----------------------------------------------------------------------- */
  #if defined  __AVR_ATtiny24__ || __AVR_ATtiny44__  || __AVR_ATtiny84__


    #define USI_DIR               DDRA
    #define USI_PORT              PORTA
    #define USI_PIN               PINA
    #define USI_DO                PA5               // Anschlusspin fuer TxD
    #define USI_DI                PA6               // Anschlusspin fuer RxD

    #define USI_PCINT             PCINT6            // Pin-Change Interrupt fuer PA6 Anschluss

  #endif

  /* -----------------------------------------------------------------------
         Anschlusspins des USI Interface fuer ATtinyx5
     ----------------------------------------------------------------------- */
  #if defined  __AVR_ATtiny25__ || __AVR_ATtiny45__  || __AVR_ATtiny85

    #define USI_DIR               DDRB
    #define USI_PORT              PORTB
    #define USI_PIN               PINB
    #define USI_DO                PB1               // Anschlusspin fuer TxD
    #define USI_DI                PB0               // Anschlusspin fuer RxD

    #define USI_PCINT             PCINT0            // Pin-Change Interrupt fuer PB0 Anschluss


    // Anpassung der Registernamen und Bitpositionen an ATtinyx4
    #define PSR10                 PSR0
    #define PCIE0                 PCIE
    #define TIFR0                 TIFR
    #define TIMSK0                TIMSK
    #define PCIF0                 PCIF
    #define PCMSK0                PCMSK

  #endif


  #define STOPBITS              1

  /* -----------------------------------------------------------------------
     Prescaler Berechnung in Abhaengigkeit der Taktrate und clockselect setzen
     F_CPU muss ueber das Makefile definiert sein, ansonsten hier einen Wert
     fuer F_CPU angeben
     ----------------------------------------------------------------------- */

  #define CYCLES_PER_BIT        ( (F_CPU) / (BAUDRATE) )
  #if (CYCLES_PER_BIT > 255)
    #define DIVISOR             8
    #define CLOCKSELECT         2
  #else
    #define DIVISOR             1
    #define CLOCKSELECT         1
  #endif

  #define FULL_BIT_TICKS        ( (CYCLES_PER_BIT) / (DIVISOR) )
  #define HALF_BIT_TICKS        ( FULL_BIT_TICKS / 2)

  // Anzahl der Takte nach Eingang eines Pin-Changes bis zum Start des USI-Timers
  #define START_DELAY           (99)

  // Anzahl der Takte die vergehen, nachdem Interrupt aktiviert ist
  #define COMPA_DELAY           42
  #define TIMER_MIN ( COMPA_DELAY / DIVISOR )

  #define TIMER_START_DELAY     ( START_DELAY  / DIVISOR )

  #if (HALF_BIT_TICKS - TIMER_START_DELAY)>0

    #define TIMER_TICKS         ( HALF_BIT_TICKS - TIMER_START_DELAY )

    #if (TIMER_TICKS < TIMER_MIN)
      #warning TIMER_TICKS zu langsam, niedrigere Baudrate waehlen
    #endif

  #else

    #error "TIMER_TICKS invalid values: F_CPU, BAUDRATE and START_DELAY must have different values"
    #define TIMER_TICKS         1

    #endif


  // Enumerator Statusvariable send
  enum USISERIAL_SEND_STATE { AVAILABLE, FIRST, SECOND };

  // Enumerator UART-Funktion
  enum UART_FUNC { TXD, RXD };

  /* ------------------------------------------------------------------------
                                      Variable
     ------------------------------------------------------------------------ */

  static volatile enum UART_FUNC uart_func = RXD;
  static volatile enum USISERIAL_SEND_STATE usiserial_send_state = AVAILABLE;
  static volatile uint8_t usiserial_tx_data;

  volatile bool usiserial_readfinished= true;
  volatile bool serialdataready = false;                  // zeigt an, ob ein Datum eingegangen ist
  volatile uint8_t serialinput;                           // letztes eingegangenes Datum


  /* ------------------------------------------------------------------------
           Funktionen die einzelne Flags setzen oder Funktionen wrappen
     ------------------------------------------------------------------------ */

  static inline enum USISERIAL_SEND_STATE usiserial_send_get_state(void)
  {
    return usiserial_send_state;
  }

  static inline void usiserial_send_set_state(enum USISERIAL_SEND_STATE state)
  {
    usiserial_send_state=state;
  }

  bool usiserial_send_available()
  {
    return usiserial_send_get_state() == AVAILABLE;
  }

  static inline uint8_t usiserial_get_tx_data(void)
  {
    return usiserial_tx_data;
  }

  static inline void usiserial_set_tx_data(uint8_t tx_data)
  {
    usiserial_tx_data = tx_data;
  }


  /* ------------------------------------------------------------------
                               reverse_byte

                   Bit MSB nach LSB und umgekehrt

         Bsp: aus 0x03 wird 0xC0
     ------------------------------------------------------------------ */

  static uint8_t reverse_byte (uint8_t x)
  {
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;
  }


  /* ------------------------------------------------------------------
                            usiserial_send_byte
     ------------------------------------------------------------------ */
  void usiserial_send_byte(uint8_t data)
  {
    while (usiserial_send_get_state() != AVAILABLE);      // wiederholen bis vorrausgegangener Frame gesendet ist

    usiserial_send_set_state(FIRST);
    usiserial_set_tx_data(reverse_byte(data));

    // Konifguration Timer0
    TCCR0A = 2 << WGM00;                                  // CTC mode
    TCCR0B = CLOCKSELECT;                                 // Rrescaler auf clk oder clk / 8 setzen
    GTCCR |= 1 << PSR10;                                  // Reset prescaler
    OCR0A = FULL_BIT_TICKS;                               // Trigger
    TCNT0 = 0;                                            // Count up from 0

    // Konfiguration USI, Startbit senden und 7 Datenbits

    USIDR = 0x00 |                                        // Start bit (low)
        usiserial_get_tx_data() >> 1;                     // gefolgt von 7 Datenbits

    USICR  = (1 << USIOIE)|                               // Interrupt zulassen
        (0 << USIWM1) | (1 << USIWM0) |                   // 3 - wire-modus
        (0 << USICS1) | (1 << USICS0) | (0<<USICLK);      // Timer0 Compare match Modus als USI Taktquelle

    USI_DIR  |= (1 << USI_DO);                            // Senden - Pin als Ausgang setzen
    USISR = 1 << USIOIF |                                 // Interruptflag loeschen
        (16 - 8);                                         // und Zaehler fuer 8 Bits
  }

  /* ------------------------------------------------------------------
                               serialreceived

       setzt Flag, dass ein Datum eingegangen ist
       und speichert dieses Datum
     ------------------------------------------------------------------ */
  void serialreceived(uint8_t data)
  {
    serialdataready = true;
    serialinput = data;
  }


  /* ------------------------------------------------------------------
                            on_serial_pinchange

       Timer nach erstem Pinchange auf Baudrate konfigurieren, starten und weitere
       Pinchanges bis zum Ende des Frames verhindern.
     ------------------------------------------------------------------ */
  void on_serial_pinchange()
  {
    GIMSK &= ~(1 << PCIE0);                               // Disable pin change interrupt
    TCCR0A = 2 << WGM00;                                  // CTC mode
    TCCR0B = CLOCKSELECT;                                 // Taktvorteiler
    GTCCR |= 1 << PSR10;                                  // Prescaler reset
    OCR0A = TIMER_TICKS;                                  // Die Zeit, an der der RxD Pin gelesen wird auf die (zeitliche) Mitte des Bits setzen
    TCNT0 = 0;
    TIFR0 = 1 << OCF0A;                                   // Output Compareflag loeschen
    TIMSK0 |= 1 << OCIE0A;                                // und Timerinterrupts zulassen
  }


  /* ------------------------------------------------------------------
                         Pinchange Interruptvektor

       reakiert auf alle Pegelereignisse an den Anschluessen des Ports
       (PortA bei ATtinyX4, PortB bei ATtinyX5) und detektiert somit
       das Startbit. Mit dem Aufruf von on_serial_pinchange wird dieser
       Interrupt fuer den Rest des  seriellen Datenframes gesperrt
     ------------------------------------------------------------------ */
  ISR (PCINT0_vect)
  {
    uint8_t pinbVal;

    usiserial_readfinished= false;

    pinbVal = USI_PIN;
    if (!(pinbVal & 1 << USI_DI))                         // wird nur eingelesen, wenn DI == 0
    {
      on_serial_pinchange();
    }
  }


  /* ------------------------------------------------------------------
                  Timer 0 Compare Match Interruptvektor

       startet in der zeitlichen Mitte von Bit 0
     ------------------------------------------------------------------ */
  ISR (TIM0_COMPA_vect)
  {
    TIMSK0 &= ~(1 << OCIE0A);                             // COMPA sperren
    TCNT0 = 0;                                            // Zaehler auf 0
    OCR0A = FULL_BIT_TICKS;                               // einzelne zeitliche Bitbreite

    // USI-Overflow zulassen, Timer 0 ist Taktquelle fuer USI
    USICR = 1 << USIOIE | 0 << USIWM0 | 1 << USICS0;

    // Resetr Start Kondition Interrupt Flag, USI OVF flag, Counter auf 8 setzen
    USISR = 1 << USIOIF | 8;
  }


  /* ------------------------------------------------------------------
                       USI Overflow Interruptvektor

       USI overflow interrupt fuer senden UND empfangen
     ------------------------------------------------------------------ */
  ISR (USI_OVF_vect)
  {
    uint8_t temp = USIBR;

    cli();

    if (uart_func == TXD)
    {
      if (usiserial_send_get_state() == FIRST)
      {
        usiserial_send_set_state(SECOND);

        USIDR = usiserial_get_tx_data() << 7             // letztes Bit (LSB) des Datums senden
            | 0x7F;                                      // plus Stopbit

        USISR = 1 << USIOIF |                            // Interrupt quittieren
            (16 - (1+ (STOPBITS)));                      // USI counter setzen um letztes Daten Bit + Stopbit zu senden
      }
      else
      {
        USI_PORT |= 1 << USI_DO;                         // USI_DO = 1
        USI_DIR  |= (1 << USI_DO);                       // USI_DO als Ausgang
        USICR = 0;                                       // USI disable
        USISR |= 1 << USIOIF;                            // Interrupt quittieren

        usiserial_send_set_state(AVAILABLE);
      }
    }
    else
    {
      USICR  =  0;                                       // Disable USI

      serialreceived(reverse_byte(temp));

      GIFR = 1 << PCIF0;                                 // Pinchange Interrupt Flag quittieren
      GIMSK |= 1 << PCIE0;                               // und Pinchange Interrupts wieder zulassen

      usiserial_readfinished= true;
      USISR |= 1 << USIOIF;                              // Interrupt quittieren
    }

    sei();
  }

  /* ------------------------------------------------------------------
                                   uart_init

       initialisiert das USI Interface als serielle Schnittstelle

       TxD - USI_DO
       RxD - USI_DI

       (Hinweis: nicht von den Anschluessen des SPI zum Flashen des
        AVR's verwirren lassen !!!)

       Aktivieren des Pinchange-Interrupts
     ------------------------------------------------------------------ */
  void uart_init()
  {
    USI_PORT |= ( 1 << USI_DI );      // Pull-Up Widerstand fuer Rx-Data
    USI_DIR &= ~( 1 << USI_DI );
    USI_DIR |= ( 1 << USI_DO );
    USI_PORT |= ( 1 << USI_DO );

    USICR = 0;                        // Disable USI.
    GIFR = 1 << PCIF0;                // Clear pin change interrupt flag (PCINT0--PCINT7)
    GIMSK |= 1 << PCIE0;              // Enable pin change interrupts (PCINT0--PCINT7)
    PCMSK0 |= 1 << USI_PCINT;         // Enable pin change ( PA6 bei TinyX4, PB0 bei TinyX5)

    sei();
  }

  /* ------------------------------------------------------------------
                                uart_putchar

       schreibt ein Zeichen auf der USI-Schnittstelle, die als UART
       konfiguriert ist. TxD ist Anschluss D0 (entspricht wirklich
       MISO Anschluss der SPI Schnittstelle)
     ------------------------------------------------------------------ */
  void uart_putchar(uint8_t c)
  {
    uart_func= TXD;
    while (!usiserial_send_available());
    usiserial_send_byte(c);
    while (!usiserial_send_available());
    uart_func= RXD;
  }


  /* ------------------------------------------------------------------
                                uart_getchar

       liest ein Zeichen auf der USI-Schnittstelle ein, die als UART
       konfiguriert ist. RxD ist Anschluss DI (das entspricht wirklich
       MOSI - Anschluss der SPI Schnittstelle)
     ------------------------------------------------------------------ */
  uint8_t uart_getchar(void)
  {
    uint8_t ch;

    while(!(serialdataready))
    while(!(usiserial_readfinished));
    serialdataready= false;
    ch= serialinput;

    _delay_us((100000 / (BAUDRATE / 100)));       // warten bis 10 Bits (Startbit, Stopbit, 8 Datenbits) eingelesen sind

    #if (echo_enable == 1)
      uart_putchar(ch)
    #endif

    return ch;
  }

  /* ------------------------------------------------------------------
                                uart_ischar

       testet, ob auf der USI-Schnittstelle die als UART konfiguriert
       ist, ein Zeichen eingetroffen ist. Hierbei wird dieses Zeichen
       jedoch NICHT gelesen.
     ------------------------------------------------------------------ */
  uint8_t uart_ischar(void)
  {
    if (serialdataready) return 1; else return 0;
  }


/* #########################################################
                        Ende USI-Serial
   ########################################################## */
#endif


/* ##########################################################
                fuer MCU's mit Hardware UART
   ########################################################## */
#if defined __AVR_ATtiny4313  || __AVR_ATtiny2313__ ||                                              \
            __AVR_ATmega48__  || __AVR_ATmega88__   || __AVR_ATmega168__ || __AVR_ATmega328P__ ||   \
            __AVR_ATmega328__ ||                                                                    \
            __AVR_ATmega8__   || __AVR_ATmega8515__

  /* --------------------------------------------------
      Initialisierung der seriellen Schnittstelle:

      Protokoll: 8 Daten-, 1 Stopbit
     -------------------------------------------------- */
  void uart_init(void)
  {
    uint16_t ubrr;

    //  Code, fuer Baudraten groesser 57600
    if (BAUDRATE> 57600)
    {
      ubrr= (F_CPU/16/(BAUDRATE>>1));
      ubrr--;
      UCSR0A |= 1<<U2X0;                                  // Baudrate verdoppeln
    }
    else
    {
      ubrr= (F_CPU/16/BAUDRATE-1);
    }

    UBRR0H = (uint8_t)(ubrr>>8);                    // Baudrate setzen
    UBRR0L = (uint8_t)ubrr;

  #if defined __AVR_ATmega8__

    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSRC  = (1<<URSEL) | (1<<UCSZ01) | (1<<UCSZ00);

  #else

    UCSR0B = (1<<RXEN0)|(1<<TXEN0);                       // Transmitter und Receiver enable
    UCSR0C = (3<<UCSZ00);                                 // 8 Datenbit, 1 Stopbit

  #endif
  }

  /* --------------------------------------------------
      Zeichen ueber die serielle Schnittstelle senden
     -------------------------------------------------- */

  void uart_putchar(uint8_t ch)
  {
    while (!( UCSR0A & (1<<UDRE0)));                      // warten bis Transmitterpuffer leer ist
    UDR0 = ch;                                            // Zeichen senden
  }

  /* --------------------------------------------------
      testen, ob ein Zeichen auf der Schnittstelle
      ansteht
     -------------------------------------------------- */

  uint8_t uart_ischar( void )
  {
    return (UCSR0A & (1<<RXC0));
  }

  /* --------------------------------------------------
      Zeichen von serieller Schnittstelle lesen
     -------------------------------------------------- */

  uint8_t uart_getchar( void )
  {
    while(!(UCSR0A & (1<<RXC0)));                         // warten bis Zeichen eintrifft

  #if (echo_enable == 1)

    char ch;
    ch= UDR0;
    uart_putchar(ch);
    return ch;

  #else

    return UDR0;

  #endif
  }

/* ##########################################################
                    Ende Hardware UART
   ########################################################## */
#endif

#if (readint_enable == 1)

  /* ------------------------------------------------------------
                              readint

       liest einen 16-Bit signed Integer auf der seriellen
       Schnittstelle ein (allerdings reicht der Eingabebereich
       nur von -32767 .. +32767).

       Korrektur ist mit der Loeschtaste nach links moeglich.
     ------------------------------------------------------------ */
  int16_t uart_readint(void)
  {
    uint8_t   signflag= 0;
    uint16_t  sum = 0;
    uint8_t   zif;
    uint8_t   ch;

    do
    {
      ch= uart_getchar();
      if (ch== 0x0a) ch= 0x0d;

      // Ziffern auswerten
      if ((ch>= '0') && (ch<= '9'))
      {
        zif= ch-'0';

        if ((sum== 0) && (zif))        // erste Ziffer
        {
          sum= zif;
          uart_putchar(ch);
        }
        else
        if (sum < 3277)
        {
          if  (!( ((sum * 10) > 32750) && (zif > 7) ))
          {
            sum= (sum*10) + zif;
            uart_putchar(ch);
          }
        }
      }

      // letzte Eingabe loeschen
      if ((ch== 127) || (ch== 8))    // letzte Eingabe loeschen
      {
        if (sum)
        {
          sum /= 10;
          uart_putchar(8);
          uart_putchar(' ');
          uart_putchar(8);
        }
        else
        if (signflag)
        {
          uart_putchar(8);
          uart_putchar(' ');
          uart_putchar(8);
          signflag= 0;
        }
      }

      // Eingabe Minuszeichen
      if ((ch== '-') && (sum == 0))
      {
        signflag= 1;
          uart_putchar('-');
      }

    } while (ch != 0x0d);              // wiederholen bis Returnzeichen eintrifft
    if (signflag) return sum *(-1); else return sum;
  }

#endif

