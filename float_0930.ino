#define _OPEN_SYS_ITOA_EXT  // itoa
#include <stdlib.h>
#include <stdio.h> 


// セットアップ
void setup() {
  // Arduinoシリアルモニタ用
  Serial.begin(115200);
  // PLC用（セットデータ=7, パリティ=0, ストップビットの設定=1）
  Serial1.begin(9600,SERIAL_7O1);   
  
  // 接続完了
  Serial.println("");               // 改行
  Serial.println("Hello World!");   // Arduino
  Serial1.println("");               // 改行
  Serial1.println("Hello World!");  // PLC用
}

// ループ
void loop() {
  float testdata = 23.45;   // 速度データ(仮)
    start(testdata);
}

void start(float testdata){
  int size;       // WRの配列の大きさ
  char* buffer;   // 変換したWRの文字列
  
  int m_count = 0;
  char m;
  char m_box;
  String m_key;       // M8000 読み出した受信データを格納する
  
  int y_flag;       // フラグ
  int y_count = 0;
  char y;
  char y_box;
  String y_key;     // Y0    読み出した受信データを格納する
    

  // BRコマンド  プログラム起動確認 M8000
  char BR_M8000[256] = {  // ENG  00  FF  BR  1  M8000  01
                    '','0','0','F','F','B','R','1','M','8','0','0','0','0','1'};

  // BRコマンド  非常停止確認 Y0  
  char BR_Y0[256] = {  // ENG  00  FF  BR  1  Y0000  01  0001
                    '','0','0','F','F','B','R','1','Y','0','0','0','0','0','1'};

  // WWコマンド　レジスタで4桁書き込み D100
  char WW[256] = {  // ENG  00  FF  WW  1  D0100  0001  0004
                    '','0','0','F','F','W','W','1','D','0','1','0','0','0','1','0','0','0','4'};
                    
  // WRコマンド　WWで書き込んだ値の読み取り D200
  char WR[256] = {  //  ENG  00  FF  WR  1  D0200  0001
                    '','0','0','F','F','W','R','1','D','0','2','0','0','0','1'};
  
  // プログラム起動確認
  Serial.write(BR_M8000,(size_t)256);   // PLCにBR_M8000コマンドの送信
  Serial.print(" ");                // コマンドと戻り値の間の空白(区切り)
  Serial.println("");               // 改行
  // プログラム起動確認_PLC
  Serial1.write(BR_M8000,(size_t)256);   // PLCにBR_M8000コマンドの送信   
  Serial1.print(" ");                // コマンドと戻り値の間の空白(区切り)
  Serial1.println("");               // 改行
  
  if ( Serial1.available() ) {        // 受信データがあるか？
    // M8000 受信データ
    m_key = Serial1.readString();     // シリアル バッファから文字列に読み取ります。タイムアウトした場合、関数は終了します。
    m_key.trim();                      // シリアルデータを待機する最大ミリ秒を設定
    Serial.print( m_key );            // 1文字送信。受信データをそのまま送り返す。
    Serial.println("");               // 改行
  }     
  // M8000 受信データなし
  if(m_key.charAt(5) == 0){
    delay(100);     // 時間待ち
  }

  // 非常停止確認
  Serial.write(BR_Y0,(size_t)256);   // PLCにBR_M8000コマンドの送信
  Serial1.print(" ");                // コマンドと戻り値の間の空白(区切り)
  Serial1.println("");               // 改行
  // プログラム起動確認_PLC
  Serial1.write(BR_Y0,(size_t)256);   // PLCにBR_M8000コマンドの送信   
  Serial1.print(" ");                // コマンドと戻り値の間の空白(区切り)
  Serial1.println("");               // 改行

  if ( Serial1.available() ) {        // 受信データがあるか？
    // Y0 受信データ
    y_key = Serial1.readString();     // シリアル バッファから文字列に読み取ります。タイムアウトした場合、関数は終了します。
    y_key.trim();                      // シリアルデータを待機する最大ミリ秒を設定
    Serial.print( y_key );            // 1文字送信。受信データをそのまま送り返す。
    Serial.println("");               // 改行
  }
    
  // 非常停止 ON
  if(y_key.charAt(5) == 0){
     y_flag = 1;    // 非常停止フラグ ON
   }  
  // 非常停止 OFF
  else{
    y_flag = 0;     // 非常停止フラグ OFF
  }

  Serial.print( y_flag );            // 1文字送信。受信データをそのまま送り返す。
  Serial.println("");               // 改行

  // 送信データ
  if(y_flag == 0) {
    Serial1.println("");              // 改行
    buffer = float1(testdata);         // float1(testdata)関数呼び出し
    bit_select(WW, buffer);            // bit_select関数呼び出し(WWのコマンド,変換したWWの文字列)
    size = sizeof(WR);                 // WRの配列の大きさを求める
    
    // PLC TeraTerm用 Serial1
    Serial1.write(WR,(size_t)256);   // PLCにWRコマンドの送信
    Serial1.print(" ");               // コマンドと戻り値の間の空白(区切り)
    // Arduinoシリアルモニタ用 Serial
    Serial.write(WR,(size_t)256);    // PLCにWRコマンドの送信
    Serial.print(" ");                // コマンドと戻り値の間の空白(区切り)
    Serial.println("");               // 改行
  }
}

//速度データを文字列の変換
char* float1(float testdata){
    int result;     // ×100したint型の速度データ
    char* buffer;   // 動的メモリの確保

    buffer = (char*)malloc(sizeof(char) * 4);   // 動的メモリの確保
    result = (double) testdata * 100;           // double型×100＝int（暗黙）
    itoa (result,buffer,DEC);                   // intを文字列変換

    return buffer;    // 速度データの文字列
}

// WWコマンドの書き換え,送信
void bit_select(char com[],char buffer[]) {
  int databit;
  int i = 0;

  databit = 15;   // WWコマンドの15文字目

  for(i=0; i<5; i++){             //bitの書き込み　4つの書き込みするためのループ
    com[i + databit] = buffer[i];   //WWコマンドを15文字目から速度データを変換
  }
  
  // PLC TeraTerm用 Serial1
  Serial1.write(com,(size_t)256);   //PLCにWWコマンドの送信
  Serial1.print(" ");               // コマンドと戻り値の間の空白(区切り)
  Serial1.println("");              // 改行
  
  // Arduinoシリアルモニタ用 Serial
  Serial.write(com,(size_t)256);   //PLCにWWコマンドの送信
  Serial.print(" ");                // コマンドと戻り値の間の空白(区切り)
  Serial.println("");               // 改行

}


