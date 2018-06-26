#ifndef WORKER_H_
#define WORKER_H_

#include "Message.h"
#include "Node.h"

namespace iwm
{
	class Worker : public Node {
		private:
			CImg<imageType>* stamp;
			int index;

		public:
			Worker( CImg<imageType>* stamp, int index );
			virtual ~Worker();

			void run();

		private:
			void insertWatermark( CImg<imageType>* image );
	};
}

#endif /* WORKER_H_ */
