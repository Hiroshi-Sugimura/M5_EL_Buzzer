#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries
/*
  cd ~/Arduino/libraries
  git clone https://github.com/earlephilhower/ESP8266Audio
  git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/

#include <M5Stack.h>
#include <WiFi.h>
#include <EL.h>
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define WIFI_SSID "sugimulab"
#define WIFI_PASS "4428211065122"

WiFiClient client;
WiFiUDP elUDP;
EL echo(elUDP, EL_Buzzer, 0x01 );

void printNetData();

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

void setup()
{
  M5.begin();
  M5.Lcd.setBrightness(30);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("wifi connect start");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("wait...");
    delay(1000);
  }
  M5.Lcd.println("wifi connect ok");
  M5.update();

  printNetData();
  echo.begin();            // EL 起動シーケンス
  echo.details[0xB1] = new byte[2] {0x01, 0x41}; // 音発生設定

  // 一般照明の状態，繋がった宣言として立ち上がったことをコントローラに知らせるINFを飛ばす
  const byte deoj[] = {0x05, 0xff, 0x01};
  const byte edt[] = {0x01, 0x30};
  echo.sendMultiOPC1(deoj, EL_INF, 0x80, edt);

  // mp3 init
  out = new AudioOutputI2S(0, 1); // Output to builtInDAC
  out->SetOutputModeMono(true);
  // out->SetGain(0.03);
  mp3 = new AudioGeneratorMP3();
}

void draw() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(130, 200);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("[play]");
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

byte *pdcedt = nullptr; // テンポラリ

void loop()
{
  M5.update();

  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      stopPlaying();
    }
    return;
  } else {
    draw();
    delay(100);
  }

  // パケット貰ったらやる
  pdcedt = nullptr;

  if ( echo.read() ) // 0!=はなくてもよいが，Warning出るのでつけとく
  { // 受け取った内容読み取り，あったら中へ

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
                if (mp3->isRunning()) {
                  stopPlaying();
                }
                file = new AudioFileSourceSD("/sound1.mp3");
                id3 = new AudioFileSourceID3(file);
                mp3->begin(id3, out);
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

        // pdcedtを使ったらクリア
        if (pdcedt != nullptr)
        {
          delete[] pdcedt;
          pdcedt = nullptr;
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


  if (M5.BtnA.wasPressed())
  {
    if (mp3->isRunning()) {
      stopPlaying();
    }
    file = new AudioFileSourceSD("/incorrect1.mp3");
    id3 = new AudioFileSourceID3(file);
    mp3->begin(id3, out);
  }

  if (M5.BtnB.wasPressed())
  {
    if (mp3->isRunning()) {
      stopPlaying();
    }
    file = new AudioFileSourceSD("/tin1.mp3");
    id3 = new AudioFileSourceID3(file);
    mp3->begin(id3, out);
  }

  if (M5.BtnC.wasPressed())
  {
    if (mp3->isRunning()) {
      stopPlaying();
    }
    file = new AudioFileSourceSD("/pno-cs.mp3");
    id3 = new AudioFileSourceID3(file);
    mp3->begin(id3, out);
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
