// progress.cpp : Defines the entry point for the console application.
//

#include "CUESDK.h"

#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include <vector>
#include <windows.h>

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

double getKeyboardWidth(CorsairLedPositions *ledPositions)
{
	const auto minmaxLeds = std::minmax_element(ledPositions->pLedPosition, ledPositions->pLedPosition + ledPositions->numberOfLed,
		[](const CorsairLedPosition &clp1, const CorsairLedPosition &clp2) {
		return clp1.left < clp2.left;
	});
	return minmaxLeds.second->left + minmaxLeds.second->width - minmaxLeds.first->left;
}

int main()
{
	CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << std::endl;
		getchar();
		return -1;
	}

	const auto ledPositions = CorsairGetLedPositions();
	if (ledPositions && ledPositions->numberOfLed > 0) {
				
		const auto keyboardWidth = getKeyboardWidth(ledPositions);
		const auto numberOfSteps = 50;
		std::cout << "Working... Press Escape to close program...";
		for (auto n = 0; !GetAsyncKeyState(VK_ESCAPE); n++) {

			std::vector<CorsairLedColor> vec;
			const auto currWidth = double(keyboardWidth) * (n % (numberOfSteps + 1)) / numberOfSteps;

			for (auto i = 0; i < ledPositions->numberOfLed; i++) {
				const auto ledPos = ledPositions->pLedPosition[i];
				auto ledColor = CorsairLedColor();
				ledColor.ledId = ledPos.ledId;
				if (ledPos.left < currWidth)
					ledColor.r = 255;
				vec.push_back(ledColor);
			}
			CorsairSetLedsColors(vec.size(), vec.data());

			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
	}
	return 0;
}

