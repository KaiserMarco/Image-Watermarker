#include "Queuet.h"

namespace iwm {

	long Queue_t::nextId = 0;

	Queue_t::Queue_t() {
		this->que = new queue<Message*>();
		this->id = generateId();
	}

	long Queue_t::getId() {
		return this->id;
	}

	void Queue_t::sendMessage( Message* mex ) {
		unique_lock<mutex> lockMessage( mux );

		que->push( mex );
		cv.notify_one();

		lockMessage.unlock();
	}

	Message* Queue_t::receiveMessage() {
		unique_lock<mutex> lockMessage( mux );

		cv.wait( lockMessage, [=]{ return !que->empty(); } );

		Message* message = que->front();
		que->pop();

		lockMessage.unlock();

		return message;
	}

	long Queue_t::generateId() {
		return nextId++;
	}

	Queue_t::~Queue_t() {
		delete[] que;
	}

} /* namespace iwm */
