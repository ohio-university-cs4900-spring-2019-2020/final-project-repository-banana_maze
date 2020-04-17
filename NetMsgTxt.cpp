#include "NetMsgTxt.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "Model.h"
#include "WOLight.h"
#include "GLViewNewModule.h"
#include "WOFTGLString.h"

using namespace std;
using namespace Aftr;

NetMsgMacroDefinition(NetMsgTxt);

NetMsgTxt::NetMsgTxt(const string message) { this->text = message; }

NetMsgTxt::~NetMsgTxt() {}

bool NetMsgTxt::toStream(NetMessengerStreamBuffer& os) const {
	os << this->text;

	return true;
}

bool NetMsgTxt::fromStream(NetMessengerStreamBuffer& is) {
	is >> this->text;
	return true;
}

void NetMsgTxt::onMessageArrived() {
	((GLViewNewModule*)ManagerGLView::getGLView())->worldTxt->setText(this->text);
	std::cout << this->toString() << endl;
}

std::string NetMsgTxt::toString() const {
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "   Message: " << this->text << "...\n";
	return ss.str();
}

#endif