#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static void _destroy(NODE* root);
static void _insert(NODE* root, NODE* newPtr);
static NODE* _delete(NODE* root, int dltKey, int* success);
static void _traverse(NODE* root);
static void _infix_print(NODE* root, int level);
NODE* _makeNode(int data);

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* BST_Create(void) {
	TREE* head = (TREE*)malloc(sizeof(TREE));
	if (head == NULL)
		return NULL;
	
	head->root = NULL;
	return head;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy(TREE* pTree) {
	if (pTree != NULL) {
		_destroy(pTree->root);
	}

	free(pTree);
}

/* internal function (not mandatory)
*/
static void _destroy(NODE* root) {
	NODE* del;
	del = root;
	if (del != NULL) {
		_destroy(del->left);
		_destroy(del->right);
		free(del);
	}
}

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert(TREE* pTree, int data) {
	NODE* newNode = _makeNode(data);
	if (newNode == NULL) {
		return 0;
	}
	
	else {
		if (pTree->root == NULL) {
			pTree->root = newNode;
		}

		else {
			_insert(pTree->root, newNode);
		}
		return 1;
	}
	
	return 0;
}

/* internal function (not mandatory)
*/
static void _insert(NODE* root, NODE* newPtr) {
	int key = newPtr->data;
	NODE* place;				// 새로운 노드가 들어갈 자리
	NODE* place_par;			// 그 자리의 부모 노드
	place = root;
	place_par = NULL;
	if (root != NULL) {			
		while (place != NULL) {					// 새로운 노드가 들어갈 자리 찾는 과정
			if (place->data > key) {
				place_par = place;
				place = place->left;
			}
			else {
				place_par = place;
				place = place->right;
			}
		}
		if (place_par->data > key)
			place_par->left = newPtr;

		else
			place_par->right = newPtr;
	}
}

NODE* _makeNode(int data) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	if (newNode == NULL)
		return NULL;

	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;

	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete(TREE* pTree, int dltKey) {
	int success = 0;
	if (pTree->root != NULL) {
		pTree->root = _delete(pTree->root, dltKey, &success);
	}
	
	return success;
}

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
static NODE* _delete(NODE* root, int dltKey, int* success) {
	NODE* del = root;
	NODE* del_par = NULL;
	NODE* ex;
	NODE* ex_par;
	*success = 0;

	if (root == NULL) {				//Tree가 비어있는 경우
		return root;
	}

	else {
		while (del->data != dltKey && ((del->data > dltKey && del->left != NULL) || (del->data < dltKey && del->right !=NULL))) {					// 삭제할 key값 찾는 과정
			if (del->data > dltKey) {
				del_par = del;
				del = del->left;
			}
			else {
				del_par = del;
				del = del->right;
			}
		}
		if (del->data != dltKey) {						//삭제할 key값 발견 못한 경우
			return root;
		}
		else if (del->data == dltKey) {				//삭제할 key값 발견한 경우
			if (del->right != NULL) {			// 삭제할 노드의 오른쪽 자식이 있는 경우
				ex = del->right;
				ex_par = del;
				while (ex->left != NULL) {					// del 노드의 값과 바꿔줄 ex노드의 값 찾는 과정
					ex_par = ex;
					ex = ex->left;
				}
				del->data = ex->data;
				if (ex == del->right) {				// ex(값 바꿔줄 노드가 del 노드 바로 밑일 때)
					ex_par->right = ex->right;
				}
				else {
					ex_par->left = ex->right;
				}
				free(ex);
			}
			else if (del->left == NULL && del->right == NULL) {					// 삭제할 노드가 자식이 없는 경우
				if (del_par != NULL) {					// 삭제할 노드의 부모가 있는 경우
					if (del_par->data > del->data) {
						del_par->left = NULL;
						free(del);
					}
					else {
						del_par->right = NULL;
						free(del);
					}
				}
				else {									// 삭제할 노드가 root인 경우
					free(del);
					root = NULL;
				}

			}
			else {								// 삭제할 노드가 왼쪽 자식만 있는 경우
				if (del_par != NULL) {
					if (del_par->left == del) {
						del_par->left = del->left;
					}
					else {
						del_par->right = del->left;
					}
				}
				else {
					root = del->left;
				}
				free(del);
			}
		}
		*success = 1;
		return root;
	}
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve( TREE *pTree, int key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
static NODE *_retrieve( NODE *root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse(TREE* pTree) {

	if (pTree != NULL) {
		_traverse(pTree->root);
	}

}
static void _traverse(NODE* root) {						//Left-To-Right (inorder traverse)
	if (root != NULL) {
		_traverse(root->left);
		printf("%d ", root->data);
		_traverse(root->right);
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree) {

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
		printf("%d", root->data);
		printf("\n");
		_infix_print(root->left, level + 1);
	}
}

/* 
	return 1 if the tree is empty; 0 if not
*/
int BST_Empty(TREE* pTree) {
	if (pTree->root == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create a tree!\n");
		return 100;
	}

	fprintf( stdout, "How many numbers will you insert into a BST: ");

	int numbers;
	scanf( "%d", &numbers);

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 0; i < numbers; i++)
	{
		data = rand() % (numbers*3) + 1; // random number (1 ~ numbers * 3)
		
		fprintf( stdout, "%d ", data);
		
		// insert funtion call
		int ret = BST_Insert( tree, data);
		if (!ret) break;
 	}
	fprintf( stdout, "\n");
			
	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	while (1)
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		int ret = scanf( "%d", &num);
		if (ret != 1) break;

		ret = BST_Delete( tree, num);
		if (!ret)
		{
			fprintf( stdout, "%d not found\n", num);
			continue;
		}
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		if (BST_Empty( tree))
		{
			fprintf( stdout, "Empty tree!\n");
			break;
		}
	}
	
	BST_Destroy( tree);

	return 0;
}


