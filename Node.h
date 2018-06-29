#ifndef NODE_H_
#define NODE_H_

#include <thread>

#include "CImg.h"
#include "Queuet.h"

using namespace std;
using namespace cimg_library;

namespace iwm {

	typedef unsigned char imageType;

	class Node {
		protected:
			int input_connections;
			int output_connections;

		private:
			vector<Queue_t*> input_queues;
			vector<Queue_t*> output_queues;

			thread _thread;

		public:
			Node();

			Message* receiveMessage( int index );

			void sendMessage( Message* message, int index );

			void sendBroadcast( Message* message );

			void connectTo( Node* node, Queue_t* shared_queue = nullptr );

			Queue_t* getOutputQueue( int index );

			Queue_t* getInputQueue( int index );

			void start();

			void join();

			virtual ~Node();


		private:
			virtual void run() = 0;

			void notifyConnection( Queue_t* queue, bool shared );

			bool checkSharedQueue( Queue_t* queue, vector<Queue_t*> queues );
	};
}

#endif  /* NODE_H_ */
