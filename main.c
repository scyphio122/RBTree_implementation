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


    return 0;
}
