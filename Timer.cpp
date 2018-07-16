#include "Timer.h"

namespace iwm {

	Timer::Timer() {}

	double Timer::getDuration() {
		return this->elapsed.count();
	}

	void Timer::startTime() {
		this->time = system_clock::now();
	}

	void Timer::stopTime(){
		this->elapsed = system_clock::now() - time;
	}

	Timer::~Timer() {}

} /* namespace iwm */
