#include "NetMsgBanana.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "Model.h"
#include "GLViewNewModule.h"

using namespace Aftr;
using namespace std;

NetMsgMacroDefinition(NetMsgBanana);

NetMsgBanana::NetMsgBanana(const Vector& pos, const Vector& dir) {
	this->position = pos;
	this->direction = dir;
}
NetMsgBanana::~NetMsgBanana(){}

bool NetMsgBanana::toStream(NetMessengerStreamBuffer& os)const {
	os << this->position.x << this->position.y << this->position.z;
	os << this->direction.x << this->direction.y << this->direction.z;
	return true;
}

bool NetMsgBanana::fromStream(NetMessengerStreamBuffer& is) {
	is >> this->position.x >> this->position.y >> this->position.z;
	is >> this->direction.x >> this->direction.y >> this->direction.z;
	return true;
}
void NetMsgBanana::onMessageArrived() {
	((GLViewNewModule*)ManagerGLView::getGLView())->banana->setPosition(this->position);
	cout << this->toString() << endl;
}

string NetMsgBanana::toString()const {
	stringstream stream;
	stream << NetMsg::toString();
	stream << "position: " << this->position << "..." << endl;
	stream << "direction: " << this->direction << "..." << endl;
	return stream.str();
}
#endif