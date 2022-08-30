# UDP_Controller
这是一个使用UDP协议遥控windows的小工具，运行后会在windows上开启一个UDP服务端，用于接收任意客户端发来的UDP数据包来在windows上触发相应的操作。
目前只支持两种操作： 
1. 发送字符串 **shutdown** 来遥控windows关机； 
2. 发送除 **shutdown** 以外的字符串会复制到windows的剪切板。

运行前需传入局域网IP地址和端口号，具体请看[X64/startup.bat](https://gitee.com/wuyesuifeng/UDPController/blob/master/X64/startup.bat)。