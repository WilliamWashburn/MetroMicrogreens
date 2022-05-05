#ifndef customTimer_h
#define customTimer_h

#include "Arduino.h"

class customTimer {
public:
	customTimer();
	customTimer(int interval, int duration);
	void changeInterval(int newInterval);
	void changeDuration(int newDuration);
	bool check();
	bool isEnabled();
	void setTimeToWait(long timeToWait);

private:
	int _interval;
	int _duration;
	bool _enable;
	long _timeToWait;
	long _lastTriggered;
};

#endif
