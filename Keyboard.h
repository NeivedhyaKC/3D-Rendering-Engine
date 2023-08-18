#pragma once
#include<queue>
#include<bitset>
#include<optional>
class Keyboard
{
    friend class Window;
public:
    Keyboard() = default;
    Keyboard(Keyboard& keyboard) = delete;
    Keyboard& operator=(Keyboard& keyboard) = delete;
    class Event
    {
    public:
        enum class Type { Press, Release };

        Event(Type type, unsigned char code) noexcept :
            m_type(type),
            m_code(code)
        {
        }

        Type GetType() const { return m_type; }
        unsigned char GetCode() const { return m_code; }
    private:
        Type m_type;
        unsigned char m_code;

    };
    bool KeyIsPressed(unsigned char keycode) noexcept;
    bool KeyIsEmpty() noexcept;
    std::optional<Event> ReadKey() noexcept;
    void FlushKey() noexcept;

    std::optional<unsigned char> ReadChar() noexcept;
    bool CharIsEmpty() noexcept;
    void FlushChar() noexcept;
    void Flush() noexcept;

private:
    void OnKeyPressed(unsigned char keycode) noexcept;
    void OnKeyReleased(unsigned char keycode) noexcept;
    void OnChar(unsigned char character) noexcept;
    void ClearState() noexcept;
    template<typename T>
    static void TrimBuffer(std::queue<T>* buffer) noexcept;

    void EnableAutoRepeat(bool autoRepeat) noexcept;
    bool IsAutoRepeatEnabled() noexcept { return m_autoRepeatEnabled; }

private:
    static constexpr unsigned int m_nkeys = 256u;
    static constexpr unsigned int m_bufferSize = 16u;
    bool m_autoRepeatEnabled = false;
    std::queue<Event> m_keyBuffer;
    std::queue<char> m_charBuffer;
    std::bitset<m_nkeys> m_keyStates;
};
