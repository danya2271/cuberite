#include "Globals.h"
#include "OSSupport/Network.h"
#include "OSSupport/NetworkSingleton.h"
#include <cstdlib>

class cBindCallbacks : public cNetwork::cListenCallbacks
{
public:

	bool Error = false;

	virtual cTCPLink::cCallbacksPtr OnIncomingConnection(const AString &, UInt16) override
	{
		return nullptr;
	}

	virtual void OnAccepted(cTCPLink &) override {}

	virtual void OnError(int, const AString &) override
	{
		Error = true;
	}
};

int main()
{
	cNetworkSingleton::Get().Initialise();
	for (const AString Address : { "127.0.0.1", "" })
	{
		auto Callbacks = std::make_shared<cBindCallbacks>();
		auto Listener = cNetwork::Listen(0, Callbacks, Address);
		if (!Listener->IsListening() || Callbacks->Error)
		{
			std::abort();
		}
		Listener->Close();
	}
	for (const AString Address : { "localhost", "127.0.0.1:25565", "invalid", "::gg" })
	{
		auto Callbacks = std::make_shared<cBindCallbacks>();
		auto Listener = cNetwork::Listen(0, Callbacks, Address);
		if (Listener->IsListening() || !Callbacks->Error)
		{
			std::abort();
		}
	}
	cNetworkSingleton::Get().Terminate();
}
