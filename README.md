# CrossWindowKeyStrokeSender
Simple library for sending keyboard messages to window application from other window application or console application.

To be able use library, copy `CrossWindowKeyStrokeSender.h` file to project source folder.

It's designed for short messages, which have no more than 1000 character, like sending some chat command to a video game.
Supported encoding formats for messages by this library: ascii, utf-8 and utf-16.
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

Result result = SendToWindow("Path of Exile", Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

Sometimes window can't make on time with prepare for receiving messages. 
If some first messages are missing, add Wait(100) before first action message. 
If some last messages are missing, add Wait(100) after last action message.
The argument (in milliseconds) for Wait might be needed bigger or lesser, depends on situation. 
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", Wait(100), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", Key(VK_RETURN), Text("/kills"), Key(VK_RETURN), Wait(100));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

With utf-16 text.
```c++
using namespace CWKSS;

Result result = SendToWindow(UTF16(), L"Path of Exile", Key(VK_RETURN), Text(L"/kills"), Key(VK_RETURN));

if (result.IsError()) wprintf("%s\n", result.GetErrorMessageUTF16().c_str());
```

With partially utf-16 text. Only "/kills" message is sent as utf-16.
```c++
using namespace CWKSS;

Result result = SendToWindow(UTF16(), "Path of Exile", Key(VK_RETURN), Text(L"/kills"), Key(VK_RETURN), Key(VK_RETURN), Text("/atlaspassives"), Key(VK_RETURN));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

With actions sent as a table.
```c++
using namespace CWKSS;

const Action actions[] = {
    Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)
};
Result result = SendToWindow("Path of Exile", actions);

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

With actions sent directly as a table.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", { Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

With actions sent as pointer to dynamicaly allocated table, and size.
```c++
using namespace CWKSS;

Action* actions = new Action[3];
actions[0] = Key(VK_RETURN);
actions[1] = Text("/kills");
actions[2] = Key(VK_RETURN);

Result result = SendToWindow("Path of Exile", actions, 3);

delete[] actions;

if (result.IsError()) puts(result.GetErrorMessageUTF16().c_str());
```

### Example 2
Sends `/kills` command to "Path of Exile"'s game window, by using delivery mode: `ModePos()`. 
Much faster, but after each message program needs to wait some small amount of time to make sure that posted message is completed, otherwise sent messages might end being crangled.
Action `Delay(10)` makes sure that after each message posting, program waits for 10 milliseconds.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", ModePost(), Delay(10), Key(VK_RETURN), Text("/kills"), Key(VK_RETURN));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

### Example 3
Sends `/kills` command to "Path of Exile"'s game window, by using action: `Input({action, ...})`. In this mode all key and text messages are sent as if they were pressed manually.
This action needs to wait some amount of time after sending message to make sure, everything were processed. All text messages are always sent in utf-16 encoding format.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", Input(Key(VK_RETURN), Text("/kills"), Key(VK_RETURN)), Wait(100));

if (result.IsError()) puts(result.GetErrorMessage().c_str());
```


### Example 4
Sends text messages to empty notepad window, with delay after each message is posted. With usign `ModePost()`, small delay after each message is needed to give target window time to process message. Value for delay might be needed bigger or smaller.
```c++
using namespace CWKSS;

Result result = SendToWindow("Untitled - Notepad", ModePost(), Delay(10), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN));
if (result.IsError()) puts(result.GetErrorMessage().c_str());
```


### Example 5
Sends text messages to empty notepad window, in one input, with waiting for 100 milliseconds after input is sent to give target window time to process message. Value for wait might be needed bigger or smaller. 
```c++
using namespace CWKSS;

Result result = SendToWindow("*Untitled - Notepad", Input(Key(VK_RETURN), Text("Some text."), Key(VK_RETURN), Text("Some other text."), Key(VK_RETURN)), Wait(100));
if (result.IsError()) puts(result.GetErrorMessage().c_str());
```

