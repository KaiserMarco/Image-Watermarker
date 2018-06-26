#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "Emitter.h"

using namespace std;
using namespace cimg_library;
using namespace boost::filesystem;

namespace iwm
{
	Emitter::Emitter( char* imagesDir, char* stampName, int iterations ) {
		this->iterations = iterations;
		this->messages = new vector<Message*>();
		findImages( imagesDir );
	}

	void Emitter::run() {
		cout << "[EMITTER]: PARTITO" << endl;
		int index = -1;
		for(unsigned int i = 0; i < this->messages->size(); i++) {
			index = (index + 1) % output_connections;

			cout << "[EMITTER]: Invio mesaggio al worker: " << index << " ..." << endl;
			Message* message = this->messages->at( i );
			sendMessage( message, index );
			cout << "[EMITTER]: Messaggio inviato!" << endl;
		}

		Message* exitMessage = new Message( "EXIT" );
		sendBroadcast( output_connections, exitMessage );
	}

	void Emitter::findImages( string imagesDir ) {
		for(int j = 0; j < iterations; j++) {
			for(auto i = directory_iterator( imagesDir ); i != directory_iterator(); i++) {
				if(!is_directory( i->path() )) {
					string imageName = i->path().filename().string();
					CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );

					Message* mex = new Message( image, imageName );
					this->messages->push_back( mex );
				}
			}
		}
	}

	Emitter::~Emitter() {
		//delete[] this->messages;
	}
}
