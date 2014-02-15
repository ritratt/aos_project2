#include <stdio.h>

typedef struct Node;
typedef struct Tree;

struct Node {
	struct Node *parent;
	struct Node *left;
	struct Node *right;
	int count;
	int locksense;
	int pid;
};

struct Tree {
	Node *root;
};

void add_node(Tree *t); //This is basically post order traversal to add a node.
void decrement_count(Node *n);
void unlock_sense(Node *r);

