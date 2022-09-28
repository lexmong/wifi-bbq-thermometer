#include <math.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;
double vRef = 4.52;
double rRef = 100000;
int16_t adc;
double volts;


double coeffs[2][3] = {
  { 
    0.001714296,
    0.000081099,
    3.693302044e-7
  },
  { 
    2.025087E-03,
    3.170937E-05,
    5.042739E-07
  }
};

void initAds(){
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while(1);
  }
  ads.setDataRate(7); 
  Serial.println("ADS initialized");
}

double getResistance(int pin){
  adc = ads.readADC_SingleEnded(pin);
  volts = ads.computeVolts(adc);
  return volts > 0? rRef * (vRef/volts) - rRef:-1;
}

double getTemperature(double res,double* coeffs){
  if(res<0) return -1;
  double temp = log(res);
  temp = 1 / (coeffs[0] + (coeffs[1] * temp) + (coeffs[2] * temp * temp * temp));

  return temp;
}

//https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
void getCoefficients(double k1,double k2,double k3,double r1,double r2,double r3,double* coeffs){
    if(r1 == 0 || r2 ==0 || r3 == 0) return;
    if (r1 == r2 || r1 == r3 || r2 == r3) return;
    
    double l1 = log(r1),l2 = log(r2),l3 = log(r3);
    if(l1+l2+l3 == 0) coeffs[0,0,0];
    
    double y1 = 1/k1, y2 = 1/k2,y3 = 1/k3;
    double g2 = (y2-y1)/(l2-l1),g3 = (y3-y1)/(l3-l1);
    
    coeffs[2] = (g3-g2)/(l3-l2)*(1/(l1+l2+l3));
    coeffs[1]= g2-coeffs[2] *(l1*l1+l1*l2+l2*l2);
    coeffs[0] = y1 - ((coeffs[1]+l1*l1*coeffs[2])*l1);
}
