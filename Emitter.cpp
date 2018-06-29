#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>

#include "Emitter.h"

using namespace std;
using namespace cimg_library;

namespace fs = experimental::filesystem;

namespace iwm {

	Emitter::Emitter( char* imagesDir, int iterations, Timer* timer, vector<Message*>* messages ) {
		this->iterations = iterations;
		this->messages = messages;
		this->timer = timer;
	}

	void Emitter::run() {
		timer->startTime();
		cout << "[EMITTER]: PARTITO" << endl;
		int index = -1;
		for(unsigned int i = 0; i < this->messages->size(); i++) {
			index = (index + 1) % output_connections;

			cout << "[EMITTER]: Invio messaggio al worker: " << index << endl;
			Message* message = this->messages->at( i );
			sendMessage( message, index );
			cout << "[EMITTER]: Messaggio inviato!" << endl;
		}

		Message* exitMessage = new Message( "EXIT" );
		sendBroadcast( exitMessage );

		cout << "[EMITTER]: TERMINATO" << endl;
	}

	Emitter::~Emitter() {
		//delete[] this->messages;
	}
}
