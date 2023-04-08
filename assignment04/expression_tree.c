#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <string.h>

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE* createTree(void) {
	TREE* head = (TREE*)malloc(sizeof(TREE));
	
	if (head == NULL)
		return NULL;

	head->root = NULL;

	return head;
}

/* Deletes all data in tree and recycles memory
*/
void destroyTree(TREE* pTree);

static void _destroy(NODE* root) {
	if (root != NULL) {
		_destroy(root->left);
		_destroy(root->right);
		free(root);
	}
}

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE* _makeNode(char ch) {
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->data = ch;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree(char* expr, TREE* pTree) {
	NODE* stack[MAX_STACK_SIZE];
	int top = -1;
	int i = 0;
	while (expr + i != NULL) {
		if (expr[0] < 48 || expr[0] > 57) {
			break;
		}
		if (expr[i] >= 48 && expr[i] <= 57) {
			stack[top + 1] = _makeNode(expr[i]);
			top += 1;
			i++;
		}
		else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
			if (top < 1) {
				break;
			}

			NODE* op = _makeNode(expr[i]);
			op->right = stack[top];
			top -= 1;
			op->left = stack[top];
			stack[top] = op;
			i++;
		}
		else {
			break;
		}
	}
	if (top == -1) {
		return 0;
	}
	else if (top >= 0) {
		pTree->root = stack[top];
	}
	if (top > 0 || i < strlen(expr)) {
		for (int j = 0; j < top; j++) {
			_destroy(stack[j]);
		}
		return 0;
	}
	else {
		return 1;
	}
	return 0;
}

/* Print node in tree using inorder traversal
*/
void traverseTree(TREE* pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse(NODE* root) {
	if (root != NULL) {
		char ch = root->data;
		if (ch >= 48 && ch <= 57) {
			printf("%c", ch);
		}
		else {
			printf("(");
			_traverse(root->left);
			printf("%c", ch);
			_traverse(root->right);
			printf(")");
		}
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree(TREE* pTree);

/* internal traversal function
*/
static void _infix_print(NODE* root, int level) {
	if (root != NULL) {
		_infix_print(root->right, level + 1);
		for (int i = 0; i < level; i++) {
			printf("\t");
		}
		printf("%c", root->data);
		printf("\n");
		_infix_print(root->left, level + 1);
	}
}

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix(char* expr) {
	float stack[MAX_STACK_SIZE];
	int top = -1;
	int i = 0;
	float op1 = 0;
	float op2 = 0;
	float result;
	while (expr + i != NULL) {
		if (expr[i] >= 48 && expr[i] <= 57) {
			stack[top + 1] = (float)(expr[i] - 48);
			top += 1;
			i++;
		}
		else {
			if (expr[i] == '+') {
				op1 = stack[top];
				top -= 1;
				op2 = stack[top];
				stack[top] = op2 + op1;
				i++;
			}
			else if (expr[i] == '*') {
				op1 = stack[top];
				top -= 1;
				op2 = stack[top];
				stack[top] = op2 * op1;
				i++;
			}
			else if (expr[i] == '-') {
				op1 = stack[top];
				top -= 1;
				op2 = stack[top];
				stack[top] = op2 - op1;
				i++;
			}
			else if (expr[i] == '/') {
				op1 = stack[top];
				top -= 1;
				op2 = stack[top];
				stack[top] = op2 / op1;
				i++;
			}
			else {
				break;
			}
		}
	}
	result = stack[top];
	return result;
}

////////////////////////////////////////////////////////////////////////////////
void destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse(pTree->root);
	
	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			destroyTree( tree);
			fprintf( stdout, "\nInput an expression (postfix): ");
			continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		// destroy tree
		destroyTree( tree);
		
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
