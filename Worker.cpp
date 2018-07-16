#include <iostream>

#include "Worker.h"

using namespace std;

namespace iwm {

	Worker::Worker( CImg<imageType>* stamp, int index ) {
		this->index = index;
		this->stamp = stamp;
	}

	void Worker::run()
	{
		cout << "[WORKER " << index << "]: PARTITO" << endl;

		bool finish = false;
		while(!finish) {
			// Read message from Emitter.
			Message* mex = receiveMessage( 0 );
			cout << "[WORKER " << index << "]: Ricevuto: " << mex->getName() << endl;

			if(mex->getName().compare( EOS ) == 0) {
				finish = true;
				cout << "[WORKER " << index << "]: Invio chiusura worker al collector" << endl;
			} else {
				insertWatermark( mex->getImage() );
				cout << "[WORKER " << index << "]: Invio immagine al collector" << endl;
			}

			// Send message to Collector.
			cout << "[WORKER " << index << "]: Messaggio inviato al collector" << endl;
			sendMessage( mex, 0 );
		}

		cout << "[WORKER " << index << "]: TERMINATO" << endl;
	}

	void Worker::insertWatermark( CImg<imageType>* image ) {
		CImg<imageType>::iterator itR, itG, itB, itS;
		int color = 0, offset = 0, numPixels = image->width() * image->height();
		for(itS = stamp->begin(); itS < stamp->end(); ++itS, ++offset) {
			if(*itS == 0) {
				itR = image->begin();
				advance( itR, offset );
				color += *itR;

				itG = image->begin();
				advance( itG, numPixels + offset );
				color += *itG;

				itB = image->begin();
				advance( itB, numPixels * 2 + offset );
				color += *itB;

				color /= 6;

				*itR = color;
				*itG = color;
				*itB = color;
			}
		}
	}

	Worker::~Worker() {}

} /* namespace iwm */
