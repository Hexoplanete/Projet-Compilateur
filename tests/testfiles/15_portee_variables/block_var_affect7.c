int main() {
    int a;
    int b = 4;
    int c = 9;
    {
        int c = 7;
        a = b + c;
    }
    a = 2;
    {
        int c =5;
        a = a + c;
        {
            c = 1;
            int c = 2;
            a = a + c;
        }
    }
    return a;
}