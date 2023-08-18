#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) noexcept
{
    return m_keyStates[keycode];
}

bool Keyboard::KeyIsEmpty() noexcept
{
    return m_keyBuffer.empty();
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
    if(m_keyBuffer.size() > 0u)
    {
        Keyboard::Event e = m_keyBuffer.front();
        m_keyBuffer.pop();
        return e;
    }
    return {};
}

void Keyboard::FlushKey() noexcept
{
    m_keyBuffer = std::queue<Event>();
}

std::optional<unsigned char> Keyboard::ReadChar() noexcept
{
    if (m_charBuffer.size() > 0u)
    {
        char c = m_charBuffer.front();
        m_charBuffer.pop();
        return c;
    }
    return {};
}

bool Keyboard::CharIsEmpty() noexcept
{
    return m_charBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    m_charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
    m_keyStates[keycode] = true;
    m_keyBuffer.push(Event(Event::Type::Press, keycode));
    TrimBuffer(&m_keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
    m_keyStates[keycode] = false;
    m_keyBuffer.push(Event(Event::Type::Release, keycode));
    TrimBuffer(&m_keyBuffer);
}

void Keyboard::OnChar(unsigned char character) noexcept
{
    m_charBuffer.push(character);
    TrimBuffer(&m_charBuffer);
}

void Keyboard::ClearState() noexcept
{
    m_keyStates.reset();
}

void Keyboard::EnableAutoRepeat(bool autoRepeat) noexcept
{
    m_autoRepeatEnabled = autoRepeat;
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>* buffer) noexcept
{
    while (buffer->size() > m_bufferSize)
    {
        buffer->pop();
    }
}