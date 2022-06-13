#include <emscripten.h>
#include <emscripten/bind.h>
#include <exception>
#include <stdexcept>
#include <string>

using namespace std;
using namespace emscripten;

class MyException : public exception
{
public:
    virtual const char *what() const throw() { return "My exception happened"; }
    int code() const { return 42; }
};

EMSCRIPTEN_KEEPALIVE extern "C" void throw_exc(int x)
{
    if (x == 1)
    {
        throw 1000;
    }
    if (x == 2)
    {
        throw 'c';
    }
    if (x == 3)
    {
        throw runtime_error("abc");
    }
    if (x == 4)
    {
        throw MyException();
    }
    if (x == 5)
    {
        throw "abc";
    }
}

/**
 * Converts a C++ exception object pointer to a JS exception object which has the message and optional error code.
 */
val getMyException(intptr_t exceptionAddress)
{
    exception *exceptionPtr = reinterpret_cast<exception *>(exceptionAddress);

    val jsException = val::object();
    jsException.set("message", exceptionPtr->what());

    // If it's our custom exception then include the error code
    const MyException *exceptionWithErrorCode = dynamic_cast<const MyException *>(exceptionPtr);
    if (exceptionWithErrorCode)
    {
        jsException.set("code", exceptionWithErrorCode->code());
    }

    return jsException;
}

int main()
{
    EM_ASM({
        for (let i = 1; i < 6; i++)
        {
            try
            {
                _throw_exc(i);
            }
            catch (p)
            {
                // Because we are catching and handling the exception in JS, the normal
                // exception catching C++ code doesn't kick in, so we need to make sure we free
                // the exception, if necessary. By incrementing and decrementing the refcount
                // we trigger the free'ing of the exception if its refcount was zero.
#ifdef __USING_EMSCRIPTEN_EXCEPTION__
                // FIXME Currently Wasm EH and Emscripten EH increases
                // refcounts in different places. Wasm EH sets the refcount to
                // 1 when throwing, and decrease it in __cxa_end_catch.
                // Emscripten EH sets the refcount to 0 when throwing, and
                // increase it in __cxa_begin_catch, and decrease it in
                // __cxa_end_catch. Fix this inconsistency later.
                incrementExceptionRefcount(p);
#endif
                console.log("Exception message:", getExceptionMessage(p));
                decrementExceptionRefcount(p);
            }
        }
    });
}

EMSCRIPTEN_BINDINGS(Main)
{
    emscripten::function("getMyException", &getMyException);
}