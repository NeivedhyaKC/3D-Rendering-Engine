#pragma once
#include<queue>
#include<utility>
#include<optional>
class Mouse
{
    friend class Window;
public:
    class Event
    {
    public:
        enum class Type
        {
            LPress, LRelease, RPress, RRelease, WheelUp, WheelDown, Move, Enter, Leave, Invalid
        };
        Event(Type type, const Mouse& parent) noexcept:
            m_type(type),
            m_xPos(parent.m_xPos),
            m_yPos(parent.m_yPos),
            m_bIsLeftButtonPressed(parent.m_bIsLeftButtonPressed),
            m_bIsRightButtonPressed(parent.m_bIsRightButtonPressed)
        {
        }
        
        Type GetType() const noexcept;
        std::pair<int, int> GetPos() const noexcept;
        bool IsLeftButtonPressed() const noexcept;
        bool IsRightButtonPressed() const noexcept;
        bool IsValid() const noexcept;

    private:
        Type m_type = Type::Invalid;
        int m_xPos = 0;
        int m_yPos = 0;
        bool m_bIsLeftButtonPressed = false;
        bool m_bIsRightButtonPressed = false;

    };
public:
    struct RawDelta
    {
        int x;
        int y;
    };

    Mouse() = default;
    Mouse& operator=(const Mouse& mouse) = delete;
    Mouse(const Mouse& mouse) = delete;

    std::pair<int, int> GetPos() const noexcept;
    bool IsLeftButtonPressed() const noexcept;
    bool IsRightButtonPressed() const noexcept;
    bool IsEmpty() const noexcept;
    bool RawCursorEnabled();
    void EnableRaw();
    void DisableRaw();
    std::optional<Event> Read() noexcept;
    std::optional<Mouse::RawDelta> ReadRawDelta();
    void Flush() noexcept;
private:

    bool IsInWindow() const noexcept;
    void OnMouseMove(int x, int y) noexcept;
    void OnLeftPressed(int x, int y) noexcept;
    void OnLeftReleased(int x, int y) noexcept;
    void OnRightPressed(int x, int y) noexcept;
    void OnRightReleased(int x, int y) noexcept;
    void OnWheelMove(int x, int y, int delta) noexcept;
    void OnWheelUp(int x, int y) noexcept;
    void OnWheelDown(int x, int y) noexcept;
    void OnMouseEnter(int x, int y) noexcept;
    void OnMouseLeave(int x, int y) noexcept;
    void OnRawDelta(int x, int y) noexcept;
    void Trimbuffer() noexcept;
    void TrimRawInputBuffer() noexcept;

    static constexpr unsigned int m_bufferSize = 16u;
    std::queue<Event> m_buffer;
    std::queue<RawDelta> rawDeltaQueue;
    int m_xPos = 0;
    int m_yPos = 0;
    bool rawCursorEnabled = true;
    bool m_bIsLeftButtonPressed = false;
    bool m_bIsRightButtonPressed = false;
    bool m_bIsInWindow = false;
    int m_wheelDeltaCarry = 0;
};

