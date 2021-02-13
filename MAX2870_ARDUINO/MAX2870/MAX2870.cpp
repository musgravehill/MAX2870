

#include <SPI.h>
#include "MAX2870.h"
#include <math.h>
#include <stdio.h>

//****************************************************************************
MAX2870::MAX2870(const uint8_t MAX2870_pin_LE, const uint8_t MAX2870_pin_CE, const uint8_t MAX2870_pin_RF_EN, const uint8_t MAX2870_pin_LD) {
  delay(20);

  pin_LE = MAX2870_pin_LE;
  pin_CE = MAX2870_pin_CE;
  pin_RF_EN = MAX2870_pin_RF_EN;
  pin_LD = MAX2870_pin_LD;

  pinMode (pin_LE, OUTPUT);
  digitalWrite(pin_LE, 1);

  pinMode (pin_CE, OUTPUT);
  digitalWrite(pin_CE, 1);

  pinMode (pin_RF_EN, OUTPUT);
  digitalWrite(pin_RF_EN, 1);

  pinMode (pin_LD, INPUT);

  SPI.setClockDivider(SPI_CLOCK_DIV16); //16MHz system clock \ 16 = 1MHz SPI
  SPI.setDataMode(SPI_MODE0); //CPOL = CPHA = 0, 8 bits per frame
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();

  delay(20);

  reg0.all = 0x007d0000;
  reg1.all = 0x2000fff9;
  reg2.all = 0x00004042;
  reg3.all = 0x0000000b;
  reg4.all = 0x6180b23c;
  reg5.all = 0x00400005;
  reg6.all = 0x00000000;

  setConfig();

  delay(20);

  setConfig();
}



//****************************************************************************
void MAX2870::writeData(uint32_t data) {
  digitalWrite(pin_LE, 0);
  delayMicroseconds(10);

  SPI.transfer((0xFF000000 & data) >> 24);  //????  в таком порядке или обратном????
  SPI.transfer((0x00FF0000 & data) >> 16);
  SPI.transfer((0x0000FF00 & data) >> 8);
  SPI.transfer( 0x000000FF & data);

  delayMicroseconds(2500);
  digitalWrite(pin_LE, 1);
  delayMicroseconds(2500);
}

//****************************************************************************
void MAX2870::setConfig() {
  writeData(reg5.all);
  writeData(reg4.all);
  writeData(reg3.all);
  writeData(reg2.all);
  writeData(reg1.all);
  writeData(reg0.all);
}



//****************************************************************************
void MAX2870::set_RF_OUT_A(double freq) {
  uint32_t n, frac, m, diva = 0;
  double pll_coefficient, fractional = 0;

  //double f_pfd = getPFD(); //set f_pfd wnen MAX2870_my.setPFD(**)  in begin() block

  while (freq * powf(2, diva) < 3000.0)  {
    diva = diva + 1;
  }
  pll_coefficient = freq * powf(2, diva) / f_pfd;
  n = floor(pll_coefficient);

  fractional = pll_coefficient - n;
  m = 4000;
  frac = round(m * fractional);

  reg0.bits.frac = frac;
  reg0.bits.n = n;
  reg1.bits.m = m;
  reg4.bits.diva = diva;

  reg3.bits.mutedel = 1;

  setConfig();
  f_rfouta = f_pfd * (reg0.bits.n + 1.0 * reg0.bits.frac / reg1.bits.m) / powf(2, reg4.bits.diva);
}

void MAX2870::setPFD(const double ref_in, const uint16_t rdiv) {
  // fPFD = fREF * [(1 + DBR)/(R * (1 + RDIV2))]  
  // DBR=0 RDIV2=0
  // fPFD = fREF * [1/R] = fREF / R
  //
  
  f_pfd = ref_in / rdiv;

  if (f_pfd > 32.0) {
    reg2.bits.lds = 1;
  }
  else {
    reg2.bits.lds = 0;
  }

  reg3.bits.cdiv = round(f_pfd / 0.10);

  reg2.bits.dbr = 0;
  reg2.bits.rdiv2 = 0;
  reg2.bits.r = rdiv;

  uint32_t bs = f_pfd * 20;

  if (bs > 1023) {
    bs = 1023;
  }
  else if (bs < 1) {
    bs = 1;
  }

  reg4.bits.bs = 0x03FF & bs;
  reg4.bits.bs2 = 0x03 & (bs >> 8);

  setConfig();
}


void MAX2870::powerOn(bool pwr) {
  reg2.bits.shdn =  !pwr;
  reg4.bits.sdldo = !pwr;
  reg4.bits.sddiv = !pwr;
  reg4.bits.sdref = !pwr;
  reg4.bits.sdvco = !pwr;
  reg5.bits.sdpll = !pwr;

  setConfig();
}








