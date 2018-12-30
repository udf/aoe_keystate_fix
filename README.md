# What is this?
A fix for the [scrolling bug](https://gaming.stackexchange.com/questions/20826/the-age-of-empires-ii-scrolling-bug) in Age of Empires.

It works by using a proxy dll that modifies the return values of the
[GetKeyboardState](https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getkeyboardstate),
[GetKeyState](https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getkeystate) and,
[GetAsyncKeystate](https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-getasynckeystate) functions when they are called.

All other functions are simply [forward exports](https://user-images.githubusercontent.com/13610073/50548649-e3cc8e80-0c58-11e9-914d-b1316134120d.png) to the original functions from `user32.dll`.


# Usage
Change the string `USER32.DLL` in your Age of Empires exe to `USER33.DLL`
and drop the user33.dll from the (release page)[https://github.com/udf/aoe_keystate_fix/releases] into the same folder as your game exe.

Note that you don't need a hex editor to find and change this string
(Notepad++ works, regular Notepad might mess up the encoding and corrupt the exe)

You'll know if it's loaded when you start the game and get a message box like this:  
![](https://user-images.githubusercontent.com/13610073/50548573-fbefde00-0c57-11e9-8976-26fac03a9f0b.png)


# Compilation
Use [MinGW](http://www.mingw.org/):
```
i686-w64-mingw32-gcc -s user33.c user33.def -shared -o user33.dll
```
(you might need to replace `i686-w64-mingw32-gcc` with the name/path of `gcc` on your system)


# Why?
As noted in the documentation for `GetKeyboardState`, `GetKeyState`, and `GetAsyncKeystate`: only the high bit should be checked if you want to know if a key is pressed.
Age of Empires II HD (and likely other versions) simply check if the return value is > 0.
This breaks when Windows or Wine uses the middle bits for internal stuff,
the latter happens when switching workspaces or putting the game in the scratchpad in [i3wm](https://i3wm.org/).

Credit goes to Steam user `Sulix` for discovering that the problem is with how the game handles the return value of the functions:  
![](https://user-images.githubusercontent.com/13610073/50548589-335e8a80-0c58-11e9-80ca-448b82c00be8.png)  
[thread](https://steamcommunity.com/app/221380/discussions/2/622954302095447538/#c154645539343670235)

Note that Sulix only patched the GetKeyboardState function, but the game calls all three of the functions in several places
(which would explain why the tech tree still bugs with his fix):
![](https://user-images.githubusercontent.com/13610073/50548597-4bcea500-0c58-11e9-83d0-baaae6638834.png)
![](https://user-images.githubusercontent.com/13610073/50548664-1aa2a480-0c59-11e9-8b0b-af98867fd7a8.png)

(screenshots from radare2)

Instead of patching the calls in the exe as he outlines, this project patches the function exports to make the game's incorrect logic always work.  
This means it easily works across different versions of the game and requires less effort to use and develop.


# Why do I have to modify my exe?
Because it makes the fix more portable:

Windows lets you redirect LoadLibrary calls of ["Known DLLs"](https://docs.microsoft.com/en-us/windows/desktop/Dlls/dynamic-link-library-redirection)
using a [manifest file](https://docs.microsoft.com/en-us/windows/desktop/sbscs/about-side-by-side-assemblies-).
More information can be found [here](https://www.ethicalhacker.net/columns/heffner/intercepted-windows-hacking-via-dll-redirection/).

However I couldn't get this to work on windows 7 ([it might have something to do with cache](https://stackoverflow.com/questions/14136160/dll-redirection-working-in-xp-not-in-windows-7), which I couldn't get to clear),
additionally Wine doesn't seem to follow manifest files, instead it uses [this](https://user-images.githubusercontent.com/13610073/50548625-74ef3580-0c58-11e9-9bed-87580495d020.png)
section in [winecfg](https://wiki.winehq.org/Winecfg) to override a library version.  

This works, however it means that I would have to do runtime function forwarding by
[jumping to the entry point of the original functions (under the "Background" section)](https://www.codeproject.com/Articles/16541/%2FArticles%2F16541%2FCreate-your-Proxy-DLLs-automatically)
(because the linker will try to link to our functions, since we have the same name as the the original dll),
which is slower and would require an absolute path (since loading by name would load our dll first) to the real `user32.dll` which is different on 64-bit and 32-bit systems.


# TODO
- Automated patching of the exe (though, how hard is it to change one byte in a hex editor?)
- Tests across more versions of the game
