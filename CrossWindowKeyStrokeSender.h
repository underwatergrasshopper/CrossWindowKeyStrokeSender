////////////////////////////////////////////////////////////////////////////////
// MIT License
// 
// Copyright (c) 2022 underwatergrasshopper
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

/**
* CrossWindowKeyStrokeSender.h
* @author underwatergrasshopper
* @version 0.1
*/

#ifndef CROSSWINDOWKEYSTROKESENDER_H_
#define CROSSWINDOWKEYSTROKESENDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <utility>
#include <string>
#include <vector>

namespace CrossWindowKeyStrokeSender {

//==============================================================================
// Macros
//==============================================================================

#if defined(_DEBUG)
#define dbg_cwkss_print_int(val) { printf(#val"=%d\n", int(val)); fflush(stdout); } (void)0
#define dbg_cwkss_print_i64(val) { printf(#val"=%lld\n", (long long)(val)); fflush(stdout); } (void)0
#define dbg_cwkss_print_u64(val) { printf(#val"=%ulld\n", (long long unsigned)(val)); fflush(stdout); } (void)0
#define dbg_cwkss_tprintf tprintf
#define dbg_cwkss_printf printf
#else
#define dbg_cwkss_print_int(val)
#define dbg_cwkss_print_i64(val)
#define dbg_cwkss_print_u64(val)
#define dbg_cwkss_tprintf(...)
#define dbg_cwkss_printf(...)
#endif

#define cwkss_print_int(val) { printf(#val"=%d\n", int(val)); fflush(stdout); } (void)0
#define cwkss_print_i64(val) { printf(#val"=%lld\n", (long long)(val)); fflush(stdout); } (void)0
#define cwkss_print_u64(val) { printf(#val"=%ulld\n", (long long unsigned)(val)); fflush(stdout); } (void)0

#define CWKSS_CASE_STR(id) case id: return #id

//==============================================================================
// Constants
//==============================================================================

enum {
    MAX_WAIT_TIME = 1000 * 60 * 60,    // in milliseconds
};

enum KeyAction {
    DOWN        = 0x01,
    UP          = 0x02,
    DOWN_AND_UP = DOWN | UP,
};

enum class EncodingModeID {
    UTF8,
    UTF16,
};

//==============================================================================
// Conversion
//==============================================================================
inline std::wstring UTF8_ToUTF16(const std::string& text) {
    std::wstring text_utf16;

    enum { COUNT = 256 };
    static wchar_t s_buffer[COUNT] = {};

    wchar_t* buffer = nullptr;

    if (text.length()) {
        const int count = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);

        buffer = (count > COUNT) ? (new wchar_t[count]) : s_buffer;

        if (MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, buffer, count)) {
            text_utf16 = std::wstring(buffer);
        }

        if (buffer != s_buffer) delete[] buffer;
    }
    return text_utf16;
}

inline std::string UTF16_ToUTF8(const std::wstring& text) {
    std::string text_utf8;

    enum { COUNT = 256 };
    static char s_buffer[COUNT] = {};

    char* buffer = nullptr;

    if (text.length()) {
        const int count = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, NULL, 0, NULL, NULL);

        buffer = (count > COUNT) ? (new char[count]) : s_buffer;

        if (WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer, count, NULL, NULL)) {
            text_utf8 = std::string(buffer);
        }

        if (buffer != s_buffer) delete[] buffer;
    }
    return text_utf8;
}

//==============================================================================
// Result
//==============================================================================

enum class ErrorID {
    NONE                                        = 0,
    CAN_NOT_SEND_MESSAGE                        = 1,
    CAN_NOT_FIND_TARGET_WINDOW                  = 2,
    CAN_NOT_FIND_FOREGROUND_WINDOW              = 3,
    CAN_NOT_RECEIVE_TARGET_WINDOW_THREAD_ID     = 4,
    CAN_NOT_RECEIVE_CALLER_WINDOW_THREAD_ID     = 5,
    CAN_NOT_ATTACH_CALLER_TO_TARGET             = 6,
    CAN_NOT_SET_TARGET_WINDOW_AS_FOREGROUND     = 7,
    CAN_NOT_GET_WINDOW_WITH_KEYBOARD_FOCUS      = 8,
    CAN_NOT_SET_CALLER_WINDOW_AS_FOREGROUND     = 9,
    CAN_NOT_DETTACH_CALLER_TO_TARGET            = 10,
    CALLER_IS_TARGET                            = 11,
    CAN_NOT_WAIT                                = 12,
};

inline bool IsOk(ErrorID error_id) {
    return error_id == ErrorID::NONE;
}
inline bool IsError(ErrorID error_id) {
    return error_id != ErrorID::NONE;
}

class Result {
public:
    Result() : m_error_id(ErrorID::NONE), m_last_error_code(0) {}

    Result(ErrorID error_id, const std::string& error_message, bool is_include_last_error_code = false) {
        m_error_id              = error_id;
        m_error_message_utf8    = "CWKSS Error: " + error_message;

        if (is_include_last_error_code) {
            m_last_error_code = GetLastError();
            this->m_error_message_utf8 += " (windows error code: " + std::to_string(m_last_error_code) + ")";
        }

        m_error_message_utf16 = UTF8_ToUTF16(m_error_message_utf8);
    }

    Result(ErrorID error_id, const std::wstring& error_message, bool is_include_last_error_code = false) {
        m_error_id              = error_id;
        m_error_message_utf16   = L"CWKSS Error: " + error_message;

        if (is_include_last_error_code) {
            m_last_error_code = GetLastError();
            m_error_message_utf16 += L" (windows error code: " + std::to_wstring(m_last_error_code) + L")";
        }

        m_error_message_utf8 = UTF16_ToUTF8(m_error_message_utf16);
    }

    bool IsOk() const { return m_error_id == ErrorID::NONE; }
    bool IsError() const { return !IsOk(); }

    ErrorID GetErrorID() const { return m_error_id; }
    std::string GetErrorMessageUTF8() const { return m_error_message_utf8; }
    std::wstring GetErrorMessageUTF16() const { return m_error_message_utf16; }
    int GetErrorCode() const { return m_last_error_code; }

private:
    ErrorID         m_error_id;           
    std::string     m_error_message_utf8;       
    std::wstring    m_error_message_utf16;
    int             m_last_error_code;    // Contains result from GetLastError() of WinApi library.
};

//==============================================================================
// Action
//==============================================================================

enum ActionTypeID {
    NONE                    = 0,
    KEY                     = 1,
    TEXT                    = 2,
    WAIT                    = 3,
    DELAY                   = 4,
    MESSAGE_ENCODING        = 5,
};

struct Action {
    int             type_id;

    int             vk_code;            // KEY
    int             key_action;         // KEY
    std::string     text_utf8;          // TEXT
    std::wstring    text_utf16;         // TEXT

    int             scan_code;          // KEY
    LPARAM          l_param_down;       // KEY
    LPARAM          l_param_up;         // KEY

    unsigned        wait_time;          // WAIT             // in milliseconds
    unsigned        delay;              // DELAY
    EncodingModeID  encoding_mode_id;   // MESSAGE_ENCODING
};

class Key {
public:
    Key()  : m_action({}) {}

    explicit Key(int vk_code, int key_action = KeyAction::DOWN_AND_UP)  : m_action({}) {
        m_action.type_id       = ActionTypeID::KEY;

        m_action.vk_code       = vk_code;
        m_action.key_action    = key_action;

        m_action.scan_code     = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

        m_action.l_param_down  = 0x00000001 | (m_action.scan_code  << 16);
        m_action.l_param_up    = 0xC0000001 | (m_action.scan_code  << 16);
    }

    operator Action() const { return m_action; }

private:
    Action m_action;  
};

class Text {
public:
    Text()  : m_action({}) {}

    explicit Text(const std::string& text)  : m_action({}) {
        m_action.type_id       = ActionTypeID::TEXT;

        m_action.text_utf8     = text;
        m_action.text_utf16    = UTF8_ToUTF16(text);
    }

    explicit Text(const std::wstring& text)  : m_action({}) {
        m_action.type_id       = ActionTypeID::TEXT;

        m_action.text_utf8     = UTF16_ToUTF8(text);
        m_action.text_utf16    = text;
    }

    operator Action() const { return m_action; }

private:
    Action m_action;  
};

class Wait {
public:
    Wait()  : m_action({}) {}

    explicit Wait(unsigned wait_time)  : m_action({}) {
        m_action.type_id       = ActionTypeID::WAIT;

        m_action.wait_time     = wait_time;
    }

    operator Action() const { return m_action; }

private:
    Action m_action;  
};

class Delay {
public:
    Delay()  : m_action({}) {}

    explicit Delay(unsigned delay)  : m_action({}) {
        m_action.type_id       = ActionTypeID::DELAY;

        m_action.delay          = delay;
    }

    operator Action() const { return m_action; }
private:
    Action m_action;  
};

class MessageEncoding {
public:
    MessageEncoding()  : m_action({}) {}

    explicit MessageEncoding(EncodingModeID encoding_mode_id)  : m_action({}) {
        m_action.type_id            = ActionTypeID::MESSAGE_ENCODING;

        m_action.encoding_mode_id   = encoding_mode_id;
    }

    operator Action() const { return m_action; }
private:
    Action m_action;  
};

class UTF8 : public MessageEncoding {
public:
    UTF8() : MessageEncoding(EncodingModeID::UTF8) {}
};

class UTF16 : public MessageEncoding {
public:
    UTF16() : MessageEncoding(EncodingModeID::UTF16) {}
};

//==============================================================================
// WaitForMS
//==============================================================================

enum class WaitResultID {
    SUCCESS                     = 0,
    ERROR_TO_BIG_WAIT_TIME      = 1,
    ERROR_INTERNAL_OVERFLOW     = 2,
};

inline bool IsError(WaitResultID id) {
    return id != WaitResultID::SUCCESS;
}

inline bool IsOk(WaitResultID id) {
    return id == WaitResultID::SUCCESS;
}

inline std::string WaitResultID_ToString(WaitResultID id) {
    switch (id) {
        CWKSS_CASE_STR(WaitResultID::SUCCESS);
        CWKSS_CASE_STR(WaitResultID::ERROR_TO_BIG_WAIT_TIME);
        CWKSS_CASE_STR(WaitResultID::ERROR_INTERNAL_OVERFLOW);
    }
    return "";
}

// Waits for specified amount of time.
// @param wait_time Time in milliseconds, not bigger than MAX_WAIT_TIME.
// @returns         WaitErrorID:
//                      NONE                - no error;
//                      TO_BIG_WAIT_TIME    - wait_time is bigger than MAX_WAIT_TIME;
//                      INTERNAL_OVERFLOW   - when internal performance counter overflow.
//                  Note: Some tests showed that, system must be running for even 29 years straight, to INTERNAL_OVERFLOW be able to happen.
//                  So this scenario is very unlikely.
inline WaitResultID WaitForMS(unsigned wait_time) {
    // Performance Counter Frequency does not change while system is running, so only need to be loaded once.
    static LARGE_INTEGER s_frequency = []() {
        LARGE_INTEGER frequency;
        if (!QueryPerformanceFrequency(&frequency)) {
            // System does not support Performance Counter.
            frequency.QuadPart = 0;
        }
        return frequency;
    }();

    if (wait_time > 0) {
        if (wait_time > MAX_WAIT_TIME) return WaitResultID::ERROR_TO_BIG_WAIT_TIME;

        dbg_cwkss_print_i64(s_frequency.QuadPart);

        if (s_frequency.QuadPart > 0) {
            // System support Performance Counter.
            int64_t elapsed; // in milliseconds

            LARGE_INTEGER begin;
            QueryPerformanceCounter(&begin);

            // Performance Counter overflow check
            if (begin.QuadPart > (LLONG_MAX / 1000)) {
                return WaitResultID::ERROR_INTERNAL_OVERFLOW;
            }
            const int64_t max_delay = (LLONG_MAX - begin.QuadPart * 1000) / s_frequency.QuadPart;
            if (int64_t(wait_time) > max_delay) {
                return WaitResultID::ERROR_INTERNAL_OVERFLOW;
            }

            dbg_cwkss_print_i64(max_delay);

            do {
                LARGE_INTEGER end;
                QueryPerformanceCounter(&end);

                elapsed = (end.QuadPart - begin.QuadPart) * 1000 / s_frequency.QuadPart;
            } while (elapsed < int64_t(wait_time));

        } else {
            // System does not support Performance Counter. Alternative substitute.
            Sleep(wait_time);
        }
    }
    return WaitResultID::SUCCESS;
}

//==============================================================================
// SendToWindow
//==============================================================================

// Sends messages to target window.
// @param target_window_name            Name of target window. Messages will be sent to this window.    [function variation]
//                                      Name of the window can be in ascii, utf-8 or utf-16 encoding: "Window Name", u8"Window Name", L"Window Name"
// @param target_window                 Handle to target window. Messages will be sent to this window.  [function variation]
// @param actions                       Array which contains any combination of following actions:
//                                          UTF8()                        - All followed messages will be sent as UTF8 message (by winapi function with A suffix).
//                                          UTF16()                       - All followed messages will be sent as UTF16 message (by winapi function with W suffix).
//                                          Delay(delay)                  - All followed messages will have dalay, in milliseconds, after each send of message.
//                                                                          Value of delay can not be bigger than MAX_WAIT_TIME.
//                                          Key(vk_code)                  - Sends key down message and key down message (as Virtual Key Code) to target window.
//                                          Key(vk_code, key_action)      - Sends key message (as Virtual Key Code) to target window.
//                                                                          vk_code:      VK_RETURN, VK_...
//                                                                          key_action:   KeyAction::DOWN, KeyAction::UP, eyAction::DOWN_AND_UP
//                                          Text(text)                    - Sends text to target target window. 
//                                                                          Text will be send to element of window which currently have keyboard focus.
//                                                                          The text can be in ascii, utf-8 or utf-16 encoding: Text("Window Name"), Text(u8"Window Name"), Text(L"Window Name").
// @param count                         Number of actions.                                              [function variation]
Result SendToWindow(HWND target_window, const Action* actions, unsigned count);
Result SendToWindow(const std::wstring& target_window_name, const Action* actions, unsigned count);
Result SendToWindow(const std::string& target_window_name, const Action* actions, unsigned count);
template <unsigned COUNT>
Result SendToWindow(const std::wstring& target_window_name, const Action (&actions)[COUNT]);
template <unsigned COUNT>
Result SendToWindow(const std::string& target_window_name, const Action (&actions)[COUNT]);


inline void PostKey(HWND window, EncodingModeID encoding_mode_id, const Action& message, Result& result) {
    if (encoding_mode_id == EncodingModeID::UTF16) {
        if (message.key_action & KeyAction::DOWN) {
            if (!PostMessageW(window, WM_KEYDOWN, message.vk_code, message.l_param_down)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post key down message.", true);
                return;
            }
        }

        if (message.key_action & KeyAction::UP) {
            if (!PostMessageW(window, WM_KEYUP, message.vk_code, message.l_param_up)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post key up message.", true);
                return;
            }
        }
    } else {
        if (message.key_action & KeyAction::DOWN) {
            if (!PostMessageA(window, WM_KEYDOWN, message.vk_code, message.l_param_down)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post key down message.", true);
                return;
            }
        }

        if (message.key_action & KeyAction::UP) {
            if (!PostMessageA(window, WM_KEYUP, message.vk_code, message.l_param_up)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post key up message.", true);
                return;
            }
        }
    }
}

inline void PostText(HWND window, EncodingModeID encoding_mode_id, const Action& message, Result& result) {
    dbg_cwkss_print_int(encoding_mode_id);

    if (encoding_mode_id == EncodingModeID::UTF16) {
        for (const auto& sign : message.text_utf16) {
            if (!PostMessageW(window, WM_CHAR, (unsigned short)sign, 0)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post character message.", true);
                return;
            }
        }
    } else {
        for (const auto& sign : message.text_utf8) {
            if (!PostMessageA(window, WM_CHAR, (unsigned short)sign, 0)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post character message.", true);
                return;
            }
        }
    }
}

inline Result SendMessages(HWND focus_window, const Action* actions, unsigned count) {
    Result result;

    unsigned        delay               = 1;
    EncodingModeID  encoding_mode_id    = EncodingModeID::UTF8;

    // Static delay to make sure that, target window goes to foreground.
    WaitForMS(100); // Pre-Initialize internal performance counters in Wait functions.

    for (unsigned ix = 0; ix < count; ix++) {
        const Action& action = actions[ix];

        switch (action.type_id) {
        case ActionTypeID::TEXT: {
            PostText(focus_window, encoding_mode_id, action, result);
            if (result.IsError()) return result;
            break;
        }
        case ActionTypeID::KEY: {
            PostKey(focus_window, encoding_mode_id, action, result);
            if (result.IsError()) return result;
            break;
        }
        case ActionTypeID::WAIT: {
            WaitResultID result_id = WaitForMS(action.wait_time);
            if (IsError(result_id))  return Result(ErrorID::CAN_NOT_WAIT, "Can not wait for specified amount of time from WAIT message (" + WaitResultID_ToString(result_id) + ").");
            break;
        }
        case ActionTypeID::DELAY: {
            delay = action.delay;
            break;
        }
        case ActionTypeID::MESSAGE_ENCODING: {
            encoding_mode_id = action.encoding_mode_id;
            break;
        }
        } // switch

        WaitResultID result_id = WaitForMS(delay);
        if (IsError(result_id))  return Result(ErrorID::CAN_NOT_WAIT, "Can not wait for specified amount of time after sending message (" + WaitResultID_ToString(result_id) + ").");
    }
    return result;
}

inline Result FocusAndSendMessages(HWND target_window, HWND foreground_window, const Action* actions, unsigned count) {
    BOOL is_success = SetForegroundWindow(target_window);

    if (!is_success) return Result(ErrorID::CAN_NOT_SET_TARGET_WINDOW_AS_FOREGROUND, "Can not set target widnow as foreground window.", true);

    HWND focus_window = GetFocus();

    dbg_cwkss_print_int(focus_window);

    if (!focus_window) return Result(ErrorID::CAN_NOT_GET_WINDOW_WITH_KEYBOARD_FOCUS, "Can not get window with keyboard focus.", true);

    Result result = SendMessages(focus_window, actions, count);
    if (result.IsError()) return result;

    is_success = SetForegroundWindow(foreground_window);

    if (!is_success) return Result(ErrorID::CAN_NOT_SET_CALLER_WINDOW_AS_FOREGROUND, "Can not set caller window back to foreground.", true);

    SetFocus(foreground_window);

    return Result();
}


inline Result SendToWindow(HWND target_window, const Action* actions, unsigned count) {
    HWND foreground_window = GetForegroundWindow();

    dbg_cwkss_print_int(foreground_window);

    if (!foreground_window) return Result(ErrorID::CAN_NOT_FIND_FOREGROUND_WINDOW, "Can not find foreground window.", true);

    DWORD target_window_thread_id = GetWindowThreadProcessId(target_window, NULL);

    dbg_cwkss_print_int(target_window_thread_id);

    if (!target_window_thread_id) return Result(ErrorID::CAN_NOT_RECEIVE_TARGET_WINDOW_THREAD_ID, "Can not receive target window thread id.");

    DWORD caller_window_thread_id = GetCurrentThreadId();

    dbg_cwkss_print_int(caller_window_thread_id);

    if (!caller_window_thread_id) return Result(ErrorID::CAN_NOT_RECEIVE_CALLER_WINDOW_THREAD_ID, "Can not receive caller window thread id.");

    if (target_window_thread_id && (target_window_thread_id != caller_window_thread_id)) {

        // BOOL is_success = AttachThreadInput(target_window_thread_id, caller_window_thread_id, TRUE); // debug

        BOOL is_success = AttachThreadInput(caller_window_thread_id, target_window_thread_id, TRUE);
        
        if (!is_success) return Result(ErrorID::CAN_NOT_ATTACH_CALLER_TO_TARGET, "Can not attach caller window thread to target window thread.", true);

        Result result = FocusAndSendMessages(target_window, foreground_window, actions, count);
        if (result.IsError()) {
            AttachThreadInput(caller_window_thread_id, target_window_thread_id, FALSE);
            return result;
        }

        // is_success = AttachThreadInput(target_window_thread_id, caller_window_thread_id, FALSE); // debug

        is_success = AttachThreadInput(caller_window_thread_id, target_window_thread_id, FALSE);

        if (!is_success) return Result(ErrorID::CAN_NOT_DETTACH_CALLER_TO_TARGET, "Can not dettach caller window thread from target window thread.", true);
    } else {
        // when target window is caller window

        Result result = SendMessages(target_window, actions, count);

        if (result.IsError()) return result;
    }

    return Result();
}

inline Result SendToWindow(const std::wstring& target_window_name, const Action* actions, unsigned count) {
    HWND target_window = FindWindowW(NULL, target_window_name.c_str());

    dbg_cwkss_print_int(target_window);
    
    if (!target_window) return Result(ErrorID::CAN_NOT_FIND_TARGET_WINDOW, "Can not find target window.", true);

    return SendToWindow(target_window, actions, count);
}

inline Result SendToWindow(const std::string& target_window_name, const Action* actions, unsigned count) {
    HWND target_window = FindWindowA(NULL, target_window_name.c_str());

    dbg_cwkss_print_int(target_window);

    if (!target_window) return Result(ErrorID::CAN_NOT_FIND_TARGET_WINDOW, "Can not find target window.", true);

    return SendToWindow(target_window, actions, count);
}

template <unsigned COUNT>
Result SendToWindow(const std::wstring& target_window_name, const Action (&actions)[COUNT]) {
    return SendToWindow(target_window_name, (const Action*)actions, COUNT);
}

template <unsigned COUNT>
Result SendToWindow(const std::string& target_window_name, const Action (&actions)[COUNT]) {
    return SendToWindow(target_window_name, (const Action*)actions, COUNT);
}

} // namespace CrossWindowKeyStrokeSender

namespace CWKSS = CrossWindowKeyStrokeSender;

#endif // CROSSWINDOWKEYSTROKESENDER_H_
