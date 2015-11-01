### GDICanvas : A GDI pseudo-canvas

GDICanvas is a petite library for doing basic graphics using  [WinAPI's][1] **GDI**([Graphics Device Interface][2]).
This project is basically an attempt to replicate all the functionality in [Tkinter's Canvas][3].

#### Building

Make sure you have a compiler with **C++11** support, and an **MSYS/Cygwin** installation,
then run `make all` in the parent directory.
The necessary header files and binaries will be placed in the *build*
directory if the build ends successfully.
MSYS/Cygwin is needed because of the *cp* and *rm* commands used in the *Makefile*.
Git Bash, unfortunately, doesn't play well with *CMD.exe's* internal commands like *del q*.

#### HelloWorld
The obligatory *'hello world'* looks something like this:

```C++
#include "Canvas.h"

int main() {
 GC::Canvas canv;
 canv.init();
 canv.text(50, 50, "Hello world");
 return canv.loop();
}
```

and is built like so:

    g++ -o HelloWorld.exe HelloWorld.cxx -lGDICanvas -std=c++11 -mwindows -Lbuild/lib -Ibuild/include

You should try different optimization levels, e.g `-Os` or use the `gcc -lstdc++`
combination in case the executable turns out to be huge.

_____

##### Events
Events are specified and bound the same way as in Tkinter, delimited by angle
brackets. An example:

```C++
#include <Canvas.h>

struct Handler : GC::EventHandler {
 virtual void handle(GCanvas::Mouse mouse) override {
   char text[30];
   snprintf(text, 30, "Pointer at (%i, %i)", mouse.x(), mouse.y());
   MessageBox(0, text, "Mouse", 32);
 }
};

int main() {
 GC::Canvas canv(450, 450, "Hover test");
 canv.init();
 int pie = canv.arc(10, 10, 400, 400, GS::PIE);
 canv.fillColor(pie, "Turquoise");
 canv.bind("<hover>", Handler(), "all");
 return canv.loop();
}

```

The handler will be called when the mouse is above that shape for a period of
time(as the hinted in the event string). Omitting the last parameter will cause
the handler to be called when the mouse is above the window/canvas.

#### Docs
Documentation can be generated using [doxygen](http://www.doxygen.org/index.html)
by running the command in the parent directory.
You should remove the stylesheet files(**doxygen.css**, **tabs.css**) from the
source folder if you prefer doxygen's default layout.

#### What next?
Creating a turtle graphics library based on GDICanvas once I figure out how to draw
fast enough without flickering

![Demo](http://i.imgur.com/z9dLXFz.gif)

[1]: http://msdn.microsoft.com/en-us/library/ff818516.aspx
[2]: http://msdn.microsoft.com/en-us/library/windows/desktop/dd145203.aspx
[3]: http://www.effbot.org/tkinterbook/canvas.htm
