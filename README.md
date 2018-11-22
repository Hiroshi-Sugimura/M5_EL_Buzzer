# M5_EL_Buzzer
ECHONET Lite Buzzer class with M5Stack


# install

- Library install

```
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
```

- sound[1-8].mp3をmicroSDにコピーする


# 動作テスト

・0x02,0xa0, 0x01　にたいして，SETIかSETCの0xe0で0x31から0x38　を受け付けます。
・音は0x31がsound1.mp3・・・0x38がsound8.mp3に対応しています。


# 使用したフリー音源

soundsは下記のフリー音源を利用しています。
http://taira-komori.jpn.org/freesound.html

