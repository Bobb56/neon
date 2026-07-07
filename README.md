# Neon

> A lightweight, concurrent scripting language designed for constrained systems.

Neon is a general-purpose interpreted programming language with built-in concurrency. It was originally created to bring true multitasking to platforms that do not provide an operating system scheduler, such as graphing calculators, while remaining portable to desktop operating systems.

Unlike traditional scripting languages, concurrency is a fundamental part of Neon rather than an external library.

### The best programming language for easily building games for TI-84 Plus CE/TI-83 Premium CE

![](https://i.postimg.cc/18Pq5gcz/ligne.png) ![](https://i.postimg.cc/XpzFrn16/mandelbrot.png) ![](https://i.postimg.cc/bG1YGLzR/apples.gif)

![](https://i.postimg.cc/QBcNBSXm/ligne.gif) ![](https://i.postimg.cc/Z96Y9jTx/premiers.gif)

---

## Features

- Native concurrent programming
- Lightweight user-space processes
- Promise-based synchronization
- Modules and namespaces
- Lists and structured containers
- Automatic memory management
- Reference counting
- Cycle-detecting garbage collector
- Interactive REPL
- Lightweight interpreter written in C
- Designed for embedded devices and graphing calculators
- Cross-platform (Linux, Windows, TI-eZ80...)

---

## Why Neon?

Neon was designed around a simple idea:

> **Concurrent programming should be available even on systems without an operating system.**

Instead of relying on OS threads, Neon implements its own scheduler directly inside the interpreter. Programs can launch multiple processes that execute cooperatively and communicate through promises, allowing complex applications to run even on highly constrained hardware.

The same language can also run on desktop operating systems without modification.

---

## Example

```
function fibonacci(n) do
    if (n <= 1) then
        return (n)
    end

    return (fibonacci(n - 1) + fibonacci(n - 2))
end

a = parallel fibonacci(35)
b = parallel fibonacci(36)

print(a)
print(b)
```

Both computations execute concurrently. The `parallel` keyword immediately returns a **Promise**, which automatically becomes the returned value once the computation finishes.

---

## Supported Platforms

Neon currently targets multiple platforms, including:

- Linux
- Windows
- TI-eZ80 graphing calculators (TI-83 Premium CE, TI-84 Plus CE, ...)
- Other architectures can be supported with only a small platform layer.

---

## Documentation

The complete language reference is available here:

- https://neon.raphaael.fr
- `documentation_en.pdf`

---

## Building

Clone the repository:

```bash
git clone https://github.com/Bobb56/neon.git
cd neon
make -f Makefile.<platform>
```

If you want to build Neon for `TI_EZ80`, you need to install the CE C/C++ Toolchain

---

## Language Highlights

Neon provides:

- Variables
- Lists
- Containers (structured records)
- Functions and methods
- First-class functions
- Modules
- Exceptions
- Dynamic typing
- Automatic memory management
- Built-in mathematical library
- File I/O
- Native concurrency

---

## Project Goals

Neon aims to be:

- simple to learn,
- lightweight enough for small devices,
- expressive enough for larger applications,
- portable across different architectures,
- particularly suitable for environments without native multitasking.
