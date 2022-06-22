#line 1 "/home/runner/work/ICSMeter/ICSMeter/include/net/Daemon.md"



```



           ______________                       DAEMON                                       HTTPD
          /              \-------------------------------------------------          ----------------------
   ______/   IC type ->  |       Wifi             |        Bluetooth       |        |     Always Serve     |
  /       \ _____________|-------------------------------------------------|        |----------------------|
  | Event |              |-------------------------------------------------|        |----------------------|
  |       |--------------|-------------------------------------------------|        |----------------------|
  |   |   |              |                                                 |        |                      |
  |   V   |              |                  (Wait for...)                  |        |     Homepage         |
  |       |        Start |       DHCP             |        PAIRING         |        |     Screenshots      |
  |       |              |                 -> Daemon Ready                 |        |     OTA Updater      |
  |       |              |                        |                        |        |                      |
  |       |--------------|-------------------------------------------------|         ----------------------
  |       |              |                                                 |
  |       |      Connect |  Python Proxy (HTTP)   |    Bluetooth Client    |
  |       |              |  -> postHTTP()         |    -> getTX()          |
  |       |              |  -> Subscribe WS       |    -> Create timers    |
  |       |              |  -> RX Ready           |    -> RX Ready         |
  |       |              |                        |                        |
  |       |--------------|-------------------------------------------------|
  |       |              |                                                 |
  |       |              |  onWSEvent                  onTimeInterval      |
  |       |         Poll |  ws->parse( packet )        bluetooth->query()  |
  |       |              |               -> Set CIV status                 |
  |       |              |                                                 |
  |       |--------------|-------------------------------------------------|
  |       |              |                                                 |
  |       |   Disconnect |                 Disable CIV client              |
  |       |              |  Reconnect attempt     |      Wait for pairing  |
  \_______/              |                        |                        |
           -------------- -------------------------------------------------



              CIV Client <========>     Daemon     <=========>  IC Unit

              Setup         =>          connect        =>       CIV_GET_TX
              Set status    <=          parse          <=       1c0001fd



```
















