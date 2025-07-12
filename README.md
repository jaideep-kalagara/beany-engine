# BeanyRenderer

**BeanyRenderer** is a small, cross-platform rendering engine built on top of [wgpu-native](https://github.com/gfx-rs/wgpu-native) and [WebGPU-Cpp](https://github.com/eliemichel/WebGPU-Cpp) — a modern C++ RAII wrapper around the WebGPU API. It uses [GLFW](https://www.glfw.org/) for window and input management.

---

## 🚀 Features

- Written in modern C++20
- Uses WebGPU via native backend (`libwgpu_native`)
- RAII-style rendering interface (thanks to `WebGPU-Cpp`)
- GLFW-based cross-platform windowing
- Hardcoded triangle shader as a starting point
- CMake-based build system
  
---

## ⚙️ How to Build (with MinGW + CMake)

```bash
# Clone recursively if using submodules (for WebGPU-Cpp)
git clone --recursive https://github.com/your-user/beany-renderer.git
cd beany-renderer

# Create build folder
mkdir build

# Configure with MinGW
cmake -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build
cmake --build .
```

If you're on **Linux** or **macOS**, just omit the generator:
```bash
cmake ..         # or use Ninja if preferred: -G Ninja
cmake --build .
```

> ✅ After build, the executable will be placed in `build/out/main(.exe)`

---

## 🎮 How to Run

Ensure you're in the `build/` directory:
```bash
./out/main      # Linux/macOS
.\out\main.exe  # Windows
```

You should see:
- A window titled “WebGPU Beany Renderer”
- A red background with a blue triangle rendered via WGSL

---

## 📦 Runtime Requirements

| Platform | Requirement                          |
|----------|---------------------------------------|
| Windows  | `wgpu_native.dll` in `build/out/`     |
| Linux    | `libwgpu_native.so` in `build/out/`   |
| macOS    | `libwgpu_native.dylib` in `build/out/`|

Make sure your `shaders/` folder is copied to `out/assets/shaders/`.

---

## 🧠 Development Notes

- Uses [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) in CMake to pull GLFW
- WebGPU-Cpp is included as headers only
---

## 📜 License

MIT — do what you want, just don't forget to give credit if you build something cool.
