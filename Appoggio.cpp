#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <boost/filesystem.hpp>

#include "CImg.h"
#include "Node.h"
#include "Worker.h"
#include "Queuet.h"
#include "Collector.h"
#include "Emitter.h"

using namespace cimg_library;
using namespace std;
using namespace chrono;
using namespace boost::filesystem;
using namespace iwm;

class Timer {
	typedef time_point<system_clock> clock;

	clock time;
	duration<double> elapsed;

	public:
		void startTime() {
			time = system_clock::now();
		}

		void stopTime(){
			elapsed = system_clock::now() - time;
		}

		double getDuration() {
			return elapsed.count();
		}
};

void insertWatermark( CImg<imageType>* stamp, CImg<imageType>* image ) {
	CImg<imageType>::iterator itR, itG, itB, itS;
	int color, offset = 0, numPixels = image->width() * image->height();
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

	delete image;
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

	Timer timeN, timeS;

	cout << "[MAIN]: INIZIATA PARTE PARALLELA" << endl;

	timeN.startTime();

    for(auto i = directory_iterator( stampDir ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) stampDir + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), stampName ) == 0) {
				stamp = new CImg<imageType>( imageName.c_str() );
			}
		}
    }

	Emitter emitter( imagesDir, stampName, iterations );

	Queue_t* queue = new Queue_t();
	Collector* collector = new Collector( saveDir );

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

	cout << "[MAIN] Waiting for workers.." << endl;
	for(int i = 0; i < nWorkers; i++) {
		workers[i]->join();
	}

	collector->join();

	emitter.join();

	timeN.stopTime();

	cout << "[MAIN]: TERMINATA PARTE PARALLELA" << endl;

	cout << "\nParallel_N time = " << timeN.getDuration() << "\n";

	/*cout << "[MAIN]: INIZIATA PARTE SEQUENZIALE" << endl;

    timeS.startTime();

    for(auto i = directory_iterator( stampDir ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) stampDir + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), stampName ) == 0) {
				CImg<imageType>* stamp = new CImg<imageType>( imageName.c_str() );

				for(int j = 0; j < iterations; j++) {
					for(auto i = directory_iterator( imagesDir ); i != directory_iterator(); i++) {
						if(!is_directory( i->path() )) {
							string imageName = (string) imagesDir + "/" + i->path().filename().string();
							CImg<imageType> *image = new CImg<imageType>( imageName.c_str() );

							insertWatermark( ref( stamp ), image );

							saveFile( saveDir + string( "/" ) + i->path().filename().string(), ref( image ) );
						}
					}
				}

				delete stamp;
				break;
			}
		}
	}

	timeS.stopTime();

	cout << "[MAIN]: TERMINATA PARTE SEQUENZIALE" << endl;

	cout << "\nSequential time = " << timeS.getDuration() << "\n"
		 << "\nParallel_N time = " << timeN.getDuration() << "\n"
		 << "\nSpeedup = " << timeS.getDuration()/timeN.getDuration() << "\n";*/

	return 0;
}
