// Script to build the C++ code with emscripten using a docker container

const shell = require('shelljs');

// Name of the docker image to run the command in (Assumes you've built the image with `docker build -t emscripten-test .`)
const imageName = "emscripten-test";

// The command to build the image that will be run in the docker container
const emscriptenCommand = `emcc src/main.cpp -Oz --bind -fwasm-exceptions -s EXPORT_ES6=1 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE="['\\$getExceptionMessage', '\\$getCppExceptionThrownObjectFromWebAssemblyException', '\\$decrementExceptionRefcount']" -s EXPORTED_FUNCTIONS="['_main', 'getCppExceptionThrownObjectFromWebAssemblyException', 'getExceptionMessage', 'decrementExceptionRefcount', '___get_exception_message', '___cpp_exception', '___cxa_increment_exception_refcount', '___cxa_decrement_exception_refcount', '___thrown_object_from_unwind_exception']" -o ./build/main.js`;

// Run the command in the docker container
shell.exec(`docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) ${imageName} ${emscriptenCommand}`);