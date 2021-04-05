#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_CHAR 128

typedef struct Node {
    int64_t ID;

    // instead of using dictionaries
    struct Node* children[MAX_CHAR];

    struct Node* suffix_link;

    int64_t start;
    int64_t* end;

} Node;

void build_and_print_suffix_tree(const uint8_t* text, int64_t length);

Node* initialize_leaf(int64_t start, int64_t* end);
int64_t label_nodes(Node* node, int64_t label);
void print_nodes(Node* node, const uint8_t* text, int64_t label_depth);
void print_node(Node* node, const uint8_t* text, int64_t label_depth);

void free_tree(Node* node);

void debug_print_node(const Node* node, const uint8_t* text, const int level);
void debug_print_tree(const Node* node, const uint8_t* text);
