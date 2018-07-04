#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
#include <condition_variable>
#include <experimental/filesystem>

#include "Emitter.h"
#include "Worker.h"
#include "Collector.h"
#include "Queuet.h"
#include "Timer.h"

using namespace cimg_library;
using namespace iwm;

namespace fs = experimental::filesystem;

int main( int argc, char* argv[] ) {
	int nWorkers    = atoi( argv[1] );
	int dimW        = atoi( argv[6] );
	int dimH        = atoi( argv[7] );
	int imageCopies = atoi( argv[8] );

	char* imagesDir = argv[2];
	char* saveDir   = argv[3];
	char* stampDir  = argv[4];
	char* stampName = argv[5];

	bool stream; stringstream( argv[9] ) >> boolalpha >> stream;
	bool save;  stringstream( argv[10] ) >> boolalpha >> save;

	vector<Message*>* messages = new vector<Message*>();

	CImg<imageType>* stamp;

	Timer* time = new Timer();

	for(auto & p : fs::directory_iterator( stampDir )) {
		if(!is_directory( p.path() )) {
			string imageName = (string) stampDir + "/" + p.path().filename().string();
			if(strcmp( p.path().filename().c_str(), stampName ) == 0) {
				stamp = new CImg<imageType>( imageName.c_str() );
				stamp->resize( dimW, dimH );
			}
		}
	}

	if(stream) {
		time->startTime();
	} else {
	    for(int j = 0; j < imageCopies; j++) {
	    	for(auto & p : fs::directory_iterator( imagesDir )) {
				if(!is_directory( p.path() )) {
					string imageName = p.path().filename().string();
					CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
					image->resize( dimW, dimH );

					Message* mex = new Message( image, imageName );
					messages->push_back( mex );
				}
			}
		}

		time->startTime();
	}

    Emitter emitter( imagesDir, imageCopies, time, messages, dimW, dimH, stream );

	Collector* collector = new Collector( saveDir, save, time );

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

	if(save) {
		time->stopTime();
	}

	string streaming = stream ? " streaming parallel" : "out streaming parallel";
	string saving = save ? " saving images" : "out saving images";
	cout << endl << "Parallel_N time = " << time->getDuration() << " secs using " << nWorkers << " workers "
				 << "with" << streaming << " and with" << saving << endl;

	delete time;

	return 0;
}
