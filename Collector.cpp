#include <iostream>

#include "Collector.h"

using namespace std;

namespace iwm
{
	Collector::Collector( string folder ) {
		this->outDir = folder;
	}

	void Collector::run()
	{
		cout << "[COLLECTOR]: PARTITO" << endl;

		while(this->input_connections > 0) {
			Message* message = receiveMessage( 0 );

			if(message->getName().compare( "EXIT" ) == 0) {
				this->input_connections--;
			} else {
				saveFile( outDir + message->getName(), message->getImage() );
			}

			delete message;
		}

		cout << "[COLLECTOR]: TERMINATO" << endl;
	}

	void Collector::saveFile( string path, CImg<imageType>* image ) {
		cout << "[COLLECTOR]: Saving: " << path << endl;
		char* directory = new char[path.size() + 1];
		copy( path.begin(), path.end(), directory );
		directory[path.size()] = '\0';

		image->save( directory );

		delete[] directory;
	}

	Collector::~Collector() {}
}
