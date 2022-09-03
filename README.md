# WebGPU starter

Base code for starting a WebGPU project using Dawn (C++).

# Dependencies:

- Dawn: https://dawn.googlesource.com/dawn
- GLFW: https://www.glfw.org/

## Build

Native:
```sh
mkdir -p dist/native && cd dist/native
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j 10 app
```

Web:
```sh
mkdir -p dist/web && cd dist/web
emcmake -DCMAKE_BUILD_TYPE=Release ../..
# cmake -DCMAKE_TOOLCHAIN_FILE=${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j 10 bundle
```

## Update dependencies

Prerequisites:
- git
- python
- depot_tools: https://chromium.googlesource.com/chromium/tools/depot_tools.git

```sh
# Update all submodules to the latest commit on the upstream. 
git submodule update --remote --merge

# Pull Dawn external packages.
cd lib/dawn && gclient sync
```