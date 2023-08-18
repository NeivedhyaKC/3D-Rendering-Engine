#pragma once
#include<chrono>
class ChilliTimer
{
public:
    ChilliTimer() noexcept;
    float Peek() const noexcept;
    float Mark() noexcept;
private:
    std::chrono::steady_clock::time_point m_last;
};

