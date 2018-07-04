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
			int imageCopies;
			Timer* timer;
			bool stream;
			int dimW, dimH;
			char* imagesDir;

		public:
			Emitter( char* imagesDir, int iterations, Timer* timer, vector<Message*>* messages, int dimW, int dimH, bool stream );
			virtual ~Emitter();

			void run();
			void findImages( string imagesDir, bool stream, int dimW, int dimH );
	};

}

#endif /* EMITTER_H_ */
