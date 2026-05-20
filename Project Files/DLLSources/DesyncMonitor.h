#pragma once

// This class adds the abiilty to detect network OOS even in singleplayer
//
// The concept is that functions, which are called in async mode only will start by declaring an instance of CxDesyncMonitor
// It will never be used. It just has to be there to be allocated as a local variable.
// Examples of this would be the exe calling UI elements in the DLL like widgets and popup windows.
//
// Constructor arguments
// TYPE_ASYNC  - default. Tells that the function is called on one computer only. Usually related to UI
// TYPE_BOTH   - tells that the function can be called on one computer only, but doesn't mark current execution as that. Useful for assert checks
// TYPE_INVERT - resets to assume always in sync. Useful for bypassing sending network messages in single player and debug code
//

class CxDesyncMonitor
{
public:
	enum TYPE
	{
		TYPE_ASYNC,
		TYPE_BOTH,   // marking code, which is known to be called both in sync and async
		TYPE_RESET,  // resets back to "always in sync". Used for network messages and debug
		TYPE_NETWORK_RECEIVE, // used when receiving data from the network
	};

	CxDesyncMonitor(TYPE eType = TYPE_ASYNC);
	~CxDesyncMonitor();

	// make sure you use the correct bool function
	// they are NOT mutually exclusive and in fact all can be false at the same time

	static bool isCurrentlySync();
	static bool isAlwaysSync();
	static bool isNeverSync();
	static bool isNetworkReceive();


private:
	int m_iAsync;
	int m_iSync;
	int m_iNetworkReceive;
};
