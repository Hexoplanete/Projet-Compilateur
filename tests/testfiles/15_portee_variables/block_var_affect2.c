int main() {
    int a;
    int b = 4;
    {
        int c = 7;
        a = b + c + (a = 1);
    }
    return a;
}