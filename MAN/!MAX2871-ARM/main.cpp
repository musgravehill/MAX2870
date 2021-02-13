#include "mbed.h"
#include <stdio.h>
 
#include "MAX2871.h"
 
int main() {
    
#define D6   P0_6
#define D8   P1_4
#define D9   P1_5
#define D10  P1_3
#define D11  P1_1
#define D12  P1_2
#define D13  P1_0
 
    
    SPI         spi(D11,D12,D13);           //mosi, miso, sclk
    Serial      pc(USBTX,USBRX,9600);       //tx, rx, baud
 
    DigitalOut  le(D10,1);                  //Latch enable pin for MAX2871
    DigitalIn   ld(D6);                     //Lock detect output pin
    DigitalOut  led(LED_BLUE,1);            //blue LED on MAX32600MBED board
 
    DigitalOut  rfouten(D8,1);              //RF output enable pin
    DigitalOut  ce(D9,1);                   //Chip enable pin 
    
    double freq_entry;                  //variable to store user frequency input
    char buffer[256];                   //array to hold string input from terminal
    
    double v_tune, temperature;         //stores TUNE voltage and die temperature of MAX2871
    uint32_t vco;                       //stores active VCO in use
    double freq_rfouta;                 //variable to calculate ouput frequency from register settings
    
    spi.format(8,0);                    //CPOL = CPHA = 0, 8 bits per frame
    spi.frequency(1000000);             //1 MHz SPI clock
 
    MAX2871 max2871(spi,D10);           //create object of class MAX2871, assign latch enable pin
    
    max2871.powerOn(true);              //set all hardware enable pins and deassert software shutdown bits
    
    max2871.setPFD(50.0,2);             //inputs are reference frequency and R divider to set phase/frequency detector comparison frequency
    
    //The routine in the while(1) loop will ask the user to input a desired
    //output frequency, check that it is in range, calculate the corresponding
    //register settings, update the MAX2871 registers, and then independently
    //use the programmed values to re-calculate the output frequency chosen
    while(1){
        pc.printf("\n\rEnter a frequency in MHz:");
        fgets(buffer,256,stdin);  
         pc.printf("Line: %s\n", buffer);                          //store entry as string until newline entered
        freq_entry = floor(1000*atof(buffer))/1000;         //convert string to a float with 1kHz precision
        if((freq_entry < 23.5) || (freq_entry > 6000.0))    //check the entered frequency is in MAX2871 range
            pc.printf("\n\rNot a valid frequency entry.");  
        else
        {
            pc.printf("\n\rTarget: %.3f MHz",freq_entry);   //report the frequency derived from user's input
            max2871.setRFOUTA(freq_entry);                  //update MAX2871 registers for new frequency
 
            while(!ld)                                      //blink an LED while waiting for MAX2871 lock detect signal to assert
            {
                led = !led;
                wait_ms(30);
            }
            led = 1;
        
            vco = max2871.readVCO();                        //read the active VCO from MAX2871
            v_tune = max2871.readADC();                     //read the digitized TUNE voltage
            freq_rfouta = max2871.getRFOUTA();              //calculate the output frequency of channel A
            temperature = max2871.readTEMP();               //read die temperature from MAX2871
 
            //print the achieved output frequency and MAX2871 diagnostics
            pc.printf("\n\rActual: %.3f MHz",freq_rfouta);
            pc.printf("\n\rVTUNE: %.3f V, VCO: %d, TEMP: %f",v_tune,vco,temperature);
        }
    }   
}
 