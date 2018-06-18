#include "CImg.h"
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <boost/filesystem.hpp>

using namespace cimg_library;
using namespace std;
using namespace chrono;
using namespace boost::filesystem;

typedef unsigned char imageType;

struct Message {
	int indexImage;
	CImg<imageType> image;
	string name;
};

int dimW, dimH;

vector<queue<Message>> workersQueues;
queue<Message> collectorQueue, emitterQueue;

mutex emitterMutex, collectorMutex;
vector<mutex> workerMutexes;
condition_variable cvEmitter, cvWorker, cvCollector;

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

void collect() {
	struct Message message;
	char *directory;
	string path;

	while(true) {
		unique_lock<mutex> lockCollectorMessage( collectorMutex );

		cvCollector.wait( lockCollectorMessage, [=]{ return !collectorQueue.empty(); } );

		message = collectorQueue.front();
		collectorQueue.pop();

		lockCollectorMessage.unlock();

		if(message.indexImage < 0) {
			break;
		}

		cout << "[COLLECTOR]: SALVATAGGIO IMMAGINE" << endl;
		path = "./ImageResults/" + message.name;
		directory = new char[path.size() + 1];
		copy( path.begin(), path.end(), directory );
		directory[path.size()] = '\0';
		message.image.save( directory );
	}

	delete[] directory;

	cout << "[COLLECTOR]: STA TERMINANDO..." << endl;
}

void work( int index, CImg<imageType>& stamp ) {
	struct Message message;

	CImg<imageType>::iterator it1, it2;
	CImg<imageType> image = message.image;

	while(true) {
		unique_lock<mutex> lockWorkerMessage( workerMutexes[index] );

		cvWorker.wait( lockWorkerMessage, [=]{ return !workersQueues[index].empty(); } );

		message = workersQueues[index].front();
		workersQueues[index].pop();

		lockWorkerMessage.unlock();

		cout << "[WORKER " << index << "]: LETTURA MESSAGGIO" << endl;
		if(message.indexImage < 0) {
			break;
		}

		int r, g, b, color, offset = 0, numPixels = dimW * dimH;
		for(it2 = stamp.begin(); it2 < stamp.end(); ++it2, ++offset) {
			if(*it2 == 0) {
				it1 = image.begin();
				advance( it1, offset );
				r = *it1;

				it1 = image.begin();
				advance( it1, numPixels + offset );
				g = *it1;

				it1 = image.begin();
				advance( it1, numPixels * 2 + offset );
				b = *it1;

				color = (r + g + b) / 3;
				color = (color + *it2) / 2;

				it1 = image.begin();
				advance( it1, offset );
				*it1 = color;

				it1 = image.begin();
				advance( it1, numPixels + offset );
				*it1 = color;

				it1 = image.begin();
				advance( it1, numPixels * 2 + offset );
				*it1 = color;
			}
		}

		unique_lock<mutex> lockCollectorMessage( collectorMutex );

		collectorQueue.push( message );
		cvCollector.notify_one();

		lockCollectorMessage.unlock();
	}

	cout << "[WORKER " << index << "]: STA TERMINANDO..." << endl;
}

void emit( int nWorkers, CImg<imageType>& stamp ) {
	vector<thread> threads;

	for(int i = 0; i < nWorkers; i++) {
		workersQueues.push_back( queue<Message>() );
		threads.emplace_back( work, i, ref( stamp ) );
		cout << "[EMITTER]: WORKER[" << i << "] GENERATO" << endl;
	}

	struct Message message;
	for(int index = 0; index < numeric_limits<int>::max(); index++) {
		unique_lock<mutex> lockEmitterMessage( emitterMutex );

		cvEmitter.wait( lockEmitterMessage, [=]{ return !emitterQueue.empty(); } );

		message = emitterQueue.front();
		emitterQueue.pop();

		lockEmitterMessage.unlock();

		cout << "[EMITTER]: LETTURA MESSAGGIO" << endl;
		if(message.image.is_empty()) {
			break;
		}

		message.image = ref( message.image );
		message.indexImage = index;

		cout << "[EMITTER]: INVIO MESSAGGIO AGLI WORKER = " << endl;
		unique_lock<mutex> lockWorkerMessage( workerMutexes[index % nWorkers] );

		workersQueues[index % nWorkers].push( message );
		cvWorker.notify_one();

		lockWorkerMessage.unlock();
	}

	cout << "[EMITTER]: TERMINE ELABORAZIONE IMMAGINI" << endl;
	// comunicate to all the threads that the images to be processed are finished
	message.indexImage = -1;
	for(int i = 0; i < nWorkers; i++) {
		unique_lock<mutex> lockWorkerMessage( workerMutexes[i] );

		workersQueues[i].push( message );
		cvWorker.notify_one();

		lockWorkerMessage.unlock();
	}

	for(thread & t : threads) {
		t.join();
	}

	// comunicate to the collector that there are no more images to save
	unique_lock<mutex> lockCollectorMessage( collectorMutex );

	collectorQueue.push( message );
	cvCollector.notify_one();

	lockCollectorMessage.unlock();

	cout << "[EMITTER]: STA TERMINANDO..." << endl;
}

int main( int argc, char* argv[] ) {
	dimW = atoi( argv[2] ), dimH = atoi( argv[3] );

	// creates images and save them in a directory
	imageType black[] = { 0, 0, 0 }, white[] = { 255, 255, 255 }, gray[] = { 127, 127, 127 }, color[] = { 255, 30, 130 };
	CImg<imageType> image1( dimW, dimH, 1, 3, { 255, 30, 130 } ), image2( dimW, dimH, 1, 3, 255 ),
					image3( dimW, dimH, 1, 3, 255 ), image4( dimW, dimH, 1, 3, { 255, 30, 130 } ),
					stampImage( dimW, dimH, 1, 3, 255 );
	image1.draw_text( dimW/6, dimH/8, "Sono", black, white, 1, 103 ); image2.draw_text( dimW/6, dimH/8, "Ciao", black, white, 1, 103 );
	image3.draw_text( dimW/6, dimH/8, "Vedo", black, white, 1, 103 ); image4.draw_text( dimW/6, dimH/8, "Ecco", black, color, 1, 103 );
	stampImage.draw_rectangle( dimW/8, dimH/2, dimW*3/4, dimH*3/4, black );
	//stampImage.draw_text( dimW/6, dimH/2, "Marco", black, white, 1, 103 );
	//image1.save( "./Images/image1.jpg" ); image2.save( "./Images/image2.jpg" ); image3.save( "./Images/image3.jpg" );
	image1.save( "./Images/image1.jpg" ); image4.save( "./Images/image4.jpg" ); stampImage.save( "./StampImage/stampImage.jpg" );

	/*CImg<imageType> img( dimW, dimH, 1, 3 ); img.fill( 255 );
	CImgDisplay disp( dimW, dimH, "", 0 );
	disp.display( stampImage );
	disp.wait();*/

	// generates the vector of the worker mutexes
	vector<mutex> list( atoi( argv[1] ) );
	workerMutexes.swap( list );

	// takes the stamp image
	CImg<imageType> stamp;
	for(auto i = directory_iterator( argv[6] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[6] + "/" + i->path().filename().string();
			if(strcmp( i->path().filename().c_str(), argv[7] ) == 0) {
				stamp.assign( imageName.c_str() );
			}
		}
	}

	Timer timeN, timeS;

	struct Message message;

	cout << "[MAIN]: INIZIATA PARTE PARALLELA" << endl;

	timeN.startTime();

	thread emitter( emit, atoi( argv[1] ), ref( stamp ) );

	thread collector( collect );

	// takes images from the directory and send them to the emitter queue
	for(auto i = directory_iterator( argv[5] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[5] + "/" + i->path().filename().string();
			for(int j = 0; j < atoi( argv[4] ); j++) {
				message.image = CImg<imageType>( imageName.c_str() );
				message.name = i->path().filename().string();
				unique_lock<mutex> lockEmitterMessage( emitterMutex );

				emitterQueue.push( message );
				cvEmitter.notify_one();

				lockEmitterMessage.unlock();
			}
		}
	}

	// comunicate to the emitter that there are no more images
	message.image = CImg<imageType>();
	unique_lock<mutex> lockEmitterMessage( emitterMutex );

	emitterQueue.push( message );
	cvEmitter.notify_one();

	lockEmitterMessage.unlock();

	emitter.join();

	collector.join();

	timeN.stopTime();

	cout << "[MAIN]: TERMINATA PARTE PARALLELA" << endl;

	cout << "[MAIN]: INIZIATA PARTE SEQUENZIALE" << endl;

    timeS.startTime();

    char *directory;
    string path;
    CImg<imageType> image;
	CImg<imageType>::iterator it1, it2;
    for(auto i = directory_iterator( argv[5] ); i != directory_iterator(); i++) {
		if(!is_directory( i->path() )) {
			string imageName = (string) argv[5] + "/" + i->path().filename().string();
			image = CImg<imageType>( imageName.c_str() );
			for(int j = 0; j < atoi( argv[4] ); j++) {
				int r, g, b, color, offset = 0, numPixels = dimW * dimH;
				for(it2 = stamp.begin(); it2 < stamp.end(); ++it2, ++offset) {
					if(*it2 == 0) {
						it1 = image.begin();
						advance( it1, offset );
						r = *it1;

						it1 = image.begin();
						advance( it1, numPixels + offset );
						g = *it1;

						it1 = image.begin();
						advance( it1, numPixels * 2 + offset );
						b = *it1;

						color = (r + g + b) / 3;
						color = (color + *it2) / 2;

						it1 = image.begin();
						advance( it1, offset );
						*it1 = color;

						it1 = image.begin();
						advance( it1, numPixels + offset );
						*it1 = color;

						it1 = image.begin();
						advance( it1, numPixels * 2 + offset );
						*it1 = color;
					}
				}

				path = "./ImageResults/" + i->path().filename().string();
				directory = new char[path.size() + 1];
				copy( path.begin(), path.end(), directory );
				directory[path.size()] = '\0';
				image.save( directory );
			}
		}
    }

	delete[] directory;

	timeS.stopTime();

	cout << "[MAIN]: TERMINATA PARTE SEQUENZIALE" << endl;

	cout << "\nSequential time = " << timeS.getDuration() << "\n"
		 << "\nParallel_N time = " << timeN.getDuration() << "\n"
		 << "\nSpeedup = " << timeS.getDuration()/timeN.getDuration() << "\n";

	return 0;
}
