#ifndef NODE_H_
#define NODE_H_

#include <thread>

#include "CImg.h"
#include "Queuet.h"

using namespace std;
using namespace cimg_library;

namespace iwm
{
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
			Node() {
				input_connections  = 0;
				output_connections = 0;
			}

			Message* receiveMessage( int index ) {
				return input_queues[index]->receiveMessage();
			}

			void sendMessage( Message* message, int index ) {
				output_queues[index]->sendMessage( message );
			}

			void sendBroadcast( int receivers, Message* message ) {
				for(int i = 0; i < receivers; i++) {
					sendMessage( message, i );
				}
			}

			void connectTo( Node* node, Queue_t* shared_queue = nullptr ) {
				Queue_t* queue;
				if(shared_queue != nullptr && checkSharedQueue( queue, output_queues )) {
					queue = shared_queue;
				} else {
					queue = new Queue_t();
					output_queues.push_back( queue );
				}

				node->notifyConnection( queue, shared_queue != nullptr );
				output_connections++;
			}

			void start() {
				_thread = std::thread( &Node::run, this );
			}

			void join() {
				_thread.join();
			}

			virtual ~Node() {
				input_queues.clear();
				output_queues.clear();
			}


		private:
			virtual void run() = 0;

			void notifyConnection( Queue_t* queue, bool shared ) {
				// Insert the queue only if it is a brand new one.
				if(!shared || !checkSharedQueue( queue, input_queues )) {
					input_queues.push_back( queue );
					input_connections++;
				}
			}

			bool checkSharedQueue( Queue_t* queue, vector<Queue_t*> queues ) {
				for(Queue_t* q : queues) {
					if(q->getId() == queue->getId()) {
						return true;
					}
				}
				return false;
			}
	};
}

#endif  /* NODE_H_ */
