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

	Emitter::Emitter( char* imagesDir, int imageCopies, Timer* timer, vector<Message*>* messages, int dimW, int dimH, bool stream ) {
		this->imagesDir = imagesDir;
		this->imageCopies = imageCopies;
		this->messages = messages;
		this->timer = timer;
		this->stream = stream;
		this->dimW = dimW;
		this->dimH = dimH;
	}

	void Emitter::run() {
		cout << "[EMITTER]: PARTITO " << endl;
		if(stream) {
			findImages( imagesDir, dimW, dimH );
		} else {
			timer->startTime();
			int index = -1;
			for(unsigned int i = 0; i < this->messages->size(); i++) {
				index = (index + 1) % output_connections;

				cout << "[EMITTER]: Invio messaggio al worker: " << index << endl;
				Message* message = this->messages->at( i );
				sendMessage( message, index );
				cout << "[EMITTER]: Messaggio inviato!" << endl;
			}
		}

		Message* exitMessage = new Message();
		sendBroadcast( exitMessage );

		cout << "[EMITTER]: TERMINATO" << endl;
	}

	void Emitter::findImages( string imagesDir, int dimW, int dimH ) {
		int index = -1;
		for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() ) && --imageCopies >= 0) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
				image->resize( dimW, dimH );

				Message* message = new Message( image, imageName );
				index = (index + 1) % output_connections;
				sendMessage( message, index );
				cout << "[EMITTER]: Messaggio inviato!" << endl;
			}
		}
	}

	Emitter::~Emitter() {}
}
