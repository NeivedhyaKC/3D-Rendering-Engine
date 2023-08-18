#pragma once
#include<string>
#include<exception>
class ChilliException : std::exception
{
public:
    ChilliException(int line,std::string file) noexcept;
    const char* what() const noexcept override;
    virtual const std::string VGetType() const noexcept;
    int GetLine() const noexcept;
    const std::string& GetFile() const noexcept;
    const std::string GetOriginString() const noexcept;
private:
    int m_line = 0;
    std::string m_file;
protected:
    mutable std::string m_whatBuffer;
};

