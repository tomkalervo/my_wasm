#include "emscripten.h"

EMSCRIPTEN_KEEPALIVE 
long fib_recursive(int n) {
    if (n <= 0)
        return 0;
    else if (n == 1)
        return 1;
    else
      return fib_recursive(n-2)+fib_recursive(n-1);
}

EMSCRIPTEN_KEEPALIVE 
long fib_dynamic_programming(int n) {
    if(n <= 0)
        return 0;
    long f0 = 0;
    long f1 = 1;
    while (n-- > 1){
        long temp = f1 + f0;
        f0 = f1;
        f1 = temp;
    }
    return f1;
}