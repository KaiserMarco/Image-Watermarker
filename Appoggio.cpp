#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <experimental/filesystem>

#include "CImg.h"

#include "Emitter.h"
#include "Worker.h"
#include "Collector.h"
#include "Queuet.h"
#include "Timer.h"

using namespace cimg_library;
using namespace std;
using namespace iwm;

namespace fs = experimental::filesystem;

void insertWatermark( CImg<imageType>* stamp, CImg<imageType>* image ) {
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

void saveFile( string path, CImg<imageType>* image ) {
	char* directory = new char[path.size() + 1];
	copy( path.begin(), path.end(), directory );
	directory[path.size()] = '\0';

	image->save( directory );

	delete directory;
}

int main( int argc, char* argv[] ) {
	int nWorkers   = atoi( argv[1] );
	int iterations = atoi( argv[8] );

	char* imagesDir = argv[2];
	char* saveDir   = argv[3];
	char* stampDir  = argv[4];
	char* stampName = argv[5];

	CImg<imageType>* stamp;

	Timer* timeN = new Timer();
	Timer* timeS = new Timer();

	for(auto & p : fs::directory_iterator( stampDir )) {
		if(!is_directory( p.path() )) {
			string imageName = (string) stampDir + "/" + p.path().filename().string();
			if(strcmp( p.path().filename().c_str(), stampName ) == 0) {
				stamp = new CImg<imageType>( imageName.c_str() );
			}
		}
	}

	vector<Message*>* messages = new vector<Message*>();

    for(int j = 0; j < iterations; j++) {
    	for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() )) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );

				Message* mex = new Message( image, imageName );
				messages->push_back( mex );
			}
		}
	}

	cout << "[MAIN]: INIZIATA PARTE PARALLELA" << endl;

	//timeN->startTime();

    Emitter emitter( imagesDir, iterations, timeN, messages );

	Collector* collector = new Collector( saveDir );

	Queue_t* queue = new Queue_t();

	vector<Worker*> workers;
	for(int i = 0; i < nWorkers; i++) {
		Worker* worker = new Worker( stamp, i );
		workers.push_back( worker );

		emitter.connectTo( worker );
		worker->connectTo( collector, queue );

		worker->start();
	}

	collector->start();

	emitter.start();
	emitter.join();

	cout << "[MAIN]: Waiting for workers.." << endl;
	for(int i = 0; i < nWorkers; i++) {
		workers[i]->join();
	}

	collector->join();

	timeN->stopTime();

	//delete[] messages;

	cout << "[MAIN]: TERMINATA PARTE PARALLELA" << endl;

	messages->clear();
    for(int j = 0; j < iterations; j++) {
    	for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() )) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );

				Message* mex = new Message( image, imageName );
				messages->push_back( mex );
			}
		}
	}

	cout << "[MAIN]: INIZIATA PARTE SEQUENZIALE" << endl;

    timeS->startTime();

    for(unsigned int i = 0; i < messages->size(); i++) {
		Message* message = messages->at( i );

		insertWatermark( stamp, message->getImage() );

		saveFile( message->getName(), message->getImage() );

		delete message;
    }

	delete stamp;

	timeS->stopTime();

	cout << "[MAIN]: TERMINATA PARTE SEQUENZIALE" << endl;

	cout << endl << "Sequential time = " << timeS->getDuration() << endl
		 << endl << "Parallel_N time = " << timeN->getDuration() << endl
		 << endl << "Speedup = " << timeS->getDuration()/timeN->getDuration() << endl;

	delete timeN;
	delete timeS;

	return 0;
}
