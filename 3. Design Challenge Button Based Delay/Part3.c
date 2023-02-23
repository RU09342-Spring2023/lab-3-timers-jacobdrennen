/* Part1.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Russell Trafford
 *
 *  This code is a template which will change the color of the LED being blinked using the interrupt routine.
 */


#include <msp430.h>

char LED_Color = 0x01;                       // Global Variable to determine which LED should be blinking

void gpioInit();
void timerInit();
unsigned int i=25000;



void main()
{
      WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();




    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;



    P2IFG &= ~BIT3;                         // P2.3 IFG cleared

    __bis_SR_register(LPM3_bits | GIE);                 // Enter LPM3 w/interrupt


    if(!(P4IN & BIT1))    //reset if button is pressed
          {
              P1OUT &= ~BIT0;   //turns red led off
              i=0;
          }    //reset if button is pressed



}


/*
 * gpioInit()
 *
 * Configures the following GPIO Pins:
 *  - P1.0: Output
 *  - P6.6: Output
 *  - P2.3: Input with Pullup Resistor
 */

void gpioInit(){
      // Configure RED LED on P1.0 as Output
      P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
      P1DIR |= BIT0;                          // Set P1.0 to output direction

      // Configure Green LED on P6.6 as Output
      P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state
      P6DIR |= BIT6;                          // Set P6.6 to output direction


      // Configure Button on P2.3 as input with pullup resistor
      P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
      P2REN |= BIT3;                          // P2.3 pull-up register enable
      P2IES &= ~BIT3;                         // P2.3 Low --> High edge
      P2IE |= BIT3;                           // P2.3 interrupt enabled
      P4DIR &= ~BIT1;
      P4REN |= BIT1;               // Enable Resistor on P4.1
      P4OUT |= BIT1;               // Configure Resistor on P4.1 to be Pullup

}




// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P1.3 IFG

    if (P2IES & BIT3)
    {
        LED_Color = 0;
        P2IES &= ~BIT3;

        i = TB2R;
        TB2CTL = TBSSEL_2 | MC_0 | TBCLR;
        TB1CTL = TBSSEL_1 | MC_2;

    }

    else if (!(P2IES & BIT3))
    {
        LED_Color = 1;

        P2IES |= BIT3;
        i = TB1R;
        TB1CTL = TBSSEL_1 | MC_0 | TBCLR;
        TB2CTL = TBSSEL_2 | MC_2;


    }
    P2IE |= BIT3;
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{

    TB1CCR0 += i;
    P1OUT ^= BIT0;       // P1.0 = toggl

}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer2_B0_ISR(void)
{
    TB2CCR0 += i;
    P1OUT ^= BIT0;
}
void timerInit()
{

    TB1CTL = TBSSEL_1 | MC_2| TBCLR;
    TB1CCR0 = 50000;
    TB1CCTL0 |= CCIE;
    TB2CTL = TBSSEL_2 | MC_2|TBCLR;
    TB2CCTL0 |= CCIE;
    TB2CCR0 = 50000;
}
