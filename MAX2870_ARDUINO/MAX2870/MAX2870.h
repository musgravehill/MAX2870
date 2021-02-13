#ifndef _MAX2870_H_
#define _MAX2870_H_

#include <Arduino.h>

/**
  @brief The MAX2870 is an ultra-wideband phase-locked loop (PLL) with integrated
  voltage control oscillators (VCOs)capable of operating in both integer-N and
  fractional-N modes. When combined with an external reference oscillator and
  loop filter, the MAX2870 is a high-performance frequency synthesizer capable
  of synthesizing frequencies from 23.5MHz to 6.0GHz while maintaining superior
  phase noise and spurious performance.

  @code
  #include "mbed.h"
  #include <stdio.h>

  #include "MAX2870.h"

  SPI         spi(D11,D12,D13);           //mosi, miso, sclk
  Serial      pc(USBTX,USBRX,9600);       //tx, rx, baud

  DigitalOut  le(D10,1);                  //latch enable
  DigitalOut  ce(D9,1);                   //chip enable
  DigitalOut  rfout_en(D8,1);             //RF output enable

  int main() {
    float freq_entry;                   //frequency input to terminal
    float freq_actual;                  //frequency based on MAX2870 settings
    float freq_pfd;                     //frequency of phase frequency detector
    float pll_coefficient;              //fractional-N coefficient (N + F/M)
    float vco_divisor;                  //divisor from f_vco to f_rfouta
    char buffer[256];                   //string input from terminal

    spi.format(8,0);                    //CPOL = CPHA = 0, 8 bits per frame
    spi.frequency(1000000);             //1 MHz SPI clock

    MAX2870 MAX2870(spi,D10);           //create object of class MAX2870

    //The routine in the while(1) loop will ask the user to input a desired
    //output frequency, calculate the corresponding register settings, update
    //the MAX2870 registers, and then independently use the programmed values
    //from the registers to re-calculate the output frequency chosen
    while(1){
        pc.printf("\n\rEnter a frequency in MHz:");
        fgets(buffer,256,stdin);        //store entry as string until newline entered
        freq_entry = atof (buffer);     //convert string to a float
        MAX2870.frequency(freq_entry);  //update MAX2870 registers for new frequency
        MAX2870.readRegister6();        //read register 6 and update MAX2870.reg6

        //Examples for how to calculate important operation parameters like
        //PFD frequency and divisor ratios using members of the MAX2870 class
        freq_pfd = MAX2870.f_reference*(1+MAX2870.reg2.bits.dbr)/(MAX2870.reg2.bits.r*(1+MAX2870.reg2.bits.rdiv2));
        pll_coefficient = (MAX2870.reg0.bits.n+1.0*MAX2870.reg0.bits.frac/MAX2870.reg1.bits.m);
        vco_divisor = powf(2,MAX2870.reg4.bits.diva);

        //calculate expected f_RFOUTA based on the register settings
        freq_actual = freq_pfd*pll_coefficient/vco_divisor;
        pc.printf("\n\rTarget: %.3f MHz\tActual: %.3f MHz",freq_entry,freq_actual);
        pc.printf("\n\rDie: %d, VCO: %d, F_PFD: %f",MAX2870.reg6.bits.die,MAX2870.reg6.bits.v,freq_pfd);
        pc.printf("\n\rN: %d, F: %d, M: %d, N+F/M: %f",MAX2870.reg0.bits.n,MAX2870.reg0.bits.frac,MAX2870.reg1.bits.m,pll_coefficient);
    }

  }

  @endcode
*/
class MAX2870 {
  public:

    ///@brief MAX2870 Constructor
    MAX2870(const uint8_t MAX2870_pin_LE, const uint8_t MAX2870_pin_CE, const uint8_t MAX2870_pin_RF_EN, const uint8_t MAX2870_pin_LD);

    //MAX2870 Registers
    enum Registers_e
    {
      REG0          = 0x00,
      REG1          = 0x01,
      REG2          = 0x02,
      REG3          = 0x03,
      REG4          = 0x04,
      REG5          = 0x05,
      REG6          = 0x06
    };

    //Register 0 bits
    union REG0_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr       : 3;
        uint32_t frac       : 12;
        uint32_t n          : 16;
        uint32_t intfrac    : 1;
      } bits;
    };

    //Register 1 bits
    union REG1_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr         : 3;
        uint32_t m            : 12;
        uint32_t p            : 12;
        uint32_t cpt          : 2;
        uint32_t cpl          : 2;
        uint32_t reserved1    : 1;
      } bits;
    };

    //Register 2 bits
    union REG2_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr      : 3;
        uint32_t rst       : 1;
        uint32_t tri       : 1;
        uint32_t shdn      : 1;
        uint32_t pdp       : 1;
        uint32_t ldp       : 1;
        uint32_t ldf       : 1;
        uint32_t cp        : 4;
        uint32_t reg4db    : 1;
        uint32_t r         : 10;
        uint32_t rdiv2     : 1;
        uint32_t dbr       : 1;
        uint32_t mux       : 3;
        uint32_t sdn       : 2;
        uint32_t lds       : 1;
      } bits;
    };

    //Register 3 bits
    union REG3_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr      : 3;
        uint32_t cdiv      : 12;
        uint32_t cdm       : 2;
        uint32_t mutedel   : 1;
        uint32_t csm       : 1;
        uint32_t reserved1 : 5;
        uint32_t vas_temp  : 1;
        uint32_t vas_shdn  : 1;
        uint32_t vco       : 6;
      } bits;
    };

    //Register 4 bits
    union REG4_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr       : 3;
        uint32_t apwr       : 2;
        uint32_t rfa_en     : 1;
        uint32_t bpwr       : 2;
        uint32_t rfb_en     : 1;
        uint32_t bdiv       : 1;
        uint32_t mtld       : 1;
        uint32_t sdvco      : 1;
        uint32_t bs         : 8;
        uint32_t diva       : 3;
        uint32_t fb         : 1;
        uint32_t bs2        : 2;
        uint32_t sdref      : 1;
        uint32_t sddiv      : 1;
        uint32_t sdldo      : 1;
        uint32_t reservered1: 3;
      } bits;
    };

    //Register 5 bits
    union REG5_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr      : 3;
        uint32_t adcm      : 3;
        uint32_t adcs      : 1;
        uint32_t reserved1 : 11;
        uint32_t mux       : 1;
        uint32_t reserved2 : 3;
        uint32_t ld        : 2;
        uint32_t f01       : 1;
        uint32_t sdpll     : 1;
        uint32_t reserved3 : 3;
        uint32_t vas_dly   : 2;
        uint32_t reserved4 : 1;

      } bits;
    };

    //Register 6 bits
    union REG6_u
    {
      //Access all bits
      uint32_t all;

      //Access individual bits
      struct BitField_s
      {
        uint32_t addr      : 3;
        uint32_t v         : 6;
        uint32_t vasa      : 1;  //max2871?
        uint32_t reserved1 : 5;  //max2871?
        uint32_t adcv      : 1;  //max2871?
        uint32_t adc       : 7;  //max2871?
        uint32_t por       : 1;  //max2871?
        uint32_t reserved2 : 4;  //max2871?
        uint32_t die       : 4;
      } bits;
    };


    ///@brief Writes raw 32-bit data pattern. The MAX2870 accepts 32-bit words at a time; 29 data bits and 3 address bits.
    ///@param[in] data - 32-bit word to write to the MAX2870. Bits[31:3] contain the register data, and Bits[2:0] contain the register address.
    void writeData(uint32_t data);

    ///@brief Updates MAX2870 settings to achieve target output frequency on channel A.\n
    ///@param[in] freq - Frequency in MHz
    void set_RF_OUT_A(double freq);

    ///@brief Provide frequency input to REF_IN pin.\n
    ///@param[in] ref_in - Frequency in MHz
    void setPFD(double ref_in, uint16_t rdiv);

    void powerOn(bool pwr);

    void setConfig();

  private:

    REG0_u reg0;
    REG1_u reg1;
    REG2_u reg2;
    REG3_u reg3;
    REG4_u reg4;
    REG5_u reg5;
    REG6_u reg6;

    double f_pfd;
    double f_rfouta;

    uint8_t pin_LE;
    uint8_t pin_CE;
    uint8_t pin_RF_EN;
    uint8_t pin_LD;
};

#endif /* _MAX2870_H_ */


