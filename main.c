#include <stdio.h>
#include "redblacktree.h"

static map_t process_memory_map;

char sector[1024];

typedef struct
{
    int index;
    int size;
}obj_t;

int main(int argc, char *argv[])
{
    printf("Hello World!\n");

    obj_t obj1;
    obj_t obj2;
    obj_t obj3;
    obj_t obj4;

    obj1.index = 1;
    obj1.size = sizeof(obj_t);
    obj2.index = 2;
    obj2.size = sizeof(obj_t);
    obj3.index = 3;
    obj3.size = sizeof(obj_t);
    obj4.index = 4;
    obj4.size = sizeof(obj_t);

    mymap_mmap(&process_memory_map, sector + 500, obj1.size, 0, &obj1);
    mymap_mmap(&process_memory_map, sector + 400, obj2.size, 0, &obj2);
    mymap_mmap(&process_memory_map, sector + 600, obj3.size, 0, &obj3);
    mymap_mmap(&process_memory_map, sector + 550, obj4.size, 0, &obj4);
    mymap_mmap(&process_memory_map, sector + 650, obj4.size, 0, &obj4);
        return 0;
}
