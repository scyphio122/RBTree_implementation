#include "redblacktree.h"
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_BUFFER_WIDTH     256
#define SCREEN_BUFFER_HEIGHT    60
#define NODE_SIZE               20

static map_node_t map_guardian = {    .object_address = NULL,
                                      .object_size = 0,
                                      .object_flags = 0,
                                      .node_color = E_BLACK,
                                      .left_child = NULL,
                                      .right_child = NULL,
                                      .parent = NULL
                                 };

static char screen_buffer[SCREEN_BUFFER_HEIGHT][SCREEN_BUFFER_WIDTH];

static map_node_t* _createNode()
{
    return (map_node_t*)malloc(sizeof(map_node_t));
}

static bool _is_strictly_lower_address(map_node_t* new_node, map_node_t* cur_node)
{
    if( new_node->object_address < cur_node->object_address &&
        (void*)((uint64_t)new_node->object_address + new_node->object_size) <= cur_node->object_address)
    {
        return true;
    }

    return false;
}

static bool _is_strictly_higher_address(map_node_t* new_node, map_node_t* cur_node)
{
    void* cur_node_end_addr = (void*)((uint64_t)cur_node->object_address + cur_node->object_size);

    if (cur_node_end_addr <= new_node->object_address &&
        cur_node_end_addr < (void*)((uint64_t)new_node->object_address + new_node->object_size))
    {
        return true;
    }

    return false;
}

static void _rotate_left(map_node_t** map_root, map_node_t* right_child)
{
    map_node_t* parent = right_child->parent;
    map_node_t* tmp = right_child->left_child;

    if (parent->parent == NULL)
    {
        *map_root = right_child;
    }
    else
    {
        /// If parent is right child of granparent
        if (parent->object_address > parent->parent->object_address)
        {
            parent->parent->right_child = right_child;
        }
        else /// If parent is left child of grandparent
        {
            parent->parent->left_child = right_child;
        }
    }

    right_child->left_child = parent;
    parent->right_child = tmp;
}

static void _rotate_right(map_node_t** map_root, map_node_t* left_child)
{
    map_node_t* parent = left_child->parent;
    map_node_t* tmp = left_child->right_child;

    if (parent->parent == NULL)
    {
        *map_root = left_child;
    }
    else
    {
        /// If parent is left child of granparent
        if (parent->object_address < parent->parent->object_address)
        {
            parent->parent->left_child = left_child;
        }
        else /// If parent is right child of grandparent
        {
            parent->parent->right_child = left_child;
        }
    }

    left_child->right_child = parent;
    parent->left_child = tmp;
}

static bool _check_case_3(map_t* map, map_node_t* node_to_check)
{
    map_node_t* root = map->root;
    map_node_t* parent = node_to_check->parent;
    //  If is root
    if (parent == NULL)
        return false;

    map_node_t* grandparent = parent->parent;
    if (grandparent == NULL)
        return false;

    if (parent == grandparent->left_child)
    {
        if (node_to_check == parent->left_child &&
                grandparent->right_child->node_color == E_BLACK &&
                parent->node_color == E_RED)
        {
            _rotate_right(&root, parent);
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (    parent->node_color == E_RED &&
                node_to_check == parent->right_child &&
                grandparent->left_child->node_color == E_BLACK)
        {
            _rotate_left(&root, parent);
        }
        else
        {
            return false;
        }
    }

    if(parent->node_color == E_BLACK)
        parent->node_color = E_RED;
    else
        parent->node_color = E_BLACK;

    if(grandparent->node_color == E_BLACK)
        grandparent->node_color = E_RED;
    else
        grandparent->node_color = E_BLACK;

    return true;
}

static bool _check_case_2(map_t* map, map_node_t* node_to_check)
{
    map_node_t* root = map->root;
    map_node_t* parent = node_to_check->parent;
    //  If is root
    if (parent == NULL)
        return false;

    map_node_t* grandparent = parent->parent;
    if (grandparent == NULL)
        return false;

    if (parent == grandparent->left_child)
    {
        if (parent->node_color == E_RED &&
                grandparent->right_child->node_color == E_BLACK &&
                node_to_check == parent->right_child)
        {
            _rotate_left(&root, node_to_check);
            /// After rotation, parent became left child of node_to_check
            _check_case_3(map, parent);

            return true;
        }
    }
    else
    {
        if (parent->node_color == E_RED &&
                grandparent->left_child->node_color == E_BLACK &&
                node_to_check == parent->left_child)
        {
            _rotate_right(&root, node_to_check);
            /// After rotation, parent became left child of node_to_check
            _check_case_3(map, parent);

            return true;
        }
    }
    return false;
}

static bool _check_case_1(map_t* map, map_node_t** node_to_check)
{
    map_node_t* root = map->root;
    map_node_t* parent = (*node_to_check)->parent;
    //  If is root
    if (parent == NULL)
        return false;

    map_node_t* grandparent = parent->parent;
    if (grandparent == NULL)
        return false;

    if (parent == grandparent->left_child)
    {
        if (parent->node_color == E_RED && grandparent->right_child->node_color == E_RED)
        {
            parent->node_color = E_BLACK;
            grandparent->right_child->node_color = E_BLACK;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (parent->node_color == E_RED && grandparent->left_child->node_color == E_RED)
        {
            parent->node_color = E_BLACK;
            grandparent->left_child->node_color = E_BLACK;
        }
        else
        {
            return false;
        }
    }

    if (grandparent != root)
    {
        grandparent->node_color = E_RED;
        return true;
    }
    else
    {
        (*node_to_check) = grandparent;
        return false;
    }

    return false;
}

static void _recover_RB_properties(map_t* map, map_node_t* current_node)
{
    map_node_t* starting_node = current_node;
    do
    {
        if (current_node->parent->node_color == E_RED)
        {
            if (_check_case_1(map, &current_node))
            {
                if(current_node != starting_node)
                {
                    starting_node = current_node;
                    continue;
                }
                else
                {
                    return;
                }
            }
            else
            if (_check_case_2(map, current_node))
            {
                return;
            }
            else
            if (_check_case_3(map, current_node))
            {
                return;
            }
            return;
        }
    }while(current_node != map->root);
}

void* map_find_lowest_available_space(map_t* map, void* start_address, uint64_t size_requested)
{
    map_node_t* tmp = map->root;

    do
    {
        if (start_address > tmp->object_address)
        {
            if (start_address < tmp->right_child->object_address)
            {
                /// If there is enough space
                if (((uint64_t)tmp->right_child->object_address - (uint64_t)start_address) >= size_requested)
                {
                    return start_address;
                }
                else /// If there is too little space, set the searched address to the next node
                {
                    /// Set the new searched address just behind the neares object
                    start_address = (void*)((uint64_t)tmp->right_child->object_address + tmp->right_child->object_size);
                    tmp = map->root;
                    continue;
                }
            }
            else
            {
                tmp = tmp->right_child;
                continue;
            }
        }
        else
        if (start_address < tmp->object_address)
        {
            /// If there is enough space
            if ((void*)((uint64_t)start_address + size_requested) < tmp->object_address)
            {
                return start_address;
            }
            else
            {
                /// Start over
                start_address = (void*)((uint64_t)tmp->object_address + tmp->object_size);
                tmp = map->root;
                continue;
            }
        }

    }while(1);

    return NULL;
}

void* mymap_mmap(map_t* map, void* vaddr, unsigned int size, unsigned int flags, void* o)
{
    bool forced_right_subtree_embedding = false;
    map_node_t* new_node = _createNode();
    ASSERT(new_node);

    new_node->parent = NULL;
    new_node->node_color = E_RED;
    new_node->object_address = vaddr;
    new_node->object_size = size;
    new_node->object_flags = flags;
    new_node->left_child = &map_guardian;
    new_node->right_child = &map_guardian;

    /// If map empty, create the first entry
    if (map->root == NULL)
    {
        map->root = new_node;
        map->root->node_color = E_BLACK;
        /// Map object in the address
//        memcpy(vaddr, o, size);
        return vaddr;
    }

    map_node_t* node_ptr = map->root;
    do
    {
        if (!forced_right_subtree_embedding)
        {
            if (new_node->object_address < node_ptr->object_address)
            {
                /// If the object address is lesser than the current object address
                if  (_is_strictly_lower_address(new_node, node_ptr))
                {
                    /// If there is no children
                    if (node_ptr->left_child == &map_guardian)
                    {
                        node_ptr->left_child = new_node;
                        new_node->parent = node_ptr;
//                        memcpy(vaddr, o, size);
                        if (new_node->parent->node_color == E_RED)
                        {
                            _recover_RB_properties(map, new_node);
                            return vaddr;
                        }

                        return vaddr;
                    }
                    else
                    {
                        node_ptr = node_ptr->left_child;
                        continue;
                    }
                }
                else
                {
                    forced_right_subtree_embedding = true;
                    node_ptr = map->root->right_child;
                    continue;
                }
            }

            if (new_node->object_address > node_ptr->object_address)
            {
                /// Check if object should be placed to the right from the current node
                if (_is_strictly_higher_address(new_node, node_ptr))
                {
                    /// If there is no childs
                    if (node_ptr->right_child == &map_guardian)
                    {
                        node_ptr->right_child = new_node;
                        new_node->parent = node_ptr;
//                        memcpy(vaddr, o, size);
                        if (new_node->parent->node_color == E_RED)
                        {
                            _recover_RB_properties(map, new_node);
                            return vaddr;
                        }

                        return vaddr;
                    }
                    else
                    {
//                        forced_right_subtree_embedding = true;
//                        node_ptr = map->root->right_child;
//                        continue;
                        node_ptr = node_ptr->right_child;
                        continue;
                    }
                }
                else
                {
                    node_ptr = node_ptr->right_child;
                    continue;
                }
            }
        }
        else
        {
            vaddr = map_find_lowest_available_space(map, vaddr, size);
            new_node->object_address = vaddr;
            node_ptr = map->root;
            continue;
        }
    }while(node_ptr != &map_guardian);

    return NULL;
}

static int dump_node(map_node_t* node, int current_depth, int current_x, int width, int height)
{
    static const int BUF_SIZE = 15;
    char pointer_buffer[BUF_SIZE];
    char size_buffer[BUF_SIZE];
    char flags_buffer[BUF_SIZE];
    char node_color[BUF_SIZE];

    memset(pointer_buffer, '\0', sizeof(pointer_buffer));
    memset(size_buffer, '\0', sizeof(size_buffer));
    memset(flags_buffer, '\0', sizeof(flags_buffer));
    memset(node_color, '\0', sizeof(node_color));

    if (node == &map_guardian)
        return current_depth;

    int left_depth = current_depth;
    int right_depth = current_depth;

    left_depth = dump_node(node->left_child, current_depth + 5, current_x - NODE_SIZE, width, height);
    right_depth = dump_node(node->right_child, current_depth + 5, current_x + NODE_SIZE, width, height);

    int chars_written = sprintf(pointer_buffer, "P:%p", node->object_address);
    memset(pointer_buffer+chars_written, ' ', sizeof(pointer_buffer) - chars_written);

    chars_written = sprintf(size_buffer, "S:%u", node->object_size);
    memset(size_buffer+chars_written, ' ', sizeof(size_buffer) - chars_written);

    chars_written = sprintf(flags_buffer, "F:%u", node->object_flags);
    memset(flags_buffer+chars_written, ' ', sizeof(flags_buffer) - chars_written);

    chars_written = sprintf(node_color, "Node color: %c", node->node_color? 'R' : 'B');
    memset(node_color+chars_written, ' ', sizeof(node_color) - chars_written);

    screen_buffer[current_depth][current_x] = '+';
    strncpy(&(screen_buffer[current_depth+1][current_x]), node_color, BUF_SIZE);
    strncpy(&(screen_buffer[current_depth+2][current_x]), pointer_buffer, BUF_SIZE);
    strncpy(&(screen_buffer[current_depth+3][current_x]), size_buffer, BUF_SIZE);
    strncpy(&(screen_buffer[current_depth+4][current_x]), flags_buffer, BUF_SIZE);

    screen_buffer[current_depth + 5][current_x] = '/';
    screen_buffer[current_depth + 5][current_x + BUF_SIZE-1] = '\\';

    screen_buffer[current_depth][SCREEN_BUFFER_WIDTH - 1] = '\0';
    screen_buffer[current_depth + 1][SCREEN_BUFFER_WIDTH - 1] = '\0';
    screen_buffer[current_depth + 2][SCREEN_BUFFER_WIDTH - 1] = '\0';
    screen_buffer[current_depth + 3][SCREEN_BUFFER_WIDTH - 1] = '\0';
    screen_buffer[current_depth + 4][SCREEN_BUFFER_WIDTH - 1] = '\0';
    screen_buffer[current_depth + 5][SCREEN_BUFFER_WIDTH - 1] = '\0';

    if (right_depth > current_depth || left_depth > current_depth)
    {
        if(right_depth > left_depth)
            return right_depth;

        return left_depth;
    }

    return current_depth;
}

void dump_tree(map_t* map)
{
    memset(screen_buffer, ' ', sizeof(screen_buffer));

    int depth = dump_node(map->root, 0, 30, SCREEN_BUFFER_WIDTH, SCREEN_BUFFER_HEIGHT);

    for(int i=5; i<depth; i += 5)
    {
        bool needs_drawing = false;
        for(int j=0; j<SCREEN_BUFFER_WIDTH; ++j)
        {

            if (screen_buffer[i][j] == '\\')
            {
                needs_drawing = true;
            }
            else
            {
                if (screen_buffer[i][j] == '/')
                {
                    needs_drawing = false;
                }
                else
                if (screen_buffer[i][j] == '+')
                {
                    needs_drawing = !needs_drawing;
                }
                else
                {
                    if (needs_drawing)
                        screen_buffer[i][j] = '-';
                }
            }
        }
    }

    for(int i=0; i< depth; ++i)
    {
        printf("%s\r", screen_buffer[i]);
    }

    printf("\n\n");
}
