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


# 対応しているEPCリスト

管理しているプロパティを**echonet lite コンバータ**で表示すると下記の様な感じになります。


```JSON
 '192.168.2.20':
   { EOJs: [ 'ノードプロファイル01(0ef001)', 'ブザー01(02a001)' ],
     'ノードプロファイル01(0ef001)':
      { EPCs:
         [ '設置場所(81)',
           '自ノードインスタンスリストS(D6)',
           '状変アナウンスプロパティマップ(9D)',
           'Setプロパティマップ(9E)',
           'Getプロパティマップ(9F)' ],
        '設置場所(81)': '未設定(00)',
        '自ノードインスタンスリストS(D6)': 'ブザー01(0102A001)',
        '状変アナウンスプロパティマップ(9D)': '01[80](0180)',
        'Setプロパティマップ(9E)': '01[80](0180)',
        'Getプロパティマップ(9F)': '0e[80, 81, 82, 83, 88, 8a, 9d, 9e, 9f, d3, d4, d5, d6, d7](0E80818283888A9D9E9FD3D4D5D6D7)' },
     'ブザー01(02a001)':
      { EPCs:
         [ '動作状態(80)',
           '設置場所(81)',
           '規格Version情報(82)',
           '識別番号(83)',
           '状変アナウンスプロパティマップ(9D)',
           'Setプロパティマップ(9E)',
           'Getプロパティマップ(9F)' ],
        '動作状態(80)': 'ON(30)',
        '設置場所(81)': '未設定(00)',
        '規格Version情報(82)': 'Ver. K(00004B00)',
        '識別番号(83)': 'referSpec(FE0000000000000000000000000000000000)',
        '状変アナウンスプロパティマップ(9D)': '02[80, d6](0280D6)',
        'Setプロパティマップ(9E)': '01[e0](01E0)',
        'Getプロパティマップ(9F)': '09[80, 81, 82, 83, 88, 8a, 9d, 9e, 9f](0980818283888A9D9E9F)' } },
```

echonet lite コンバータ-: https://www.npmjs.com/package/echonet-lite-conv

# Version

- 1.3.0 profile object
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

