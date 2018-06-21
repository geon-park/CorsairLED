// CorsairLED.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <thread>
#include "include\CUESDK.h"

std::vector<CorsairLedPosition> getAvailableKeys()
{
	auto posVector = std::vector<CorsairLedPosition>();

	// Get Available Keys
	for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++)
	{
		if (CDT_Keyboard == CorsairGetDeviceInfo(deviceIndex)->type)
		{
			auto ledPositions = CorsairGetLedPositions();
			if (ledPositions)
			{
				for (auto i = 0; i < ledPositions->numberOfLed; i++)
				{
					posVector.push_back(ledPositions->pLedPosition[i]);
				}
			}
		}
	}

	return posVector;
}

std::vector<CorsairLedColor> sortbyPosition(const std::vector<CorsairLedPosition>& orgposVector)
{
	auto colorVector = std::vector<CorsairLedColor>();
	auto posVector = orgposVector;

	// Sort by Top Position
	std::sort(posVector.begin(), posVector.end(), [](auto& a, auto& b) -> bool
	{
		return a.top < b.top;
	});

	// Sort by Left Position
	std::vector<std::vector<CorsairLedPosition>> sortedKeyList;
	std::vector<CorsairLedPosition> lineList;
	double prevTop = posVector[0].top;
	int index = 0;
	bool bFirst = true;

	// Separate Each Line
	for (auto key : posVector)
	{
		if (prevTop != key.top)
		{
			if (true == bFirst)
			{
				bFirst = false;
				lineList.clear();
			}
			else
			{
				sortedKeyList.push_back(lineList);
				lineList.clear();
			}
		}
		lineList.push_back(key);
		prevTop = key.top;
		++index;
	}
	sortedKeyList.push_back(lineList);

	// Convert to CorsairLedColor
	index = 0;
	for (auto& line : sortedKeyList)
	{
		if (0 == index % 2)
		{
			std::sort(line.begin(), line.end(), [](auto& a, auto& b) -> bool
			{
				return a.left < b.left;
			});
		}
		else
		{
			std::sort(line.rbegin(), line.rend(), [](auto& a, auto& b) -> bool
			{
				return a.left < b.left;
			});
		}
		++index;

		for (auto& key : line)
		{
			colorVector.push_back(CorsairLedColor{ key.ledId, 0, 0, 0 });
		}
	}
	return colorVector;
}

int main()
{
	CorsairPerformProtocolHandshake();

	auto allKeyList = getAvailableKeys();
	auto KeyList = sortbyPosition(allKeyList);

	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		for (auto& key : KeyList)
		{
			key.g = (0 == key.g) ? 255 : 0;
			CorsairSetLedsColors(1, &key);

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	return 0;
}