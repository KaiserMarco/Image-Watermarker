#include "Message.h"

namespace iwm {

	Message::Message( CImg<imageType>* image, string name ) {
		this->image = image;
		this->name  = name;
	}

	Message::Message() {
		this->image = nullptr;
		this->name  = EOS;
	}

	CImg<imageType>* Message::getImage() {
		return image;
	}

	string Message::getName() {
		return name;
	}

	Message* Message::clone() {
		return new Message( this->image, this->name );
	}

	Message::~Message() {
		/*if(this->image != nullptr) {
			delete this->image;
		}*/
	}

} /* namespace iwm */
