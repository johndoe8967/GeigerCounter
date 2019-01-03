// SDS011 dust sensor PM2.5 and PM10
// ---------------------
//
// By R. Zschiegner (rz@madavi.de)
// April 2016
//
// Documentation:
//		- The iNovaFitness SDS011 datasheet
//

#include "SDS011.h"

static const byte SLEEPCMD[19] = {
	0xAA,	// head
	0xB4,	// command id
	0x06,	// data byte 1
	0x01,	// data byte 2 (set mode)
	0x00,	// data byte 3 (sleep)
	0x00,	// data byte 4
	0x00,	// data byte 5
	0x00,	// data byte 6
	0x00,	// data byte 7
	0x00,	// data byte 8
	0x00,	// data byte 9
	0x00,	// data byte 10
	0x00,	// data byte 11
	0x00,	// data byte 12
	0x00,	// data byte 13
	0xFF,	// data byte 14 (device id byte 1)
	0xFF,	// data byte 15 (device id byte 2)
	0x05,	// checksum
	0xAB	// tail
};

SDS011::SDS011(void) {

}

// --------------------------------------------------------
// SDS011:read
// --------------------------------------------------------
bool SDS011::read(float &p25, float &p10) {

	p25 = this->p25;
	p10 = this->p10;
	return newData;
}

// --------------------------------------------------------
// SDS011:sleep
// --------------------------------------------------------
void SDS011::sleep() {
	for (uint8_t i = 0; i < 19; i++) {
		sds_data->write(SLEEPCMD[i]);
	}
	sds_data->flush();
}



// --------------------------------------------------------
// SDS011:wakeup
// --------------------------------------------------------
void SDS011::wakeup() {
	sds_data->write(0x01);
	sds_data->flush();
}

void SDS011::receive(Stream& stream, char arrivedChar,
		uint16_t availableCharsCount) {
	byte buffer;
	int value;
	int checksum_ok = 0;

	while ((stream.available() > 0) && (stream.available() >= (10-this->len))) {
		buffer = stream.read();
		value = int(buffer);
		switch (len) {
			case (0): if (value != 170) { len = -1; }; break;
			case (1): if (value != 192) { len = -1; }; break;
			case (2): p25_serial = value; checksum_is = value; break;
			case (3): p25_serial += (value << 8); checksum_is += value; break;
			case (4): p10_serial = value; checksum_is += value; break;
			case (5): p10_serial += (value << 8); checksum_is += value; break;
			case (6): checksum_is += value; break;
			case (7): checksum_is += value; break;
			case (8): if (value == (checksum_is % 256)) { checksum_ok = 1; } else { len = -1; }; break;
			case (9): if (value != 171) { len = -1; }; break;
		}
		if (len == -1) {
			error = 1;
		}
		len++;
		if (len == 10 && checksum_ok == 1) {
			newData = true;
			p25 = p25_serial/10;
			p10 = p10_serial/10;
			len = 0; checksum_ok = 0; checksum_is = 0;
			error = 0;
		}
	}
}

void SDS011::begin(HardwareSerial* serial) {
	serial->begin(9600);
	this->receiveDelegate = StreamDataReceivedDelegate(&SDS011::receive,this);
	serial->setCallback(this->receiveDelegate,true);
	sds_data = serial;
}

