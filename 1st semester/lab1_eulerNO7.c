#include <stdio.h>

int PrimeCheck(int n){
    int counter = 3;
    while ((counter < n) && (counter != 1)) {
        if (n % counter == 0) {
            counter = -1;
        }
        counter += 2;
    }
    return counter;
}

int DoesTheJob(int n){
    int x = 3;
    int counter = 1;
    while (counter < n) {
        if (x == PrimeCheck(x)) {
            counter += 1;
        }
        x += 2;
    }
    return (x-2);
}

int main() {
    int n = 10001;
    int result = DoesTheJob(n);
    printf("%d \n", result);
    return 0;
}