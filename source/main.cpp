#include <time.h>
#include <iostream>

#include "../headers/dispatcher.h"


int main() {
    srand(time(NULL));
    Dispatcher dis;
    dis.simulate();
}