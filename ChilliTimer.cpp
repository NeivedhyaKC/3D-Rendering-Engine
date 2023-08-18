#include "ChilliTimer.h"

ChilliTimer::ChilliTimer() noexcept :
    m_last(std::chrono::steady_clock::now())
{

}

float ChilliTimer::Peek() const noexcept
{
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_last).count();
}

float ChilliTimer::Mark() noexcept
{
    auto old = m_last;
    m_last = std::chrono::steady_clock::now();
    std::chrono::duration<float> delta = m_last - old;
    return delta.count();
}
