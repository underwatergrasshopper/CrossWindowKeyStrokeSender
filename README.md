# CrossWindowKeyStrokeSender
Simple library which provides ability to send a key messages or text messages from one window to another window.
Any of both windows can be either an application window or a console window.

It's designed for sending short messages, which have around 1000 character or keystrokes.

It might be used for:
- Sending command to other window. For example, to a video game or specific console window.
- Sending text messages to open text editor.

## Install
To be able use library, copy `CrossWindowKeyStrokeSender.h` file to project source folder.

## Build Info
- Compiled on: Visual Studio 2022
- Target platform: Windows 7/8/10 (32bit and 64bit)
- Language: C++11

# Message Delivery Method
Library uses three message delivery methods: Input, Send, Post

## Input Delivery Method
Input delivery method simulates keyboard input messages. Messages are delivered as they would be pressed on keyboard. 
After sending input message, a wait time (`Wait(time_in_milliseconds)`) is required to be relatively sure if message is processed by target window.
If multiple input messages are send, setting delay time (`Delay(time_in_milliseconds)`) is required. 
Delay makes `SendToWindow` function to wait a given amount of time after sending each input message, to relatively prevent collision of processing input messages.
Function `SendToWindow` must be called from main thread of application.
Using this method is most recommended.

*Note: Setting correct amount of time in `Delay` action and `Wait` action depends on: size of messages, speed of processor, reactability of target window.*

### Input Example 1
Sends text message to Notepad window as single text message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Input(Text("Some Text.\nOther text.\n")),
    Wait(100));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Input Example 2
Sends text message in utf-16 encoding format to Notepad window as single text message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Input(Text(L"Some Text.\nOther text.\nф𤭢\n")),
    Wait(100));

wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
```

### Input Example 3
Sends text message to Notepad window as single input of multiple messages.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Input(
        Text("Some Text."),
        Key(VK_RETURN),
        Text("Other Text."),
        Key(VK_RETURN)),
    Wait(100));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Input Example 4
Sends text message to Notepad window as multiple inputs of single messages.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Delay(10),
    Input(Text("Some Text.")),
    Input(Key(VK_RETURN)),
    Input(Text("Other Text.")),
    Input(Key(VK_RETURN)),
    Wait(100));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Input Example 5
Sends `ś` character to Notepad window by using key sequence.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Input(
        Key(VK_RMENU, KeyState::DOWN),
        Key('S'),
        Key(VK_RMENU, KeyState::UP)),
    Wait(100));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Input Example 6
Sends `/kills` command to "Path of Exile" game window.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Path of Exile", 
    Input(
        Key(VK_RETURN),
        Text("/kills"),
        Key(VK_RETURN)),
    Wait(100));

printf("%s\n", result.GetErrorMessage().c_str());
```

## Send Delivery Method
Allows to send key messages and text messages to target window. For each message sent, `SendToWindow` function waits until message is processed by target window.
Sending messages might take some time. Setting delay time or wait time is NOT required.
Function `SendToWindow` must be called from main thread of application.

### Send Example 1
Sends text message to Notepad window as single text message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Text("Some Text.\nOther text.\n"));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Send Example 2
Sends text message in utf-16 encoding format to Notepad window as single message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    UTF16(),
    Text(L"Some Text.\nOther text.\nф𤭢\n"));

wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
```

### Send Example 3
Sends text message to Notepad window as multiple messages.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    Text("Some Text.\n"),
    Text("Other Text.\n"));

printf("%s\n", result.GetErrorMessage().c_str());

```

### Send Example 4
Sends `/kills` command to "Path of Exile" game window.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Path of Exile", 
    Key(VK_RETURN),
    Text("/kills"),
    Key(VK_RETURN));

printf("%s\n", result.GetErrorMessage().c_str());
```

## Post Delivery Method
Allows to send key messages and text messages to target window. Messages are sent to target window message queue before they are processed.
Setting delay time (`Delay(time_in_milliseconds)`) is required. Delay makes `SendToWindow` function to wait given amount of time after sending each message, to relatively prevent collision of processing messages.
Function `SendToWindow` not necessary needs to be called from main thread of application.

If sent message seems to be crangled when arrives to target window, try using bigger value in `Delay(time_in_milliseconds)` or `Wait(time_in_milliseconds)` in `SendToWindow` function.

*Note: Setting correct amount of time in `Delay` action and `Wait` action depends on: size of messages, speed of processor, reactability of target window.*

### Post Example 1
Sends text message to Notepad window as single text message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    ModePost(),
    Delay(10),
    Text("Some Text.\nOther text.\n"));

printf("%s\n", result.GetErrorMessage().c_str());
```

### Post Example 2
Sends text message in utf-16 encoding format to Notepad window as single message.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    UTF16(),
    ModePost(),
    Delay(10),
    Text(L"Some Text.\nOther text.\nф𤭢\n"));

wprintf(L"%s\n", result.GetErrorMessageUTF16().c_str());
```

### Post Example 3
Sends text message to Notepad window as multiple messages.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Untitled - Notepad", 
    ModePost(),
    Delay(10),
    Text("Some Text.\n"),
    Text("Other Text.\n"));

printf("%s\n", result.GetErrorMessage().c_str());

```

### Post Example 4
Sends `/kills` command to "Path of Exile" game window.
```c++
using namespace CWKSS;

result = SendToWindow(
    "Path of Exile", 
    ModePost(),
    Delay(10),
    Key(VK_RETURN),
    Text("/kills"),
    Key(VK_RETURN));

printf("%s\n", result.GetErrorMessage().c_str());
```