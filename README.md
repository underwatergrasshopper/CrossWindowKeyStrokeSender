# CrossWindowKeyStrokeSender
Simple library for sending keyboard messages to window application from other window application or console application.

To be able use library, copy `CrossWindowKeyStrokeSender.h` file to project source folder.

It's designed for short messages, which have no more than 1000 character, like sending some chat command to a video game.
Encoding formats of message supported by library: ascii, utf-8 and utf-16.
If sent message seems to be crangled when arrives to target window, try using bigger value in `Delay(delay)` in `SendToWindow` function.

## Build Info
- Compiled on: Visual Studio 2022
- Target paltform: Windows 7/8/10 (32bit and 64bit)
- Language: C++11

## Usage Examples

### Example 1
Sends `/kills` command to "Path of Exile"'s game window, by using delivery mode: `ModeSend()`, which is default. Might be slow, but it is most reliable method.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", { Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

Same example, but with utf-16 text.
```c++
using namespace CWKSS;

Result result = SendToWindow(UTF16(), L"Path of Exile", { Key(VK_RETURN), Text(L"/kills"), Key(VK_RETURN) });

if (result.IsError()) puts(result.GetErrorMessageUTF16().c_str());
```

### Example 2
Sends `/kills` command to "Path of Exile"'s game window, by using delivery mode: `ModePos()`. 
Much faster, but after each message program needs to wait some small amount of time to make sure that posted message is completed, otherwise sent messages might end being crangled.
Action `Delay(10)` makes sure that after each message posting, program waits for 10 milliseconds.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", { ModePost(), Delay(10), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

### Example 3
Sends `/kills` command to "Path of Exile"'s game window, by using action: `Input({action, ...})`. In this mode all key and text messages are sent as if they were pressed manually.
This action needs to wait some amount of time after sending message to make sure, everything were processed. All text messages are always sent in utf-16 encoding format.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", { Input({ Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) }), Wait(100)});

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```


### Example 4
Sends text messages to empty notepad window, with delay after each message is posted.
```c++
using namespace CWKSS;

Result result = SendToWindow("Untitled - Notepad", { ModePost(), Delay(10), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN) });
if (result.IsError()) puts(result.GetErrorMessage().c_str());
```


### Example 5
Sends text messages to empty notepad window, in one input, with waiting for 100 milliseconds after input is sent.
```c++
using namespace CWKSS;

Result result = SendToWindow("*Untitled - Notepad", { Input({ Key(VK_RETURN), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN) }), Wait(100) });
if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

