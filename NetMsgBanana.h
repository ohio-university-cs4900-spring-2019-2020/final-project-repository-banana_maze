#pragma once
#include "../../../include/aftr/NetMsg.h"
#include "Vector.h"
#include "Vector.h"
#include "../cwin64/WObanana.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {
	class NetMsgBanana : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgBanana);
		NetMsgBanana(const Vector& pos = Vector(0, 0, 0), const Vector& dir = Vector(0, 0, 0));
		virtual ~NetMsgBanana();
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;
	private:
		Vector position;
		Vector direction;
	};
}
#endif