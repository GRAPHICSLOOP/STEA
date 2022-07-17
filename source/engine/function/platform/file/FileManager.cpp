#include "FileManager.h"
#include "core/base/macro.h"

bool FileManager::readFile(const std::string& filepath, uint8_t*& dataPtr, uint32_t& dataSize)
{
    FILE* file;
    fopen_s(&file,filepath.c_str(), "rb");

    if (!file)
    {
        STEALOG_ERROR("File not found :%s", filepath.c_str());
        return false;
    }

    fseek(file, 0, SEEK_END);
    dataSize = (uint32_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    if (dataSize <= 0)
    {
        fclose(file);
        STEALOG_WARN("File has no data :%s", filepath.c_str());
        return false;
    }

    dataPtr = new uint8[dataSize];
    fread(dataPtr, 1, dataSize, file);
    fclose(file);

    return true;
}
