int test(int a) {
    return 3;
}

int test(int a) {
    return 2*a;
}

int main() {
    int a;
    a = 10;
    a = test(a);
    return a;
}