#include <sstream>
#include <experimental/filesystem>
#include <fstream>

#include "Emitter.h"
#include "Worker.h"
#include "Collector.h"
#include "Timer.h"
#include "Queuet.h"

using namespace cimg_library;
using namespace iwm;

namespace fs = experimental::filesystem;

int main( int argc, char* argv[] ) {
	if(argc != 11) {
		cout << "arg 1  = # workers" << endl;
		cout << "arg 2  = directory where to find the images" << endl;
		cout << "arg 3  = directory where to save the images" << endl;
		cout << "arg 4  = directory where to find the stamp image" << endl;
		cout << "arg 5  = name of the stamp image (.jpg)" << endl;
		cout << "arg 6  = width of the image/s" << endl;
		cout << "arg 7  = height of the image/s" << endl;
		cout << "arg 8  = # of copies of the image/s" << endl;
		cout << "arg 9  = flag to decide if consider or not the loading images time in the computation one" << endl;
		cout << "arg 10 = flag to decide if consider or not the saving time in the computation one" << endl;

		return 0;
	}

	ofstream file;
	file.open( "results_par.txt", ios_base::app );

	if(!file.is_open()) {
		return 0;
	}

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

	CImg<imageType>* stamp = NULL;

	Timer* time = new Timer();

	// takes the stamp image
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
		Message* message = NULL;
		for(auto & p : fs::directory_iterator( imagesDir )) {
			if(!is_directory( p.path() ) && imageCopies >= 0) {
				string imageName = p.path().filename().string();
				CImg<imageType> *image = new CImg<imageType>( ((string) imagesDir + "/" + imageName).c_str() );
				image->resize( dimW, dimH );

				message = new Message( image, imageName );
				break;

				//Message* mex = new Message( image, imageName );
				//messages->push_back( mex );
			}
		}

		for(int k = 0; k < imageCopies; k++) {
			messages->push_back( message->clone() );
		}

		time->startTime();
	}

	Emitter emitter( imagesDir, imageCopies, time, messages, dimW, dimH, false );

	Collector* collector = new Collector( saveDir, save, time );

	Queue_t* queue = new Queue_t();

	vector<Worker*>* workers = new vector<Worker*>();
	for(int i = 0; i < nWorkers; i++) {
		Worker* worker = new Worker( stamp, i );
		workers->push_back( worker );

		emitter.connectTo( worker );
		worker->connectTo( collector, queue );

		worker->start();
	}

	collector->start();

	emitter.start();
	emitter.join();

	cout << "[MAIN]: Waiting for workers.." << endl;
	for(int i = 0; i < nWorkers; i++) {
		workers->at( i )->join();
	}

	collector->join();

	if(save) {
		time->stopTime();
	}

	string streaming = stream ? " streaming parallel" : "out streaming parallel";
	string saving = save ? " saving images" : "out saving images";

	cout << endl << "Parallel_N time = " << time->getDuration() << " secs using " << nWorkers << " workers "
				 << "with" << streaming << " and with" << saving << endl;
	file << time->getDuration() << " " << nWorkers << " " << stream << " " << save << endl;
	file.close();

	for(int i = 0; i < nWorkers; i++) {
		delete workers->at( i );
	}

	delete collector;

	workers->clear();
	delete workers;

	delete time;
	delete stamp;
	delete messages;

	return 0;
}
