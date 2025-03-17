int test(int a) {
    if (a == 0) {
        return 42;
    }
    return test(a-1);
}

int main() {
    int a;
    a = 10;
    a = test(a);
    return a;
}