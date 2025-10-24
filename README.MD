# inTUItive

> An inTUItive, declarative TUI framework starting from C

⚠️ **Early Stage Project**: inTUItive is under active development. APIs are not stable yet and may change without notice. Use at your own risk, but feel free to experiment and provide feedback!

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
        Text("Hello, inTUItive!", TEXT_DEFAULT),
        Button("Click me", handle_click),
        Text("Simple and clean", TEXT_DEFAULT),
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
- **Layout System**: VStack, HStack with alignment, spacing, and padding
- **Interactive Elements**: Buttons, inputs, lists, tables, scrollviews
- **Modern Styling**: Colors, bold, underline via config structs
- **Config Structs**: SwiftUI/Compose-style API (named parameters, extensible)
- **Mouse Support**: Full mouse interaction (click, scroll, hover)
- **Visual Feedback**: Spinners, toast notifications, customizable scroll bars
- **Performance**: Component diffing and reconciliation for efficient rendering

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
VStack(Text("A", TEXT_DEFAULT), Text("B", TEXT_DEFAULT), NULL)  // Vertical arrangement (NULL-terminated)
HStack(Text("A", TEXT_DEFAULT), Text("B", TEXT_DEFAULT), NULL)  // Horizontal arrangement
VStackArray(children)               // From array

// UI components
Text("Hello", TEXT_DEFAULT)         // Simple text (default styling)
Button("OK", callback)              // Clickable button

// Text with styling (config-based)
Text("Error!", (TextConfig){ .fg_color = COLOR_RED, .style = STYLE_BOLD })
Text("Warning", (TextConfig){ .fg_color = COLOR_YELLOW, .bg_color = COLOR_BLACK })

// Or use convenience macros
ErrorText("Error!")                 // Shortcut for red bold text
WarningText("Warning")              // Shortcut for yellow bold text

// Config-based components (inspired by SwiftUI)
Input((InputConfig){ .buffer = buf, .size = 256 })
List((ListConfig){ .items = items, .count = 5, .max_visible = 10, .scroll_offset = &scroll, .selected_index = &idx, .on_select = callback })
Modal((ModalConfig){ .is_open = &open, .title = "Alert", .content = Text("Hi!", TEXT_DEFAULT), .on_close = close })
ScrollView(content, &scroll, (ScrollConfig){ .max_height = 10, .show_indicators = true })
Table((TableConfig){ .headers = headers, .rows = rows, .column_count = 3, .row_count = 5, .show_borders = true })
Spinner((SpinnerConfig){ .style = SPINNER_DOTS, .speed = 80, .text = "Loading...", .progress = &progress })
Toast((ToastConfig){ .message = "Saved!", .is_visible = &visible, .position = TOAST_BOTTOM, .on_close = callback })
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
        Text(state.title, TEXT_DEFAULT),
        HStack(
            Button("-", decrement),
            Text(count_str, TEXT_DEFAULT),
            Button("+", increment),
            NULL
        ),
        NULL
    );
}
```

### Dynamic Sizing (NEW!)
```c
// Get terminal dimensions for responsive layouts
int width, height;
if (tui_get_terminal_size(&width, &height)) {
    int max_visible = height - 10;  // Reserve space for UI
    // Use max_visible for List or ScrollView
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

### ✅ Phase 3: Advanced Features (COMPLETE!)
- ✅ ScrollView with viewport clipping
- ✅ Scroll indicators (▲/▼)
- ✅ Table component with bordered/borderless modes
- ✅ List auto-scrolling with selection
- ✅ Dynamic terminal sizing API
- ✅ **Milestone**: File Manager app

### ✅ Phase 4: Modern UX (COMPLETE!)
- ✅ Full mouse support (click, scroll wheel, hover)
- ✅ Cross-platform scroll detection (macOS + Linux)
- ✅ Focus-follows-mouse
- ✅ Advanced layout system (alignment, spacing, padding)
- ✅ Component diffing/reconciliation for performance
- ✅ Customizable scroll bars
- ✅ **Milestone**: System Monitor app with real-time updates

### ✅ Phase 5: Visual Enhancements (COMPLETE!)
- ✅ Spinner/loading indicators with multiple styles
- ✅ Smooth scrolling animations
- ✅ Toast notifications
- ✅ **Milestone**: Enhanced demos with spinners and toasts

### 📋 Future Ideas
- Progress bars with smooth animations
- Custom key bindings
- Text wrapping
- Async/concurrent operations
- Tree view component
- Advanced table features (sorting, filtering, pagination)

## Examples

Check out the [`examples/`](examples/) directory (all working!):
- **hello_world**: Basic declarative UI
- **counter**: State management with buttons
- **todo_app**: Full TODO list with input and dynamic lists
- **styling_demo**: Colors and text styles showcase
- **dashboard**: Modal dialogs and styled components
- **scrollview_demo**: ScrollView with vertical scrolling
- **table_demo**: Table component with bordered/borderless modes
- **file_manager**: Interactive file browser with auto-scrolling (Phase 3 milestone!)
- **mouse_demo**: Full mouse support demonstration
- **layout_demo**: Advanced layout system with alignment and spacing
- **scrollbar_demo**: Customizable scroll bar showcase
- **spinner_demo**: Loading indicators with multiple animation styles
- **toast_demo**: Toast notifications at different positions
- **sysmon**: System monitor with real-time CPU/memory updates (Phase 4 milestone!)

## Contributing

inTUItive is just getting started! We need help with:

- **Core Components**: Implementing basic UI elements
- **Platform Support**: Testing on different terminals
- **Documentation**: Examples and tutorials
- **Testing**: Cross-platform compatibility

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

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