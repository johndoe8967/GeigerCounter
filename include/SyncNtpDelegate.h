#ifndef INCLUDE_SYNCNTPDELEGATE_H_
#define INCLUDE_SYNCNTPDELEGATE_H_

/*
 * File: SyncNtpDelegate.h
 * NTP time synchronization
 * 	each 5min
 * 	returns valid on first sync
 */

class SyncNTP
{
public:
	SyncNTP()
	{
		ntpcp = new NtpClient("pool.ntp.org", 300, NtpTimeResultDelegate(&SyncNTP::ntpResult, this));
	};
	virtual ~SyncNTP() {
		delete(ntpcp);
	}

	void ntpResult(NtpClient& client, time_t ntpTime)
	{
		time_t time = RTC.getRtcSeconds();
		Debug.printf("ntpClient TimeDiff = %lu\r\n",time-ntpTime);
		SystemClock.setTime(ntpTime, eTZ_UTC);
		Debug.printf("ntpClient Time = %s\r\n",SystemClock.getSystemTimeString().c_str());
		valid = true;
	}
	bool valid = false;

private:
	NtpClient *ntpcp;
};

#endif /* INCLUDE_SYNCNTPDELEGATE_H_ */
