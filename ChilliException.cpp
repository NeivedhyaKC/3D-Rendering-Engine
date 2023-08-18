#include "ChilliException.h"
#include<sstream>

ChilliException::ChilliException(int line, std::string file) noexcept :
    m_line(line),
    m_file(file)
{

}

const char* ChilliException::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type} " << VGetType() << std::endl
        << GetOriginString() << std::endl;
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const std::string ChilliException::VGetType() const noexcept
{
    return "ChilliException";
}

int ChilliException::GetLine() const noexcept
{
    return m_line;
}

const std::string& ChilliException::GetFile() const noexcept
{
    return m_file;
}

const std::string ChilliException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[Line] " << GetLine() << std::endl
        << "[File] " << GetFile() << std::endl;
    return oss.str();
}
