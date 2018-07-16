#include <iostream>

#include "Node.h"

using namespace std;

namespace iwm
{
	Node::Node() {
		this->input_connections  = 0;
		this->output_connections = 0;
	}

	Message* Node::receiveMessage( int index ) {
		return this->input_queues[index]->receiveMessage();
	}

	void Node::sendMessage( Message* message, int index ) {
		this->output_queues[index]->sendMessage( message );
	}

	void Node::sendBroadcast( Message* message ) {
		for(int i = 0; i < this->output_connections; i++) {
			sendMessage( message->clone(), i );
		}
	}

	void Node::connectTo( Node* node, Queue_t* shared_queue ) {
		Queue_t* queue;
		if(shared_queue != nullptr) {
			queue = shared_queue;
			if(!checkSharedQueue( queue, this->output_queues )) {
				this->output_queues.push_back( queue );
			}
		} else {
			queue = new Queue_t();
			this->output_queues.push_back( queue );
		}

		node->notifyConnection( queue, shared_queue != nullptr );
		this->output_connections++;
	}

	Queue_t* Node::getInputQueue( int index ) {
		return input_queues[index];
	}

	Queue_t* Node::getOutputQueue( int index ) {
		return output_queues[index];
	}

	void Node::start() {
		_thread = thread( &Node::run, this );
	}

	void Node::join() {
		_thread.join();
	}

	Node::~Node() {
		this->input_queues.clear();
		this->output_queues.clear();
	}

	void Node::notifyConnection( Queue_t* queue, bool shared ) {
		// Insert the queue only if it is a brand new one.
		if(!shared || !checkSharedQueue( queue, this->input_queues )) {
			this->input_queues.push_back( queue );
		}

		this->input_connections++;
	}

	bool Node::checkSharedQueue( Queue_t* queue, vector<Queue_t*> queues ) {
		for(Queue_t* q : queues) {
			if(q->getId() == queue->getId()) {
				return true;
			}
		}
		return false;
	}
}
