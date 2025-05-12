#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>  // for sleep function

#define HASH_SIZE 100
#define MAX_USER_ID 50
#define MAX_QUEUE_SIZE 1000

// Queue structure to store timestamps
typedef struct {
    time_t* timestamps;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

// Node for hash table chaining
typedef struct Node {
    char user_id[MAX_USER_ID];
    Queue* queue;
    struct Node* next;
} Node;

// Rate Limiter structure
typedef struct {
    Node* hash_table[HASH_SIZE];
    int requests_limit;
    int time_window;
} RateLimiter;

// Queue operations
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->timestamps = (time_t*)malloc(capacity * sizeof(time_t));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
    return queue;
}

bool isQueueEmpty(Queue* queue) {
    return queue->size == 0;
}

bool isQueueFull(Queue* queue) {
    return queue->size == queue->capacity;
}

void enqueue(Queue* queue, time_t timestamp) {
    if (isQueueFull(queue)) return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->timestamps[queue->rear] = timestamp;
    queue->size++;
}

time_t dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) return 0;
    time_t timestamp = queue->timestamps[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return timestamp;
}

time_t peek(Queue* queue) {
    if (isQueueEmpty(queue)) return 0;
    return queue->timestamps[queue->front];
}

// Hash function
unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// Rate Limiter operations
RateLimiter* createRateLimiter(int requests_limit, int time_window) {
    RateLimiter* limiter = (RateLimiter*)malloc(sizeof(RateLimiter));
    limiter->requests_limit = requests_limit;
    limiter->time_window = time_window;
    
    // Initialize hash table
    for (int i = 0; i < HASH_SIZE; i++) {
        limiter->hash_table[i] = NULL;
    }
    
    return limiter;
}

Node* findOrCreateUser(RateLimiter* limiter, const char* user_id) {
    unsigned int index = hash(user_id);
    Node* current = limiter->hash_table[index];
    
    // Search for existing user
    while (current != NULL) {
        if (strcmp(current->user_id, user_id) == 0) {
            return current;
        }
        current = current->next;
    }
    
    // Create new user if not found
    Node* newNode = (Node*)malloc(sizeof(Node));
    strncpy(newNode->user_id, user_id, MAX_USER_ID - 1);
    newNode->user_id[MAX_USER_ID - 1] = '\0';
    newNode->queue = createQueue(MAX_QUEUE_SIZE);
    newNode->next = limiter->hash_table[index];
    limiter->hash_table[index] = newNode;
    
    return newNode;
}

bool isRequestAllowed(RateLimiter* limiter, const char* user_id) {
    Node* user = findOrCreateUser(limiter, user_id);
    Queue* queue = user->queue;
    time_t current_time = time(NULL);
    
    // Remove expired timestamps
    while (!isQueueEmpty(queue) && 
           (current_time - peek(queue)) >= limiter->time_window) {
        dequeue(queue);
    }
    
    // Check if user has exceeded the rate limit
    if (queue->size >= limiter->requests_limit) {
        return false;
    }
    
    // Add current timestamp
    enqueue(queue, current_time);
    return true;
}

void destroyQueue(Queue* queue) {
    free(queue->timestamps);
    free(queue);
}

void destroyRateLimiter(RateLimiter* limiter) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Node* current = limiter->hash_table[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            destroyQueue(temp->queue);
            free(temp);
        }
    }
    free(limiter);
}

// Demo function
void demoRateLimiter() {
    RateLimiter* limiter = createRateLimiter(3, 5);  // 3 requests per 5 seconds
    const char* user = "user1";
    
    printf("Testing Rate Limiter (3 requests per 5 seconds):\n");
    for (int i = 0; i < 5; i++) {  // Make 5 attempts
        bool allowed = isRequestAllowed(limiter, user);
        printf("User: %s, Request allowed: %s\n", 
               user, allowed ? "true" : "false");
        sleep(1);  // Wait for 1 second
    }
    
    destroyRateLimiter(limiter);
}

int main() {
    demoRateLimiter();
    return 0;
}