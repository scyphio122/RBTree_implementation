#include <stdint-gcc.h>

#define SECTION_NAME_SIZE (uint8_t)16

#define ASSERT(x)       do{ }while(x == 0);

/**
  \brief This enum describes the only two types of node - red and black
  **/
typedef enum
{
    E_BLACK,
    E_RED
}e_node_color;

/**
  \brief This structure describes the single map node
  **/
typedef struct map_node_t
{
    void*           object_address; /// Pointer to the embedded in the node object
    uint32_t        object_size;    /// Object size
    uint32_t        object_flags;   /// Flags describing the object, (READ_ONLY, etc.)
    e_node_color    node_color;
    struct map_node_t*     left_child;     /// Pointer to the left child
    struct map_node_t*     right_child;    /// Pointer to the right child
    struct map_node_t*     parent;         /// Pointer to the parent
}map_node_t;

typedef struct
{
    struct map_node_t* root;
}map_t;

/**
  \brief This structure describes the section which will hold objects in its own red black map
  \
  */
typedef struct
{
    /// Pointer to the map containing objects embedded in the section
    map_t*      objects_map;
    /// Buffer for section name - debug purpose only
    char        section_name[SECTION_NAME_SIZE];
}section_t;




void* map_find_lowest_available_space(map_t* map, void* start_address, uint64_t size_requested);

void* mymap_mmap(map_t* map, void* vaddr, unsigned int size, unsigned int flags, void* o);



