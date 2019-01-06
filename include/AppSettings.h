/*
 * AppSettings.h
 *
 * Applikation settings are stored in spiffs
 *
 * Created on June 5, 2016
 *      Original Author: Anakod
 *      Modified by https://github.com/johndoe8967
 */

//#define USEPWM
#include <SmingCore/SmingCore.h>

#ifndef INCLUDE_APPSETTINGS_H_
#define INCLUDE_APPSETTINGS_H_

#define APP_SETTINGS_FILE ".settings.conf" // leading point for security reasons :)

struct ApplicationSettingsStorage
{
	unsigned int measureTime = 60;
	float doseRatio = 120;
	String tsAPI;
	String tsAPIDust;
	String RadmonUser;
	String RadmonPWD;
	String WLANSSID;
	String WLANPWD;

	void load()
	{
		debugf("Appsettings load\r\n");
		DynamicJsonBuffer jsonBuffer;
		if (exist())
		{
			debugf("Appsettings exist\r\n");
			int size = fileGetSize(APP_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(APP_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& geiger = root["geiger"];
			measureTime = geiger["measureTime"];
			doseRatio = geiger["doseRatio"];
			tsAPI = geiger["thingspeak"].asString();
			tsAPIDust = geiger["thingspeakDust"].asString();
			RadmonUser = geiger["radmonuser"].asString();
			RadmonPWD = geiger["radmonpwd"].asString();
			WLANSSID = geiger["WLANSSID"].asString();
			WLANPWD = geiger["WLANPWD"].asString();

			delete[] jsonString;
		}
	}

	void save()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();

		JsonObject& geiger = jsonBuffer.createObject();
		root["geiger"] = geiger;
		geiger["measureTime"] = measureTime;
		geiger["doseRatio"] = doseRatio;
		geiger["thingspeak"] = tsAPI;
		geiger["thingspeakDust"] = tsAPIDust;
		geiger["radmonuser"] = RadmonUser;
		geiger["radmonpwd"] = RadmonPWD;
		geiger["WLANSSID"] = WLANSSID;
		geiger["WLANPWD"] = WLANPWD;

		String rootString;
		root.printTo(rootString);
		fileSetContent(APP_SETTINGS_FILE, rootString);
		debugf("file: %s",rootString.c_str());
	}

	bool exist() { return fileExist(APP_SETTINGS_FILE); }
};
extern ApplicationSettingsStorage AppSettings;

#endif /* INCLUDE_APPSETTINGS_H_ */
