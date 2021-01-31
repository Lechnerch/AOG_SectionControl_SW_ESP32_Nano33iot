
// WIFI handling 31. Jan 2021 for ESP32  -------------------------------------------

void WiFi_handle_connection() {
    if (WiFi_connect_step > 0) {
        IPAddress gwIP, myIP;
        now = millis();

        //1-9 to ping or check connection and disconnect, here not used
        if (WiFi_connect_step < 10) { WiFi_connect_step = 10; }

        if (now > (WiFi_connect_timer + 500)) {
            //do every half second
            if (Set.debugmode) { Serial.print("WiFi_connect_step: "); Serial.println(WiFi_connect_step); }
            switch (WiFi_connect_step) {
                /*     case 1:
                         if (Ping.ping(Set.WiFi_gwip)) { //retry to connect NTRIP, WiFi is available
                             WiFi_connect_timer = 0;
                             Ntrip_restart = 1;
                             WiFi_connect_step = 0;
                             if ((Set.NtripClientBy == 2) && (!task_NTRIP_running)) {
                                 {
                                     xTaskCreatePinnedToCore(NTRIPCode, "Core1", 3072, NULL, 1, &Core1, 1);
                                     delay(500);
                                 }
                             }
                         }
                         else { WiFi_connect_timer = now; WiFi_connect_step++; }
                         break;
                     case 2:
                         WiFi_netw_nr = 0;
                         WiFi_Ntrip_cl.stop();
                         WiFi_connect_step++;
                         WiFi_connect_timer = now;
                         break;
                     case 3:WiFi.mode(WIFI_OFF);
                         WiFi_connect_step = 10;
                         WiFi_connect_timer = now;
                         break;
                         */
            case 10:
                WiFi_netw_nr = 0;
                WiFi_network_search_timeout = 0;
                WiFi_connect_tries = 0;
                WebIORunning = false;
                UDP_running = false;
#if HardwarePlatform == 0  //ESP32  
                WiFi.mode(WIFI_STA);   //  Workstation
#endif
                WiFi_connect_step++;
                WiFi_connect_timer = now;
                break;
            case 11:
                if (WiFi_network_search_timeout == 0) {   //first run                 
                    WiFi_network_search_timeout = now + (Set.timeoutRouter * 1000);
                }
                WiFi_scan_networks();
                //timeout?
                if (now > WiFi_network_search_timeout) { WiFi_connect_step = 50; }
                else {
                    if (WiFi_netw_nr > 0) {
                        //found network
                        WiFi_connect_step++;
                        WiFi_network_search_timeout = 0;//reset timer
                    }
                }
                WiFi_connect_timer = now;
                break;

            case 12:
                WiFi_STA_connect_network();
                if (WiFi_network_search_timeout == 0) {   //first run                   
                    WiFi_network_search_timeout = now + (Set.timeoutRouter * 500);//half time
                }
                if (WiFi.status() != WL_CONNECTED) {
                    Serial.print(".");
                    if (now > WiFi_network_search_timeout) {
                        //timeout
                        WiFi_connect_step = 19;//close WiFi and try again
                        WiFi_connect_tries++;
                        WiFi_network_search_timeout += (Set.timeoutRouter * 500);//add rest of time
                    }
                }
                else {
                    //connected
                    WiFi_connect_step++;
                    WiFi_network_search_timeout = 0;//reset timer
                }
                WiFi_connect_timer = now;
                break;

            case 13:               
                WiFi_connect_step++; //need to wait to get correct IP
                WiFi_connect_timer = now;
                break;

            case 14:
                //connected
                Serial.println();
                Serial.println("WiFi Client successfully connected");
                Serial.print("Connected IP - Address : ");
                myIP = WiFi.localIP();
                Serial.println(myIP);
                //after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
                myIP[3] = Set.WiFi_myip[3]; //set ESP32 IP to x.x.x.myIP_ending
                Serial.print("changing IP to: ");
                Serial.println(myIP);
                gwIP = WiFi.gatewayIP();
#if HardwarePlatform == 0  //ESP32 
                if (!WiFi.config(myIP, gwIP, Set.mask, gwIP)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1  //nano 33iot
                WiFi.config(myIP, gwIP, gwIP, Set.mask);
#endif
                WiFi_connect_step++;
                WiFi_connect_timer = now;
                break;

            case 15:
                Serial.print("Connected IP - Address : ");
                myIP = WiFi.localIP();
                WiFi_ipDestination = myIP;
                WiFi_ipDestination[3] = 255;
                Serial.println(myIP);
                gwIP = WiFi.gatewayIP();
                Serial.print("Gateway IP - Address : ");
                Serial.println(gwIP);
                Set.WiFi_ipDestination[0] = myIP[0];
                Set.WiFi_ipDestination[1] = myIP[1];
                Set.WiFi_ipDestination[2] = myIP[2];
                Set.WiFi_ipDestination[3] = 255;//set IP to x.x.x.255 according to actual network
                my_WiFi_Mode = 1;// WIFI_STA;
                WiFi_connect_step = 20;
                WiFi_connect_timer = now;
                break;

            case 19://no connection at first try, try again
#if HardwarePlatform == 0  //ESP32  
                WiFi.disconnect();
#endif
#if HardwarePlatform == 1  //nano 33iot 
                WiFi.end();
#endif
                
                if (WiFi_connect_tries > 1) {//start access point
                    WiFi_connect_step = 50;
                    Serial.println();
                    Serial.println("error connecting to WiFi network");
                }
                else { WiFi_connect_step = 12; Serial.print("-"); }
                WiFi_connect_timer = now;
                break;

            case 20://init WiFi UDP sendng to AOG
                if (UDPFromAOG.begin(Set.PortSCToAOG))
                {
                    Serial.print("UDP writing to IP: ");
                    Serial.println(WiFi_ipDestination);
                    Serial.print("UDP writing to port: ");
                    Serial.println(Set.PortDestination);
                    Serial.print("UDP writing from port: ");
                    Serial.println(Set.PortSCToAOG);
                }
                WiFi_connect_step++;
                WiFi_connect_timer = now;
                break;

            case 21:
                //init WiFi UPD listening to AOG 
                if (UDPFromAOG.begin(Set.PortFromAOG))
                {
                    Serial.print("Section Control UDP Listening to port: ");
                    Serial.println(Set.PortFromAOG);
                    Serial.println();
                }
                delay(5);
                UDP_running = true;
                WiFi_connect_step = 100;
                WiFi_connect_timer = now;
                break;

            case 50://start access point
                WiFi_Start_AP();
                WiFi_connect_step = 100;
                WiFi_connect_timer = now;
                break;

            case 100:
                //start Server for Webinterface
                StartServer();
                WiFi_connect_step++;
                WiFi_connect_timer = now;
                break;

            case 101:
                WebIORunning = true;
                WebIOTimeOut = millis() + (long(Set.timeoutWebIO) * 60000);
                WiFi_connect_step = 0;
                WiFi_connect_timer = 0;
                break;

            default:
                WiFi_connect_step++;
                break;
            }
        }
    }
}





//---------------------------------------------------------------------
// scanning for known WiFi networks

void WiFi_scan_networks()
{
    Serial.println("scanning for WiFi networks");
    // WiFi.scanNetworks will return the number of networks found
    int WiFi_num_netw_inReach = WiFi.scanNetworks();
    Serial.print("scan done: ");
    if (WiFi_num_netw_inReach == 0) {
        Serial.println("no networks found");        
    }
    else
    {
        Serial.print(WiFi_num_netw_inReach);
        Serial.println(" network(s) found");
        for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
            Serial.println("#" + String(i + 1) + " network : " + WiFi.SSID(i));
        }
        delay(800);//.SSID gives no value if no delay
        delay(500);

        for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
            if (WiFi.SSID(i) == Set.ssid1) {
                // network found in list
                Serial.println("Connecting to: " + WiFi.SSID(i));
                WiFi_netw_nr = 1;
                break;
            }
        }
        if ((WiFi_netw_nr == 0) && (Set.ssid2 != "")) {
            for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
                if (WiFi.SSID(i) == Set.ssid2) {
                    // network found in list
                    Serial.println("Connecting to: " + WiFi.SSID(i));
                    WiFi_netw_nr = 2;
                    break;
                }
            }
        }
        if ((WiFi_netw_nr == 0) && (Set.ssid3 != "")) {
            for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
                if (WiFi.SSID(i) == Set.ssid3) {
                    // network found in list
                    Serial.println("Connecting to: " + WiFi.SSID(i));
                    WiFi_netw_nr = 3;
                    break;
                }
            }
        }
        if ((WiFi_netw_nr == 0) && (Set.ssid4 != "")) {
            for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
                if (WiFi.SSID(i) == Set.ssid4) {
                    // network found in list
                    Serial.println("Connecting to: " + WiFi.SSID(i));
                    WiFi_netw_nr = 4;
                    break;
                }
            }
        }
        if ((WiFi_netw_nr == 0) && (Set.ssid5 != "")) {
            for (int i = 0; i < WiFi_num_netw_inReach; ++i) {
                if (WiFi.SSID(i) == Set.ssid5) {
                    // network found in list
                    Serial.println("Connecting to: " + WiFi.SSID(i));
                    WiFi_netw_nr = 5;
                    break;
                }
            }
        }
    }
}  //end WiFi_scan_networks()

//-------------------------------------------------------------------------------------------------
//connects to WiFi network

void WiFi_STA_connect_network() {//run WiFi_scan_networks first
    switch (WiFi_netw_nr) {
    case 1: WiFi.begin(Set.ssid1, Set.password1); break;
    case 2: WiFi.begin(Set.ssid2, Set.password2); break;
    case 3: WiFi.begin(Set.ssid3, Set.password3); break;
    case 4: WiFi.begin(Set.ssid4, Set.password4); break;
    case 5: WiFi.begin(Set.ssid5, Set.password5); break;
    }   
    if (WiFi_connect_timer == 0) { WiFi.config(0U, 0U, 0U); } //set IP to DHCP on first run. call immediately after begin!
}
/*       //WIFI LED blink in double time while connecting
        if (!LED_WIFI_ON) {
            if (millis() > (LED_WIFI_time + (LED_WIFI_pause >> 2)))
            {
                LED_WIFI_time = millis();
                LED_WIFI_ON = true;
                digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);
            }
        }
        if (LED_WIFI_ON) {
            if (millis() > (LED_WIFI_time + (LED_WIFI_pulse >> 2))) {
                LED_WIFI_time = millis();
                LED_WIFI_ON = false;
                digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
            }
        }*/

//-------------------------------------------------------------------------------------------------
// start WiFi Access Point = only if no existing WiFi or connection fails

//ESP32
#if HardwarePlatform == 0 
void WiFi_Start_AP() {
    WiFi.mode(WIFI_AP);   // Accesspoint
    WiFi.softAP(Set.ssid_ap, "");
    delay(5);
    while (!SYSTEM_EVENT_AP_START) // wait until AP has started
    {
        delay(100);
        Serial.print(".");
    }
    delay(150);//right IP adress only with this delay 
    WiFi.softAPConfig(Set.WiFi_gwip, Set.WiFi_gwip, Set.mask);  // set fix IP for AP  
    delay(300);
    IPAddress myIP = WiFi.softAPIP();
    //AP_time = millis();
    Serial.print("Accesspoint started - Name : ");
    Serial.println(Set.ssid_ap);
    Serial.print(" IP address: ");
    WiFi_ipDestination = myIP;
    Serial.println(WiFi_ipDestination);
    WiFi_ipDestination[3] = 255;
    my_WiFi_Mode = WIFI_AP;
}
#endif


// AccessPoint on Nano 33 IoT
#if HardwarePlatform == 1
void WiFi_Start_AP() {
    WiFi.end();
    delay(500);
    delay(300);

    WiFi.config(Set.WiFi_gwip, Set.WiFi_gwip, Set.WiFi_gwip, Set.mask);// set fix IP for AP  
    delay(300);  //right IP adress only with this delay
    byte my_Wifi_mode = WiFi.beginAP(Set.ssid_ap);
    delay(500);
    delay(300);
    Serial.print("status of WiFi AP: ");
    Serial.println(my_Wifi_mode);

    IPAddress myIP = WiFi.localIP();
    //AP_time = millis();
    Serial.print("Accesspoint started - Name : ");
    Serial.println(Set.ssid_ap);
    Serial.print(" IP address: ");
    WiFi_ipDestination = myIP;
    Serial.println(WiFi_ipDestination);
    WiFi_ipDestination[3] = 255;//set IP to x.x.x.255 according to actual network
    my_WiFi_Mode = WIFI_AP;
}
#endif


//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
/*
void Eth_Start() {
    Ethernet.init(Set.Eth_CS_PIN);
    delay(50);
    Ethernet.begin(mac);
    delay(200);
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("no Ethernet hardware");
    }
    else {
        Serial.println("Ethernet hardware found, checking for connection");
        if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println("Ethernet cable is not connected.");
        }
        else {
            Serial.println("Ethernet status OK");
            Serial.print("Got IP ");
            Serial.println(Ethernet.localIP());
            for (byte n = 0; n < 3; n++) {
                Set.Eth_myip[n] = Ethernet.localIP()[n];
                Eth_ipDestination[n] = Ethernet.localIP()[n];
            }
            Eth_ipDestination[3] = 255;
            Ethernet.setLocalIP(Set.Eth_myip);
            delay(100);
            Serial.print("changed IP to ");
            Serial.println(Ethernet.localIP());
            Ethernet_running = true;
            //init UPD Port sending to AOG
            if (Eth_udpRoof.begin(Set.portMy))
            {
                Serial.print("Ethernet UDP sending from port: ");
                Serial.println(Set.portMy);
            }
            delay(50);
        }
    }
    Serial.println();
}

*/
//-------------------------------------------------------------------------------------------------





/*
// WIFI handling 16. Maerz 2020 for ESP32 and Nano 33 IoT -------------------------------------------


//here supporting 2 WiFi networks, so set number of network!!

void WiFi_STA_connect_network() {
  unsigned long timeout, timeout2;
#if HardwarePlatform == 0  //ESP32  
  WiFi.mode(WIFI_STA);   //  Workstation
#endif
  if (NetWorkNum == 2) {
	  WiFi.begin(Set.ssid2, Set.password2); 
      timeout = millis() + (Set.timeoutRouter2 * 1000); 
      timeout2 = timeout - (Set.timeoutRouter2 * 500);
      Serial.print("try to connect to WiFi: "); Serial.println(Set.ssid2);
  }
  else {
	  Serial.print("try to connect to WiFi: "); Serial.println(Set.ssid);
      WiFi.begin(Set.ssid, Set.password); 
      timeout = millis() + (Set.timeoutRouter * 1000); 
      timeout2 = timeout - (Set.timeoutRouter * 500);
  }
  while (WiFi.status() != WL_CONNECTED && millis() < timeout) {
      delay(200);//300
      Serial.print(".");
      if ((millis() > timeout2) && (WiFi.status() != WL_CONNECTED)) {
#if HardwarePlatform == 0  //ESP32  
          WiFi.disconnect();
#endif
#if HardwarePlatform == 1  //nano 33iot 
          WiFi.end();
#endif
          delay(200);
          if (NetWorkNum == 2) { WiFi.begin(Set.ssid2, Set.password2); }
          else { WiFi.begin(Set.ssid, Set.password); }
          timeout2 = timeout + 300;
      }
      //WIFI LED blink in double time while connecting
      WiFi_LED_blink(2);

      //switches -> set relais
      if ((Set.SectSWInst) || (Set.SectMainSWType != 0)) {
          SectSWRead();
          SetRelays();
      }
      //Rate switches and motor drive
      if ((Set.RateSWLeftInst == 1) || (Set.RateSWRightInst == 1)) { RateSWRead(); }
      if (Set.RateControlLeftInst == 0) { motorDrive(); } //if Manual do everytime, not only in timed loop
  }  //connected or timeout  
  
  Serial.println(""); //NL  
  if (WiFi.status() == WL_CONNECTED)
  {
      delay(300);  
      Serial.println();
      Serial.print("WiFi Client successfully connected to : ");
      if (NetWorkNum == 2) { Serial.println(Set.ssid2); } else { Serial.println(Set.ssid); }
      Serial.print("Connected IP - Address : ");
      IPAddress myIP = WiFi.localIP();
      Serial.println(myIP);
      IPAddress gwip = WiFi.gatewayIP();
      //after connecting get IP from router -> change it to x.x.x.IP Ending (from settings)
      if (myIP[3] != Set.myIPEnding) {
          myIP[3] = Set.myIPEnding; //set ESP32 IP to x.x.x.myIP_ending
          Serial.print("changing IP to: ");
          Serial.println(myIP);
#if HardwarePlatform == 0  //ESP32 
          if (!WiFi.config(myIP, gwip, Set.mask, gwip)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1  //nano 33iot
          WiFi.config(myIP, gwip, gwip, Set.mask);
#endif
          delay(200);
          delay(100);
          Serial.print("Connected IP - Address : ");
          myIP = WiFi.localIP();
      }
      Set.myip[0] = myIP[0];
      Set.myip[1] = myIP[1];
      Set.myip[2] = myIP[2];
      Set.myip[3] = myIP[3];
      Serial.println(myIP);
      Serial.print("Gateway IP - Address : ");
      Serial.println(gwip);
      Set.gwip[0] = gwip[0];
      Set.gwip[1] = gwip[1];
      Set.gwip[2] = gwip[2];
      Set.gwip[3] = gwip[3];
      Set.IPToAOG[0] =myIP[0];
      Set.IPToAOG[1] = myIP[1];
      Set.IPToAOG[2] = myIP[2];
      Set.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network
      LED_WIFI_ON = true;
      digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
      my_WiFi_Mode = 1;// WIFI_STA;
  }
  else
  {
     // WiFi.end();
      Serial.println("WLAN-Client-Connection failed");
      Serial.println();
      LED_WIFI_ON = false;
      digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);
  }
  delay(20);
}


//-------------------------------------------------------------------------------------------------
// start WiFi Access Point = only if no existing WiFi

//ESP32
#if HardwarePlatform == 0 
void WiFi_Start_AP() {
  WiFi.mode(WIFI_AP);   // Accesspoint
  delay(5);
  WiFi.softAP(Set.ssid_ap, "");
  while (!SYSTEM_EVENT_AP_START) // wait until AP has started
   {
    delay(100);
    Serial.print(".");
   }   
  delay(200);//right IP adress only with this delay 
  WiFi.softAPConfig(Set.gwip, Set.gwip, Set.mask);  // set fix IP for AP  
  delay(300);//right IP adress only with this delay 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Accesspoint started - Name : ");
  Serial.println(Set.ssid_ap);
  Serial.print( " IP address: ");
  Serial.println(myIP);
  Set.IPToAOG[0] = myIP[0];
  Set.IPToAOG[1] = myIP[1];
  Set.IPToAOG[2] = myIP[2];
  Set.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network
  Set.myip[0] = myIP[0];
  Set.myip[1] = myIP[1];
  Set.myip[2] = myIP[2];
  Set.myip[3] = myIP[3];

  LED_WIFI_ON = true;
  digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
  my_WiFi_Mode = WIFI_AP;
}
#endif

// AccessPoint on Nano 33 IoT

#if HardwarePlatform == 1
void WiFi_Start_AP() {
    WiFi.end();
    delay(500);
    delay(300);

    WiFi.config(Set.gwip, Set.gwip, Set.gwip, Set.mask);// set fix IP for AP  
    delay(300);  //right IP adress only with this delay
    byte my_Wifi_mode = WiFi.beginAP(Set.ssid_ap);
    delay(500);
    delay(300);
    Serial.print("status of WiFi AP: ");
    Serial.println(my_Wifi_mode);
    IPAddress myip = WiFi.localIP();
    // delay(300);
    Serial.print("Accesspoint started - Name : ");
    Serial.println(Set.ssid_ap);
    Serial.print(" IP address: ");
    Serial.println(myip);
    Set.myip[0] = myip[0];
    Set.myip[1] = myip[1];
    Set.myip[2] = myip[2];
    Set.myip[3] = myip[3];
    Set.IPToAOG[0] = myip[0];
    Set.IPToAOG[1] = myip[1];
    Set.IPToAOG[2] = myip[2];
    Set.IPToAOG[3] = 255;//set IP to x.x.x.255 according to actual network    

    myip = WiFi.gatewayIP();
    Serial.print("Gateway IP - Address : ");
    Serial.println(myip);
    Set.gwip[0] = myip[0];
    Set.gwip[1] = myip[1];
    Set.gwip[2] = myip[2];
    Set.gwip[3] = myip[3];
    delay(50);

    LED_WIFI_ON = true;
    digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
    my_WiFi_Mode = 2;
}
#endif


//-------------------------------------------------------------------------------------------------
void UDP_Start()
{   //init UPD
    if (UDPToAOG.begin(Set.PortToAOG))
    {
        Serial.print("UDP sendig to IP: ");
        for (byte n = 0; n < 4; n++) {
            Serial.print(Set.IPToAOG[n]);
            Serial.print(".");
        }
        Serial.print(" from port: ");
        Serial.print(Set.PortToAOG);
        Serial.print(" to port: ");
        Serial.println(Set.PortDestination);
    }
    delay(300);
    if (UDPFromAOG.begin(Set.PortFromAOG))
    {
        Serial.print("UDP listening for AOG data on IP: ");
        for (byte n = 0; n < 4; n++) {
            Serial.print(Set.myip[n]);
            Serial.print(".");
        }
        Serial.print(" on port: ");
        Serial.println(Set.PortFromAOG);
        UDP_running = true;
        //getAOGSteerData();
    }
}
*/

//-------------------------------------------------------------------------------------------------

/*
void WiFi_connection_check() {
    delay(2);
    if (WiFi.status() == WL_CONNECTED) {
        if (SectAuto) {
            //WIFI LED blink 8x faster while no new data
            WiFi_LED_blink(3);
#if HardwarePlatform == 1//nano 33iot
            pingResult = WiFi.ping(Set.gwip);
            delay(1);
            Serial.print("no Section control Data, ping to Gateway (ms): "); Serial.print(pingResult);
            Serial.print("   Watchdog counter: "); Serial.println(WiFiWatchDog);
            if (pingResult >= 0) { WiFiWatchDog = 0; PingToNetworkLastTime = millis();
            }
            else WiFiWatchDog++;
        }
        if (WiFiWatchDog > 3) {//reconnect
            LED_WIFI_ON = false;
            digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);
            Serial.print("WiFi error: no data for "); Serial.print(now - DataFromAOGTime);
            Serial.print(" ms. No ping to "); for (byte n = 0; n < 4; n++) { Serial.print(Set.gwip[n]); Serial.print("."); }
            Serial.println();
            Serial.print("Closing WiFi and try to reconnect to network: ");
            if (NetWorkNum == 2) { Serial.println(Set.ssid2); }
            else { Serial.println(Set.ssid); }
            WiFi.end();
            delay(200);
            delay(200);
            timeout = millis() + 10000;//close wifi every 10s when no new connection
            WiFiWatchDog = 0;
#endif
        }
        if (WiFi.localIP()[3] != Set.myIPEnding) {
#if HardwarePlatform == 0  //ESP32 
            if (!WiFi.config(Set.myip, Set.gwip, Set.mask, Set.gwip)) { Serial.println("STA Failed to configure"); }
#endif
#if HardwarePlatform == 1//nano 33iot
            WiFi.config(Set.myip, Set.gwip, Set.gwip, Set.mask);
#endif
            delay(200);
            delay(5);
            Serial.println();
            Serial.print("WiFi Client connected to : ");
            if (NetWorkNum == 2) { Serial.println(Set.ssid2); }
            else { Serial.println(Set.ssid); }
            Serial.print("Connected IP - Address : ");
            Serial.println(WiFi.localIP());
            WiFiWatchDog = 0;
        }
        if (!UDP_running) { UDP_Start(); }
    }
    else {
        //Serial.print("Wifi Watchdog: "); Serial.println(WiFiWatchDog);
        UDP_running = false;
        digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);
        LED_WIFI_ON = false;
        WiFiWatchDog++;
        if (WiFiWatchDog > 50) {
            WiFiWatchDog = 0;//give router some time to check request
            if (millis() > timeout) {
                Serial.println();
                Serial.print("Closing WiFi and try to reconnect to network: ");
                if (NetWorkNum == 2) { Serial.println(Set.ssid2); }
                else { Serial.println(Set.ssid); }
#if HardwarePlatform == 0  //ESP32  
                WiFi.disconnect();
#endif
#if HardwarePlatform == 1//nano 33iot
                WiFi.end();
#endif
                delay(200);
                delay(200);
#if HardwarePlatform == 0  //ESP32  
                WiFi.mode(WIFI_STA);   //  Workstation
#endif
                timeout = millis() + 10000;//close wifi every 10s when no new connection
            }
            //reconnection
            Serial.print(".");
            if (NetWorkNum == 2) { WiFi.begin(Set.ssid2, Set.password2); }
            else { WiFi.begin(Set.ssid, Set.password); }
            delay(10);
        }
    }
}
*/

//-------------------------------------------------------------------------------------------------
// Server Index Page for OTA update
//-------------------------------------------------------------------------------------------------

#if HardwarePlatform == 0

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<head>"
"<title>Firmware updater</title>"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0;\" />\r\n""<style>divbox {background-color: lightgrey;width: 200px;border: 5px solid red;padding:10px;margin: 10px;}</style>"
"</head>"
"<body bgcolor=\"#ccff66\">""<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">"
"<h1>ESP firmware update</h1>"
"ver 4.3 - 10. Mai. 2020<br><br>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<br>Create a .bin file with Arduino IDE: Sketch -> Export compiled Binary<br>"
"<br><b>select .bin file to upload</b>"
"<br>"
"<br>"
"<input type='file' name='update'>"
"<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
"$('form').submit(function(e){"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
" $.ajax({"
"url: '/update',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData:false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(d, s) {"
"console.log('success!')"
"},"
"error: function (a, b, c) {"
"}"
"});"
"});"
"</script>";

#endif