#pragma once

#include <string>
#include <yuni/core/string.h>

#include "i_writer.h"

namespace Antares
{
namespace Solver
{
class ImmediateFileResultWriter : public IResultWriter
{
public:
    ImmediateFileResultWriter(const char* folderOutput);
    virtual ~ImmediateFileResultWriter();
    // Write to file immediately, creating directories if needed
    void addEntryFromBuffer(const std::string& entryPath, Yuni::Clob& entryContent) override;
    void addEntryFromBuffer(const std::string& entryPath, std::string& entryContent) override;
    void addEntryFromFile(const std::string& entryPath, const std::string& filePath) override;
    bool needsTheJobQueue() const override;
    void finalize(bool verbose) override;

private:
    Yuni::String pOutputFolder;
};
} // namespace Solver
} // namespace Antares