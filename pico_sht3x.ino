#include <Wire.h>
#include "ClosedCube_SHT31D.h"

ClosedCube_SHT31D sht3xd;

#include "SevSeg.h"
SevSeg sevseg;  //Instantiate a seven segment controller object

#include <Thread.h>
#include <ThreadController.h>

// ThreadController that will controll all threads
ThreadController controll = ThreadController();

//My Thread (as a pointer)
Thread* myThread = new Thread();
//His Thread (not pointer)
Thread hisThread = Thread();

// callback for myThread
void niceCallback(){
	printResult("Pooling Mode", sht3xd.readTempAndHumidity(SHT3XD_REPEATABILITY_LOW, SHT3XD_MODE_CLOCK_STRETCH, 1));

}

// callback for hisThread
void boringCallback(){
}


void setup() {
  Serial.begin(115200);
  delay(100);
  Wire.setClock(400000000);

  Wire.begin();//A4 A5

  Serial.println("ClosedCube SHT3X-D Single Shot Mode Example");
  Serial.println("supports SHT30-D, SHT31-D and SHT35-D");

  sht3xd.begin(0x44);  // I2C address: 0x44 or 0x45

  Serial.print("Serial #");
  Serial.println(sht3xd.readSerialNumber());

  byte numDigits = 3;                                       // 顯示器的位數，這裡設置為4位
  byte digitPins[] = { 15, 12, 11 };                        // 連接到七段顯示器數字引腳的Arduino引腳
  byte segmentPins[] = { 14, 10, 19, 17, 16, 13, 20, 18 };  // 連接到七段顯示器段引腳的Arduino引腳
  bool resistorsOnSegments = true;                         // 如果電阻連接在段引腳上，設置為'true'；若連接在數字引腳上，則設置為'false'
  byte hardwareConfig = COMMON_ANODE;                       // 顯示器硬體配置，這裡選擇共陽（COMMON_ANODE）；共陰選擇COMMON_CATHODE
  bool updateWithDelays = false;                            // 是否在更新顯示器時使用延遲，建議使用默認值'false'
  bool leadingZeros = false;                                // 是否保留前導零，若要保留，設置為'true'
  bool disableDecPoint = false;                             // 如果小數點不存在或未連接，設置為'true'以禁用小數點

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(250);
  sevseg.setNumber(000, 0);
  sevseg.refreshDisplay();  // Must run repeatedly

  	// Configure myThread
	myThread->onRun(niceCallback);
	myThread->setInterval(5000);

	// Configure myThread
	hisThread.onRun(boringCallback);
	hisThread.setInterval(0);

	// Adds both threads to the controller
	controll.add(myThread);
	controll.add(&hisThread); // & to pass the pointer to it
}

void loop() {
	controll.run();
  	  sevseg.refreshDisplay();  // Must run repeatedly

}
void printResult(String text, SHT31D result) {
  if (result.error == SHT3XD_NO_ERROR) {
    sevseg.setNumber(int(result.rh*10), 1);
    Serial.print(text);
		Serial.print(": T=");
		Serial.print(result.t);
		Serial.print("C, RH=");
		Serial.print(result.rh);
		Serial.println("%");
  } else {
    Serial.print(text);
    Serial.print(": [ERROR] Code #");
    Serial.println(result.error);
  }
}
/// END ///
