# inTUItive

> An inTUItive, declarative TUI framework starting from C

inTUItive makes it easy to build beautiful terminal applications using modern declarative patterns. Write UI as functions, let the framework handle the rest.

## Why inTUItive?

I'm literally tired of writing boilerplate terminal apps. After using FTXUI, nCursor, Dear ImGui and etc in product, I wanted something:
**Simple**: Learn in 30 minutes, no complex concepts  
**Fast**: Built in C for maximum performance  
**Modern**: Declarative syntax inspired by Compose / SwiftUI (My main job is a mobile dev IRL)

## Why C?
C is everywhere. It's fast, portable, embeddable, and has minimal dependencies. Another reason is, I don't like Python or Rust, and, especially JS/TS, though I use Python quite often. C is a great middle ground for low-level control without the complexity of C++.

## Quick Example

Here's a real "Hello World" app in inTUItive (yes, it actually works now!):
```c
#include "intuitive.h"

component_t* app(void) {
    return VStack(
        Text("Hello, inTUItive!"),
        Button("Click me", handle_click),
        Text("Simple and clean"),
        NULL  // NULL-terminated
    );
}

int main() {
    tui_init();
    tui_set_root(app);
    tui_run();
    return 0;
}
```

That's it! No classes, no inheritance, no boilerplate.

## Core Features

- **Declarative Components**: UI as pure functions
- **Simple State**: Global state with automatic updates
- **Layout System**: VStack, HStack for easy arrangements
- **Interactive Elements**: Buttons, inputs, scrollviews with event handling
- **Modern Styling**: Colors, bold, underline - all chainable!
- **Config Structs**: SwiftUI/Compose-style API (named parameters, extensible)

## Installation

```bash
git clone https://github.com/zachgenius/inTUItive.git
cd inTUItive
mkdir build && cd build
cmake ..
make
```

## Basic Usage

All these components are working now! The API evolved to be more SwiftUI/Compose-like with config structs.

### Components

```c
// Layout components
VStack(Text("A"), Text("B"), NULL)  // Vertical arrangement (NULL-terminated)
HStack(Text("A"), Text("B"), NULL)  // Horizontal arrangement
VStackArray(children)               // From array

// UI components
Text("Hello")                       // Display text
Button("OK", callback)              // Clickable button

// Config-based components (inspired by SwiftUI)
Input((InputConfig){ .buffer = buf, .size = 256 })
List((ListConfig){ .items = items, .count = 5, .max_visible = 10 })
Modal((ModalConfig){ .is_open = &open, .title = "Alert", .content = Text("Hi!"), .on_close = close })
ScrollView(content, &scroll, (ScrollConfig){ .max_height = 10, .show_indicators = true })

// Style modifiers (chainable!)
Bold(FgColor(Text("Error!"), COLOR_RED))
```

### Simple State
```c
// Global state - just a struct
typedef struct {
    char title[256];
    int counter;
} app_state_t;

app_state_t state = {0};

// Update state - request render
void increment(void) {
    state.counter++;
    tui_request_render();  // Tell framework to re-render
}
```

### Putting It Together
```c
component_t* counter_app(void) {
    char count_str[32];
    snprintf(count_str, sizeof(count_str), "%d", state.counter);

    return VStack(
        Text(state.title),
        HStack(
            Button("-", decrement),
            Text(count_str),
            Button("+", increment),
            NULL
        ),
        NULL
    );
}
```

## Roadmap

### ✅ Phase 0: Foundation (COMPLETE!)
- ✅ Basic terminal control (POSIX termios + ANSI)
- ✅ Simple Text component
- ✅ VStack/HStack layout
- ✅ **Milestone**: Hello World app

### ✅ Phase 1: Interactivity (COMPLETE!)
- ✅ Button component with clicks
- ✅ Input field with text editing
- ✅ Event system and focus management
- ✅ Simple state management
- ✅ **Milestone**: TODO app

### ✅ Phase 2: Polish (COMPLETE!)
- ✅ List component (scrollable)
- ✅ Colors and styling (16-color ANSI, Bold, Underline)
- ✅ Modal dialogs
- ✅ Config struct API (SwiftUI/Compose style)
- ✅ **Milestone**: Dashboard app

### 🚧 Phase 3: Advanced Features (IN PROGRESS)
- ✅ ScrollView with viewport clipping
- ✅ Scroll indicators (▲/▼)
- 🚧 Table/Grid component
- 🚧 **Milestone**: File Manager app

### 📋 Phase 4: Future Ideas
- Animations and transitions
- Mouse support
- Custom key bindings
- Component diffing/reconciliation
- Performance optimizations

## Examples

Check out the [`examples/`](examples/) directory (all working!):
- **hello_world**: Basic declarative UI
- **counter**: State management with buttons
- **todo_app**: Full TODO list with input and dynamic lists
- **styling_demo**: Colors and text styles showcase
- **dashboard**: Modal dialogs and styled components
- **scrollview_demo**: ScrollView with vertical scrolling

## Contributing

inTUItive is just getting started! We need help with:

- **Core Components**: Implementing basic UI elements
- **Platform Support**: Testing on different terminals
- **Documentation**: Examples and tutorials
- **Testing**: Cross-platform compatibility

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Current Status

**Phase 3 - Advanced Features**

Recently completed:
- [X] ScrollView component with vertical scrolling
- [X] Viewport clipping for clean rendering
- [X] Scroll indicators (▲/▼)
- [X] Config struct API refactor (SwiftUI/Compose-style)
- [X] All Phase 0, 1, 2 milestones (Hello World, TODO app, Dashboard)

Currently working on:
- [ ] Table/Grid component for structured data
- [ ] File Manager example app (Phase 3 milestone)

What's already working:
- ✅ Declarative UI with VStack/HStack
- ✅ Text, Button, Input components
- ✅ List and ScrollView
- ✅ Modal dialogs
- ✅ Colors and styling (Bold, Underline, 16-color ANSI)
- ✅ Event system and focus management
- ✅ State management with re-rendering

The framework is actually usable now! Build something cool with it and let me know! 🚀


## License

This project is licensed under the **Mozilla Public License 2.0**.

**What this means:**
- ✅ Free for personal, educational, and commercial use
- ✅ You can build proprietary applications using inTUItive
- ✅ No need to open source your application code
- 📤 If you modify inTUItive's source code, you must share those changes

This keeps the framework open and improving while allowing you to build whatever you want with it. 

And yes, you can use it in closed-source commercial projects. But I recommend to buy me a coffee if you do! ☕️ Just kidding.

See [LICENSE](LICENSE) for full details.