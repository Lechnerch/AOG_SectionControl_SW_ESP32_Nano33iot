void getDataFromAOG()
{
    byte incomingBytes[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 }, incomingByteNum = 0, tempByt = 0;
    int tempInt = 0;
    float tempFlo = 0;
   // bool steerSettingChanged = false;


    //get new AOG Data
    //USB
    if ((Set.DataTransVia == 0) || (Set.DataTransVia == 1)) {

        while (Serial.available())
        {
            incomingByte = Serial.read();
            if (NewDataFromAOG || NewSettingFromAOG)
            {
                DataFromAOG[incomingByteNum] = incomingByte;
                incomingByteNum++;
                if (Set.debugmodeDataFromAOG) {
                    Serial.print(incomingByte); Serial.print(" ");
                }
                if (incomingByteNum > 10) { break; }//sentence too long
            }
            else {//sentence not started yet
                if ((incomingByte == Set.DataFromAOGHeader[0]) || (incomingByte == Set.SettingsFromAOGHeader[0]))
                {
                    incomingByte = Serial.read();
                    if (incomingByte == Set.DataFromAOGHeader[1]) {
                        NewDataFromAOG = true;
                        DataFromAOG[0] = Set.DataFromAOGHeader[0];
                        DataFromAOG[1] = Set.DataFromAOGHeader[1];
                        incomingByteNum = 2;
                    }
                    if (incomingByte == Set.SettingsFromAOGHeader[1]) {
                        NewSettingFromAOG = true;
                        DataFromAOG[0] = Set.SettingsFromAOGHeader[0];
                        DataFromAOG[1] = Set.SettingsFromAOGHeader[1];
                        incomingByteNum = 2;
                    }
                    if (Set.debugmodeDataFromAOG) { Serial.print("data from AOG via USB: "); }
                }
            }
        }
    }//end USB
    
    //WiFi UDP 
    if ((Set.DataTransVia == 7))
    {
        //Serial.println("checking for UDP packet");
        byte leng = UDPFromAOG.parsePacket();
        //check packet length and process only fitting ones
        if ((leng > 0) && (Set.debugmodeDataFromAOG)) { Serial.print("UDP packet found, lenght: "); Serial.println(leng); }
        if ((leng >= 6) && (leng <= 12))
        {
            UDPFromAOG.read(incomingBytes, leng);
            if ((incomingBytes[0] == Set.DataFromAOGHeader[0]) && (incomingBytes[1] == Set.DataFromAOGHeader[1]))
            {
                NewDataFromAOG = true;
            }
            if ((incomingBytes[0] == Set.SettingsFromAOGHeader[0]) && (incomingBytes[1] == Set.SettingsFromAOGHeader[1]))
            {
                NewSettingFromAOG = true;
            }
            if ((NewDataFromAOG) || (NewSettingFromAOG)) {
                if (Set.debugmodeDataFromAOG) { Serial.print("data from AOG via UDP: "); }
                for (byte n = 0; n < leng; n++) {
                    if (Set.debugmodeDataFromAOG) { Serial.print(incomingBytes[n]); Serial.print(" "); }
                    DataFromAOG[n] = incomingBytes[n];
                }
                if (Set.debugmodeDataFromAOG) { Serial.println(); }
            }
        }
    }//end UDP

    //DATA Header 
    if (NewDataFromAOG)  //Do we have a match?
    {
        RelayFromAOG[1] = DataFromAOG[2];   // read relay control from AgOpenGPS
        RelayFromAOG[0] = DataFromAOG[3];
        groundSpeed = DataFromAOG[4] >> 2;  //actual speed times 4, single byte
        uTurnRelay = DataFromAOG[5];
        //[6] tree
        //[7] hydraulik lift
        //reset watchdog as we just heard from AgOpenGPS
        DataFromAOGTime = millis();
        NewDataFromAOG = false;
    }
#if HardwarePlatform == 1 //nano33iot
    else delay(5);//do WiFi
#endif
        
        
    //SETTINGS Header has been found, 6 bytes are the settings
    if (NewSettingFromAOG) //actual no rate control in AOG, so not used
    {
        //accumulated volume, 0 it if this is 32700 sent
        float tempf = (float)(DataFromAOG[2] << 8 | DataFromAOG[3]);   //high,low bytes
        if (tempf == 32700) accumulatedCountsDual = 0;

        //flow meter cal factor in counts per Liter
        flowmeterCalFactorLeft = ((float)(DataFromAOG[4] << 8 | DataFromAOG[5]));   //high,low bytes
        flowmeterCalFactorRight = ((float)(DataFromAOG[6] << 8 | DataFromAOG[7]));   //high,low bytes
        NewSettingFromAOG = false;
    }

}

//-------------------------------------------------------------------------------------------------

void AOGDataSend()
{
    //USB
    if ((Set.DataTransVia == 0) || (Set.DataTransVia == 1)) {
        byte idx;
       // if (Set.DataTransVia == 0) { idx = 2; }//8 bytes
      //  else {  }//V4 10 Bytes
    idx = 0;
        for (idx; idx < DataToAOGLength; idx++) {
            Serial.print(RelayToAOG[idx]); 
            if (idx < (DataToAOGLength - 1)) { Serial.print(","); }
        }
        Serial.println();
    }
    /*
#if HardwarePlatform == 0
    if (Set.DataTransVia == 7)
    {
        UDPToAOG.writeTo(RelayToAOG, DataToAOGLength, WiFi_ipDestination, Set.PortDestination);
    }
#endif
 */   
    if (Set.DataTransVia == 7)
    {
        UDPToAOG.beginPacket(Set.WiFi_ipDestination, Set.PortDestination);
        for (byte idx = 0; idx < DataToAOGLength; idx++) {
            UDPToAOG.write(RelayToAOG[idx]);
        }
        UDPToAOG.endPacket();  
    }
}

