//
// Created by agwen on 15.03.2022.
//
#include "wclibrary.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv) {
    create_block_table(3);
    count_stuff(argv, argc - 2);
    reserve_block();
    print_block(0);
}
