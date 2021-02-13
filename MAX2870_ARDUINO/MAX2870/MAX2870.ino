
//TODO
/*
  pins in constrructor  MAX2870 MAX2870_my(10);
  function setPFD???
  How to determinate f_pfd = frequency of PFD
  How to determinate frequency reference from oscillator?

  include <> ""
  angle brackets (<>) causes the compiler to search the default include directory.
  Double quotes ("") causes it to search the current working directory and, if that search fails, it defaults to the default include directory.
*/


#include <SPI.h>
#include "MAX2870.h" //is .h auto-connect .cpp ?  #include "MAX2870.cpp" cause error

#define MAX2870_reference_frequency_mhz 25.0  //reference frequency 25-50-100MHz quartz
#define MAX2870_R_divider 2                   //R divider to set phase/frequency detector comparison frequency
#define MAX2870_pin_LE 10                     //Load Enable Input. When LE goes high the data stored in the shift register is loaded into the appropriate latches.
#define MAX2870_pin_CE 9                      //init =1 //chip enable
#define MAX2870_pin_RF_EN 8                   //init =1 //RF output enable  PDBRF. RF Power-Down. A logic low on this pin mutes the RF outputs
#define MAX2870_pin_LD 7                      //input for Lock detect  


MAX2870 MAX2870_my(MAX2870_pin_LE, MAX2870_pin_CE, MAX2870_pin_RF_EN, MAX2870_pin_LD);

void setup() {
  MAX2870_my.powerOn(true); //set all hardware enable pins and deassert software shutdown bits
  MAX2870_my.setPFD(MAX2870_reference_frequency_mhz , MAX2870_R_divider); //inputs are reference frequency and R divider to set phase/frequency detector comparison frequency
  MAX2870_my.set_RF_OUT_A(433);
}

void loop() {


}
