int test(int a, int b) {
    return a+b;
}

int main() {
    int a;
    a = 5;
    int b;
    b = 7;
    int c;
    c = 9;
    a = test(a, b, c);
    return a;
}