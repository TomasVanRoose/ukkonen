#include "suffix.h"

// ActivePoint
typedef struct ActivePoint {
    Node* active_node;
    int64_t active_edge;
    int32_t active_length;
} ActivePoint;

Node* initialize_leaf(int64_t start, int64_t* end) {
    Node* new = (Node*)calloc(1, sizeof(Node));
    new->start = start;
    new->end = end;
    return new;
}

int64_t edge_length(Node* node) {
    // root
    if (node->start == -1) {
        return 0;
    }
    return *(node->end) - node->start + 1;
}

/*
 * Skip/count trick
 * Returns true if a jump was necessary
 */
bool jump_active_point(ActivePoint* active_point, Node* node) {
    int64_t length = edge_length(node);

    if (active_point->active_length >= length) {
        active_point->active_length -= length;
        active_point->active_edge += length;
        active_point->active_node = node;
        return true;
    }
    return false;
}

void build_and_print_suffix_tree(const uint8_t* text, int64_t length) {

    // initialize root
    Node* root = (Node*)calloc(1, sizeof(Node));
    root->start = -1;
    root->end = NULL;

    // Active point
    ActivePoint active_point = { root, -1, 0 };
    int64_t remainder = 0;

    // last node for suffix links
    Node* last_created_node = NULL;

    int64_t end = -1;

    while (end < length - 1) {
        end += 1;
        remainder += 1;
        uint8_t character = text[end];
        last_created_node = NULL;

        while (remainder > 0) {
            if (active_point.active_length == 0) {
                // if we find character already -> rule 3 extension
                Node* next_node = active_point.active_node->children[character];
                if (next_node != NULL) {
                    active_point.active_edge = next_node->start;
                    active_point.active_length += 1;
                    break;
                }

                Node* new_node = initialize_leaf(end, &end);
                active_point.active_node->children[character] = new_node;
                // Also add a suffix link when creating a leaf, if this is not the first created node this phase
                if (last_created_node != NULL) {
                    last_created_node->suffix_link = active_point.active_node;
                }
                active_point.active_node = root;
                remainder -= 1;

            } else {
                Node* next_node = active_point.active_node->children[text[active_point.active_edge]];
                // skip/count trick: if active length >= the length of the edge we need to jump to the next
                if (jump_active_point(&active_point, next_node)) {
                    continue;
                }

                if (text[next_node->start + active_point.active_length] == character) {
                    active_point.active_length += 1;
                    break;
                }
                /* Splitting */
                
                // calculate splitting point
                int64_t* edge_ending = (int64_t*)calloc(1, sizeof(int64_t));
                *edge_ending = next_node->start + active_point.active_length - 1;
                // change end of old node
                next_node->end = edge_ending;
                // append old second half
                Node* second = initialize_leaf(*edge_ending + 1, &end);
                next_node->children[text[second->start]] = second;
                // append last node
                Node* leaf = initialize_leaf(end, &end);
                next_node->children[character] = leaf;

                // Our nex splitted node's suffix link points to root
                next_node->suffix_link = root;

                // Not the first internal node created this step -> last created node links to this
                if (last_created_node != NULL) {
                    last_created_node->suffix_link = next_node;
                }

                if (active_point.active_node == root) {
                    active_point.active_edge += 1;
                    active_point.active_length -= 1;
                } else {
                    // active point is not root so follow its suffix link
                    active_point.active_node = active_point.active_node->suffix_link;
                }

                last_created_node = next_node;
                remainder -= 1;

            } 
        }
    }
    
    label_nodes(root, 0);
    print_nodes(root, text, 0);
    // before `i` goes out of scope, free all nodes to avoid dangling pointers
    free_tree(root);
}

// before printing we need to label the nodes
int64_t label_nodes(Node* node, int64_t label) {
    node->ID = label;
    for (int32_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            label = label_nodes(node->children[i], label + 1);
        }
    }
    return label;
}

// print all nodes
void print_nodes(Node* node, const uint8_t* text, int64_t label_depth) {
    print_node(node, text, label_depth);

    int64_t label_length = edge_length(node) + label_depth;

    for (int32_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            print_nodes(node->children[i], text, label_length);
        }
    }
}

// print individual node
void print_node(Node* node, const uint8_t* text, int64_t label_depth) {
    printf("%lld @ ", node->ID);

    if (node->start < 0) {
        printf(" ");
    } else {
        printf("%lld", *(node->end) - label_depth - edge_length(node) + 1);
    }
    printf("-");
    if (node->end == NULL) {
        printf(" ");
    } else {
        printf("%lld", *(node->end));
    }

    bool is_leaf = true;
    for (uint8_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            is_leaf = false;
            break;
        }
    }

    if (!is_leaf) {
        printf(" = ");
        uint8_t i = 0;
        while (i < MAX_CHAR) {
            Node* child = node->children[i];
            if (child != NULL) {
                printf("%d:", text[child->start]);
                printf("%lld,", child->ID);
                printf("%lld-%lld ", child->start, *(child->end));
                i += 1;
                break;
            }
            i += 1;
        }
        while (i < MAX_CHAR) {
            Node* child = node->children[i];
            if (child != NULL) {
                printf("| %d:", text[child->start]);
                printf("%lld,", child->ID);
                printf("%lld-%lld ", child->start, *(child->end));
            }
            i += 1;
        }
    }
    printf("\n");
}


void free_tree(Node* node) {
    bool is_internal = false;
    for (uint8_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            is_internal = true;
            free_tree(node->children[i]);
        }
    }
    // End is not global end so we need to free
    if (is_internal) {
        free(node->end);
    }
    free(node);
}

// Debugging
void debug_print_node(const Node* node, const uint8_t* text, const int level) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
    int64_t end = *(node->end);
    printf("(%lld, %lld) ", node->start, end);
    for (int64_t i = node->start; i <= end; i++) {
        printf("%c", text[i]);
    }
    printf("\n");
    for (int32_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            debug_print_node(node->children[i], text, level + 1);
        }
    }
}

void debug_print_tree(const Node* node, const uint8_t* text) {
    printf("Root\n");
    for (int32_t i = 0; i < MAX_CHAR; i++) {
        if (node->children[i] != NULL) {
            debug_print_node(node->children[i], text, 1);
        }
    }
    printf("\n");
}
