#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*name;
	int		freq;
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	void		*dataPtr;	//tName		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos; // unused
	NODE	*head;
	NODE	*rear;
	int		(*compare)(const void *, const void *); // used in _search function
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

static int _insert(LIST *pList, NODE *pPre, void *dataInPtr);
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr);
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu);
int emptyList(LIST *pList);

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList(int(*compare)(const void *, const void *)) {
	LIST * list = (LIST*)malloc(sizeof(LIST));
	if (list == NULL) {
		return NULL;
	}
	else {
		list->count = 0;
		list->head = NULL;
		list->rear = NULL;
		list->compare = compare;
	}
	return list;
}

/* Deletes all data in list and recycles memory
*/
void destroyList(LIST *pList, void(*callback)(void *)) {
	NODE * pDEL;
	while (pList->head != NULL) {
		pDEL = pList->head;
		pList->head = pList->head->rlink;
		callback(pDEL->dataPtr);
		free(pDEL);
		pList->count -= 1;
	}
	if (pList->head == NULL) {
		pList->rear = NULL;
	}
	free(pList);
}

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode(LIST *pList, void *dataInPtr, void(*callback)(const void *, const void *)) {		//tName
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	tName* datain = (tName*)dataInPtr;
	int found;
	int confirm;
	found = _search(pList, &pPre, &pLoc, datain);
	if (found == 0) {                                      // search가 안 된 경우(list의 중간에서 수색을 멈추거나 마지막까지 간 경우)
		confirm = _insert(pList, pPre, datain);
		if (confirm == 0) {                                // overflow가 일어난 경우
			return 0;
		}
		else if (confirm == 1) {                           // 노드가 제대로 add된 경우
			pList->count += 1;
			return 1;
		}
	}
	else {			// 이미 있는 이름
		callback((pLoc->dataPtr), datain);
		return 2;
	}
	return 0;
}

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode(LIST *pList, void *keyPtr, void **dataOut) {		//tName
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	int found;
	int empty = emptyList(pList);
	if (empty == 1) {
		return 0;
	}
	tName* key = (tName*)keyPtr;
	found = _search(pList, &pPre, &pLoc, key);
	if (found == 0) {
		return 0;
	}
	else {
		_delete(pList, pPre, pLoc, dataOut);
		pList->count -= 1;
		return 1;
	}
	return 0;
}

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList(LIST *pList, void *pArgu, void **dataOutPtr) {		//tName
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	int found;
	found = _search(pList, &pPre, &pLoc, pArgu);
	if (found == 1) {
		*dataOutPtr = pLoc->dataPtr;
		return 1;
	}
	else {
		return 0;
	}
	return 0;
}

/* returns number of nodes in list
*/
int countList(LIST *pList) {
	return pList->count;
}

/* returns	1 empty
			0 list has data
*/
int emptyList(LIST *pList) {
	if (pList->count == 0)
		return 1;
	else
		return 0;
}

/* traverses data from list (forward)
*/
void traverseList(LIST *pList, void(*callback)(const void *)) {
	NODE* node;
	node = pList->head;
	while (node != NULL) {
		callback(node->dataPtr);
		node = node->rlink;
	}
}

/* traverses data from list (backward)
*/
void traverseListR(LIST *pList, void(*callback)(const void *)) {
	NODE* node;
	node = pList->rear;
	while (node != NULL) {
		callback(node->dataPtr);
		node = node->llink;
	}
}

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert(LIST *pList, NODE *pPre, void *dataInPtr) { //tName
	NODE* pNew = (NODE*)malloc(sizeof(NODE));
	if (pNew == NULL) {         // list가 full인 경우
		return 0;
	}
	else {
		pNew->dataPtr = dataInPtr;
		pNew->llink = pPre;
		if (pPre == NULL) {
			if (pList->head == NULL) {                // 비어있는 list에 삽입
				pNew->rlink = NULL;
				pList->head = pNew;
				pList->rear = pNew;
			}
			else {                                    // list의 맨 앞에 삽입
				pList->head->llink = pNew;
				pNew->rlink = pList->head;
				pList->head = pNew;
			}
		}
		else {                                        // list의 중간에 삽입
			pNew->rlink = pPre->rlink;
			if (pPre->rlink == NULL) {                // list의 맨 뒤에 삽입
				pList->rear = pNew;
			}
			else {
				pPre->rlink->llink = pNew;
			}
			pPre->rlink = pNew;
		}
		return 1;
	}
	return 1;
}

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr) {	//tName
	*dataOutPtr = pLoc->dataPtr;
	if (pLoc->llink == NULL) {
		pLoc->rlink->llink = NULL;
		pList->head = pLoc->rlink;
	}
	else if (pLoc->rlink == NULL) {
		pLoc->llink->rlink = NULL;
		pList->rear = pLoc->llink;
	}
	else {
		pLoc->rlink->llink = pLoc->llink;
		pLoc->llink->rlink = pLoc->rlink;
	}
	free(pLoc);
}

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {
	*pPre = NULL;
	*pLoc = pList->head;
	tName* argu = (tName*)pArgu;
	while (*pLoc != NULL && strcmp(argu->name, ((tName*)((*pLoc)->dataPtr))->name) > 0) {
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	if (*pLoc == NULL) {                // pLoc이 list의 끝까지 간 경우 또는 비어있는 리스트인 경우
		return 0;
	}
	else {
		if (strcmp(argu->name, ((tName*)((*pLoc)->dataPtr))->name) == 0)             // list에서 이름 찾은 경우
			return 1;
		else                                                                          //찾다가 list 중간에서 멈춘 경우 
			return 0;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
/* Allocates dynamic memory for a name structure, initialize fields(name, freq) and returns its address to caller
	return	name structure pointer
			NULL if overflow
*/
tName *createName(char *str, int freq) {
	tName* names = (tName*)malloc(sizeof(tName));
	names->name = strdup(str);
	names->freq = freq;
	return names;
}

/* Deletes all data in name structure and recycles memory
*/
void destroyName(void *pNode) {		//tName
	tName* dname = (tName*)pNode;
	free(dname->name);
	free(dname);
}

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two names in name structures
// for createList function
int cmpName( const void* pName1, const void* pName2)
{
	return strcmp( ((tName *)pName1)->name, ((tName *)pName2)->name);
}

// prints name and freq in name structure
// for traverseList and traverseListR functions
void print_name(const void *dataPtr)
{
	printf( "%s\t%d\n", ((tName *)dataPtr)->name, ((tName *)dataPtr)->freq);
}

// increases freq in name structure
// for addNode function
void increse_freq(const void *dataOutPtr, const void *dataInPtr)
{
	((tName *)dataOutPtr)->freq += ((tName *)dataInPtr)->freq;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char str[1024];
	int freq;
	
	tName *pName;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList( cmpName);
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	void *p;
	
	while(fscanf( fp, "%*d\t%s\t%*c\t%d", str, &freq) != EOF)
	{
		pName = createName( str, freq);
		
		ret = addNode( list, pName, increse_freq);
		
		if (ret == 2) // duplicated
		{
			destroyName( pName);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		void *p;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list, destroyName);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_name);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_name);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a string to find: ");
				fscanf( stdin, "%s", str);

				pName = createName( str, 0);

				if (searchList( list, pName, &p))
				{
					print_name( p);
				}
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				
				pName = createName( str, 0);

				if (removeNode( list, pName, &p))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ((tName *)p)->name, ((tName *)p)->freq);
					destroyName( (tName *)p);
				}
				else fprintf( stdout, "%s not found\n", str);
				
				destroyName( pName);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}
