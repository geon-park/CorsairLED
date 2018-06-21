// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <iostream>
#include <thread>
#include <string>

const char* toString(CorsairError error) 
{
	switch (error) {
	case CE_Success : 
		return "CE_Success";
	case CE_ServerNotFound:
		return "CE_ServerNotFound";
	case CE_NoControl:
		return "CE_NoControl";
	case CE_ProtocolHandshakeMissing:
		return "CE_ProtocolHandshakeMissing";
	case CE_IncompatibleProtocol:
		return "CE_IncompatibleProtocol";
	case CE_InvalidArguments:
		return "CE_InvalidArguments";
	default:
		return "unknown error";
	}
}

void highlightKey(CorsairLedId ledId)
{
	for (auto x = .0; x < 2; x += .1) {
		auto val = (1 - abs(x - 1)) * 255;
		auto ledColor = CorsairLedColor{ ledId, (int)val, (int)val, (int)val };
		CorsairSetLedsColors(1, &ledColor);
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}

int main()
{
	CorsairPerformProtocolHandshake();

	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}
	CorsairRequestControl(CAM_ExclusiveLightingControl);

	std::cout << "Please, input a word... ";
	auto userInputStr = std::string();
	std::cin >> userInputStr;
		
	for (const auto &symbol : userInputStr) {
		auto ledId = CorsairGetLedIdForKeyName(symbol);
		if (ledId != CLI_Invalid)
			highlightKey(ledId);
	}
    return 0;
}

