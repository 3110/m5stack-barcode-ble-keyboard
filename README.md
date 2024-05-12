# BLEキーボードとして接続するバーコードリーダ

[Unit QR Code](https://docs.m5stack.com/en/unit/Unit-QRCode)を接続した[ATOM Lite](https://docs.m5stack.com/en/core/ATOM%20Lite)をBLEキーボードとして接続し，読み込んだバーコードの情報をキー入力します。

BLEキーボードとして接続するとLEDが緑色に光ります。

ボタンAを押すと，バーコードの情報をキー入力した後にリターンキーを入力するかどうかを設定できます。
ボタンAを押すたびに改行を入力する（LEDが黄色に光る）・入力しない（LEDが緑色に光る）が切り替わります。

## 参考

* [UART COMMAND](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/products/unit/Unit-QRCode/AT%20COMMAND.pdf)
* [I2C Protocol](https://github.com/m5stack/M5Unit-QRCode/blob/main/docs/Unit_QRCode_I2C_Protocol.pdf)
