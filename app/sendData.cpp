/*
 * sendData.cpp
 *
 *  Created on: 15.05.2016
 *      Author: johndoe
 */
#include "../include/sendData.h"
#include <SmingCore/Debug.h>
#include "../include/AppSettings.h"


#define useRadmon
#ifdef useRadmon
String RadmonHost = "http://radmon.org";     // no need to change this
HttpClient radmon;
#endif

//#define useThingSpeak
#ifdef useThingSpeak
String ThingSpeakHost = "http://api.thingspeak.com";  // no need to change this
HttpClient thingSpeak;
#endif

void onDataSent(HttpClient& client, bool successful)
{
	if (successful)
		Debug.printf("Success sent\r\n");
	else
		Debug.printf("Failed\r\n");

	String response = client.getResponseString();
	Debug.printf("Server response: '%s'\r\n",response.c_str());
	if (response.length() > 0)
	{
		int intVal = response.toInt();

		if (intVal == 0)
			Debug.printf("Sensor value wasn't accepted. May be we need to wait a little?\r\n");
	}
}


void sendData(uint32 events, uint32 intervall, bool send) {

	float cpm = float(events)/ (float(intervall)/60000000.0);
	float dose = cpm / AppSettings.doseRatio;


	Debug.printf ("CPM: %f Dose: %f Time: %s\r\n", cpm, dose, SystemClock.now(eTZ_UTC).toISO8601().c_str());

	String url;
	if (send) {
#ifdef useRadmon
		if (radmon.isProcessing()) return;
		url = RadmonHost;
		url += "/radmon.php?function=submit&user=";
		url += AppSettings.RadmonUser;
		url += "&password=";
		url += AppSettings.RadmonPWD;
		url += "&value=";
		url += cpm;
		url += "&unit=CPM";
		radmon.downloadString(url, onDataSent);
#endif
#ifdef useThingSpeak
		if (thingSpeak.isProcessing()) return; // We need to wait while request processing was completed
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
#endif
	}
}


