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
#define dbg_cwkss_print_ptr(val) { printf(#val"=%08X\n", reinterpret_cast<unsigned>(val)); fflush(stdout); } (void)0
#define dbg_cwkss_print_ptr64(val) { printf(#val"=%016llX\n", reinterpret_cast<long long unsigned>(val)); fflush(stdout); } (void)0
#define dbg_cwkss_tprintf tprintf
#define dbg_cwkss_printf printf
#else
#define dbg_cwkss_print_int(val)
#define dbg_cwkss_print_i64(val)
#define dbg_cwkss_print_u64(val)
#define dbg_cwkss_print_ptr(val) 
#define dbg_cwkss_print_ptr64(val) 
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

enum class MessageEncodingID {
    ASCII,
    UTF16,
};

enum class DeliveryModeID {
    SEND,
    POST,
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
    std::string GetErrorMessage() const { return m_error_message_utf8; }
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

enum class ActionTypeID {
    NONE                    = 0,
    KEY                     = 1,
    TEXT                    = 2,
    WAIT                    = 3,
    DELAY                   = 4,
    MESSAGE_ENCODING        = 5,
    DELIVERY_MODE           = 6,
    INPUT                   = 7,
};

struct Action {
    ActionTypeID        type_id;

    int                 vk_code;                // KEY
    int                 key_action;             // KEY
    std::string         text_utf8;              // TEXT
    std::wstring        text_utf16;             // TEXT

    int                 scan_code;              // KEY
    LPARAM              l_param_down;           // KEY
    LPARAM              l_param_up;             // KEY

    unsigned            wait_time;              // WAIT                 // in milliseconds
    unsigned            delay;                  // DELAY
    MessageEncodingID   message_encoding_id;    // MESSAGE_ENCODING
    DeliveryModeID      delivery_mode_id;       // DELIVERY_MODE

    std::vector<INPUT>  inputs;                 // INPUT
};

class KeyMessage {
public:
    KeyMessage()  : m_action({}) {}

    explicit KeyMessage(int vk_code, int key_action = KeyAction::DOWN_AND_UP)  : m_action({}) {
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

class TextMessage {
public:
    TextMessage()  : m_action({}) {}

    // @param text      Unicode text in utf-8 format.
    explicit TextMessage(const std::string& text)  : m_action({}) {
        m_action.type_id       = ActionTypeID::TEXT;

        m_action.text_utf8     = text;
        m_action.text_utf16    = UTF8_ToUTF16(text);
    }

    // @param text      Unicode text in utf-16 format.
    explicit TextMessage(const std::wstring& text)  : m_action({}) {
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

class EachMessageAfterDelay {
public:
    EachMessageAfterDelay()  : m_action({}) {}

    explicit EachMessageAfterDelay(unsigned delay)  : m_action({}) {
        m_action.type_id       = ActionTypeID::DELAY;

        m_action.delay         = delay;
    }

    operator Action() const { return m_action; }
private:
    Action m_action;  
};

class MessageEncoding {
public:
    MessageEncoding()  : m_action({}) {}

    explicit MessageEncoding(MessageEncodingID message_encoding_id)  : m_action({}) {
        m_action.type_id                = ActionTypeID::MESSAGE_ENCODING;

        m_action.message_encoding_id    = message_encoding_id;
    }

    operator Action() const { return m_action; }
private:
    Action m_action;  
};

class MessageEncodingASCII : public MessageEncoding {
public:
    MessageEncodingASCII() : MessageEncoding(MessageEncodingID::ASCII) {}
};

class MessageEncodingUTF16 : public MessageEncoding {
public:
    MessageEncodingUTF16() : MessageEncoding(MessageEncodingID::UTF16) {}
};

class DeliveryMode {
public:
    DeliveryMode()  : m_action({}) {}

    explicit DeliveryMode(DeliveryModeID delivery_mode_id)  : m_action({}) {
        m_action.type_id                = ActionTypeID::DELIVERY_MODE;

        m_action.delivery_mode_id       = delivery_mode_id;
    }

    operator Action() const { return m_action; }
private:
    Action m_action;  
};

class DeliveryModeSend : public DeliveryMode {
public:
    DeliveryModeSend() : DeliveryMode(DeliveryModeID::SEND) {}
};

class DeliveryModePost : public DeliveryMode {
public:
    DeliveryModePost() : DeliveryMode(DeliveryModeID::POST) {}
};

class InputMessage {
public:
    InputMessage()  : m_action({}) {}

    // Makes input messages. All Text actions are converted to messages in utf-16 format.
    // @actions         Array of actions. Only Key and Text actions are processed. Other are ignorored.
    template <unsigned COUNT>
    explicit InputMessage(const Action (&actions)[COUNT]) : m_action({}) {
        Initalize(actions);
    }

    // Makes input messages. All Text actions are converted to messages in utf-16 format.
    // @actions         Array of actions. Only Key and Text actions are processed. Other are ignorored.
    template <typename... Actions>
    InputMessage(Actions&&... actions) : m_action({}) {
        Initalize({ std::forward<Actions>(actions)... });
    }

    operator Action() const { return m_action; }

private:
    template <unsigned COUNT>
    void Initalize(const Action (&actions)[COUNT]) {
        m_action.type_id = ActionTypeID::INPUT;

        //m_action.inputs.reserve(COUNT * 2);

        for (const auto& action : actions) {
            switch (action.type_id) {
            case ActionTypeID::KEY:
                MakeKeyInput(m_action.inputs, action.vk_code, action.key_action);
                break;
            case ActionTypeID::TEXT:
                MakeTextInputUTF16(m_action.inputs, action.text_utf16);
                break;
            } 
        }
    }

    static void MakeKeyInput(std::vector<INPUT>& inputs, int vk_code, int key_action) {
        if (key_action & KeyAction::DOWN) {
            INPUT input = {};

            input.type              = INPUT_KEYBOARD;
            input.ki.wVk            = VK_RETURN;
            input.ki.wScan          = 0;
            input.ki.time           = 0;
            input.ki.dwFlags        = 0;
            input.ki.dwExtraInfo    = 0;

            inputs.push_back(input);
        }

        if (key_action & KeyAction::DOWN) {
            INPUT input = {};

            input.type              = INPUT_KEYBOARD;
            input.ki.wVk            = VK_RETURN;
            input.ki.wScan          = 0;
            input.ki.time           = 0;
            input.ki.dwFlags        = KEYEVENTF_KEYUP;
            input.ki.dwExtraInfo    = 0;

            inputs.push_back(input);
        }
    }

    static void MakeTextInputUTF16(std::vector<INPUT>& inputs, const std::wstring& text) {
        for (const wchar_t& sign : text) {
            INPUT input = {};

            input.type              = INPUT_KEYBOARD;
            input.ki.wVk            = 0;
            input.ki.wScan          = (short)sign;
            input.ki.time           = 0;
            input.ki.dwFlags        = KEYEVENTF_UNICODE;
            input.ki.dwExtraInfo    = 0;
      
            inputs.push_back(input);

            input = {};

            input.type              = INPUT_KEYBOARD;
            input.ki.wVk            = 0;
            input.ki.wScan          = (short)sign;
            input.ki.time           = 0;
            input.ki.dwFlags        = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            input.ki.dwExtraInfo    = 0;

            inputs.push_back(input);
        }
    }

    Action m_action;  
};

class TextInputMessage : public InputMessage {
public:
    TextInputMessage() : InputMessage() {}
    explicit TextInputMessage(const std::string& text)  : InputMessage({TextMessage(text)}) {}
    explicit TextInputMessage(const std::wstring& text)  : InputMessage({TextMessage(text)}) {}
};

// NOTE: Regarding to: Action class don't have subclasses. 
// I decided to make SendToWindow function not have array of pointers to Actions classes, because it's adds operator 'new' to each Action object passed as element to array. It's makes the fuction call bloated.
// And I decided not use the 'object slicing', because it's hard to differ that feature from: if it was intentional or a bug.


// Use '#undef CWKSS_NO_SHORT_NAMES' if there is a name collision whit any short name bellow.
#ifndef CWKSS_NO_SHORT_NAMES
using Delay     = EachMessageAfterDelay;
using ASCII     = MessageEncodingASCII;
using UTF16     = MessageEncodingUTF16;
using ModeSend  = DeliveryModeSend;
using ModePost  = DeliveryModePost;
using Key       = KeyMessage;
using Text      = TextMessage;
using Input     = InputMessage;
using TextInput = TextInputMessage;
#endif // CWKSS_NO_SHORT_NAMES

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

inline void PreInitializeWaitForMS() {
    WaitForMS(0);
}

//==============================================================================
// SendToWindow
//==============================================================================

// Sends messages to target window.
// @param target_window_name            Name of target window. Messages will be sent to this window.    [function variation]
//                                      Name of the window can be in ascii, utf-8 or utf-16 encoding: "Window Name", u8"Window Name", L"Window Name"
// @param target_window                 Handle to target window. Messages will be sent to this window.  [function variation]
// @param actions                       Array which contains any combination of following actions:
//                                          ModeSend()                    - (Default) Function waits until message is delivered before sending another.
//                                          ModePos()                     - Function does not waits until message is delivered before sending another.
//                                          ASCII()                       - (Default) All key and text messages will be sent as ASCII message (by winapi function with A suffix).
//                                          UTF16()                       - All key and text messages will be sent as UTF16 message (by winapi function with W suffix).
//                                          Delay(delay)                  - All key and text messages will have dalay, in milliseconds, after each send of message (message is: Text, Key or Input).
//                                                                          Value of delay can not be bigger than MAX_WAIT_TIME.
//                                          Wait(wait_time)               - No message is send. Program wait for given amount of time.
//                                                                          Value of wait_time can not be bigger than MAX_WAIT_TIME.
//                                          Key(vk_code)                  - Sends key down message and key down message (as Virtual Key Code) to target window.
//                                          Key(vk_code, key_action)      - Sends key message (as Virtual Key Code) to target window.
//                                                                          vk_code:      VK_RETURN, VK_...
//                                                                          key_action:   KeyAction::DOWN, KeyAction::UP, eyAction::DOWN_AND_UP
//                                          Text(text)                    - Sends text to target target window. 
//                                                                          Text will be send to element of window which currently have keyboard focus.
//                                                                          The text can be in ascii, utf-8 or utf-16 encoding: Text("Window Name"), Text(u8"Window Name"), Text(L"Window Name").
//                                          Input(action, ...) or Input({action, ...}) - Sends messages in one input. Accepts only Key and Text actions. Sends messages in utf-16 encoding format only.
//                                      If this short actions names collide with external names, define CWKSS_NO_SHORT_NAMES, and go to CWKSS_NO_SHORT_NAMES to check what are longer names.
// @param count                         Number of actions.                                              [function variation]
Result SendToWindow(HWND target_window, const Action* actions, uint64_t count);

Result SendToWindow(const std::wstring& target_window_name, const Action* actions, uint64_t count);
Result SendToWindow(const std::string& target_window_name, const Action* actions, uint64_t count);

template <unsigned COUNT>
Result SendToWindow(const std::wstring& target_window_name, const Action (&actions)[COUNT]);
template <unsigned COUNT>
Result SendToWindow(const std::string& target_window_name, const Action (&actions)[COUNT]);

template <typename... Actions>
Result SendToWindow(const std::wstring& target_window_name, Action&& action, Actions&&... actions);
template <typename... Actions>
Result SendToWindow(const std::string& target_window_name, Action&& action, Actions&&... actions);

//==============================================================================

inline void PostKey(HWND window, MessageEncodingID message_encoding_id, const Action& message, Result& result) {
    dbg_cwkss_printf("PostKey\n");
    dbg_cwkss_print_int(message_encoding_id);

    if (message_encoding_id == MessageEncodingID::ASCII) {
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
    } else {
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
    }
}

inline void SendKey(HWND window, MessageEncodingID message_encoding_id, const Action& message, Result& result) {
    dbg_cwkss_printf("SendKey\n");
    dbg_cwkss_print_int(message_encoding_id);

    if (message_encoding_id == MessageEncodingID::ASCII) {
        if (message.key_action & KeyAction::DOWN) {
            SendMessageA(window, WM_KEYDOWN, message.vk_code, message.l_param_down);
        }

        if (message.key_action & KeyAction::UP) {
            SendMessageA(window, WM_KEYUP, message.vk_code, message.l_param_up);
        }
    } else {
        if (message.key_action & KeyAction::DOWN) {
            SendMessageW(window, WM_KEYDOWN, message.vk_code, message.l_param_down);
        }

        if (message.key_action & KeyAction::UP) {
            SendMessageW(window, WM_KEYUP, message.vk_code, message.l_param_up);
        }
    }
}

inline void PostText(HWND window, MessageEncodingID message_encoding_id, const Action& message, Result& result) {
    dbg_cwkss_printf("PostText\n");
    dbg_cwkss_print_int(message_encoding_id);

    if (message_encoding_id == MessageEncodingID::ASCII) {
        for (const auto& sign : message.text_utf8) {
            if (!PostMessageA(window, WM_CHAR, (unsigned short)sign, 0)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post character message.", true);
                return;
            }
        }
    } else {
        for (const auto& sign : message.text_utf16) {
            if (!PostMessageW(window, WM_CHAR, (unsigned short)sign, 0)) {
                result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not post character message.", true);
                return;
            }
        }
    }
}

inline void SendText(HWND window, MessageEncodingID message_encoding_id, const Action& message, Result& result) {
    dbg_cwkss_printf("SendText\n");
    dbg_cwkss_print_int(message_encoding_id);

    if (message_encoding_id == MessageEncodingID::ASCII) {
        for (const auto& sign : message.text_utf8) {
            SendMessageA(window, WM_CHAR, (unsigned short)sign, 0);
        }
    } else {
        for (const auto& sign : message.text_utf16) {
            SendMessageW(window, WM_CHAR, (unsigned short)sign, 0);
        }
    }
}

inline void SendInput(const Action& action, Result& result) {
    dbg_cwkss_printf("SendInput\n");

    std::vector<INPUT> inputs = action.inputs; // Unfortunately SendInput accepts a non constant pointer only.

    dbg_cwkss_print_int(inputs.size());

    UINT count = SendInput((UINT)inputs.size(), &(inputs[0]), sizeof(INPUT));

    if (count < inputs.size()) {
        result = Result(ErrorID::CAN_NOT_SEND_MESSAGE, "Can not send input message.", true);
    }
}

inline Result SendMessages(HWND focus_window, const Action* actions, uint64_t count) {
    Result result;

    unsigned            delay                   = 0;
    MessageEncodingID   message_encoding_id     = MessageEncodingID::ASCII;
    DeliveryModeID      delivery_mode_id        = DeliveryModeID::SEND;

    PreInitializeWaitForMS(); 

    auto WaitForMS_AndHandleResult = [](Result& result, unsigned delay) {
        WaitResultID result_id = WaitForMS(delay);
        if (IsError(result_id)) result = Result(ErrorID::CAN_NOT_WAIT, "Can not wait for specified amount of time after sending message (" + WaitResultID_ToString(result_id) + ").");
    };

    for (uint64_t ix = 0; ix < count; ix++) {
        const Action& action = actions[ix];

        switch (action.type_id) {
        case ActionTypeID::TEXT: {
            switch (delivery_mode_id) {
            case DeliveryModeID::POST:          PostText(focus_window, message_encoding_id, action, result);   break;
            case DeliveryModeID::SEND:          SendText(focus_window, message_encoding_id, action, result);   break;
            }
            if (result.IsError()) return result;

            WaitForMS_AndHandleResult(result, delay);
            if (result.IsError()) return result;

            break;
        }
        case ActionTypeID::KEY: {
            switch (delivery_mode_id) {
            case DeliveryModeID::POST:          PostKey(focus_window, message_encoding_id, action, result);   break;
            case DeliveryModeID::SEND:          SendKey(focus_window, message_encoding_id, action, result);   break;
            }
            if (result.IsError()) return result;

            WaitForMS_AndHandleResult(result, delay);
            if (result.IsError()) return result;

            break;
        }
        case ActionTypeID::INPUT: {
            SendInput(action, result);
            if (result.IsError()) return result;

            WaitForMS_AndHandleResult(result, delay);
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
            message_encoding_id = action.message_encoding_id;
            break;
        }
        case ActionTypeID::DELIVERY_MODE: {
            delivery_mode_id = action.delivery_mode_id;
            break;
        }
        } // switch
    }
    return result;
}

inline Result FocusAndSendMessages(HWND target_window, HWND foreground_window, const Action* actions, uint64_t count) {
    if (IsIconic(target_window)) ShowWindow(target_window, SW_RESTORE);

    BOOL is_success = SetForegroundWindow(target_window);

    if (!is_success) return Result(ErrorID::CAN_NOT_SET_TARGET_WINDOW_AS_FOREGROUND, "Can not set target widnow as foreground window.", true);

    // Note: Should be hardoced, use Wait action instead.
    // WaitForMS(100); // Reduces situation of: when window is not ready on time to receive messages.

    HWND focus_window = GetFocus();

    dbg_cwkss_print_ptr64(focus_window);

    if (!focus_window) return Result(ErrorID::CAN_NOT_GET_WINDOW_WITH_KEYBOARD_FOCUS, "Can not get window with keyboard focus.", true);

    Result result = SendMessages(focus_window, actions, count);
    if (result.IsError()) return result;

    is_success = SetForegroundWindow(foreground_window);

    if (!is_success) return Result(ErrorID::CAN_NOT_SET_CALLER_WINDOW_AS_FOREGROUND, "Can not set caller window back to foreground.", true);

    SetFocus(foreground_window);

    return Result();
}


inline Result SendToWindow(HWND target_window, const Action* actions, uint64_t count) {
    HWND foreground_window = GetForegroundWindow();

    dbg_cwkss_print_ptr64(foreground_window);

    if (!foreground_window) return Result(ErrorID::CAN_NOT_FIND_FOREGROUND_WINDOW, "Can not find foreground window.", true);

    DWORD target_window_thread_id = GetWindowThreadProcessId(target_window, NULL);

    dbg_cwkss_print_int(target_window_thread_id);

    if (!target_window_thread_id) return Result(ErrorID::CAN_NOT_RECEIVE_TARGET_WINDOW_THREAD_ID, "Can not receive target window thread id.");

    DWORD caller_window_thread_id = GetCurrentThreadId();

    dbg_cwkss_print_int(caller_window_thread_id);

    if (!caller_window_thread_id) return Result(ErrorID::CAN_NOT_RECEIVE_CALLER_WINDOW_THREAD_ID, "Can not receive caller window thread id.");

    if (target_window_thread_id && (target_window_thread_id != caller_window_thread_id)) {
        BOOL is_success = AttachThreadInput(caller_window_thread_id, target_window_thread_id, TRUE); // && AttachThreadInput(target_window_thread_id, caller_window_thread_id, TRUE); // debug
        
        if (!is_success) return Result(ErrorID::CAN_NOT_ATTACH_CALLER_TO_TARGET, "Can not attach caller window thread to target window thread.", true);

        Result result = FocusAndSendMessages(target_window, foreground_window, actions, count);
        if (result.IsError()) {
            AttachThreadInput(caller_window_thread_id, target_window_thread_id, FALSE);
            return result;
        }

        is_success = AttachThreadInput(caller_window_thread_id, target_window_thread_id, FALSE); // && AttachThreadInput(target_window_thread_id, caller_window_thread_id, FALSE); // debug

        if (!is_success) return Result(ErrorID::CAN_NOT_DETTACH_CALLER_TO_TARGET, "Can not dettach caller window thread from target window thread.", true);
    } else {
        // When target window is caller window.

        Result result = SendMessages(target_window, actions, count);

        if (result.IsError()) return result;
    }

    return Result();
}

inline Result SendToWindow(const std::wstring& target_window_name, const Action* actions, uint64_t count) {
    HWND target_window = FindWindowW(NULL, target_window_name.c_str());

    dbg_cwkss_print_ptr64(target_window);
    
    if (!target_window) return Result(ErrorID::CAN_NOT_FIND_TARGET_WINDOW, "Can not find target window.", true);

    return SendToWindow(target_window, actions, count);
}

inline Result SendToWindow(const std::string& target_window_name, const Action* actions, uint64_t count) {
    HWND target_window = FindWindowA(NULL, target_window_name.c_str());

    dbg_cwkss_print_ptr64(target_window);

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

// Note: Code 'Action&& action' fixes some call collisions, when this function was supposed to be called, but instead SendToWindow(const std::string&, const Action*, count) is prioritized to call.
template <typename... Actions>
inline Result SendToWindow(const std::wstring& target_window_name, Action&& action, Actions&&... actions) {
    return SendToWindow(target_window_name, { std::forward<Action>(action), std::forward<Actions>(actions)... });
}

template <typename... Actions>
inline Result SendToWindow(const std::string& target_window_name, Action&& action, Actions&&... actions) {
    return SendToWindow(target_window_name, { std::forward<Action>(action), std::forward<Actions>(actions)... });
}

} // namespace CrossWindowKeyStrokeSender

namespace CWKSS = CrossWindowKeyStrokeSender;

#endif // CROSSWINDOWKEYSTROKESENDER_H_
