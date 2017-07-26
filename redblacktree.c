#include "redblacktree.h"
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

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

static void _rotate_left(map_node_t* map_root, map_node_t* right_child)
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

void* map_find_lowest_available_space(map_t* map, void* start_address, uint32_t size_requested)
{
    map_node_t* tmp = map->root;

    do
    {
        if (start_address > tmp->object_address)
        {
            if (start_address < tmp->right_child->object_address)
            {
                /// If there is enough space
                if (((uint32_t)tmp->right_child->object_address - start_address) >= size_requested)
                {
                    return start_address;
                }
                else /// If there is too little space, set the searched address to the next node
                {
                    /// Set the new searched address just behind the neares object
                    start_address = (void*)((uint32_t)tmp->right_child->object_address + tmp->right_child->object_size);
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
            if ((void*)((uint32_t)start_address + size_requested) < tmp->object_address)
            {
                return start_address;
            }
            else
            {
                /// Start over
                start_address = (void*)((uint32_t)tmp->object_address + tmp->object_size);
                tmp = map->root;
                continue;
            }
        }

    }while(1);

    return NULL;
}

void* mymap_mmap(map_t* map, void* vaddr, unsigned int size, unsigned int flags, void* o)
{
    bool needs_rotation = false;
    bool forced_right_subtree_embedding = false;
    map_node_t* new_node = _createNode();
    ASSERT(new_node);

    new_node->parent = NULL;
    new_node->node_color = E_RED;
    new_node->object_address = vaddr;
    new_node->object_size = size;
    new_node->object_flags = flags;
    new_node->left_child = NULL;
    new_node->right_child = NULL;

    /// If map empty, create the first entry
    if (map->root == NULL)
    {
        map->root = new_node;
        map->root->node_color = E_BLACK;
        /// Map object in the address
        memcpy(vaddr, o, size);
        return vaddr;
    }

    map_node_t* node_ptr = map->root;
    do
    {
        if (!forced_right_subtree_embedding)
        {
            /// If the object address is lesser than the current object address
            if (_is_strictly_lower_address(new_node, node_ptr))
            {
                /// If there is no children
                if (node_ptr->left_child == NULL)
                {
                    node_ptr->left_child = new_node;
                    new_node->parent = node_ptr;
                    memcpy(vaddr, o, size);
                    if (node_ptr->parent->node_color == E_RED)
                    {
                        new_node->node_color = E_BLACK;
                        needs_rotation = true;
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

            /// Check if object should be placed to the right from the current node
            if (_is_strictly_higher_address(new_node, node_ptr))
            {
                /// If there is no childs
                if (node_ptr->right_child == NULL)
                {
                    node_ptr->right_child = new_node;
                    new_node->parent = node_ptr;
                    memcpy(vaddr, o, size);
                    if (node_ptr->node_color == E_RED)
                    {
                        new_node->node_color = E_BLACK;
                        needs_rotation = true;
                    }

                    return vaddr;
                }
                else
                {
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
        else
        {
            vaddr = map_find_lowest_available_space(map, vaddr, size);
            new_node->object_address = vaddr;
            node_ptr = map->root;
            continue;
        }
    }while(node_ptr != NULL);

    return NULL;
}
