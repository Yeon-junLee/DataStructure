#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time

#define MAX_ELEM	20

typedef struct
{
	void **heapArr;
	int	last;
	int	capacity;
	int (*compare) (void *arg1, void *arg2);
} HEAP;

static void _reheapUp(HEAP* heap, int index);
static void _reheapDown(HEAP* heap, int index);
int compare(void* arg1, void* arg2);

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP* heapCreate(int capacity, int (*compare) (void* arg1, void* arg2)) {
	HEAP* heap = (HEAP*)malloc(sizeof(HEAP));

	if (heap != NULL) {
		heap->heapArr = malloc(sizeof(void*) * capacity);
		heap->last = -1;
		heap->capacity = capacity;
		heap->compare = compare;
	}

	return heap;
}

/* Free memory for heap
*/
void heapDestroy(HEAP* heap) {
	
	for (int i = 0; i < heap->capacity; i++) {
		free(heap->heapArr[i]);
	}

	free(heap->heapArr);

	free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert(HEAP* heap, void* dataPtr) {
	if (heap->last == heap->capacity) {
		return 0;
	}

	else {
		(heap->last)++;
		heap->heapArr[heap->last] = dataPtr;

		_reheapUp(heap, heap->last);

		return 1;
	}

	return 0;
}

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp(HEAP* heap, int index) {
	if (index % 2 == 1) {
		int par_index = (index - 1) / 2;
		if (heap->compare(heap->heapArr[index], heap->heapArr[par_index]) > 0) {
			void* temp = heap->heapArr[par_index];
			heap->heapArr[par_index] = heap->heapArr[index];
			heap->heapArr[index] = temp;
			_reheapUp(heap, par_index);
		}
	}

	else {
		int par_index = index / 2;
		if (heap->compare(heap->heapArr[index], heap->heapArr[par_index]) > 0) {
			void* temp = heap->heapArr[par_index];
			heap->heapArr[par_index] = heap->heapArr[index];
			heap->heapArr[index] = temp;
			_reheapUp(heap, par_index);
		}
	}
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete(HEAP* heap, void** dataOutPtr) {
	if (heap->heapArr[0] == NULL) {
		return 0;
	}

	else {
		*dataOutPtr = (int*)(heap->heapArr[0]);
		heap->heapArr[0] = heap->heapArr[heap->last];
		heap->heapArr[heap->last] = NULL;
		(heap->last)--;
		_reheapDown(heap, 0);

		return 1;
	}

	return 0;
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown(HEAP* heap, int index) {
	void* left_sib;
	void* right_sib;
	int result;

	if (index * 2 + 1 > heap->last)
		return;

	if (heap->heapArr[index * 2 + 1] != NULL) {				// 왼쪽 subtree가 null이 아닌 경우
		left_sib = heap->heapArr[index * 2 + 1];

		if (index * 2 + 2 > heap->last)
			return;

		if (heap->heapArr[index * 2 + 2] != NULL) {			// 오른쪽 subtree가 null이 아닌 경우
			right_sib = heap->heapArr[index * 2 + 2];

			if (heap->compare(left_sib, right_sib) > 0) {			// 왼쪽게 오른쪽거 보다 클때
				result = heap->compare(left_sib, heap->heapArr[index]);

				if (result > 0) {									// 왼쪽거가 현재 index거 보다 클때
					void* temp = heap->heapArr[index * 2 + 1];
					heap->heapArr[index * 2 + 1] = heap->heapArr[index];
					heap->heapArr[index] = temp;
					_reheapDown(heap, index * 2 + 1);
				}
			}
			else {													// 오른쪽게 왼쪽거보다 클때
				result = heap->compare(right_sib, heap->heapArr[index]);

				if (result > 0) {									// 오른쪽거가 현재 index거 보다 클때
					void* temp = heap->heapArr[index * 2 + 2];
					heap->heapArr[index * 2 + 2] = heap->heapArr[index];
					heap->heapArr[index] = temp;
					_reheapDown(heap, index * 2 + 2);
				}
			}
		}
		else {											// 오른쪽 subtree가 null인 경우


				result = heap->compare(left_sib, heap->heapArr[index]);

				if (result > 0) {
					int* temp = heap->heapArr[index * 2 + 1];
					heap->heapArr[index * 2 + 1] = heap->heapArr[index];
					heap->heapArr[index] = temp;
					_reheapDown(heap, index * 2 + 1);
				}
			
		}
	}
}

/* user-defined compare function */
int compare(void *arg1, void *arg2)
{
	int *a1 = (int *)arg1;
	int *a2 = (int *)arg2;
	
	return *a1 - *a2;
}

/* Print heap array */
void heapPrint( HEAP *heap)
{
	int i;
	int last = heap->last;
	
	for( i = 0; i <= last; i++)
	{
		int *value = (int *)(heap->heapArr[i]);
		printf("%6d", *value);
	}
	printf( "\n");
}

int main(void)
{
	HEAP *heap;
	int data;
	int *dataPtr;
	int i;
	
	heap = heapCreate(MAX_ELEM, compare);
	
	srand( time(NULL));
	
	for (i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % (MAX_ELEM * 3) + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		int *newdata = (int *)malloc( sizeof(int));
		*newdata = data;
		
		// insert function call
		heapInsert( heap, newdata);
		
		heapPrint( heap);
 	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete( heap, (void **)&dataPtr);

		printf( "Deleting %d: ", *(int *)dataPtr);

		free(dataPtr);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}

