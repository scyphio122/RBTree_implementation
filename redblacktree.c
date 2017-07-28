#include "redblacktree.h"
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static map_node_t map_guardian = {    .object_address = NULL,
                                      .object_size = 0,
                                      .object_flags = 0,
                                      .node_color = E_BLACK,
                                      .left_child = NULL,
                                      .right_child = NULL,
                                      .parent = NULL
                                 };

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

static int _print_t(map_node_t *tree, int is_left, int offset, int depth, char s[20][255])
{
    char b[20];
    int width = 15;

    if (tree == &map_guardian
            ) return 0;

    sprintf(b, "(%p,%c)",(void*)((int64_t)(tree->object_address) % 0x1000), tree->node_color? 'R' : 'B');

    int left  = _print_t(tree->left_child,  1, offset,                depth + 1, s);
    int right = _print_t(tree->right_child, 0, offset + left + width, depth + 1, s);

#ifdef COMPACT
    for (int i = 0; i < width; i++)
        s[depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[depth - 1][offset + left + width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[depth - 1][offset - width/2 + i] = '-';

        s[depth - 1][offset + left + width/2] = '.';
    }
#else
    for (int i = 0; i < width; i++)
        s[2 * depth][offset + left + i] = b[i];

    if (depth && is_left) {

        for (int i = 0; i < width + right; i++)
            s[2 * depth - 1][offset + left + width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset + left + width + right + width/2] = '+';

    } else if (depth && !is_left) {

        for (int i = 0; i < left + width; i++)
            s[2 * depth - 1][offset - width/2 + i] = '-';

        s[2 * depth - 1][offset + left + width/2] = '+';
        s[2 * depth - 1][offset - width/2 - 1] = '+';
    }
#endif

    return left + width + right;
}


void print_t(map_node_t *tree)
{
    char s[20][255];
    for (int i = 0; i < 20; i++)
        sprintf(s[i], "%80s", " ");

    _print_t(tree, 0, 0, 0, s);

    for (int i = 0; i < 20; i++)
        printf("%s\n", s[i]);
}
