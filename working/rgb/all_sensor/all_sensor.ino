#include <Wire.h>
#include "TSL2561.h"
#include "Adafruit_MLX90614.h"



TSL2561 tsl(TSL2561_ADDR_FLOAT); 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup(void) {
  Serial.begin(115200);
  ////////////////////////////////////////////////////////////////
  Serial.println("Adafruit MLX90614 test");  

  mlx.begin();  
  //////////////////////////////////////////////////////////////
  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
    while (1);
  }
    
 
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  
  
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  
  
}

void loop(void) {
  
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);     
  
  Serial.println(x, DEC);

  
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("IR: "); Serial.print(ir);   Serial.print("\t\t");
  Serial.print("Full: "); Serial.print(full);   Serial.print("\t");
  Serial.print("Visible: "); Serial.print(full - ir);   Serial.print("\t");
  
  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));
  //////////////////////////////////////////////////////////////////////////////////////////////
   Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  Serial.println();
  
  delay(100); 
}
