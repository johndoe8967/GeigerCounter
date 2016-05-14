#ifndef INCLUDE_SYNCNTPDELEGATE_H_
#define INCLUDE_SYNCNTPDELEGATE_H_


class SyncNTP
{
public:
	SyncNTP()
	{
		ntpcp = new NtpClient("pool.ntp.org", 30, NtpTimeResultDelegate(&SyncNTP::ntpResult, this));
	};

	void ntpResult(NtpClient& client, time_t ntpTime)
	{
		SystemClock.setTime(ntpTime, eTZ_UTC);
		Serial.print("ntpClientDemo Callback Time_t = ");
		Serial.print(ntpTime);
		Serial.print(" Time = ");
		Serial.println(SystemClock.getSystemTimeString());
	}

private:
	NtpClient *ntpcp;
};

#endif /* INCLUDE_SYNCNTPDELEGATE_H_ */
