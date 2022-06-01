#include <emscripten.h>
#include <exception>
#include <stdexcept>

using namespace std;
class myexception : public exception
{
    virtual const char *what() const throw() { return "My exception happened"; }
} myex;

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
        throw myex;
    }
    if (x == 5)
    {
        throw "abc";
    }
}

int main()
{
    EM_ASM({
        for (let i = 1; i < 6; i++)
        {
            try
            {
                console.log("Testing?", i);
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