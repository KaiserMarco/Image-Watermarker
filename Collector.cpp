#include <iostream>

#include "Collector.h"

using namespace std;

namespace iwm
{
	Collector::Collector( string folder, bool save, Timer* time ) {
		this->outDir = folder;
		this->save = save;
		this->time = time;
		this->messages = new vector<Message*>();
	}

	void Collector::run()
	{
		cout << "[COLLECTOR]: PARTITO" << endl;

		while(this->input_connections > 0) {
			Message* message = receiveMessage( 0 );

			if(message->getName().compare( EOS ) == 0) {
				this->input_connections--;
			} else if(save) {
				saveFile( outDir + message->getName(), message->getImage() );
				delete message;
			} else {
				messages->push_back( message );
			}
		}

		if(!save) {
			time->stopTime();
			for(unsigned int i = 0; i < messages->size(); i++) {
				Message* message = messages->at( i );
				saveFile( outDir + message->getName(), message->getImage() );
				delete message;
			}
		}

		cout << "[COLLECTOR]: TERMINATO" << endl;
	}

	void Collector::saveFile( string path, CImg<imageType>* image ) {
		cout << "[COLLECTOR]: Saving: " << path << endl;
		char* directory = new char[path.size() + 1];
		copy( path.begin(), path.end(), directory );
		directory[path.size()] = '\0';

		//image->save( directory );

		delete[] directory;
	}

	Collector::~Collector() {
		//delete messages;
	}
}
