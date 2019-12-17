#include <YDLidar.h>
#include <table.h>
#include <MsTimer2.h>
// You need to create an driver instance
YDLidar lidar;

bool isScanning = false;


#define YDLIDAR_MOTOR_SCTP 3 // The PWM pin for control the speed of YDLIDAR's motor.
#define YDLIDAR_MOTRO_EN   7 // The ENABLE PIN for YDLIDAR's motor

void setMotorSpeed(float vol){
  uint8_t PWM = (uint8_t)(51*vol);
  analogWrite(YDLIDAR_MOTOR_SCTP, PWM);
}
int truc=0;

 void affichetab(){
   truc++;
   Serial.println("tablau1");
   Serial.println(truc);
   Serial.println();

   for(int i=0;i<100;i++){
     for(int j=0;j<150;j++){
       Serial.print(table[j][i]);
       table[j][i]=0;
     }
     Serial.println();
 }
 initTable();
}
void restartScan(){
    device_info deviceinfo;
    if (lidar.getDeviceInfo(deviceinfo, 100) == RESULT_OK) {
         int _samp_rate=4;
         String model;
         float freq = 7.0f;
         switch(deviceinfo.model){
            case 1:
                model="F4";
                _samp_rate=4;
                freq = 7.0;
                break;
            case 4:
                model="S4";
                _samp_rate=4;
                freq = 7.0;
                break;
            case 5:
                model="G4";
                _samp_rate=9;
                freq = 7.0;
                break;
            case 6:
                model="X4";
                _samp_rate=5;
                freq = 7.0;
                break;
            default:
                model = "Unknown";
          }

          uint16_t maxv = (uint16_t)(deviceinfo.firmware_version>>8);
          uint16_t midv = (uint16_t)(deviceinfo.firmware_version&0xff)/10;
          uint16_t minv = (uint16_t)(deviceinfo.firmware_version&0xff)%10;
          if(midv==0){
            midv = minv;
            minv = 0;
          }

          /*
          Serial.print("Firmware version:");
          Serial.print(maxv,DEC);
          Serial.print(".");
          Serial.print(midv,DEC);
          Serial.print(".");
          Serial.println(minv,DEC);

          Serial.print("Hardware version:");
          Serial.println((uint16_t)deviceinfo.hardware_version,DEC);

          Serial.print("Model:");
          Serial.println(model);

          Serial.print("Serial:");
          for (int i=0;i<16;i++){
            Serial.print(deviceinfo.serialnum[i]&0xff, DEC);
          }
          Serial.println("");

          Serial.print("[YDLIDAR INFO] Current Sampling Rate:");
          Serial.print(_samp_rate,DEC);
          Serial.println("K");

          Serial.print("[YDLIDAR INFO] Current Scan Frequency:");
          Serial.print(freq,DEC);
          Serial.println("Hz");*/
          delay(100);
          device_health healthinfo;
          if (lidar.getHealth(healthinfo, 100) == RESULT_OK){
             // detected...
              //Serial.print("[YDLIDAR INFO] YDLIDAR running correctly! The health status:");
              //Serial.println( healthinfo.status==0?"well":"bad");
              if(lidar.startScan() == RESULT_OK){
                isScanning = true;
                //start motor in 1.8v
		        setMotorSpeed(1.8);
	/*	digitalWrite(YDLIDAR_MOTRO_EN, HIGH);
                Serial.println("Now YDLIDAR is scanning ......");
              //delay(1000);
              }else{
                  Serial.println("start YDLIDAR is failed!  Continue........");
              }
          }else{
              Serial.println("cannot retrieve YDLIDAR health");
          }


       }else{
             Serial.println("YDLIDAR get DeviceInfo Error!!!");
       }*/
}}}}

void initTable(){
  //definition des tassots sur la table2020 et initialise le tableau

  for (int  i=93;i<100;i++){
    table[45][i]=0x01;
    table[46][i]=1;
  }

  for (int   i=85;i<100;i++){
    table[75][i]=1;
    table[76][i]=1;
  }

  for (int   i=93;i<100;i++){
    table[105][i]=1;
    table[106][i]=1;
  }
}

void posEnemi(int posx,int posy){
  for(int i = posx-5;i<posx+5;i++){
    table[i][posy-5]=2;
  }
  for(int i = posx-5;i<posx+5;i++){
    table[i][posy+5]=2;
  }
  for(int i = posy-5;i<posy+5;i++){
    table[posx-5][i]=2;
  }
  for(int i = posy-5;i<posy+5;i++){
    table[posx+5][i]=2;
  }
}
void setup() {


  lidar.begin(Serial2, 128000);

  pinMode(YDLIDAR_MOTOR_SCTP, OUTPUT);
  pinMode(YDLIDAR_MOTRO_EN, OUTPUT);
  initTable();
  Serial.begin(9600);
  MsTimer2::set(10000,affichetab);
  MsTimer2::start();
}

void loop() {
    if(isScanning){
      if (lidar.waitScanDot() == RESULT_OK) {
          float distance = lidar.getCurrentScanPoint().distance; //distance value in mm unit
          float angle    = lidar.getCurrentScanPoint().angle; //anglue value in degrees
          if(((360-15)<angle||angle<=15)&&distance<300&&distance!=0){
            //Serial.println("robot devant capitaine! %");
            //Serial.println(distance, DEC);
            int posxenemi= distance*cos(angle);
            int posyenemi= distance*sin(angle);
            posEnemi(posxenemi,posyenemi);
                //Serial.println(table);
          }
      }else{
         //Serial.println(" YDLIDAR get Scandata failed!!");
      }
    }else{
        //stop motor

	    restartScan();
    }

}
