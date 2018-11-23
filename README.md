# M5_EL_Buzzer
ECHONET Lite Buzzer class with M5Stack


# install

- Library install

```
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
```

- LibraryマネージャからEL_dev_arduinoをインストール

- sound[1-8].mp3をmicroSDにコピーする。

- 下記の定義を自分のWiFi環境に変更する。

```
15 | #define WIFI_SSID "your SSID"
16 | #define WIFI_PASS "your WiFi password"
```


# 動作テスト

・0x02,0xa0, 0x01　にたいして，SETIかSETCの0xe0で0x31から0x38 を受け付けます。
・音は0x31がsound1.mp3・・・0x38がsound8.mp3に対応しています。



# Version

- 1.0.1 Readme, refactoring
- 1.0.0 M5Stack
- 0.1.0 commit and publish


# 使用したフリー音源

- soundsは下記のフリー音源を利用しています。
 - http://taira-komori.jpn.org/freesound.html


# Author

- 神奈川工科大学 創造工学部 ホームエレクトロニクス開発学科
 - Dept. of Home Electronics, Faculty of Creative Engineering, Kanagawa Institute of Technology.

- 杉村　博
 - SUGIMURA, Hiroshi

