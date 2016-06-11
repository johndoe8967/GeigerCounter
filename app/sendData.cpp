/*
 * sendData.cpp
 * Original Author: https://github.com/johndoe8967
 *
 * sends data to ThingSpeak
 * expects 3 fields:
 * 		CPM, dose, RSSI
 *
 * sends data to RadMon
 * 		based on GKMon, rewritten to use in Sming framework
 *
 *  Created on: 15.05.2016
 */
#include "../include/sendData.h"
#include <SmingCore/Debug.h>
#include "../include/AppSettings.h"


#define useRadmon
#ifdef useRadmon
String RadmonHost = "http://radmon.org";     // no need to change this
HttpClient radmon;
#endif

#define useThingSpeak
#ifdef useThingSpeak
String ThingSpeakHost = "http://api.thingspeak.com";  // no need to change this
HttpClient thingSpeak;
Timer delayThingSpeak;
#endif


String url;
float cpm;
float dose;


void onDataSent(HttpClient& client, bool successful)
{
	if (successful)
		Debug.printf("Success sent\r\n");
	else
		Debug.printf("Failed\r\n");

	Debug.printf("Server response: '%s'\r\n",client.getResponseString().c_str());
}


#ifdef useThingSpeak
void sendThingSpeak () {
		if (thingSpeak.isProcessing()) {
			Debug.print("!!!!ThingSpeak not ready -> close");
			thingSpeak.reset();
		} else {
			Debug.print("Delayed sebd ThingSpeak\r\n");
			url = ThingSpeakHost;
			url += "/update?key=";
			url += AppSettings.tsAPI;
			url += "&field1=";
			url += cpm;
			url += "&field2=";
			url += dose;
			url += "&field3=";
			url += WifiStation.getRssi();
			url += "&created_at=";
			url += SystemClock.now(eTZ_UTC).toISO8601();
			thingSpeak.downloadString(url, onDataSent);
		}

}
#endif

void sendData(uint32 events, uint32 intervall, bool send) {

	cpm = float(events)/ (float(intervall)/60000000.0);
	dose = cpm / AppSettings.doseRatio;


	Debug.printf ("CPM: %f Dose: %f Time: %s\r\n", cpm, dose, SystemClock.now(eTZ_UTC).toISO8601().c_str());

	if (send) {
#ifdef useRadmon
		if (radmon.isProcessing()) {
			Debug.print("!!!!RadMon not ready -> close\r\n");
			radmon.reset();
		} else {
			Debug.print("Send Radmon\r\n");
			url = RadmonHost;
			url += "/radmon.php?function=submit&user=";
			url += AppSettings.RadmonUser;
			url += "&password=";
			url += AppSettings.RadmonPWD;
			url += "&value=";
			url += cpm;
			url += "&unit=CPM";
			radmon.downloadString(url, onDataSent);
		}
#endif

#ifdef useThingSpeak
		delayThingSpeak.initializeMs(5000,TimerDelegate(&sendThingSpeak)).startOnce();
#endif
	}
}


