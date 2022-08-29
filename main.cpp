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

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "CrossWindowKeyStrokeSender.h"

void RunTests() {
    using namespace CWKSS;

    // --- MessageEncodingUTF16_ToUTF8 andMessageEncodingASCI_ToUTF16 tests --- //
    assert(UTF16_ToUTF8(L"abc") == "abc");
    assert(UTF8_ToUTF16("abc") == L"abc");

    assert(UTF16_ToUTF8(L"śćń") == u8"śćń");
    assert(UTF8_ToUTF16(u8"śćń") == L"śćń");

    // --- Result tests --- //
    assert(Result(ErrorID::NONE, "abc", false).GetErrorMessage() == "CWKSS Error: abc");
    assert(Result(ErrorID::NONE, "abc", false).GetErrorMessageUTF16() == L"CWKSS Error: abc");

    assert(Result(ErrorID::NONE, u8"abc śćń", false).GetErrorMessage() == u8"CWKSS Error: abc śćń");
    assert(Result(ErrorID::NONE, u8"abc śćń", false).GetErrorMessageUTF16() == L"CWKSS Error: abc śćń");

    assert(Result(ErrorID::NONE, L"abc śćń", false).GetErrorMessage() == u8"CWKSS Error: abc śćń");
    assert(Result(ErrorID::NONE, L"abc śćń", false).GetErrorMessageUTF16() == L"CWKSS Error: abc śćń");

    DWORD last_error = GetLastError();

    assert(Result(ErrorID::NONE, "abc", true).GetErrorMessage() == "CWKSS Error: abc (windows error code: " + std::to_string(last_error) + ")");
    assert(Result(ErrorID::NONE, "abc", true).GetErrorMessageUTF16() == L"CWKSS Error: abc (windows error code: " + std::to_wstring(last_error) + L")");

    // --- Wait tests --- //
#if 0
    WaitForMS(1);
#endif
#if 0
    WaitForMS(1000);
#endif
#if 0
    unsigned wait_time = 10000;

    time_t start = time(NULL);

    WaitForMS(wait_time);

    time_t stop = time(NULL);
    time_t diff = difftime(stop, start);
    printf("time: %dsec\n", diff);

    assert(unsigned(diff) * 1000 == wait_time);
#endif

    // --- random tests --- //
#if 0

#endif
    // --- SendToWindow test scenarios --- //

    Result result;

    // ---
#if 0
    result = SendToWindow("Path of Exile", Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    const Action actions[] = {
        Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)
    };
    result = SendToWindow("Path of Exile", actions);
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    Action* actions = new Action[3];
    actions[0] = Key(VK_RETURN);
    actions[1] = Text("/kills");
    actions[2] = Key(VK_RETURN);

    result = SendToWindow("Path of Exile", actions, 3);

    delete[] actions;
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", ModePost(), Delay(10), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", ModePost(), Delay(10), Wait(100), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", Input(Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)), Wait(100));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

    // ---

#if 0
    result = SendToWindow("*Untitled - Notepad", Text("Some other text."));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("*Untitled - Notepad", ModePost(), Delay(10), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("*Untitled - Notepad", Input(Key(VK_RETURN), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN)), Wait(100));
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

    // --- SendToWindow junk scenario tests --- //

    // TODO: Make a proper Test of sending message where a caller window is the target window.
    // Note: This example does not work.
#if 0
    Sleep(1000);
    Message messages[] = { Text("notepad"), Key(VK_RETURN) };

    HWND target_window = GetForegroundWindow();
    dbg_cwkss_print_int(target_window);

    if (target_window) {
        result = SendToWindow(target_window, messages, 2, 1);
        puts(result.GetErrorMessage().c_str());
    } else {
        puts("Foreground window can not be found.");
    }
#endif

#if 0
    result = SendToWindow(L"Untitled - Notepad", { EachMessageAfterDelay(5), Text(L"śćńó"), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow(L"Untitled - Notepad", { EachMessageAfterDelay(5), Text(L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa0"), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow(L"Untitled - Notepad", { MessageEncodingUTF16(), EachMessageAfterDelay(5), Text(L"Some not long text."), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());

    result = SendToWindow(L"*Untitled - Notepad", { MessageEncodingUTF16(), EachMessageAfterDelay(5), Text(L"Some a litle longer text."), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());

    result = SendToWindow(L"*Untitled - Notepad", { MessageEncodingUTF16(), EachMessageAfterDelay(5), Text(L"And another text."), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("*Untitled - Notepad", { MessageEncodingUTF16(), EachMessageAfterDelay(1), Key(VK_RETURN), Text("Some not long text."), Key(VK_RETURN) };
    puts(result.GetErrorMessage().c_str());

    result = SendToWindow(L"*Untitled - Notepad", { MessageEncodingUTF16(), EachMessageAfterDelay(1), Key(VK_RETURN), Text(L"Some a litle longer text."), Key(VK_RETURN) });
    puts(result.GetErrorMessage().c_str());

    result = SendToWindow(L"*Untitled - Notepad", {MessageEncodingASCII(), EachMessageAfterDelay(10), Key(VK_RETURN), Text(L"And another text."), Key(VK_RETURN) });
    puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { MessageEncodingUTF16(), EachMessageAfterDelay(10), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { Input({ Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) }), Wait(100) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("*Untitled - Notepad", { DeliveryModeSend(), MessageEncodingUTF16(), EachMessageAfterDelay(0), Wait(100), Key(VK_RETURN), TextInput("/kill"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("*Untitled - Notepad", { Input({ Key(VK_RETURN), Text("/kill"), Key(VK_RETURN) }), Wait(100)});
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { DeliveryModeSend(), MessageEncodingASCI(), EachMessageAfterDelay(0), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow(L"Simple Window \u0444 안녕하세요", { DeliveryModeSend(), MessageEncodingASCI(), EachMessageAfterDelay(0), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", {MessageEncodingASCI(), EachMessageAfterDelay(10), Wait(1000), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });
    if (result.IsError()) puts(result.GetErrorMessage().c_str());
#endif

#if 0
    result = SendToWindow(L"Path of Exile", { MessageEncodingUTF16(), EachMessageAfterDelay(1), Key(VK_RETURN), Text(L"/kills"), Key(VK_RETURN)  };
    wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", {MessageEncodingASCI(), EachMessageAfterDelay(1), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)  });
    wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { MessageEncodingUTF16(), EachMessageAfterDelay(10), Key(VK_RETURN, KeyAction::DOWN), Key(VK_RETURN, KeyAction::UP), Text("/kills"), Key(VK_RETURN, KeyAction::DOWN), Key(VK_RETURN, KeyAction::UP) };
    wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
#endif

#if 0
    result = SendToWindow("Path of Exile", { MessageEncodingUTF16(), EachMessageAfterDelay(1), Key(VK_RETURN), Text("/atlaspassives"), Key(VK_RETURN), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)  });
    wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
#endif

}

int main() {
    RunTests();
    return 0;
}