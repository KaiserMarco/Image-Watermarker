#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

#include "CImg.h"

#define EOS "END"

typedef unsigned char imageType;

using namespace cimg_library;
using namespace std;

namespace iwm {

	class Message {
		private:
			CImg<imageType>* image;
			string name;

		public:
			Message();
			Message( CImg<imageType>* image, string name );
			virtual ~Message();

			CImg<imageType>* getImage();
			string getName();
			Message* clone();
	};

} /* namespace iwm */

#endif /* MESSAGE_H_ */
