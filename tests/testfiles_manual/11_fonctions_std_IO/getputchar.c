#include <stdio.h>

int main() {
    int a;
    int b;
    char c;
    int d;
    b = 10;
    c = '9';
    d = getchar();
    a = putchar(7);
    a = a + putchar(d);
    a = a + putchar('7');
    a = a + putchar(c);
    a = a + putchar(getchar());
    a = a + putchar(getchar() + 9);
    a = a + putchar(c + 5);
    a = a + putchar(b);
    a = a + putchar(b + 7);
    return a;
}