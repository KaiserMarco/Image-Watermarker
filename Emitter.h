#ifndef EMITTER_H_
#define EMITTER_H_

#include <queue>
#include <string>
#include <vector>
#include <iostream>

#include "Message.h"
#include "Node.h"
#include "Timer.h"

using namespace std;

namespace iwm {

	class Emitter : public Node {
		private:
			vector<Message*>* messages;
			int iterations;
			Timer* timer;

		public:
			Emitter( char* imagesDir, int iterations, Timer* timer, vector<Message*>* messages );
			virtual ~Emitter();

			void run();
	};

}

#endif /* EMITTER_H_ */
