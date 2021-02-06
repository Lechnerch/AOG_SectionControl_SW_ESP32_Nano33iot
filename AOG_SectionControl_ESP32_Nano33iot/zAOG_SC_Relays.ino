void SetRelays(void)
{
	if (SCSet.SectNum > 8) {

		for (byte i = 0; i < 8; i++)
		{
			if (bitRead(RelayOUT[0], i) == 0) {
        pinMode(SCSet.Relay_PIN[i], OUTPUT);
				digitalWrite(SCSet.Relay_PIN[i], LOW);
     Serial.print(bitRead(RelayOUT[0], i));
      Serial.print(": ");
     Serial.print(SCSet.Relay_PIN[i]);
     Serial.println(" LOW"); 
			}
			else {
        pinMode(SCSet.Relay_PIN[i], OUTPUT);
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
     Serial.print(bitRead(RelayOUT[0], i));
      Serial.print(": ");
      Serial.print(SCSet.Relay_PIN[i]);
      Serial.println(" HIGH"); 
			}
		}
		for (byte i = 8; i < SCSet.SectNum; i++)
		{
			if (bitRead(RelayOUT[1], (i - 8)) == 0) {
        pinMode(SCSet.Relay_PIN[i], OUTPUT);
				digitalWrite(SCSet.Relay_PIN[i], LOW);
     Serial.print(bitRead(RelayOUT[1], i));
      Serial.print(": ");
     Serial.print(SCSet.Relay_PIN[i]);
     Serial.println(" LOW"); 
			}
			else {
        pinMode(SCSet.Relay_PIN[i], OUTPUT);
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
     Serial.print(bitRead(RelayOUT[1], i));
      Serial.print(": ");
      Serial.print(SCSet.Relay_PIN[i]);
      Serial.println(" HIGH"); 
  			}
		}
		if (SCSet.debugmodeRelay) {
			Serial.print("numbSect: "); Serial.print(SCSet.SectNum);
			Serial.print("  byte Relays 16-8: "); Serial.print(RelayOUT[1]);
			Serial.print("  byte Relays 8-1: "); Serial.println(RelayOUT[0]);
		}
	}
	else
	{
		for (byte i = 0; i < SCSet.SectNum; i++)
		{
			if (bitRead(RelayOUT[0], i) == LOW) {
				digitalWrite(SCSet.Relay_PIN[i], LOW);
			}
			else {
				digitalWrite(SCSet.Relay_PIN[i], HIGH);
			}
		}
		//Serial.println(RelayOUT[0]);
	}
}
