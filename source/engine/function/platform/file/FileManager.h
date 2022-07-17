#pragma once
#include <string>

class FileManager
{
public:
	static bool readFile(const std::string& filepath, uint8_t*& dataPtr, uint32_t& dataSize);
};

