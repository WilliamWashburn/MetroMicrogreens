#include "Arduino.h"
#include "customTimer.h"

customTimer::customTimer() {

}

customTimer::customTimer(int interval, int duration) {
	_interval = interval;
	_timeToWait = 0;
	_duration = duration;
	if (_interval > 0) {
		_enable = true;
		_timeToWait = 86400000L / interval;
		Serial.println("timer enabled");
	}
	else {
		_enable = false;
		Serial.println("timer disabled");
	}
	_lastTriggered = millis();
}

void customTimer::changeInterval(int newInterval){
	_interval = newInterval;

	if (_interval > 0) {
		if (_enable == false){
			Serial.println("timer enabled");
			_lastTriggered = millis();
		}
		else{
			Serial.println("interval updated");
		}

		_enable = true;
		_timeToWait = 86400000L / _interval;
		Serial.println("The time between timers is set to " + String(_timeToWait) + " milliseconds");
	}
	else {
		_enable = false;
		Serial.println("timer disabled");
	}
}

void customTimer::changeDuration(int newDuration){
	_duration = newDuration;
	Serial.println("duration updated to " + String(_duration) + " milliseconds");
}

bool customTimer::check(){
	bool shouldTrigger = false;
	if (_enable == true) {
		long now = millis();
		if (now - _lastTriggered > _timeToWait) {
			_lastTriggered = now;
			shouldTrigger = true;
		}
	}
	return shouldTrigger;
}

bool customTimer::isEnabled() {
	bool isenabled = false;
	if (_enable == true) {
		isenabled = true;
	}
	return isenabled;
}

void customTimer::setTimeToWait(long timeToWait) {
	_timeToWait = timeToWait;
}
