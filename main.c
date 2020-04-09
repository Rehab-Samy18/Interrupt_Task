/**     Interrupt Task that blinks red led with timer and if we press SW1 the green led turns on then blue led turns on and return
 * to red led blinking again
 * Created On : 9/4/2020
 *     By : Rehab Samy
 */
void delayMs(int n);
#define REG volatile unsigned long int *

REG RCGCTIMER = 0x400FE604;
REG GPTMCTL   = 0x4003000C;
REG GPTMCFG   = 0x40030000;
REG GPTMTAMR  = 0x40030004;
REG GPTMTAILR = 0x40030028;
REG GPTMRIS   = 0x4003001C;
REG GPTMICR   = 0x40030024;

REG GPIOICR =  0x4005D41C;
REG GPIOHBCTL = 0x400FE06C;
REG RCGCGPIO =  0x400FE608;
REG GPIOAFSEL_F = 0x4005D420;
REG GPIODIRF =  0x4005D400;
REG GPIODEN_F =  0x4005D51C;
REG GPIODATA_F1 = 0x4005D3FC;
REG GPIOPUR = 0x4005D510;
REG GPIOIBE =  0x4005D408;
REG GPIOIEV = 0x4005D40C;
REG GPIOIM    =  0x4005D410;
REG GPIORIS =  0x4005D414;

REG  NVIC = 0xE000E100;
REG NVIC_PRI0 = 0XE000E400;


int main(void)
{


    /**   Configure GPIO F red led (PF1) **/
    *GPIOHBCTL |= (1<<5);  //ENABLE AHP FOR PORTF
    *RCGCGPIO |= (1<<5); //Enable clock in PortF

  //  *GPIOAFSEL_F &= ~(1<<1);  //Disable the alternate function on PF1
    *GPIODIRF |= (1<<1);      //make PF1 OUTPUT
    *GPIODIRF |= (1<<2);      //make PF2 OUTPUT
    *GPIODIRF |= (1<<3);      //make PF3 OUTPUT
    *GPIODIRF &= ~(1<<4);      //make PF4 INPUT

    *GPIOPUR |= (1<<4); /* enable pull up for PORTF4 */


    *GPIODEN_F  |= (1<<1);   //Enable digital I/O for PF1
    *GPIODEN_F  |= (1<<2);   //Enable digital I/O for PF2
    *GPIODEN_F  |= (1<<3);   //Enable digital I/O for PF3
    *GPIODEN_F  |= (1<<4);   //Enable digital I/O for PF4


    *GPIORIS &= ~0x11; /* make bit 4, 0 edge sensitive */
    *GPIOIBE &= ~0x11; /* trigger is controlled by IEV */
    *GPIOIEV &= ~0x11; /* falling edge trigger */
    *GPIOICR |= 0x11; /* clear any prior interrupt */
    *GPIOIM |= 0x11; /* unmask interrupt */

    /* enable interrupt in NVIC and set priority to 3 */
    *NVIC |= 0x40000000;   // enable IRQ30
    *NVIC_PRI0 = 3 << 21 ;
    /**     11.4 Initialization and Configuration of GPTM  **/
    //To use a GPTM, the appropriate TIMERn bit must be set in the RCGCTIMER
   *RCGCTIMER |= (1<<0); //Enable and provide a clock to 16/32-bit general-purpose timer0 in Run mode

    /**     11.4.1 One-Shot/Periodic Timer Mode  **/

   //1. Ensure the timer is disabled(the TnEN bit in the GPTMCTL register is cleared) before making any changes.
    *GPTMCTL &= ~(1<<0);

   //2. Write the GPTM Configuration Register (GPTMCFG) with a value of 0x0000.0000.
    *GPTMCFG = 0x00000000;

   //3. Configure the TnMR field in the GPTM Timer n Mode Register (GPTMTnMR)
    *GPTMTAMR |= (0x2<<0);

   //4. Optionally configure the TnSNAPS, TnWOT, TnMTE, and TnCDIR bits in the GPTMTnMR register
   //to select whether to capture the value of the free-running timer at time-out, use an external
   //trigger to start counting, configure an additional trigger or interrupt, and count up or down
    *GPTMTAMR &= ~(1<<4);   //Count-Down Timer

    //5. Load the start value into the GPTM Timer n Interval Load Register (GPTMTnILR)
    *GPTMTAILR = 0x00F42400;  //16MHz

   //6.If interrupts are required, set the appropriate bits in the GPTM Interrupt Mask Register(GPTMIMR)
    //*GPTMIMR |= (1<<0);


    //7. Set the TnEN bit in the GPTMCTL register to enable the timer and start counting
    *GPTMCTL |= (1<<0);

    while(1){
        //8. Poll the GPTMRIS register or wait for the interrupt to be generated (if enabled). In both cases,
        //the status flags are cleared by writing a 1 to the appropriate bit of the GPTM Interrupt Clear
        //Register (GPTMICR).
     if( (*GPTMRIS & 0x00000001) == 1){
         *GPTMICR |= (1<<0);
         //Writing a 1 to this bit clears the TATORIS bit in the GPTMRIS register
         //and the TATOMIS bit in the GPTMMIS register.
      *GPIODATA_F1 ^= (1<<1); //Toggle the LED

     }

    }

}



void GPIOF_Handler(void)
{

   *GPIODATA_F1 |= (1<<3);
   delayMs(500);
   *GPIODATA_F1 &= ~(1<<3);
   delayMs(500);
   *GPIODATA_F1 |= (1<<2);
   delayMs(500);
   *GPIODATA_F1 &= ~(1<<2);
   delayMs(500);


*GPIOICR |= 0x11; /* clear the interrupt flag before return */
}
/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
int i, j;
for(i = 0 ; i < n; i++)
for(j = 0; j < 3180; j++)
{} /* do nothing for 1 ms */
}
