#include <stdio.h>

int main() {
    int a;
    int b;
    char c;
    b = 10;
    a = putchar(7);
    a = a + putchar('7');
    a = a + putchar(b);
    a = a + putchar(b + 7);
    return a;
}