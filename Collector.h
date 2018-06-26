#ifndef COLLECTOR_H_
#define COLLECTOR_H_

#include "Message.h"
#include "Node.h"

namespace iwm
{
	class Collector : public Node {
		private:
			string outDir;

		public:
			Collector( string outDir );
			virtual ~Collector();

			void run();

			void saveFile( string path, CImg<imageType>* image );
	};
}

#endif /* COLLECTOR_H_ */
