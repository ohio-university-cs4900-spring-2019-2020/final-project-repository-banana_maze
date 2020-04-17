#pragma once

#include "../../../include/aftr/NetMsg.h"
#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgTxt : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgTxt);

		NetMsgTxt(const std::string message = "");
		virtual ~NetMsgTxt();
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		std::string text;

	};
}

#endif