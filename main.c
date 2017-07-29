#include <stdio.h>
#include "redblacktree.h"
#include <strings.h>
#include <string.h>
#include <stdbool.h>

static map_t process_memory_map;

char sector[1024];

typedef struct
{
    int index;
    int size;
}obj_t;

int main()
{
    const uint8_t INSTRUCTION_BUFFER_SIZE = 64;
    char instruction[INSTRUCTION_BUFFER_SIZE];
    obj_t obj1;

    bool continue_program = true;
    do
    {
        printf("Waiting for instruction... \nAdd element - add [address] [size]\nDraw tree - dump\nExit program - exit\n%>");
        fgets(instruction, sizeof(instruction), stdin);
        char* token = strtok(instruction, " \n");
        if (token == NULL)
            continue;
        if (!strcmp(token, "add"))
        {
            token = strtok(NULL, " \n");
            sscanf(token, "%x", &(obj1.index));

            token = strtok(NULL, " \n");
            sscanf(token, "%u", &(obj1.size));

            mymap_mmap(&process_memory_map, obj1.index, obj1.size, 0, &obj1);
            printf("\nDumping tree:\n");
            dump_tree(&process_memory_map);
        }
        else
        {
            if (!strcmp(token, "dump"))
            {
                printf("Dumping tree:\n");
                dump_tree(&process_memory_map);
            }
            else
            if (!strcmp(token, "exit"))
                continue_program = false;
        }
    }while(continue_program);

    //printf("Step: %d\n", step++);
//    print_t(process_memory_map.root);
    //dump_tree(&process_memory_map);

//    void* retval = mymap_mmap(&process_memory_map, 0x400, obj2.size, 0, &obj2);
////    printf("Step: %d, inserted: %i\n", step++, retval);

//    retval = mymap_mmap(&process_memory_map, 0x600, obj3.size, 0, &obj3);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);

//    retval = mymap_mmap(&process_memory_map, 0x550, obj4.size, 0, &obj4);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);

//    retval = mymap_mmap(&process_memory_map, 0x650, obj4.size, 0, &obj4);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);

//    retval = mymap_mmap(&process_memory_map, 0x700, obj4.size, 0, &obj4);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);

//    retval = mymap_mmap(&process_memory_map, 0x575, obj4.size, 0, &obj4);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);

//    //rotacja w lewo
//    //retval = mymap_mmap(&process_memory_map, 0x585, 0x100, 0, &obj4);

//    dump_tree(&process_memory_map);

//    retval = mymap_mmap(&process_memory_map, 0x560, 0x10, 0, &obj4);
////    printf("Step: %d, inserted: %i\n", step++, retval);
////    print_t(process_memory_map.root);
//    dump_tree(&process_memory_map);

    return 0;
}
