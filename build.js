// Script to build the C++ code with emscripten using a docker container

const shell = require('shelljs');

// Name of the docker image to run the command in (Assumes you've built the image with `docker build -t emscripten-test .`)
const imageName = "emscripten-test";

// The command to build the image that will be run in the docker container
const emscriptenCommand = `emcc src/main.cpp --bind -fwasm-exceptions -s EXPORT_ES6=1 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE="['\\$getExceptionMessage', '\\$incrementExceptionRefcount', '\\$decrementExceptionRefcount']" -s EXPORTED_FUNCTIONS="['_main', 'getExceptionMessage', '___get_exception_message', '___cpp_exception', '___increment_wasm_exception_refcount', '___decrement_wasm_exception_refcount']" -o ./build/main.js`;

// Run the command in the docker container
shell.exec(`docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) ${imageName} ${emscriptenCommand}`);