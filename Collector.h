#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include "Message.h"
#include "Node.h"
#include "Timer.h"

namespace iwm
{
	class Collector : public Node {
		private:
			string outDir;
			bool save;
			vector<Message*>* messages;
			Timer* time;

		public:
			Collector( string outDir, bool save, Timer* time );
			virtual ~Collector();

			void run();

			void saveFile( string path, CImg<imageType>* image );
	};
}

#endif /* COLLECTOR_H_ */
