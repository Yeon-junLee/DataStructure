#define SHOW_STEP 0
#define BALANCING 1

#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

static void _destroy(NODE* root);
static NODE* _insert(NODE* root, NODE* newPtr);
static NODE* _makeNode(char* data);
static NODE* _retrieve(NODE* root, char* key);
static void _traverse(NODE* root);
static void _infix_print(NODE* root, int level);
static int getHeight(NODE* root);
static NODE* rotateRight(NODE* root);
static NODE* rotateLeft(NODE* root);

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE* AVL_Create(void) {
	AVL_TREE* tree = (AVL_TREE*)malloc(sizeof(AVL_TREE));

	if (tree != NULL) {
		tree->root = NULL;
		tree->count = 0;
	}

	return tree;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy(AVL_TREE* pTree) {
	if (pTree != NULL)
		_destroy(pTree->root);

	free(pTree);
}
static void _destroy(NODE* root) {
	NODE* del;
	del = root;
	if (del != NULL) {
		_destroy(del->left);
		_destroy(del->right);
		free(del->data);
		free(del);
	}
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert(AVL_TREE* pTree, char* data) {
	NODE* newNode = _makeNode(data);
	if (newNode == NULL) {
		return 0;
	}

	else {
		if (pTree->root == NULL) {
			pTree->root = newNode;
		}

		else {
			pTree->root = _insert(pTree->root, newNode);
		}
		(pTree->count)++;
		return 1;
	}

	return 0;
}

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE* _insert(NODE* root, NODE* newPtr) {
	if (root != NULL) {
		if (strcmp(root->data, newPtr->data) > 0) {
			if (root->left == NULL) {
				root->left = newPtr;
				root->height = max(getHeight(root->left) + 1, getHeight(root->right) + 1);
			}

			else {
				root->left = _insert(root->left, newPtr);
			}
		}
		else {
			if (root->right == NULL) {
				root->right = newPtr;
				root->height = max(getHeight(root->left) + 1, getHeight(root->right) + 1);
			}

			else {
				root->right = _insert(root->right, newPtr);
			}
		}
	}

#if BALANCING
	if (root != NULL) {
		int lh = getHeight(root->left);
		int rh = getHeight(root->right);

		if (lh - rh > 1) {
			int sib_left_h = getHeight(root->left->left);
			int sib_right_h = getHeight(root->left->right);

			if (sib_left_h > sib_right_h) {				// LL
				root = rotateRight(root);
			}

			else {										// RL
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
		}

		else if (rh - lh > 1) {
			int sib_left_h = getHeight(root->right->left);
			int sib_right_h = getHeight(root->right->right);

			if (sib_right_h > sib_left_h) {				// RR
				root = rotateLeft(root);
			}

			else {										// LR
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}
		}
	}
#endif

	root->height = max(getHeight(root->left) + 1, getHeight(root->right) + 1);

	return root;
}

static NODE* _makeNode(char* data) {
	NODE* node = (NODE*)malloc(sizeof(NODE));

	if (node != NULL) {
		node->data = strdup(data);
		node->height = 1;
		node->left = NULL;
		node->right = NULL;
	}

	return node;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char* AVL_Retrieve(AVL_TREE* pTree, char* key) {
	NODE* found;
	found = _retrieve(pTree->root, key);

	if (found != NULL)
		return found->data;

	else
		return NULL;

}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE* _retrieve(NODE* root, char* key) {
	NODE* check = root;
	int result;
	result = strcmp(check->data, key);
	if (result > 0) {
		check = root->left;

		if (check == NULL)
			return check;

		check = _retrieve(check, key);
	}
	else if (result < 0) {
		check = root->right;

		if (check == NULL)
			return check;

		check = _retrieve(check, key);
	}
	else if (result == 0) {
		return check;
	}

	return check;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse(AVL_TREE* pTree) {
	if (pTree->root != NULL) {
		_traverse(pTree->root);
	}
}

static void _traverse(NODE* root) {
	if (root != NULL) {
		_traverse(root->left);

		printf("%s ", root->data);

		_traverse(root->right);
	}
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree(AVL_TREE* pTree) {
	if (pTree->root != NULL) {
		_infix_print(pTree->root, 0);
	}
}
/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {
	if (root != NULL) {
		_infix_print(root->right, level + 1);

		for (int i = 0; i < level; i++) {
			printf("\t");
		}
		printf("%s\n", root->data);

		_infix_print(root->left, level + 1);
	}
}

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight(NODE* root) {
	if (root == NULL) {
		return 0;
	}

	else {
		return root->height;
	}
}

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE* rotateRight(NODE* root) {
	NODE* temp;
	temp = root->left;

	root->left = temp->right;
	temp->right = root;

	root = temp;

	root->right->height = max(getHeight(root->right->left) + 1, getHeight(root->right->right) + 1);
	root->height = max(getHeight(root->left) + 1, getHeight(root->right) + 1);

	return root;
}

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE* rotateLeft(NODE* root) {
	NODE* temp;
	temp = root->right;

	root->right = temp->left;
	temp->left = root;

	root = temp;

	root->left->height = max(getHeight(root->left->left) + 1, getHeight(root->left->right) + 1);
	root->height = max(getHeight(root->left) + 1, getHeight(root->right) + 1);

	return root;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	//AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
	fprintf( stdout, "Height of tree: %d\n", tree->root->height);
	fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
		key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}

