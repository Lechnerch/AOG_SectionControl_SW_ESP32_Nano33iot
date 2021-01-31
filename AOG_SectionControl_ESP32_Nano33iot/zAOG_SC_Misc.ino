//-------------------------------------------------------------------------------------------------

void WiFi_LED_blink(byte blkSpeed)   //8. Maerz 2020
{
	unsigned long now = millis();
	if (!LED_WIFI_ON) {
		if (now > (LED_WIFI_time + (LED_WIFI_pause >> blkSpeed))) {
			LED_WIFI_time = now;
			LED_WIFI_ON = true;
#if useLED_BUILTIN
			digitalWrite(LED_BUILTIN, HIGH);
#endif
			digitalWrite(Set.LEDWiFi_PIN, Set.LEDWiFi_ON_Level);
		}
	}
	if (LED_WIFI_ON) {
		if (now > (LED_WIFI_time + (LED_WIFI_pulse >> blkSpeed))) {
			LED_WIFI_time = now;
			LED_WIFI_ON = false;
#if useLED_BUILTIN
			digitalWrite(LED_BUILTIN, LOW);
#endif
			digitalWrite(Set.LEDWiFi_PIN, !Set.LEDWiFi_ON_Level);
		}
	}
}




/*

struct Storage {
//User config: ***********************************************************************************

	uint8_t DataTransVia = 1; //NEW to do !!!!! //transfer data via 0 = USB / 1 = Bluetooth / 7 = UDP / 8 = UDP 2x / 10 = Ethernet UDP//transfer data via 0: USB, 1: WiFi, 4: USB 10 byte format for AOG V4

	uint8_t LEDWiFi_PIN = 5;//13;     // WiFi Status LED 0 = off
	uint8_t LEDWiFi_ON_Level = 0;     // 1 = HIGH = LED on high, 0 = LOW = LED on low

	//WiFi---------------------------------------------------------------------------------------------
	//tractors WiFi
	char ssid[24] = "WLANHammer";// "GPS_unit_ESP_M8T";       // WiFi network Client name
	char password[24] = "aveasillrac";                   // WiFi network password//Accesspoint name and password
	char ssid2[24] = "Deutz_6006";            // WiFi network Client name
	char password2[24] = "";                  // WiFi network password//Accesspoint name and password
	char ssid_ap[24] = "SectionControlNet";   // name of Access point, if no WiFi found, no password!!
	uint8_t timeoutRouter = 20;               //time (s) to search for existing WiFi, than starting Accesspoint
	uint8_t timeoutRouter2 = 5;               //time (s) to search for existing WiFi, than starting Accesspoint
	byte timeoutWebIO = 10;					  //time (min) afterwards webinterface is switched off
	//static IP
	uint8_t myip[4] = { 192, 168, 1, 71 };    // section control module; in DHCP Network replaced by x.x.x.71 = myIPEnding
	uint8_t gwip[4] = { 192, 168, 1, 1 };     // Gateway IP also used if Accesspoint created; in DHCP network replaced by networks GW IP
	uint8_t mask[4] = { 255, 255, 255, 0 };
	uint8_t IPToAOG[4] = { 192,168,1,255 };   // 255 = broadcast; in DHCP Network replaced by x.x.x.255
	uint8_t myIPEnding = 71;
	uint16_t PortToAOG = 5555;              //this is port of this module: Autosteer = 5577 IMU = 5566 Section Control = 5555 GPS = 5544
	uint16_t PortFromAOG = 8888;              // port to listen for AOG
	uint16_t PortDestination = 9999;          // Port of AOG that listens
	uint16_t BaudRate = 38400;                //Baudrate = speed of serial port or USB or Bluetooth. AOG uses 38400 for UART

//the following lines should be configed by the user to fit the programm to the sprayer/ESP32
//GPIOs of the ESP32 (current setting is for the layout shown as example WIKI)

// if only 1 flowrate is used, use left
//Example1: motor valve is controled only by Switch not by AOG, no Flowmeter, : RateControl..Equiped = false; RateSW..Equiped = true; RateControlPWM = false;
//Example2: PWM valve, with flowmeter all controled by AOG:   RateControl..Equiped = true; RateSW..Equiped = true; RateControlPWM = true;
	uint8_t RateControlLeftInst = 0;		    //1 if Rate control is there, else: 0
	uint8_t RateSWLeftInst = 0;	//1		      //1 if Rate control Pressure switch is there, else: 0
	uint8_t RateSWLeft_PIN = A6;//34;				  //Rate +/- switch
	uint8_t RateControlPWM = 0;				        //1 if PWM valve, 0 if Motor drive for pressure change

	uint8_t	FlowDirLeft_PIN = 11;//23;				//Rate-Control Valve/Motor Direktion
	uint8_t	FlowPWMLeft_PIN = 12;//22;				//Rate-Control Valve PWM/Motor ON/OFF
	uint8_t	FlowEncALeft_PIN = 255;				    //Flowmeter left/1

	uint8_t RateControlRightInst = 0;	    //1 if Rate control is there, else: 0
	uint8_t RateSWRightInst = 0;			      //1 if Rate control Pressure switch is there, else: 0
	uint8_t	RateSWRight_PIN = 255;				    //Rate +/- switch
	uint8_t	FlowDirRight_PIN = 255;				    //255  = unused Rate-Control Valve Direktion
	uint8_t	FlowPWMRight_PIN = 255;				    //255  = unused Rate-Control Valve PWM
	uint8_t	FlowEncARight_PIN = 255;				  //Flowmeter right/2


	uint8_t SectNum = 5;	                    // number of sections equiped max 16 (in AOG)
	uint8_t SectRelaysInst = 1;				    //relays for SC output are equiped (0=no output, only documentation)
	uint8_t SectRelaysON = 1;					        //relays spray on 1 or 0 (high or low)
	uint8_t Relay_PIN[16] = { 10,9,8,7,6,5,255,255,255,255,255,255,255,255,255,255 };//{ 15,2,0,4,16,17,18,19,21,255,255,255,255,255,255,255 };   		//GPIOs of ESP32 OUT to sections of sprayer HIGH/3.3V = ON
						  //Relay pin set for nano 33 iot:  //
	uint8_t SectSWInst = 1;					      //1 if section input switches are equiped, else: 0
	uint8_t SectSW_PIN[16] = { A0,A1,A2,A3,A4,A5,3,255,255,255,255,255,255,255,255,255 };//{ 255,12,14,27,26,25,33,36,255,255,255,255,255,255,255,255 };   //section switches to GPIOs of ESP32 GND = section off, open/+3.3V section auto/on
						  //Switch pin set for nano 33 iot:  //
	uint8_t	SectMainSWType = 1;						    // 0 = not equiped 1 = (ON)-OFF-(ON) toggle switch or push buttons 2 = connected to hitch level sensor 3 = inverted hitch level sensor
	uint16_t	HitchLevelVal = 500;//	2000;		// Value for hitch level: switch AOG section control to Auto if lower than... ESP:2000 nano 500
	uint8_t	SectMainSW_PIN = A7;//32;					//ESP32 to AOG Main auto toggle switch open=nothing/AOG button GND=OFF +3,3=AOG Auto on	OR connected to hitch level sensor
	uint8_t	SectAutoManSW_PIN = 4;//39;			// Main Auto/Manual switch 39:!!no internal pullup!!

	uint8_t DocOnly = 0;					  // 0: use as section control, 1: Documentation only = AOG writes the state of the input switches
	uint8_t SectSWAutoOrOn = LOW;

	uint8_t DataToAOGHeader[2] = { 127,249 }, DataFromAOGHeader[2] = { 127, 250 }, SettingsFromAOGHeader[2] = { 127,248 }, n = 0xF8;
	uint8_t DataToAOGLength = 10;             //items per UDP packet
	uint8_t DataFromAOGLength = 10;           //items comming from AOG
	uint8_t SettingsFromAOGLength = 10;

	bool debugmode = true;
	bool debugmodeRelay = false;
	bool debugmodeSwitches = false;
	bool debugmodeDataFromAOG = false;
	bool debugmodeDataToAOG = false;

 // END of user config ****************************************************************************
}; Storage Set;

*/



/*




//-------------------------------------------------------------------------------------------------
// HTML Seite 01 aufbauen
//-------------------------------------------------------------------------------------------------
void make_HTML01() {

    strcpy(HTML_String, "<!DOCTYPE html>");
    strcat(HTML_String, "<html>");
    strcat(HTML_String, "<head>");
    strcat(HTML_String, "<title>Section Control Config Page</title>");
    strcat(HTML_String, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0;\" />\r\n");
    //strcat( HTML_String, "<meta http-equiv=\"refresh\" content=\"10\">");
    strcat(HTML_String, "<style>divbox {background-color: lightgrey;width: 200px;border: 5px solid red;padding:10px;margin: 10px;}</style>");
    strcat(HTML_String, "</head>");
    strcat(HTML_String, "<body bgcolor=\"#ccff66\">");//ff9900
    strcat(HTML_String, "<font color=\"#000000\" face=\"VERDANA,ARIAL,HELVETICA\">");
    strcat(HTML_String, "<h1>Section control setup</h1>");
    strcat(HTML_String, "Section control software for AgOpenGPS <br>");
    strcat(HTML_String, "supports data via USB / WiFi UDP <br>");
    strcat(HTML_String, "more settings like IPs, UPD ports... in setup zone of INO code<br>");
    strcat(HTML_String, "(Rev. 5.01 by MTZ8302 Webinterface by MTZ8302 & WEDER)<br><br><hr>");


    //---------------------------------------------------------------------------------------------
     //load values of INO setup zone
    strcat(HTML_String, "<h2>Load default values of INO setup zone</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(270, 250, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"2\">Only load default values, does NOT save them</td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?ACTION=");
    strcati(HTML_String, ACTION_LoadDefaultVal);
    strcat(HTML_String, "')\" style= \"width:150px\" value=\"Load default values\"></button></td>");
    strcat(HTML_String, "</tr>");
    strcat(HTML_String, "</table></form><br><hr>");

    //-----------------------------------------------------------------------------------------
    // WiFi Client Access Data

    strcat(HTML_String, "<h2>WiFi Network #1 Client Access Data</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "</b>If access to network fails, 2nd network will be tried.<br><br><table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Network SSID:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?SSID_MY='+this.value)\" style= \"width:200px\" name=\"SSID_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, Set.ssid);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Password:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?Password_MY='+this.value)\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, Set.password);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">time, trying to connect to network</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">trying to connect to 2nd network, after time has passed</td></tr>");
    strcat(HTML_String, "<tr><td><b>Timeout (s):</b></td><td><input type = \"number\" onchange=\"sendVal('/?timeoutRout='+this.value)\" name = \"timeoutRout\" min = \"20\" max = \"200\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, Set.timeoutRouter);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr><td colspan=\"2\"><b>Restart for changes to take effect:</b></td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?ACTION=");
    strcati(HTML_String, ACTION_RESTART);
    strcat(HTML_String, "')\" style= \"width:120px\" value=\"Restart\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "</table></form><br><hr>");

    //-----------------------------------------------------------------------------------------
    // 2nd WiFi Client Access Data

    strcat(HTML_String, "<h2>WiFi Network #2 Client Access Data</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "</b>If access to both networks fails, an accesspoint will be created:<br>SSID: <b>");
    strcat(HTML_String, Set.ssid_ap);
    strcat(HTML_String, "</b>     with no password<br><br><table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Network SSID 2:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?SSID_MY2='+this.value)\" style= \"width:200px\" name=\"SSID_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, Set.ssid2);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Password 2:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?Password_MY2='+this.value)\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, Set.password2);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">time, trying to connect to network</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">after time has passed access point is opened</td></tr>");
    strcat(HTML_String, "<tr><td><b>Timeout 2 (s):</b></td><td><input type = \"number\" onchange=\"sendVal('/?timeoutRout2='+this.value)\" name = \"timeoutRout\" min = \"5\" max = \"200\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, Set.timeoutRouter2);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr><td colspan=\"2\"><b>Restart for changes to take effect:</b></td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?ACTION=");
    strcati(HTML_String, ACTION_RESTART);
    strcat(HTML_String, "')\" style= \"width:120px\" value=\"Restart\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "</table></form><br><hr>");

    //-----------------------------------------------------------------------------------------
    // timeout webinterface

    strcat(HTML_String, "<h2>Webinterface timeout</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<b>Webinterface needs lots of calculation time, so if switched off, program runs better.</b><br>");
    strcat(HTML_String, "After this time (minutes) from restart, or last usage, webinterface is turned off.<br><br>");
    strcat(HTML_String, "Set to 255 to keep active.<br><br><table>");
    set_colgroup(300, 250, 150, 0, 0);

    strcat(HTML_String, "<tr><td><b>Webinterface timeout (min)</b></td><td><input type = \"number\"  onchange=\"sendVal('/?timeoutWebIO='+this.value)\" name = \"timeoutWebIO\" min = \"2\" max = \"255\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, Set.timeoutWebIO);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "</table></form><br><hr>");

    //---------------------------------------------------------------------------------------------
    // Data transfer via USB/Bluetooth/Wifi/Ethernet
    strcat(HTML_String, "<h2>Data transfer from/to AgOpenGPS via</h2>");
    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(250, 300, 150, 0, 0);

    //transfer data via 0 = USB / 1 = USB V5 / 7 = UDP / 8 = UDP 2x / 10 = Ethernet UDP
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=0')\" name=\"DataTransfVia\" id=\"JZ\" value=\"0\"");
    if (Set.DataTransVia == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">USB AOG V4</label></td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=1')\" name=\"DataTransfVia\" id=\"JZ\" value=\"1\"");
    if (Set.DataTransVia == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">USB AOG V5 (not working yet)</label></td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=7')\" name=\"DataTransfVia\" id=\"JZ\" value=\"7\"");
    if (Set.DataTransVia == 7)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">WiFi (UDP) (default)</label></td></tr>");
/*
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=8')\" name=\"DataTransfVia\" id=\"JZ\" value=\"8\"");
    if (Set.DataTransVia == 8)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">WiFi (UDP) send message 2x</label></td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=10')\" name=\"DataTransfVia\" id=\"JZ\" value=\"10\"");
    if (Set.DataTransVia == 10)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">Ethernet (UDP)</label></td></tr>");
*/
//strcat(HTML_String, "</table></form><br><hr>");

/*   //---------------------------------------------------------------------------------------------
   // WiFi LED light on high/low
   strcat(HTML_String, "<h2>WiFi LED light on</h2>");
   strcat(HTML_String, "<form>");
   strcat(HTML_String, "<table>");
   set_colgroup(150, 270, 150, 0, 0);

   strcat(HTML_String, "<tr>");
   strcat(HTML_String, "<td></td><td><input type = \"radio\" name=\"WiFiLEDon\" id=\"JZ\" value=\"0\"");
   if (Set.LEDWiFi_ON_Level == 0)strcat(HTML_String, " CHECKED");
   strcat(HTML_String, "><label for=\"JZ\">LOW</label></td>");
   strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
   strcati(HTML_String, ACTION_SET_WiFiLEDon);
   strcat(HTML_String, "\">Apply and Save</button></td>");
   strcat(HTML_String, "</tr>");

   strcat(HTML_String, "<tr>");
   strcat(HTML_String, "<td></td><td><input type = \"radio\" name=\"WiFiLEDon\" id=\"JZ\" value=\"1\"");
   if (Set.LEDWiFi_ON_Level == 1)strcat(HTML_String, " CHECKED");
   strcat(HTML_String, "><label for=\"JZ\">HIGH</label></td></tr>");

   strcat(HTML_String, "</table>");
   strcat(HTML_String, "</form>");
   strcat(HTML_String, "<br><hr>");
   */

   //---------------------------------------------------------------------------------------------  
   // Number of sections
/*
strcat(HTML_String, "<h2>Number of sections</h2>");

strcat(HTML_String, "<form>");
strcat(HTML_String, "<table>");
set_colgroup(250, 300, 150, 0, 0);

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"3\">Set number of sections (1-100).</td></tr>");
strcat(HTML_String, "<tr><td><b>number of sections</b></td><td><input type = \"number\"  onchange=\"sendVal('/?SectNum='+this.value)\" name = \"SectNum\" min = \"1\" max = \"100\" step = \"1\" style= \"width:200px\" value = \"");
strcati(HTML_String, Set.SectNum);
strcat(HTML_String, "\"></td>");

strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "</table></form><br><hr>");


//---------------------------------------------------------------------------------------------  
// Checkboxes IO functions
strcat(HTML_String, "<h2>Section control uses</h2>");
strcat(HTML_String, "<form>");
strcat(HTML_String, "<table>");
set_colgroup(150, 250, 150, 145, 150);

//relais
strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"2\"><b>Relay output</b></td><td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?seRelInst='+this.checked)\" name=\"seRelInst\" id = \"Part\" value = \"1\" ");
if (Set.SectRelaysInst == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> installed</label>");
strcat(HTML_String, "</td>");
strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td>if installed</td><td><input type = \"radio\" onclick=\"sendVal('/?seRelON=0')\" name=\"seRelON\" id=\"JZ\" value=\"0\"");
if (Set.SectRelaysON == 0)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"JZ\"> LOW</label></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "<td></td><td>relays spray on</td><td><input type = \"radio\" onclick=\"sendVal('/?seRelON=1')\" name=\"seRelON\" id=\"JZ\" value=\"1\"");
if (Set.SectRelaysON == 1)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"JZ\"> HIGH</label></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

//switches
strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"2\"><b>Switches for sections</b></td><td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?seSWInst='+this.checked)\" name=\"seSWInst\" id = \"Part\" value = \"1\" ");
if (Set.SectSWInst == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> installed</label>");
strcat(HTML_String, "</td>");
strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td>if installed</td><td><input type = \"radio\" onclick=\"sendVal('/?DocSWspr=0')\" name=\"DocSWspr\" id=\"JZ\" value=\"0\"");
if (Set.SectSWAutoOrOn == 0)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"JZ\"> LOW</label></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "<td></td><td>switches spray on</td><td><input type = \"radio\" onclick=\"sendVal('/?DocSWspr=1')\" name=\"DocSWspr\" id=\"JZ\" value=\"1\"");
if (Set.SectSWAutoOrOn == 1)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"JZ\"> HIGH</label></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

strcat(HTML_String, "<tr><td colspan=\"4\"><b>Use as section control, or documentation<br>doc = AgOpenGPS writes status of input switches.</b></td></tr>");
strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"2\"></td><td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?DocOnly='+this.checked)\" name=\"DocOnly\" id = \"Part\" value = \"1\" ");
if (Set.DocOnly == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> Documentation only</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");


//rate SW
strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"4\"><b>Rate Control Switch only for +/- Motor valve</b></td>");
strcat(HTML_String, "<tr><td colspan=\"2\"></td><td><input type = \"checkbox\" onclick=\"sendVal('/?RCSWInst='+this.checked)\" name=\"RCSWInst\" id = \"RCS\" value = \"1\" ");
if (Set.RateSWLeftInst == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"RCS\"> installed</label>");
strcat(HTML_String, "</td>");
strcat(HTML_String, "<td></td><td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

//main SW
strcat(HTML_String, "<tr><td colspan=\"4\"><b>Main switch for section control</b></td>");
strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=0')\" name=\"MainSW\" id=\"MS\" value=\"0\"");
if (Set.SectMainSWType == 0)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"MS\"> none</label></td>");
strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
strcat(HTML_String, "</tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=1')\" name=\"MainSW\" id=\"MS\" value=\"1\"");
if (Set.SectMainSWType == 1)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"MS\"> (ON)-OFF-(ON) toggle switch or push buttons</label></td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=2')\" name=\"MainSW\" id=\"MS\" value=\"2\"");
if (Set.SectMainSWType == 2)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"MS\"> connected to hitch level sensor</label></td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=3')\" name=\"MainSW\" id=\"MS\" value=\"3\"");
if (Set.SectMainSWType == 3)strcat(HTML_String, " CHECKED");
strcat(HTML_String, "><label for=\"MS\"> inverted hitch level sensor</label></td></tr>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

//analog MAIN SW from hitch level
if (Set.SectMainSWType > 1) {
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"4\"><br>Analog Main Section control Threshold value</td></tr>");
    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

    strcat(HTML_String, "<tr><td></td><td><divbox align=\"right\"><font size=\"+2\"><b>");
    strcati(HTML_String, (Set.HitchLevelVal));
    strcat(HTML_String, "</b></font></divbox></td><td colspan=\"2\">0-4095</td>");
    //Refresh button
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"location.reload()\" style= \"width:120px\" value=\"Refresh\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    //set treshold
    strcat(HTML_String, "<tr><td></td><td><input type= \"button\" onclick= \"sendVal('/?ACTION=");
    strcati(HTML_String, ACTION_SET_WS_THRESHOLD);
    strcat(HTML_String, "')\" style= \"width:200px\" value=\"Use Current\"></button></td></tr>");
    strcat(HTML_String, "<tr><td colspan=\"4\"><b>Put your tool in the middle between ON and OFF position.</b></td>");
}

strcat(HTML_String, "</table></form><br><hr>");

//-------------------------------------------------------------
// Checkboxes debugmode
strcat(HTML_String, "<h2>Debugmode</h2>");
strcat(HTML_String, "<form>");
strcat(HTML_String, "<table>");
set_colgroup(300, 250, 150, 0, 0);

strcat(HTML_String, "<tr> <td colspan=\"2\">debugmode sends messages to USB serial</td>");
strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmode='+this.checked)\" name=\"debugmode\" id = \"Part\" value = \"1\" ");
if (Set.debugmode == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> general debugmode on</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmSW='+this.checked)\" name=\"debugmSW\" id = \"Part\" value = \"1\" ");
if (Set.debugmodeSwitches == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> debugmode switches on (display switch status)</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmRel='+this.checked)\" name=\"debugmRel\" id = \"Part\" value = \"1\" ");
if (Set.debugmodeRelay == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> debugmode relais on</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmDatFromAOG='+this.checked)\" name=\"debugmDatFromAOG\" id = \"Part\" value = \"1\" ");
if (Set.debugmodeDataFromAOG == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> debugmode data from AgOpenGPS on</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "<tr>");
strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmDatToAOG='+this.checked)\" name=\"debugmDatToAOG\" id = \"Part\" value = \"1\" ");
if (Set.debugmodeDataToAOG == 1) strcat(HTML_String, "checked ");
strcat(HTML_String, "> ");
strcat(HTML_String, "<label for =\"Part\"> debugmode data to AgOpenGPS on</label>");
strcat(HTML_String, "</td></tr>");

strcat(HTML_String, "</table></form><br><hr>");

/*
    //---------------------------------------------------------------------------------------------
    // Relay PINs selection

    //ESP crashes sometimes, when GPIOs are changed, Nano33iot: not enough memory HTML string lenght: 60000
    if (Set.SectRelaysInst) {
        strcat(HTML_String, "<h2>Relay pin setting</h2>");
        strcat(HTML_String, "<br>");

        strcat(HTML_String, "<form>");
        strcat(HTML_String, "<table><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\">");
        strcat(HTML_String, "<col width = \"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\"><col width=\"50\">");

        strcat(HTML_String, "<tr><td colspan=\"11\"><b>select for every section relay a GPIO pin</b></td>");
        strcat(HTML_String, "<td colspan=\"3\"><button style= \"width:120px\" name=\"ACTION\" value=\"");
        strcati(HTML_String, ACTION_SET_GPIO);
        strcat(HTML_String, "\">Apply and Save</button></td></tr>");
        strcat(HTML_String, "<tr><td colspan=\"17\"><b>!! NO check, if pin is selected twice!!  Use every pin only 1 time !!</b></td>");
        strcat(HTML_String, "<tr> <td colspan=\"17\">&nbsp;</td> </tr>");

        strcat(HTML_String, "<tr><td></td><td></td><td colspan=\"6\"><b>section #</b></td>");
        strcat(HTML_String, "</tr><tr><td></td>");
        byte num = 0;
        for (num = 0; num < Set.SectNum; num++) {
            strcat(HTML_String, "<td align=center><b>");
            strcati(HTML_String, (num + 1));
            strcat(HTML_String, "</b></td>");
        }
        strcat(HTML_String, "</tr>");

        strcat(HTML_String, "<tr><td><b>GPIO</b></td>");
        for (num = 0; num < Set.SectNum; num++) {//colums
            strcat(HTML_String, "<td><input type = \"radio\" name=\"RP");
            strcati(HTML_String, num);
            strcat(HTML_String, "\" id=\"Rel");
            strcati(HTML_String, num);
            strcat(HTML_String, "\" value=\"255\"");
            if (Set.Relay_PIN[num] == 255) { strcat(HTML_String, " CHECKED"); }
            strcat(HTML_String, "><label for=\"Rel");
            strcati(HTML_String, num);
            strcat(HTML_String, "\">nc</label></td>");
        }
        strcat(HTML_String, "</tr>");


        for (int io = 2; io < 40; io++) {//rows
            //skip not usabel GPIOs
            if (io == 3) { io++; } //3: 6-11: not use! USB 12: ESP wouldn't boot
            if (io == 6) { io = 13; }

            strcat(HTML_String, "<tr><td>pin #</td>");
            num = 0;
            for (num = 0; num < Set.SectNum; num++) {//colums
                strcat(HTML_String, "<td><input type = \"radio\" name=\"RP");
                strcati(HTML_String, num);
                strcat(HTML_String, "\" id=\"R");
                strcati(HTML_String, num);
                strcat(HTML_String, "\" value=\"");
                strcati(HTML_String, io);
                strcat(HTML_String, "\"");
                if (Set.Relay_PIN[num] == io) { strcat(HTML_String, " CHECKED"); }
                strcat(HTML_String, "><label for=\"R");
                strcati(HTML_String, num);
                strcat(HTML_String, "\">");
                strcati(HTML_String, io);
                strcat(HTML_String, "</label></td>");
            }
            strcat(HTML_String, "</tr>");
        }
        strcat(HTML_String, "</table>");
        strcat(HTML_String, "</form>");
    }*/


    //-------------------------------------------------------------
    // firmware update
/*
strcat(HTML_String, "<h2>Firmware Update for ESP32</h2>");
strcat(HTML_String, "<form>");
strcat(HTML_String, "<table>");
set_colgroup(300, 250, 150, 0, 0);

strcat(HTML_String, "<tr> <td colspan=\"3\">build a new firmware with Arduino IDE selecting</td> </tr>");
strcat(HTML_String, "<tr> <td colspan=\"3\">Sketch -> Export compiled Binary</td> </tr>");
strcat(HTML_String, "<tr> <td colspan=\"3\">upload this file via WiFi/Ethernet connection</td> </tr>");

strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
strcat(HTML_String, "<tr><td></td>");
//button
strcat(HTML_String, "<td><input type='submit' onclick='openUpload(this.form)' value='Open Firmware uploader'></td></tr>");

strcat(HTML_String, "<script>");
strcat(HTML_String, "function openUpload(form)");
strcat(HTML_String, "{");
strcat(HTML_String, "window.open('/serverIndex')");
strcat(HTML_String, "}");
strcat(HTML_String, "</script>");

strcat(HTML_String, "</table></form><br><hr>");

//-------------------------------------------------------------  
strcat(HTML_String, "</font>");
strcat(HTML_String, "</body>");
strcat(HTML_String, "</html>");

//script to send values from webpage to ESP for process request
strcat(HTML_String, "<script>");
strcat(HTML_String, "function sendVal(ArgStr)");
strcat(HTML_String, "{");
strcat(HTML_String, "  var xhttp = new XMLHttpRequest();");
strcat(HTML_String, "  xhttp.open(\"GET\",ArgStr, true);");
strcat(HTML_String, "  xhttp.send();");
strcat(HTML_String, " if (ArgStr == '/?ACTION=");
strcati(HTML_String, ACTION_LoadDefaultVal);
strcat(HTML_String, "') { window.setTimeout('location.reload()',400); }");
strcat(HTML_String, "}");
strcat(HTML_String, "</script>");

}














*/