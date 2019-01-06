// SDS011 dust sensor PM2.5 and PM10
// ---------------------------------
//
// By R. Zschiegner (rz@madavi.de)
// April 2016
//
// Documentation:
//		- The iNovaFitness SDS011 datasheet
//
#include <SmingCore/SmingCore.h>
#include <Debug.h>


class SDS011 {
	public:
		SDS011(void);
		void begin(HardwareSerial* serial);
		bool read(float &p25, float &p10);
		void sleep();
		void wakeup();
		void receive(Stream &stream, char arrivedChar, uint16_t availableCharsCount);
	private:
		Stream *sds_data;
		StreamDataReceivedDelegate receiveDelegate;
		float p25_serial = 0.0;
		float p10_serial = 0.0;
		float p25;
		float p10;
		int checksum_is = 0;
		int len = 0;
		int error = 1;
		bool newData = false;


};
