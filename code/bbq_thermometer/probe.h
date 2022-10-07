#include <math.h>
#include <Adafruit_ADS1X15.h>

const int N_PROBES = 2;

Adafruit_ADS1115 ads;
double vRef = 4.52;
double rRef = 100000;
double coeffs[N_PROBES][3];

void initAds(){
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while(1);
  }
  ads.setDataRate(7); 
  Serial.println("ADS initialized");
}

/**
 * Read resistance from adc
 * pin: input on the adc
 */
double getResistance(int pin){
  int16_t adc = ads.readADC_SingleEnded(pin);
  double volts= ads.computeVolts(adc);
  return volts > 0? rRef * (vRef/volts) - rRef:-1;
}

/**
 * Get temperature from resistance and Steinhart–Hart coefficients
 * res: resistance in ohm
 * coeffs: coefficients double[3]
 */
double getTemperature(double res,double* coeffs){
  if(res<0) return -1;
  double temp = log(res);
  temp = 1 / (coeffs[0] + (coeffs[1] * temp) + (coeffs[2] * temp * temp * temp));

  return temp;
}

/**
 * Get coefficients of Steinhart–Hart from 3 points  
 * k1,k2,k3: temperature in Kelvin
 * r1,r2,r3: resistance in ohm
 * 
 * https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
 */ 
void getCoefficients(double k1,double k2,double k3,double r1,double r2,double r3,double* coeffs){
    if(r1 == 0 || r2 ==0 || r3 == 0) return;
    if (r1 == r2 || r1 == r3 || r2 == r3) return;
    
    double l1 = log(r1),l2 = log(r2),l3 = log(r3);
    if(l1+l2+l3 == 0) return;
    
    double y1 = 1/k1, y2 = 1/k2,y3 = 1/k3;
    double g2 = (y2-y1)/(l2-l1),g3 = (y3-y1)/(l3-l1);
    
    coeffs[2] = (g3-g2)/(l3-l2)*(1/(l1+l2+l3));
    coeffs[1] = g2-coeffs[2] *(l1*l1+l1*l2+l2*l2);
    coeffs[0] = y1 - ((coeffs[1]+l1*l1*coeffs[2])*l1);
}
