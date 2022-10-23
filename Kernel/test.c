#include <queue.h>
#include <stdlib.h>
#include <memoryManager.h>

int main() {
    queueADT queue = initQueue();
    freeQueue(queue);
    queue = initQueue();

    printf("queue size: %d\n", getQueueSize(queue));
    
    int numero1 = malloc(sizeof(int));
    numero1 = 1;

    enqueue(queue, numero1);

    int numero2 = malloc(sizeof(int));
    numero2 = 2;

    enqueue(numero2, numero2);

    printf("%d\n", peek(queue));

    printf("queue size: %d\n", getQueueSize(queue));
    
    printf("dequeue: %d \n",dequeue(queue));

    printf("queue size: %d\n", getQueueSize(queue));

    int numero3 = malloc(sizeof(int));
    numero3 = 3;
    enqueue(numero3, numero3); 

    int numero4 = malloc(sizeof(int));
    numero4 = 4;
    enqueue(numero4, numero4);
    
    int numero5 = malloc(sizeof(int));
    numero5 = 5;
    enqueue(numero5, numero5);

    if(find(queue, numero1) != NULL)
        printf("encontro el 1 \n");

    if(includes(queue, 3) != NULL)
        printf("hay un 3 en la cola \n");

    free(queue);

    return 0;
}