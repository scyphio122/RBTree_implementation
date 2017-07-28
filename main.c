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

    int step = 1;
    mymap_mmap(&process_memory_map, 0x500, obj1.size, 0, &obj1);
    //printf("Step: %d\n", step++);
//    print_t(process_memory_map.root);
    //dump_tree(&process_memory_map);

    void* retval = mymap_mmap(&process_memory_map, 0x400, obj2.size, 0, &obj2);
//    printf("Step: %d, inserted: %i\n", step++, retval);

    retval = mymap_mmap(&process_memory_map, 0x600, obj3.size, 0, &obj3);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);

    retval = mymap_mmap(&process_memory_map, 0x550, obj4.size, 0, &obj4);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);

    retval = mymap_mmap(&process_memory_map, 0x650, obj4.size, 0, &obj4);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);

    retval = mymap_mmap(&process_memory_map, 0x700, obj4.size, 0, &obj4);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);

    retval = mymap_mmap(&process_memory_map, 0x575, obj4.size, 0, &obj4);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);

    //rotacja w lewo
    retval = mymap_mmap(&process_memory_map, 0x585, obj4.size, 0, &obj4);
//    printf("Step: %d, inserted: %i\n", step++, retval);
//    print_t(process_memory_map.root);
    dump_tree(&process_memory_map);

    return 0;
}
