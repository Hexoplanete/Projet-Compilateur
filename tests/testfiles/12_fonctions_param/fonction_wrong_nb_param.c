int test(int a) {
    return 2*a;
}

int main() {
    int a,b;
    a = 5;
    b = 7;
    a = test(a, b);
    return a;
}