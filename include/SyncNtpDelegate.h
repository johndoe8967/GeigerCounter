#ifndef INCLUDE_SYNCNTPDELEGATE_H_
#define INCLUDE_SYNCNTPDELEGATE_H_


class SyncNTP
{
public:
	SyncNTP()
	{
		ntpcp = new NtpClient("pool.ntp.org", 60, NtpTimeResultDelegate(&SyncNTP::ntpResult, this));
	};

	void ntpResult(NtpClient& client, time_t ntpTime)
	{
		SystemClock.setTime(ntpTime, eTZ_UTC);
		Debug.printf("ntpClientDemo Callback Time_t = %ld Time = %s\r\n",ntpTime,SystemClock.getSystemTimeString().c_str());
	}

private:
	NtpClient *ntpcp;
};

#endif /* INCLUDE_SYNCNTPDELEGATE_H_ */
