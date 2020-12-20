/* -----------------------------------------------
                     avr_gpio.h

     Makros und Defines zum Einfachen Umgang mit
     GPIO - Portpins

     Wird ein Portpin als Eingang initialisiert
     wird der interne Pop-Up Widerstand einge-
     schaltet !

     22.12.2017  R. Seelig
   ----------------------------------------------- */


// Bitmasken
// ----------------------------------------------
#define MASK0                ( 1 << 0 )
#define MASK1                ( 1 << 1 )
#define MASK2                ( 1 << 2 )
#define MASK3                ( 1 << 3 )
#define MASK4                ( 1 << 4 )
#define MASK5                ( 1 << 5 )
#define MASK6                ( 1 << 6 )
#define MASK7                ( 1 << 7 )


// PortA
// -----------------------------------------------
#define PA0_output_init()    ( DDRA |= MASK0 )
#define PA0_set()            ( PORTA |= MASK0 )
#define PA0_clr()            ( PORTA &= ~MASK0 )
#define PA0_input_init()     { DDRA &= ~MASK0; PORTA |= MASK0; }
#define is_PA0()             ( (PINA & MASK0) >> 0 )

#define PA1_output_init()    ( DDRA |= MASK1 )
#define PA1_set()            ( PORTA |= MASK1 )
#define PA1_clr()            ( PORTA &= ~MASK1 )
#define PA1_input_init()     { DDRA &= ~MASK1; PORTA |= MASK1; }
#define is_PA1()             ( (PINA & MASK1) >> 1 )

#define PA2_output_init()    ( DDRA |= MASK2 )
#define PA2_set()            ( PORTA |= MASK2 )
#define PA2_clr()            ( PORTA &= ~MASK2 )
#define PA2_input_init()     { DDRA &= ~MASK2; PORTA |= MASK2; }
#define is_PA2()             ( (PINA & MASK2) >> 2 )

#define PA3_output_init()    ( DDRA |= MASK3 )
#define PA3_set()            ( PORTA |= MASK3 )
#define PA3_clr()            ( PORTA &= ~MASK3 )
#define PA3_input_init()     { DDRA &= ~MASK3; PORTA |= MASK3; }
#define is_PA3()             ( (PINA & MASK3) >> 3 )

#define PA4_output_init()    ( DDRA |= MASK4 )
#define PA4_set()            ( PORTA |= MASK4 )
#define PA4_clr()            ( PORTA &= ~MASK4 )
#define PA4_input_init()     { DDRA &= ~MASK4; PORTA |= MASK4; }
#define is_PA4()             ( (PINA & MASK4) >> 4 )

#define PA5_output_init()    ( DDRA |= MASK5 )
#define PA5_set()            ( PORTA |= MASK5 )
#define PA5_clr()            ( PORTA &= ~MASK5 )
#define PA5_input_init()     { DDRA &= ~MASK5; PORTA |= MASK5; }
#define is_PA5()             ( (PINA & MASK5) >> 5 )

#define PA6_output_init()    ( DDRA |= MASK6 )
#define PA6_set()            ( PORTA |= MASK6 )
#define PA6_clr()            ( PORTA &= ~MASK6 )
#define PA6_input_init()     { DDRA &= ~MASK6; PORTA |= MASK6; }
#define is_PA6()             ( (PINA & MASK6) >> 6 )

#define PA7_output_init()    ( DDRA |= MASK7 )
#define PA7_set()            ( PORTA |= MASK7 )
#define PA7_clr()            ( PORTA &= ~MASK7 )
#define PA7_input_init()     { DDRA &= ~MASK7; PORTA |= MASK7; }
#define is_PA7()             ( (PINA & MASK7) >> 7 )

// PortB
// -----------------------------------------------
#define PB0_output_init()    ( DDRB |= MASK0 )
#define PB0_set()            ( PORTB |= MASK0 )
#define PB0_clr()            ( PORTB &= ~MASK0 )
#define PB0_input_init()     { DDRB &= ~MASK0; PORTB |= MASK0; }
#define is_PB0()             ( (PINB & MASK0) >> 0 )

#define PB1_output_init()    ( DDRB |= MASK1 )
#define PB1_set()            ( PORTB |= MASK1 )
#define PB1_clr()            ( PORTB &= ~MASK1 )
#define PB1_input_init()     { DDRB &= ~MASK1; PORTB |= MASK1; }
#define is_PB1()             ( (PINB & MASK1) >> 1 )

#define PB2_output_init()    ( DDRB |= MASK2 )
#define PB2_set()            ( PORTB |= MASK2 )
#define PB2_clr()            ( PORTB &= ~MASK2 )
#define PB2_input_init()     { DDRB &= ~MASK2; PORTB |= MASK2; }
#define is_PB2()             ( (PINB & MASK2) >> 2 )

#define PB3_output_init()    ( DDRB |= MASK3 )
#define PB3_set()            ( PORTB |= MASK3 )
#define PB3_clr()            ( PORTB &= ~MASK3 )
#define PB3_input_init()     { DDRB &= ~MASK3; PORTB |= MASK3; }
#define is_PB3()             ( (PINB & MASK3) >> 3 )

#define PB4_output_init()    ( DDRB |= MASK4 )
#define PB4_set()            ( PORTB |= MASK4 )
#define PB4_clr()            ( PORTB &= ~MASK4 )
#define PB4_input_init()     { DDRB &= ~MASK4; PORTB |= MASK4; }
#define is_PB4()             ( (PINB & MASK4) >> 4 )

#define PB5_output_init()    ( DDRB |= MASK5 )
#define PB5_set()            ( PORTB |= MASK5 )
#define PB5_clr()            ( PORTB &= ~MASK5 )
#define PB5_input_init()     { DDRB &= ~MASK5; PORTB |= MASK5; }
#define is_PB5()             ( (PINB & MASK5) >> 5 )

#define PB6_output_init()    ( DDRB |= MASK6 )
#define PB6_set()            ( PORTB |= MASK6 )
#define PB6_clr()            ( PORTB &= ~MASK6 )
#define PB6_input_init()     { DDRB &= ~MASK6; PORTB |= MASK6; }
#define is_PB6()             ( (PINB & MASK6) >> 6 )

#define PB7_output_init()    ( DDRB |= MASK7 )
#define PB7_set()            ( PORTB |= MASK7 )
#define PB7_clr()            ( PORTB &= ~MASK7 )
#define PB7_input_init()     { DDRB &= ~MASK7; PORTB |= MASK7; }
#define is_PB7()             ( (PINB & MASK7) >> 7 )

// PortC
// -----------------------------------------------
#define PC0_output_init()    ( DDRC |= MASK0 )
#define PC0_set()            ( PORTC |= MASK0 )
#define PC0_clr()            ( PORTC &= ~MASK0 )
#define PC0_input_init()     { DDRC &= ~MASK0; PORTC |= MASK0; }
#define is_PC0()             ( (PINC & MASK0) >> 0 )

#define PC1_output_init()    ( DDRC |= MASK1 )
#define PC1_set()            ( PORTC |= MASK1 )
#define PC1_clr()            ( PORTC &= ~MASK1 )
#define PC1_input_init()     { DDRC &= ~MASK1; PORTC |= MASK1; }
#define is_PC1()             ( (PINC & MASK1) >> 1 )

#define PC2_output_init()    ( DDRC |= MASK2 )
#define PC2_set()            ( PORTC |= MASK2 )
#define PC2_clr()            ( PORTC &= ~MASK2 )
#define PC2_input_init()     { DDRC &= ~MASK2; PORTC |= MASK2; }
#define is_PC2()             ( (PINC & MASK2) >> 2 )

#define PC3_output_init()    ( DDRC |= MASK3 )
#define PC3_set()            ( PORTC |= MASK3 )
#define PC3_clr()            ( PORTC &= ~MASK3 )
#define PC3_input_init()     { DDRC &= ~MASK3; PORTC |= MASK3; }
#define is_PC3()             ( (PINC & MASK3) >> 3 )

#define PC4_output_init()    ( DDRC |= MASK4 )
#define PC4_set()            ( PORTC |= MASK4 )
#define PC4_clr()            ( PORTC &= ~MASK4 )
#define PC4_input_init()     { DDRC &= ~MASK4; PORTC |= MASK4; }
#define is_PC4()             ( (PINC & MASK4) >> 4 )

#define PC5_output_init()    ( DDRC |= MASK5 )
#define PC5_set()            ( PORTC |= MASK5 )
#define PC5_clr()            ( PORTC &= ~MASK5 )
#define PC5_input_init()     { DDRC &= ~MASK5; PORTC |= MASK5; }
#define is_PC5()             ( (PINC & MASK5) >> 5 )

#define PC6_output_init()    ( DDRC |= MASK6 )
#define PC6_set()            ( PORTC |= MASK6 )
#define PC6_clr()            ( PORTC &= ~MASK6 )
#define PC6_input_init()     { DDRC &= ~MASK6; PORTC |= MASK6; }
#define is_PC6()             ( (PINC & MASK6) >> 6 )

#define PC7_output_init()    ( DDRC |= MASK7 )
#define PC7_set()            ( PORTC |= MASK7 )
#define PC7_clr()            ( PORTC &= ~MASK7 )
#define PC7_input_init()     { DDRC &= ~MASK7; PORTC |= MASK7; }
#define is_PC7()             ( (PINC & MASK7) >> 7 )

// PortD
// -----------------------------------------------
#define PD0_output_init()    ( DDRD |= MASK0 )
#define PD0_set()            ( PORTD |= MASK0 )
#define PD0_clr()            ( PORTD &= ~MASK0 )
#define PD0_input_init()     { DDRD &= ~MASK0; PORTD |= MASK0; }
#define is_PD0()             ( (PIND & MASK0) >> 0 )

#define PD1_output_init()    ( DDRD |= MASK1 )
#define PD1_set()            ( PORTD |= MASK1 )
#define PD1_clr()            ( PORTD &= ~MASK1 )
#define PD1_input_init()     { DDRD &= ~MASK1; PORTD |= MASK1; }
#define is_PD1()             ( (PIND & MASK1) >> 1 )

#define PD2_output_init()    ( DDRD |= MASK2 )
#define PD2_set()            ( PORTD |= MASK2 )
#define PD2_clr()            ( PORTD &= ~MASK2 )
#define PD2_input_init()     { DDRD &= ~MASK2; PORTD |= MASK2; }
#define is_PD2()             ( (PIND & MASK2) >> 2 )

#define PD3_output_init()    ( DDRD |= MASK3 )
#define PD3_set()            ( PORTD |= MASK3 )
#define PD3_clr()            ( PORTD &= ~MASK3 )
#define PD3_input_init()     { DDRD &= ~MASK3; PORTD |= MASK3; }
#define is_PD3()             ( (PIND & MASK3) >> 3 )

#define PD4_output_init()    ( DDRD |= MASK4 )
#define PD4_set()            ( PORTD |= MASK4 )
#define PD4_clr()            ( PORTD &= ~MASK4 )
#define PD4_input_init()     { DDRD &= ~MASK4; PORTD |= MASK4; }
#define is_PD4()             ( (PIND & MASK4) >> 4 )

#define PD5_output_init()    ( DDRD |= MASK5 )
#define PD5_set()            ( PORTD |= MASK5 )
#define PD5_clr()            ( PORTD &= ~MASK5 )
#define PD5_input_init()     { DDRD &= ~MASK5; PORTD |= MASK5; }
#define is_PD5()             ( (PIND & MASK5) >> 5 )

#define PD6_output_init()    ( DDRD |= MASK6 )
#define PD6_set()            ( PORTD |= MASK6 )
#define PD6_clr()            ( PORTD &= ~MASK6 )
#define PD6_input_init()     { DDRD &= ~MASK6; PORTD |= MASK6; }
#define is_PD6()             ( (PIND & MASK6) >> 6 )

#define PD7_output_init()    ( DDRD |= MASK7 )
#define PD7_set()            ( PORTD |= MASK7 )
#define PD7_clr()            ( PORTD &= ~MASK7 )
#define PD7_input_init()     { DDRD &= ~MASK7; PORTD |= MASK7; }
#define is_PD7()             ( (PIND & MASK7) >> 7 )

