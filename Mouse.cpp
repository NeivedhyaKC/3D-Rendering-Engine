#include "Mouse.h"
#include "ChiliWin.h"

Mouse::Event::Type Mouse::Event::GetType() const noexcept
{
    return m_type;
}

std::pair<int, int> Mouse::Event::GetPos() const noexcept
{
    return std::pair<int, int>(m_xPos, m_yPos);
}

bool Mouse::Event::IsLeftButtonPressed() const noexcept
{
    return m_bIsLeftButtonPressed;
}

bool Mouse::Event::IsRightButtonPressed() const noexcept
{
    return m_bIsRightButtonPressed;
}

bool Mouse::Event::IsValid() const noexcept
{
    return m_type != Type::Invalid;
}

std::pair<int, int> Mouse::GetPos() const noexcept
{
    return std::pair<int, int>( m_xPos, m_yPos);
}

bool Mouse::IsLeftButtonPressed() const noexcept
{
    return m_bIsLeftButtonPressed;
}

bool Mouse::IsRightButtonPressed() const noexcept
{
    return m_bIsRightButtonPressed;
}

bool Mouse::IsEmpty() const noexcept
{
    return m_buffer.empty();
}

bool Mouse::RawCursorEnabled()
{
    return rawCursorEnabled;
}

void Mouse::EnableRaw()
{
    rawCursorEnabled = true;
}

void Mouse::DisableRaw()
{
    rawCursorEnabled = false;
}

std::optional<Mouse::Event> Mouse::Read()  noexcept
{
    if (m_buffer.size() > 0u)
    {
        Mouse::Event e = m_buffer.front();
        m_buffer.pop();
        return e;
    }
    return {};
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta()
{
    if (rawDeltaQueue.empty())
    {
        return std::nullopt;
    }
    const RawDelta d = rawDeltaQueue.front();
    rawDeltaQueue.pop();
    return d;
}

void Mouse::Flush() noexcept
{
    m_buffer = std::queue<Event>();
}

bool Mouse::IsInWindow() const noexcept
{
    return m_bIsInWindow;
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_buffer.push(Event(Event::Type::Move, *this));
    Trimbuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsLeftButtonPressed = true;
    m_buffer.push(Event(Event::Type::LPress, *this));
    Trimbuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsLeftButtonPressed = false;
    m_buffer.push(Event(Event::Type::LRelease, *this));
    Trimbuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsRightButtonPressed = true;
    m_buffer.push(Event(Event::Type::RPress, *this));
    Trimbuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsRightButtonPressed = false;
    m_buffer.push(Event(Event::Type::RRelease, *this));
    Trimbuffer();
}

void Mouse::OnWheelMove(int x, int y, int delta) noexcept
{
    m_wheelDeltaCarry += delta;
    if (m_wheelDeltaCarry >= WHEEL_DELTA)
    {
        m_wheelDeltaCarry -= WHEEL_DELTA;
        OnWheelUp(x, y);
    }
    else if (m_wheelDeltaCarry <= -WHEEL_DELTA)
    {
        m_wheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown(x, y);
    }
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_buffer.push(Event(Event::Type::WheelUp, *this));
    Trimbuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_buffer.push(Event(Event::Type::WheelDown, *this));
    Trimbuffer();
}

void Mouse::OnMouseEnter(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsInWindow = true;
    m_buffer.push(Event(Event::Type::Enter, *this));
    Trimbuffer();
}

void Mouse::OnMouseLeave(int x, int y) noexcept
{
    m_xPos = x;
    m_yPos = y;
    m_bIsInWindow = false;
    m_buffer.push(Event(Event::Type::Leave, *this));
    Trimbuffer();
}

void Mouse::OnRawDelta(int x, int y) noexcept
{
    rawDeltaQueue.push({ x,y });
    TrimRawInputBuffer();
}

void Mouse::Trimbuffer() noexcept
{
    while(m_buffer.size() > m_bufferSize)
    {
        m_buffer.pop();
    }
}

void Mouse::TrimRawInputBuffer() noexcept
{
    while (rawDeltaQueue.size() > m_bufferSize)
    {
        rawDeltaQueue.pop();
    }
}
