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

The following piece of code is an ideal "Hello World" app in inTUItive, though it's not yet implemented:
```c
#include "inTUItive.h"

component_t* app(void) {
    return VStack(
        Text("Hello, inTUItive!"),
        Button("Click me", handle_click),
        Text("Simple and clean")
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
- **Interactive Elements**: Buttons, inputs with event handling
- **Modern Styling**: Colors, borders, padding

## Installation

```bash
git clone https://github.com/zachgenius/inTUItive.git
cd inTUItive
mkdir build && cd build
cmake ..
make
```

## Basic Usage

The following components are just examples of what will be available. They'll be changed as development progresses. Don't take it too seriously (just because they were written by LLM as a suggestion, lol).

Hope they could be inTUItive enough, and available in 0.1.0.

### Components

```c
// Layout components
VStack(children, count)    // Vertical arrangement
HStack(children, count)    // Horizontal arrangement

// UI components  
Text("Hello")              // Display text
Button("OK", callback)     // Clickable button
Input(buffer, size)        // Text input field
```

### Simple State
```c
// Global state - just a struct
typedef struct {
    char title[256];
    int counter;
} app_state_t;

app_state_t app_state = {0};

// Update state - UI updates automatically
void increment(void) {
    app_state.counter++;
}
```

### Putting It Together
```c
component_t* counter_app(void) {
    return VStack(
        Text(app_state.title),
        HStack(
            Button("-", decrement),
            Text(format("%d", app_state.counter)),
            Button("+", increment)
        )
    );
}
```

## Roadmap

### ✅ Phase 0: Foundation 
- Basic terminal control
- Simple Text component
- VStack/HStack layout
- **Milestone**: Hello World app

### 🚧 Phase 1: Interactivity 
- Button component with clicks
- Input field with text editing
- Event system and focus management
- Simple state management
- **Milestone**: TODO app

### 📋 Phase 2: Polish 
- List and Table components
- Colors and basic styling
- Modal dialogs
- **Milestone**: Dashboard app

### 🎨 Phase 3: Advanced Features 
- Modifier system for styling
- Animations and transitions
- Custom drawing canvas
- **Milestone**: Rich interface

### 🌐 Phase 4: Cross-Platform 
- WASM compilation support
- Web browser deployment
- **Milestone**: Web-compatible framework

## Examples

Check out the [`examples/`](examples/) directory:
- **Counter**: Basic state management
- **TODO List**: Input handling and lists
- **File Browser**: Navigation and selection
- **System Monitor**: Real-time data display

## Contributing

inTUItive is just getting started! We need help with:

- **Core Components**: Implementing basic UI elements
- **Platform Support**: Testing on different terminals
- **Documentation**: Examples and tutorials
- **Testing**: Cross-platform compatibility

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Current Status

**Starting Point - Foundation Phase**

Currently working on:
- [X] Create a new C project
- [X] Write README

Next up:
- [ ] Basic terminal abstraction
- [ ] Simple component system
- [ ] Text rendering
- [ ] Button component
- [ ] Event handling
- [ ] Input components


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