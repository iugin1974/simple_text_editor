#include <iostream>

using namespace std;

bool is_char(char c) {
  return ( c >= 'A' && c <= 'Z' ) ||
    ( c >= 'a' && c <= 'z');
}


int main() {
char str[] = "int x=0;";

int s = 0;
int e = 0;

while (str[e] != '\0') {
cout << str[e] << " " << is_char(str[e]) << endl;
e++;
}

return 0;
}
