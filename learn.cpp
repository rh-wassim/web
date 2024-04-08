#include <iostream>
#include <unistd.h>


int main() {

    int c = fork();
    int a = fork();
    int b = fork();

    while(1);
    return 0;
}