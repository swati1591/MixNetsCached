#include <stdio.h>
#include <stdlib.h>
 
// A Queue Node (Queue is implemented using Doubly Linked List)
typedef struct QNode
{
   struct QNode *prev, *next;
    int pageNumber;  // the page number stored in this QNode
} QNode;
 
// A Queue (A FIFO collection of Queue Nodes)
typedef struct Queue1
{
    unsigned count;  // Number of filled frames
    unsigned numberOfFrames; // total number of frames
    QNode *front, *rear;
} Queue1;
 
// A hash (Collection of pointers to Queue Nodes)

typedef struct Hasht
{
    int capacity; // how many pages can be there
    QNode* *array; // an array of queue nodes

} Hasht;
 
// A utility function to create a new Queue Node. The queue Node
// will store the given 'pageNumber'


QNode* newQNode( int pageNumber )
{
    // Allocate memory and assign 'pageNumber'
    QNode* temp = (QNode *)malloc( sizeof( QNode ) );
    temp->pageNumber = pageNumber;
 
    // Initialize prev and next as NULL
    temp->prev = temp->next = NULL;
 
    return temp;
}

// A utility function to create an empty Queue.
// The queue can have at most 'numberOfFrames' nodes
Queue1* createQueue( int numberOfFrames )
{
    Queue1* queue = (Queue1 *)malloc( sizeof( Queue1 ) );
 
    // The queue is empty
    queue->count = 0;
    queue->front = queue->rear = NULL;
 
    // Number of frames that can be stored in memory
    queue->numberOfFrames = numberOfFrames;
 
    return queue;
}
 
// A utility function to create an empty Hash of given capacity
Hasht* createHash( int capacity )
{
    // Allocate memory for hash
    Hasht* hash = (Hasht *) malloc( sizeof( Hasht ) );
    hash->capacity = capacity;
 
    // Create an array of pointers for refering queue nodes
    hash->array = (QNode **) malloc( hash->capacity * sizeof( QNode* ) );
 
    // Initialize all hash entries as empty
    int i;
    for( i = 0; i < hash->capacity; ++i )
        hash->array[i] = NULL;
 
    return hash;
}

// A function to check if there is slot available in memory
int AreAllFramesFull( Queue1* queue )
{
    return queue->count == queue->numberOfFrames;
}
 
// A utility function to check if queue is empty
int isQueueEmpty( Queue1* queue )
{
    return queue->rear == NULL;
}
 
// A utility function to delete a frame from queue
void deQueue( Queue1* queue )
{
    if( isQueueEmpty( queue ) )
        return;
 
    // If this is the only node in list, then change front
    if (queue->front == queue->rear)
        queue->front = NULL;
 
    // Change rear and remove the previous rear
    QNode* temp = queue->rear;
    queue->rear = queue->rear->prev;
 
    if (queue->rear)
        queue->rear->next = NULL;
 
    free( temp );
 
    // decrement the number of full frames by 1
    queue->count--;
}
 
// A function to add a page with given 'pageNumber' to both queue
// and hash
void Enqueue( Queue1* queue, Hasht* hash, int pageNumber )
{
    // If all frames are full, remove the page at the rear
    if ( AreAllFramesFull ( queue ) )
    {
        // remove page from hash
        hash->array[ queue->rear->pageNumber ] = NULL;
        deQueue( queue );
    }
 
    // Create a new node with given page number,
    // And add the new node to the front of queue
    QNode* temp = newQNode( pageNumber );
    temp->next = queue->front;
 
    // If queue is empty, change both front and rear pointers
    if ( isQueueEmpty( queue ) )
        queue->rear = queue->front = temp;
    else  // Else change the front
    {
        queue->front->prev = temp;
        queue->front = temp;
    }
 
    // Add page entry to hash also
    hash->array[ pageNumber ] = temp;
 
    // increment number of full frames
    queue->count++;
}

// This function is called when a page with given 'pageNumber' is referenced
// from cache (or memory). There are two cases:
// 1. Frame is not there in memory, we bring it in memory and add to the front
//    of queue
// 2. Frame is there in memory, we move the frame to front of queue
void ReferencePage( Queue1* queue, Hasht* hash, int pageNumber )
{
    QNode* reqPage = hash->array[ pageNumber ];
 
    // the page is not in cache, bring it
    if ( reqPage == NULL )
        Enqueue( queue, hash, pageNumber );
 
    // page is there and not at front, change pointer
    else if (reqPage != queue->front)
    {
        // Unlink rquested page from its current location
        // in queue.
        reqPage->prev->next = reqPage->next;
        if (reqPage->next)
           reqPage->next->prev = reqPage->prev;
 
        // If the requested page is rear, then change rear
        // as this node will be moved to front
        if (reqPage == queue->rear)
        {
           queue->rear = reqPage->prev;
           queue->rear->next = NULL;
        }
 
        // Put the requested page before current front
        reqPage->next = queue->front;
        reqPage->prev = NULL;
 
        // Change prev of current front
        reqPage->next->prev = reqPage;
 
        // Change front to the requested page
        queue->front = reqPage;
    }
}

