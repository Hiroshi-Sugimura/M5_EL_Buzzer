////////////////////////////////////////////////////////////
// Buzzer with ECHONET Lite protocol
// Copyright (C) Hiroshi SUGIMURA 2018.11.16
////////////////////////////////////////////////////////////
#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries

#include <M5Stack.h>
#include <WiFi.h>
#include <EL.h>  // need install ECHONET Lite module from Library Manager
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define WIFI_SSID "testwifi2G"      // !!! change to yor wifi SSID
#define WIFI_PASS "0001223344"      // !!! change to yor wifi password

WiFiClient client;
WiFiUDP elUDP;
EL echo(elUDP, EL_Buzzer, 0x01 ); // EL_Buzzer = 0x02, 0xA0

int soundNumber;
char soundFilename[12]; // /sound0.mp3 \0
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;


////////////////////////////////////////////////////////////
// local functions (プロトタイプ宣言は必要ないけどIndexになるので)
void draw();
void stopPlaying();
void printNetData();


void draw() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(30, 180);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("[<<]   [sound");
  M5.Lcd.print(soundNumber);
  M5.Lcd.print("]   [>>]");
}

void stopPlaying()
{
  if (mp3) {
    mp3->stop();
  }
  if (id3) {
    id3->close();
    delete id3;
    id3 = nullptr;
  }
  if (file) {
    file->close();
    delete file;
    file = nullptr;
  }
}


// debug
void printNetData()
{
  Serial.println("-----------------------------------");

  // IP
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP  Address: ");
  Serial.println(ip);

  IPAddress dgwip = WiFi.gatewayIP();
  Serial.print("DGW Address: ");
  Serial.println(dgwip);

  IPAddress smip = WiFi.subnetMask();
  Serial.print("SM  Address: ");
  Serial.println(smip);

  Serial.println("-----------------------------------");
}


////////////////////////////////////////////////////////////
// arduino: initialize
void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(30);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("wifi connect start");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  M5.Lcd.println("WiFi wait");
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    Serial.print(".");
    delay(1000);
  }
  M5.Lcd.println("wifi connect ok");
  M5.update();

  printNetData();
  echo.begin();            // EL 起動シーケンス
  echo.details[0x9E] = new byte[3] {0x02, 0x01, 0xE0}; // change set property map
  echo.details[0xB1] = new byte[2] {0x01, 0x41}; // 音発生設定

  // 機器の状態，繋がった宣言として立ち上がったことをコントローラに知らせるINFを飛ばす
  const byte deoj[] = {0x05, 0xff, 0x01};
  const byte edt[]  = {0x01, 0x30}; // power on
  echo.sendMultiOPC1(deoj, EL_INF, 0x80, edt);

  // mp3 init
  soundNumber = 1;
  strcpy( soundFilename, "/sound1.mp3");

  out = new AudioOutputI2S(0, 1); // Output to builtInDAC
  out->SetOutputModeMono(true);
  mp3 = new AudioGeneratorMP3();
}


// arduino: mainloop
void loop()
{
  M5.update();

  // 音が鳴っているとき
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      stopPlaying();
    }
    return;  // 音が鳴っている間は操作を受け付けないことにする。
  } else {
    draw();
    delay(100);
  }
  // WiFiが調子悪い
  if ( WiFi.status() != WL_CONNECTED) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("WiFi wait");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      M5.Lcd.print(".");
      Serial.print(".");
      delay(1000);  // 接続するまで別の処理で邪魔しない
    }
  }

  if ( echo.read() )  // 受け取った内容読み取り，あったら中へ
  {

    // -----------------------------------
    // ESVがSETとかGETとかで動作をかえる
    switch (echo._rBuffer[EL_ESV])
    {
      // -----------------------------------
      // 動作状態の変更 Set対応
      case EL_SETI:
      case EL_SETC:
        switch (echo._rBuffer[EL_EPC])
        {
          case 0x80: // 電源
            // 電源変更なし。ONのまま。
            break;

          case 0xE0: //ブザー音種別設定, 0x31--0x38
            switch ( echo._rBuffer[EL_EDT] ) {
              case 0x31:
              case 0x32:
              case 0x33:
              case 0x34:
              case 0x35:
              case 0x36:
              case 0x37:
              case 0x38:
                soundNumber = echo._rBuffer[EL_EDT] - 0x30;
                if (mp3->isRunning()) {
                  stopPlaying();
                }
                sprintf(soundFilename, "/sound%d.mp3", soundNumber);
                file = new AudioFileSourceSD(soundFilename);
                id3 = new AudioFileSourceID3(file);
                mp3->begin(id3, out);
                echo.details[ 0xE0 ] = new byte[2] {0x01, soundNumber}; // power
                break;
              default:
                break;
            }
            break;

          default: // 不明なEPC
            M5.Lcd.print("??? packet esv, epc, edt is : ");
            // set
            // ESV, EPC, EDT
            M5.Lcd.print(echo._rBuffer[EL_ESV], HEX);
            M5.Lcd.print(" ");
            M5.Lcd.print(echo._rBuffer[EL_EPC], HEX);
            M5.Lcd.print(" ");
            M5.Lcd.println(echo._rBuffer[EL_EDT], HEX);
            break;
        }

        if (echo._rBuffer[EL_ESV] == EL_SETC)
        { // SETCなら返信必要
          echo.returner();
        }
        break; // SETI, SETCここまで

      // -----------------------------------
      // Get,INF_REQ対応
      // SETの時にきちんとupdate関数でECHONET Liteの状態変更をライブラリに教えておけばここは簡素になる
      case EL_GET:
      case EL_INF_REQ:
        // update関数でdetailsに状態が登録されていれば自動で返信する
        echo.returner();
        break; // GetとINF_REQここまで

      case EL_INF:
        break;

      default: // 解釈不可能なESV
        M5.Lcd.print("error? ESV = ");
        Serial.println(echo._rBuffer[EL_ESV]);
        break;
    }
  }

  // sound check
  if (M5.BtnA.wasPressed())  // prev sound number
  {
    if (soundNumber == 1) {
      soundNumber = 8;
    } else {
      soundNumber -= 1;
    }
  }

  if (M5.BtnB.wasPressed())  // play sound
  {
    if (mp3->isRunning()) {
      stopPlaying();
    }
    sprintf( soundFilename, "/sound%d.mp3", soundNumber);
    Serial.print("filename: ");
    Serial.println(soundFilename);
    file = new AudioFileSourceSD(soundFilename);
    id3 = new AudioFileSourceID3(file);
    mp3->begin(id3, out);
  }

  if (M5.BtnC.wasPressed()) // next sound number
  {
    if (soundNumber == 8) {
      soundNumber = 1;
    } else {
      soundNumber += 1;
    }
  }
}

////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////
