#include <BH1750.h>                                                                     //PLEASE NOTE: this library will have to be added manually through the particle IDE 
#include "Particle.h"

//SET PINOUTS FOR SYSTEM
BH1750 sensor(0x23, Wire);
int lowWaterLED = D7;
int lowWaterLevelSensor = D6;
int pumpRelay = D5;
int growLightRelay = D4;


//INSTANTIATE VARIABLES
bool waterLevelState = false;
bool lightState = false;
int waterLevelValue = 0;
double currentLight = 0;
double lightSetPoint = 500;                                                            //TO USER!!!!!!! calibrate room lighting HERE!

void setup() 
{
    sensor.begin();
    sensor.set_sensor_mode(BH1750::forced_mode_high_res2);                              //configures the sensor (from library)
    Particle.variable("currentLightLevel", currentLight);                               
    
    pinMode(lowWaterLED, OUTPUT);
    pinMode(lowWaterLevelSensor, INPUT_PULLUP);
    pinMode(pumpRelay, OUTPUT);
    pinMode(growLightRelay, OUTPUT);
}

void loop() 
{
    
    //GROW LIGHT CONTROL
    readLight();
    controlLight();
    
    //LEVEL SENSOR, PUMP AND LOW LEVEL LED CONTROL
    readWaterLevel();
    controlPump();
    controlLowWaterLed();
    
    //REFRESH RATE
    delay(600000);                                                                      //delay has been set to every 10 minute(s)
}

void readLight()
{
    sensor.make_forced_measurement();
    currentLight = sensor.get_light_level();

    Particle.publish("lightLevel", String::format("%.2f", currentLight), PRIVATE);      //used to print the current light sensor value to console for user calibration purposes
    delay(500);
}


//"lightSetPoint" requires calibration to suit the room the indoor garden is installed in, please see variable setting above. Please see the "INSTANTIATE VARIABLES" section.
void controlLight()
{
    if (currentLight > lightSetPoint)                                                   
    {
        if (lightState == true)
        {
            digitalWrite(growLightRelay, HIGH);
            delay(500);
            digitalWrite(growLightRelay, LOW);
            delay(500);
            digitalWrite(growLightRelay, HIGH);
            delay(500);
            digitalWrite(growLightRelay, LOW);
            lightState = false;
            Particle.publish("lightstate", "Grow light is OFF.", PRIVATE);        //used to print the current grow light state to console
        }
        else
        {
            Particle.publish("lightstate", "Grow light is OFF.", PRIVATE);        //used to print the current grow light state to console
        }

    }
    else                                              
    {
        if (lightState == false)
        {
            digitalWrite(growLightRelay, HIGH);
            delay(500);
            digitalWrite(growLightRelay, LOW);
            Particle.publish("lightstate", "Grow light is ON.", PRIVATE);         //used to print the current grow light state to console
            lightState = true;
        }
        else
        {
            Particle.publish("lightstate", "Grow light is ON.", PRIVATE);         //used to print the current grow light state to console
        }
    }
    delay(500);
}

void readWaterLevel()
{
    waterLevelValue = digitalRead(lowWaterLevelSensor);
    
    delay(250);
    
    if (waterLevelValue == HIGH)
    {
        waterLevelState = true;
        Particle.publish("waterlevel", "Water Level is OK.", PRIVATE);            //used to print the current water level state to console
    }
    if (waterLevelValue == LOW)
    {
        waterLevelState = false;
        Particle.publish("waterlevel", "Water Level is LOW.", PRIVATE);           //used to print the current water level state to console (ALSO BEING USED AT IFTTT HOOK)
    }
    delay(500);
}

void controlPump()
{
    if (waterLevelState == false)
    {
        digitalWrite(pumpRelay, LOW);
        Particle.publish("pumpstate", "Pump is currently OFF.", PRIVATE);         //used to print the current pump state to console
    }
    if (waterLevelState == true)
    {
        digitalWrite(pumpRelay, HIGH);
        Particle.publish("pumpstate", "Pump is currently ON.", PRIVATE);          //used to print the current pump state to console
    }
    delay(500);
}

void controlLowWaterLed()
{
    if (waterLevelState == true)
    {
        digitalWrite(lowWaterLED, LOW);
        Particle.publish("lowlevelled", "Low level warning light is currently OFF.", PRIVATE);     //used to print the current LED state to console
    }
    if (waterLevelState == false)
    {
        digitalWrite(lowWaterLED, HIGH);
        Particle.publish("lowlevelled", "Low level warning light is currently ON.", PRIVATE);      //used to print the current LED state to console
    }
    delay(500);
}

