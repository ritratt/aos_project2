#include <treebarrier.h>

void add_node(Node *root) {
	if(root->left == NULL) {
		Node *l = malloc(sizeof(Node);
		root->left = l;
		left->right = NULL;
		left->left = NULL;
		l->count = 2;
		l->locksense = 0;
		return;
	}
	else if(root->right == NULL) {
		Node *r = malloc(sizeof(Node);
		root->right = r;
		left->right = NULL;
		left->left = NULL;
		l->count = 2;
		l->locksense = 0;
		return;
	}
//	else if(root->left != NULL)
	//	add_node(
	
}

void *postorder(Node *r) {
	if(r->left != NULL)
		postorder(left);
	if(r->right != NULL)
		postorder(right):
}
	
