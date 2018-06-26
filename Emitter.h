#ifndef EMITTER_H_
#define EMITTER_H_

#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "Message.h"
#include "Node.h"

using namespace std;
namespace iwm {
	class Emitter : public Node {
		private:
			vector<Message*>* messages;
			int iterations;

		public:
			Emitter( char* imagesDir, char* stampName, int iterations );
			virtual ~Emitter();

			void run();

			void findImages( string imagesDir );
	};
}

#endif /* EMITTER_H_ */
