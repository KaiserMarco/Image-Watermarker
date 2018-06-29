#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

using namespace std;
using namespace chrono;

namespace iwm {

	typedef time_point<system_clock> clock;

	class Timer {
		private:
			clock time;
			duration<double> elapsed;

		public:
			Timer();
			virtual ~Timer();

			double getDuration();

			void startTime();

			void stopTime();
	};
} /* namespace iwm */

#endif /* TIMER_H_ */
