# Rate Limiter Implementation

A C implementation of a Rate Limiter that controls the number of requests a user can make within a specific time window. This implementation uses a hash table for O(1) lookup time and a circular queue for efficient timestamp management.

## Features
- User-specific rate limiting
- Configurable request limit (N) and time window (T)
- Efficient implementation using Hash Tables and Queues
- O(1) lookup time complexity
- Automatic cleanup of expired timestamps
- Thread-safe implementation

## Prerequisites
- C compiler (gcc recommended)
- Standard C libraries:
  - stdio.h
  - stdlib.h
  - string.h
  - time.h
  - stdbool.h
  - unistd.h

## Installation

1. Clone or download the repository:
```bash
git clone <repository-url>
cd <repository-directory>
```

2. Compile the program:
```bash
gcc rate_limiter.c -o rate_limiter
```

## Running the Program

1. Execute the compiled program:
```bash
./rate_limiter
```

The demo program will run a test scenario with:
- 2 different users
- Rate limit of 3 requests per 5 seconds
- 5 consecutive attempts for each user

## Usage in Your Code

```c
// Create a rate limiter with limit of 3 requests per 60 seconds
RateLimiter* limiter = createRateLimiter(3, 60);

// Check if a request is allowed for a specific user
bool isAllowed = isRequestAllowed(limiter, "user123");
if (isAllowed) {
    printf("Request allowed for user123\n");
    // Process the request
} else {
    printf("Rate limit exceeded for user123\n");
    // Handle rate limit exceeded
}

// Clean up when done
destroyRateLimiter(limiter);
```

## Data Structures Used

1. **Hash Table**:
   - Maps user IDs to their request queues
   - Uses separate chaining for collision resolution
   - Fixed size of 100 buckets

2. **Circular Queue**:
   - Maintains timestamps of requests in FIFO order
   - Efficient memory usage with circular implementation
   - Automatic cleanup of expired timestamps

## Configuration

You can modify these constants in `rate_limiter.c`:
- `HASH_SIZE`: Size of the hash table (default: 100)
- `MAX_USER_ID`: Maximum length of user IDs (default: 50)
- `MAX_QUEUE_SIZE`: Maximum number of timestamps per user (default: 1000)

## Performance

- Time Complexity:
  - Lookup: O(1) average case
  - Insert: O(1) average case
  - Memory: O(n) where n is the number of active users
