/*
 * Queuet.h
 *
 *  Created on: 25 giu 2018
 *      Author: marco
 */

#ifndef QUEUET_H_
#define QUEUET_H_

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Message.h"

using namespace std;

namespace iwm {

	class Queue_t {
		private:
			queue<Message*>* que;
			mutex mux;
			condition_variable cv;

			long id;
			static long nextId;

		public:
			Queue_t();
			virtual ~Queue_t();

			long getId();

			void sendMessage( Message* mex );
			Message* receiveMessage();

		private:
			static long generateId();
	};

} /* namespace iwm */

#endif /* QUEUET_H_ */
