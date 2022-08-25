# CrossWindowKeyStrokeSender
Simple library for sending keyboard messages to window application from other window application or console application.

To be able use library, copy 'CrossWindowKeyStrokeSender.h' to project source folder.

It's designed for short messages, which have no more than 1000 character, like sending some chat command to a video game. 
If sent message seems crangled when arrived to target window, try using bigger `delay` in `SendToWindow` function.

## Build Info
Compiled on: Visual Studio 2022
Target paltform: Windows 7/8/10 (32bit and 64bit)
Language: C++11

## Usage Examples

### Example 1
Sends three chains of text messages to empty notepad window. Sended text is encoded in UTF16 and each message in chain have 5 seconds delay.
```c++
using namespace CWKSS;

Result result;

result = SendToWindow(L"Untitled - Notepad", { Text(L"Some not long text."), Key(VK_RETURN) }, EncodingMode::UTF16, 5);
if (result.IsError()) puts(result.GetErrorMessageUTF8().c_str());

result = SendToWindow(L"*Untitled - Notepad", { Text(L"Some a litle longer text."), Key(VK_RETURN) }, EncodingMode::UTF16, 5);
if (result.IsError()) puts(result.GetErrorMessageUTF8().c_str());

result = SendToWindow(L"*Untitled - Notepad", { Text(L"And another text."), Key(VK_RETURN) }, EncodingMode::UTF16, 5);
if (result.IsError()) puts(result.GetErrorMessageUTF8().c_str());
```

### Example 2
Sends `/kills` command to "Path of Exile"'s game window.
```c++
using namespace CWKSS;

Result result = SendToWindow("Path of Exile", { Key(VK_RETURN), Text("/kills"), Key(VK_RETURN) });

if (result.IsError()) puts(result.GetErrorMessageUTF8().c_str());
```
