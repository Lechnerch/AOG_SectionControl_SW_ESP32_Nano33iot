// Wifi variables & definitions
char HTML_String[20000];
int action;
long temLong = 0;
double temDoub = 0;

#define ACTION_LoadDefaultVal   1
#define ACTION_RESTART          2
#define ACTION_SET_WS_THRESHOLD 3

//-------------------------------------------------------------------------------------------------
//10. Mai 2020

void StartServer() {

    //return index page which is stored in serverIndex 
    server.on("/", HTTP_GET, []() {
        make_HTML01();
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", HTML_String);
        WebIOTimeOut = millis() + long((SCSet.timeoutWebIO * 60000));
        if (SCSet.debugmode) { 
            Serial.println("Webpage root"); Serial.print("Timeout WebIO: "); Serial.println(WebIOTimeOut); 
           // Serial.print("length HTML string: "); Serial.println(HTML_String.lenght); 
        }
        process_Request();
        make_HTML01();
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", HTML_String);
        if (SCSet.debugmode) {
            Serial.println("page reloaded");
        }
        });
#if HardwarePlatform == 0 
    server.on("/serverIndex", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
        });
   
    //handling uploading firmware file 
    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
        }, []() {
            HTTPUpload& upload = server.upload();
            if (upload.status == UPLOAD_FILE_START) {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE) {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) { //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else {
                    Update.printError(Serial);
                }
            }
        });
#endif
    server.onNotFound(handleNotFound);

    server.begin();
}

//---------------------------------------------------------------------
// Process given values 10. Mai 2020
//---------------------------------------------------------------------
void process_Request()
{
    action = 0;
    if (SCSet.debugmode) { Serial.print("From webinterface: number of arguments: "); Serial.println(server.args()); }
    for (byte n = 0; n < server.args(); n++) {
        if (SCSet.debugmode) {
            Serial.print("argName "); Serial.print(server.argName(n));
            Serial.print(" val: "); Serial.println(server.arg(n));
        }
        if (server.argName(n) == "ACTION") {
            action = int(server.arg(n).toInt());
            if (SCSet.debugmode) { Serial.print("Action found: "); Serial.println(action); }
        }
        if (action != ACTION_RESTART) { EEprom_unblock_restart(); }
        if (action == ACTION_LoadDefaultVal) {
            if (SCSet.debugmode) { Serial.println("load default settings from EEPROM"); }
            EEprom_read_default();
            delay(2);
        }
        //save changes
        if (server.argName(n) == "Save") {
            if (SCSet.debugmode) { Serial.println("Save button pressed in webinterface"); }
            EEprom_write_all();
        }

        if (server.argName(n) == "SSID_MY") {
            for (int i = 0; i < 24; i++) SCSet.ssid[i] = 0x00;
            int tempInt = server.arg(n).length() + 1;
            server.arg(n).toCharArray(SCSet.ssid, tempInt);
        }
        if (server.argName(n) == "Password_MY") {
            for (int i = 0; i < 24; i++) SCSet.password[i] = 0x00;
            int tempInt = server.arg(n).length() + 1;
            server.arg(n).toCharArray(SCSet.password, tempInt);
        }
        if (server.argName(n) == "timeoutRout") {
            argVal = server.arg(n).toInt();
            if ((argVal >= 20) && (argVal <= 200)) { SCSet.timeoutRouter = byte(argVal); }
        }

        if (server.argName(n) == "SSID_M2Y") {
            for (int i = 0; i < 24; i++) SCSet.ssid2[i] = 0x00;
            int tempInt = server.arg(n).length() + 1;
            server.arg(n).toCharArray(SCSet.ssid2, tempInt);
        }
        if (server.argName(n) == "Password_MY2") {
            for (int i = 0; i < 24; i++) SCSet.password2[i] = 0x00;
            int tempInt = server.arg(n).length() + 1;
            server.arg(n).toCharArray(SCSet.password2, tempInt);
        }
        if (server.argName(n) == "timeoutRout2") {
            argVal = server.arg(n).toInt();
            if ((argVal >= 0) && (argVal <= 200)) { SCSet.timeoutRouter2 = byte(argVal); }
        }
        if (server.argName(n) == "timeoutWebIO") {
            temLong = server.arg(n).toInt();
            if ((temLong >= 2) && (temLong <= 255)) { SCSet.timeoutWebIO = byte(temLong); }
        }
        if (server.argName(n) == "DataTransfVia") {
            SCSet.DataTransVia = byte(server.arg(n).toInt());
        }
        if (server.argName(n) == "SectNum") {
            argVal = server.arg(n).toInt();
            if ((argVal >= 1) && (argVal <= 100)) { SCSet.SectNum = byte(argVal); }
        }
        if (server.argName(n) == "seRelInst") {
            if (server.arg(n) == "true") { SCSet.SectRelaysInst = 1; }
            else { SCSet.SectRelaysInst = 0; }
        }
                if (server.argName(n) == "seRelON") {
            if (server.arg(n) == "1") { SCSet.SectRelaysON = 1; }
            else { SCSet.SectRelaysON = 0; }
        }
        if (server.argName(n) == "seSWInst") {
            if (server.arg(n) == "true") { SCSet.SectSWInst = 1; }
            else { SCSet.SectSWInst = 0; }
        }
        if (server.argName(n) == "DocOnly") {
            if (server.arg(n) == "true") { SCSet.DocOnly = 1; }
            else { SCSet.DocOnly = 0; }
        }
        if (server.argName(n) == "DocSWspr") {
            if (server.arg(n) == "1") { SCSet.SectSWAutoOrOn = 1; }
            else { SCSet.SectSWAutoOrOn = 0; }
        }
        if (server.argName(n) == "RCSWInst") {
            if (server.arg(n) == "true") { SCSet.RateSWLeftInst = 1; }
            else { SCSet.RateSWLeftInst = 0; }
        }
        if (server.argName(n) == "MainSW") {
            argVal = server.arg(n).toInt();
            if ((argVal >= 0) && (argVal <= 5)) { SCSet.SectMainSWType = byte(argVal); }
        }
        if (action == ACTION_SET_WS_THRESHOLD) {
            unsigned int WSThres_avg = 0;
            for (int i = 0; i < 8; i++) {
                WSThres_avg += analogRead(SCSet.SectMainSW_PIN);
                delay(100);
            }
            SCSet.HitchLevelVal = WSThres_avg >> 3;
            EEprom_write_all();
        }
        if (server.argName(n) == "debugmode") {
            if (server.arg(n) == "true") { SCSet.debugmode = true; }
            else { SCSet.debugmode = false; }
        }
        if (server.argName(n) == "debugmSW") {
            if (server.arg(n) == "true") { SCSet.debugmodeSwitches = true; }
            else { SCSet.debugmodeSwitches = false; }
        }
        if (server.argName(n) == "debugmRel") {
            if (server.arg(n) == "true") { SCSet.debugmodeRelay = true; }
            else { SCSet.debugmodeRelay = false; }
        }
        if (server.argName(n) == "debugmDatFromAOG") {
            if (server.arg(n) == "true") { SCSet.debugmodeDataFromAOG = true; }
            else { SCSet.debugmodeDataFromAOG = false; }
        }
        if (server.argName(n) == "debugmDatToAOG") {
            if (server.arg(n) == "true") { SCSet.debugmodeDataToAOG = true; }
            else { SCSet.debugmodeDataToAOG = false; }
        }

        if (action == ACTION_RESTART) {
            Serial.println("reboot ESP32: selected by webinterface");
            EEprom_block_restart();//prevents from restarting, when webpage is reloaded. Is set to 0, when other ACTION than restart is called
            delay(1000);
#if HardwarePlatform == 0
            WiFi.disconnect();
            delay(500);
            ESP.restart();
#endif
#if HardwarePlatform == 1
            WiFi.end();
            delay(2000);
            Serial.println("restarting WiFi");
            WiFi_Start_STA();
            delay(200);
            if (my_WiFi_Mode == 0) {// if failed start AP
                WiFi_Start_AP();
                delay(100);
            }
            delay(200);
#endif
        }
    }
}




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
    strcat(HTML_String, SCSet.ssid);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Password:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?Password_MY='+this.value)\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.password);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">time, trying to connect to network</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">trying to connect to 2nd network, after time has passed</td></tr>");
    strcat(HTML_String, "<tr><td><b>Timeout (s):</b></td><td><input type = \"number\" onchange=\"sendVal('/?timeoutRout='+this.value)\" name = \"timeoutRout\" min = \"20\" max = \"200\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, SCSet.timeoutRouter);
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
    strcat(HTML_String, SCSet.ssid_ap);
    strcat(HTML_String, "</b>     with no password<br><br><table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Network SSID 2:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?SSID_MY2='+this.value)\" style= \"width:200px\" name=\"SSID_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.ssid2);
    strcat(HTML_String, "\"></td>");

    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td><b>Password 2:</b></td>");
    strcat(HTML_String, "<td>");
    strcat(HTML_String, "<input type=\"text\" onchange=\"sendVal('/?Password_MY2='+this.value)\" style= \"width:200px\" name=\"Password_MY\" maxlength=\"22\" Value =\"");
    strcat(HTML_String, SCSet.password2);
    strcat(HTML_String, "\"></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">time, trying to connect to network</td></tr>");
    strcat(HTML_String, "<td colspan=\"3\">after time has passed access point is opened</td></tr>");
    strcat(HTML_String, "<tr><td><b>Timeout 2 (s):</b></td><td><input type = \"number\" onchange=\"sendVal('/?timeoutRout2='+this.value)\" name = \"timeoutRout\" min = \"5\" max = \"200\" step = \"1\" style= \"width:200px\" value = \"");// placeholder = \"");
    strcati(HTML_String, SCSet.timeoutRouter2);
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
    strcati(HTML_String, SCSet.timeoutWebIO);
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
    if (SCSet.DataTransVia == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">USB AOG V4</label></td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=1')\" name=\"DataTransfVia\" id=\"JZ\" value=\"1\"");
    if (SCSet.DataTransVia == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">USB AOG V5 (not working yet)</label></td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=7')\" name=\"DataTransfVia\" id=\"JZ\" value=\"7\"");
    if (SCSet.DataTransVia == 7)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\">WiFi (UDP) (default)</label></td></tr>");
    /*
        strcat(HTML_String, "<tr>");
        strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=8')\" name=\"DataTransfVia\" id=\"JZ\" value=\"8\"");
        if (SCSet.DataTransVia == 8)strcat(HTML_String, " CHECKED");
        strcat(HTML_String, "><label for=\"JZ\">WiFi (UDP) send message 2x</label></td></tr>");

        strcat(HTML_String, "<tr>");
        strcat(HTML_String, "<td></td><td><input type = \"radio\" onclick=\"sendVal('/?DataTransfVia=10')\" name=\"DataTransfVia\" id=\"JZ\" value=\"10\"");
        if (SCSet.DataTransVia == 10)strcat(HTML_String, " CHECKED");
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
       if (SCSet.LEDWiFi_ON_Level == 0)strcat(HTML_String, " CHECKED");
       strcat(HTML_String, "><label for=\"JZ\">LOW</label></td>");
       strcat(HTML_String, "<td><button style= \"width:120px\" name=\"ACTION\" value=\"");
       strcati(HTML_String, ACTION_SET_WiFiLEDon);
       strcat(HTML_String, "\">Apply and Save</button></td>");
       strcat(HTML_String, "</tr>");

       strcat(HTML_String, "<tr>");
       strcat(HTML_String, "<td></td><td><input type = \"radio\" name=\"WiFiLEDon\" id=\"JZ\" value=\"1\"");
       if (SCSet.LEDWiFi_ON_Level == 1)strcat(HTML_String, " CHECKED");
       strcat(HTML_String, "><label for=\"JZ\">HIGH</label></td></tr>");

       strcat(HTML_String, "</table>");
       strcat(HTML_String, "</form>");
       strcat(HTML_String, "<br><hr>");
       */

       //---------------------------------------------------------------------------------------------  
       // Number of sections
    
    strcat(HTML_String, "<h2>Number of sections</h2>");

    strcat(HTML_String, "<form>");
    strcat(HTML_String, "<table>");
    set_colgroup(250, 300, 150, 0, 0);

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\">Set number of sections (1-100).</td></tr>");
    strcat(HTML_String, "<tr><td><b>number of sections</b></td><td><input type = \"number\"  onchange=\"sendVal('/?SectNum='+this.value)\" name = \"SectNum\" min = \"1\" max = \"100\" step = \"1\" style= \"width:200px\" value = \"");
    strcati(HTML_String, SCSet.SectNum);
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
    if (SCSet.SectRelaysInst == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> installed</label>");
    strcat(HTML_String, "</td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td>if installed</td><td><input type = \"radio\" onclick=\"sendVal('/?seRelON=0')\" name=\"seRelON\" id=\"JZ\" value=\"0\"");
    if (SCSet.SectRelaysON == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\"> LOW</label></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<td></td><td>relays spray on</td><td><input type = \"radio\" onclick=\"sendVal('/?seRelON=1')\" name=\"seRelON\" id=\"JZ\" value=\"1\"");
    if (SCSet.SectRelaysON == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\"> HIGH</label></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

    //switches
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"2\"><b>Switches for sections</b></td><td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?seSWInst='+this.checked)\" name=\"seSWInst\" id = \"Part\" value = \"1\" ");
    if (SCSet.SectSWInst == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> installed</label>");
    strcat(HTML_String, "</td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td>if installed</td><td><input type = \"radio\" onclick=\"sendVal('/?DocSWspr=0')\" name=\"DocSWspr\" id=\"JZ\" value=\"0\"");
    if (SCSet.SectSWAutoOrOn == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\"> LOW</label></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<td></td><td>switches spray on</td><td><input type = \"radio\" onclick=\"sendVal('/?DocSWspr=1')\" name=\"DocSWspr\" id=\"JZ\" value=\"1\"");
    if (SCSet.SectSWAutoOrOn == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"JZ\"> HIGH</label></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

    strcat(HTML_String, "<tr><td colspan=\"4\"><b>Use as section control, or documentation<br>doc = AgOpenGPS writes status of input switches.</b></td></tr>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"2\"></td><td colspan=\"2\"><input type=\"checkbox\" onclick=\"sendVal('/?DocOnly='+this.checked)\" name=\"DocOnly\" id = \"Part\" value = \"1\" ");
    if (SCSet.DocOnly == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> Documentation only</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");


    //rate SW
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"4\"><b>Rate Control Switch only for +/- Motor valve</b></td>");
    strcat(HTML_String, "<tr><td colspan=\"2\"></td><td><input type = \"checkbox\" onclick=\"sendVal('/?RCSWInst='+this.checked)\" name=\"RCSWInst\" id = \"RCS\" value = \"1\" ");
    if (SCSet.RateSWLeftInst == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"RCS\"> installed</label>");
    strcat(HTML_String, "</td>");
    strcat(HTML_String, "<td></td><td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

    //main SW
    strcat(HTML_String, "<tr><td colspan=\"4\"><b>Main switch for section control</b></td>");
    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=0')\" name=\"MainSW\" id=\"MS\" value=\"0\"");
    if (SCSet.SectMainSWType == 0)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"MS\"> none</label></td>");
    strcat(HTML_String, "<td><input type= \"button\" onclick= \"sendVal('/?Save=true')\" style= \"width:120px\" value=\"Save\"></button></td>");
    strcat(HTML_String, "</tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=1')\" name=\"MainSW\" id=\"MS\" value=\"1\"");
    if (SCSet.SectMainSWType == 1)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"MS\"> (ON)-OFF-(ON) toggle switch or push buttons</label></td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=2')\" name=\"MainSW\" id=\"MS\" value=\"2\"");
    if (SCSet.SectMainSWType == 2)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"MS\"> connected to hitch level sensor</label></td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td></td><td colspan=\"3\"><input type = \"radio\" onclick=\"sendVal('/?MainSW=3')\" name=\"MainSW\" id=\"MS\" value=\"3\"");
    if (SCSet.SectMainSWType == 3)strcat(HTML_String, " CHECKED");
    strcat(HTML_String, "><label for=\"MS\"> inverted hitch level sensor</label></td></tr>");

    strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

    //analog MAIN SW from hitch level
    if (SCSet.SectMainSWType > 1) {
        strcat(HTML_String, "<tr>");
        strcat(HTML_String, "<td colspan=\"4\"><br>Analog Main Section control Threshold value</td></tr>");
        strcat(HTML_String, "<tr> <td colspan=\"3\">&nbsp;</td> </tr>");

        strcat(HTML_String, "<tr><td></td><td><divbox align=\"right\"><font size=\"+2\"><b>");
        strcati(HTML_String, (SCSet.HitchLevelVal));
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
    if (SCSet.debugmode == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> general debugmode on</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmSW='+this.checked)\" name=\"debugmSW\" id = \"Part\" value = \"1\" ");
    if (SCSet.debugmodeSwitches == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> debugmode switches on (display switch status)</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmRel='+this.checked)\" name=\"debugmRel\" id = \"Part\" value = \"1\" ");
    if (SCSet.debugmodeRelay == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> debugmode relais on</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmDatFromAOG='+this.checked)\" name=\"debugmDatFromAOG\" id = \"Part\" value = \"1\" ");
    if (SCSet.debugmodeDataFromAOG == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> debugmode data from AgOpenGPS on</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "<tr>");
    strcat(HTML_String, "<td colspan=\"3\"><input type=\"checkbox\" onclick=\"sendVal('/?debugmDatToAOG='+this.checked)\" name=\"debugmDatToAOG\" id = \"Part\" value = \"1\" ");
    if (SCSet.debugmodeDataToAOG == 1) strcat(HTML_String, "checked ");
    strcat(HTML_String, "> ");
    strcat(HTML_String, "<label for =\"Part\"> debugmode data to AgOpenGPS on</label>");
    strcat(HTML_String, "</td></tr>");

    strcat(HTML_String, "</table></form><br><hr>");


        //-------------------------------------------------------------
        // firmware update
    
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



//-------------------------------------------------------------------------------------------------

void handleNotFound() {
    const char* notFound =
        "<!doctype html>"
        "<html lang = \"en\">"
        "<head>"""
        "<meta charset = \"utf - 8\">"
        "<meta http - equiv = \"x - ua - compatible\" content = \"ie = edge\">"
        "<meta name = \"viewport\" content = \"width = device - width, initial - scale = 1.0\">"
        "<title>Redirecting</title>"
        "</head>"
        "<body onload = \"redirect()\">"
        "<h1 style = \"text - align: center; padding - top: 50px; display: block; \"><br>404 not found<br><br>Redirecting to settings page in 3 secs ...</h1>"
        "<script>"
        "function redirect() {"
        "setTimeout(function() {"
        "    window.location.replace(\"/root\");"//new landing page
        "}"
        ", 3000);"
        "}"
        "</script>"
        "</body>"
        "</html>";

    server.sendHeader("Connection", "close");
    server.send(200, "text/html", notFound);
    if (SCSet.debugmode) { Serial.println("redirecting from 404 not found to Webpage root"); }
}

//-------------------------------------------------------------------------------------------------

void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
    strcat(HTML_String, "<colgroup>");
    set_colgroup1(w1);
    set_colgroup1(w2);
    set_colgroup1(w3);
    set_colgroup1(w4);
    set_colgroup1(w5);
    strcat(HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
    if (ww == 0) return;
    strcat(HTML_String, "<col width=\"");
    strcati(HTML_String, ww);
    strcat(HTML_String, "\">");
}
//---------------------------------------------------------------------
void strcatf(char* tx, float f, byte leng, byte dezim) {
    char tmp[8];
#if HardwarePlatform == 0
    dtostrf(f, leng, dezim, tmp);//f,6,2,tmp
#endif
    strcat(tx, tmp);
}
//---------------------------------------------------------------------
void strcati(char* tx, int i) {
    char tmp[8];

    itoa(i, tmp, 10);
    strcat(tx, tmp);
}


/*
//----------------------------------------------------------------------------------------------
void set_colgroup(int w1, int w2, int w3, int w4, int w5) {
	strcat(HTML_String, "<colgroup>");
	set_colgroup1(w1);
	set_colgroup1(w2);
	set_colgroup1(w3);
	set_colgroup1(w4);
	set_colgroup1(w5);
	strcat(HTML_String, "</colgroup>");

}
//------------------------------------------------------------------------------------------
void set_colgroup1(int ww) {
	if (ww == 0) return;
	strcat(HTML_String, "<col width=\"");
	strcati(HTML_String, ww);
	strcat(HTML_String, "\">");
}


//---------------------------------------------------------------------
void strcatf(char* tx, float f, byte leng, byte dezim) {
	char tmp[8];

//	dtostrf(f, leng, dezim, tmp);//f,6,2,tmp
	strcat(tx, tmp);
}
//---------------------------------------------------------------------
//void strcatl(char* tx, long l) {
  //char tmp[sizeof l];
  //memcpy(tmp, l, sizeof l);
  //strcat (tx, tmp);
//}

//---------------------------------------------------------------------
void strcati(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
void strcati2(char* tx, int i) {
	char tmp[8];

	itoa(i, tmp, 10);
	if (strlen(tmp) < 2) strcat(tx, "0");
	strcat(tx, tmp);
}

//---------------------------------------------------------------------
int Pick_Parameter_Zahl(const char* par, char* str) {
	int myIdx = Find_End(par, str);

	if (myIdx >= 0) return  Pick_Dec(str, myIdx);
	else return -1;
}
//---------------------------------------------------------------------
int Find_End(const char* such, const char* str) {
	int tmp = Find_Start(such, str);
	if (tmp >= 0)tmp += strlen(such);
	return tmp;
}

//---------------------------------------------------------------------
int Find_Start(const char* such, const char* str) {
	int tmp = -1;
	int ww = strlen(str) - strlen(such);
	int ll = strlen(such);

	for (int i = 0; i <= ww && tmp == -1; i++) {
		if (strncmp(such, &str[i], ll) == 0) tmp = i;
	}
	return tmp;
}
//---------------------------------------------------------------------
int Pick_Dec(const char* tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && (tx[p] >= '0' && tx[p] <= '9'); p++) {
		tmp = 10 * tmp + tx[p] - '0';
	}
	return tmp;
}
//----------------------------------------------------------------------------
int Pick_N_Zahl(const char* tx, char separator, byte n) {//never used?

	int ll = strlen(tx);
	int tmp = -1;
	byte anz = 1;
	byte i = 0;
	while (i < ll && anz < n) {
		if (tx[i] == separator)anz++;
		i++;
	}
	if (i < ll) return Pick_Dec(tx, i);
	else return -1;
}

//---------------------------------------------------------------------
int Pick_Hex(const char* tx, int idx) {
	int tmp = 0;

	for (int p = idx; p < idx + 5 && ((tx[p] >= '0' && tx[p] <= '9') || (tx[p] >= 'A' && tx[p] <= 'F')); p++) {
		if (tx[p] <= '9')tmp = 16 * tmp + tx[p] - '0';
		else tmp = 16 * tmp + tx[p] - 55;
	}

	return tmp;
}

//---------------------------------------------------------------------
void Pick_Text(char* tx_ziel, char* tx_quelle, int max_ziel) {

	int p_ziel = 0;
	int p_quelle = 0;
	int len_quelle = strlen(tx_quelle);

	while (p_ziel < max_ziel && p_quelle < len_quelle && tx_quelle[p_quelle] && tx_quelle[p_quelle] != ' ' && tx_quelle[p_quelle] != '&') {
		if (tx_quelle[p_quelle] == '%') {
			tx_ziel[p_ziel] = (HexChar_to_NumChar(tx_quelle[p_quelle + 1]) << 4) + HexChar_to_NumChar(tx_quelle[p_quelle + 2]);
			p_quelle += 2;
		}
		else if (tx_quelle[p_quelle] == '+') {
			tx_ziel[p_ziel] = ' ';
		}
		else {
			tx_ziel[p_ziel] = tx_quelle[p_quelle];
		}
		p_ziel++;
		p_quelle++;
	}

	tx_ziel[p_ziel] = 0;
}
//---------------------------------------------------------------------
char HexChar_to_NumChar(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 55;
	return 0;
}
//---------------------------------------------------------------------
void exhibit(const char* tx, int v) {
	Serial.print(tx);
	Serial.print(v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, unsigned int v) {
	Serial.print(tx);
	Serial.print((int)v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, unsigned long v) {
	Serial.print(tx);
	Serial.print((long)v, 1);
}
//---------------------------------------------------------------------
void exhibit(const char* tx, const char* v) {
	Serial.print(tx);
	Serial.print(v);
}
*/