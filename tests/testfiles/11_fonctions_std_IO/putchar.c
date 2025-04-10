#include <stdio.h>

int main() {
    int a;
    int b;
    int c;
    b = 10;
    c = '9';
    a = putchar(7);
    a = a + putchar('7');
    a = a + putchar(c);
    a = a + putchar(c + 5);
    a = a + putchar(b);
    a = a + putchar(b + 7);
    return a;
}