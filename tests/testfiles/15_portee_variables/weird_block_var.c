int main() {
    int c = 5;
    {
        c = 2;
        int c = 3;
    }
    return c;
}