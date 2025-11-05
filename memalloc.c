#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef char ALIGN[16];

union header {
  struct {
    size_t size;
    bool is_free;
    union header* next;
  } s;
  ALIGN stub;
};

typedef union header header_t;

header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock;

header_t* get_free_block(size_t size) {
  header_t* cur = head;
  while (cur) {
    if (cur->s.is_free && cur->s.size >= size) return cur;
    cur = cur->s.next;
  }
  return NULL;
}

void* malloc(size_t size) {
  if (!size) return NULL;

  size_t total_size;
  void* block;
  header_t* header;
  pthread_mutex_lock(&global_malloc_lock);
  header = get_free_block(size);

  if (header) {
    header->s.is_free = false;
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header + 1);
  }

  total_size = sizeof(header_t) + size;
  block = sbrk(total_size);
  if (block == (void*)-1) {
    pthread_mutex_unlock(&global_malloc_lock);
    return NULL;
  }

  header = block;
  header->s.size = size;
  header->s.is_free = false;
  header->s.next = NULL;
  if (!head) head = header;
  if (tail) tail->s.next = header;
  tail = header;
  pthread_mutex_unlock(&global_malloc_lock);
  return (void*)(header + 1);
}

void free(void* block) {
  if (!block) return;

  header_t *header, *tmp;
  void* programbreak;
  pthread_mutex_lock(&global_malloc_lock);
  header = (header_t*)block - 1;

  programbreak = sbrk(0);
  if ((char*)block + header->s.size == programbreak) {
    if (head == tail) {
      head = tail = NULL;
    } else {
      tmp = head;
      while (tmp) {
        if (tmp->s.next == tail) {
          tmp->s.next = NULL;
          tail = tmp;
        }
        tmp = tmp->s.next;
      }
    }
    sbrk(0 - sizeof(header_t) - header->s.size);
    pthread_mutex_unlock(&global_malloc_lock);
    return;
  }
  header->s.is_free = true;
  pthread_mutex_unlock(&global_malloc_lock);
}

void* calloc(size_t num, size_t nsize) {
  if (!num || !nsize) return NULL;

  size_t size = num * nsize;
  if (nsize != size / num) return NULL;

  void* block = malloc(size);
  if (!block) return NULL;

  memset(block, 0, size);
  return block;
}

void* realloc(void* block, size_t size) {
  if (!block || !size) return malloc(size);

  header_t* header;
  header = (header_t*)block - 1;
  if (header->s.size >= size) return block;

  void* ret = malloc(size);
  if (ret) {
    memcpy(ret, block, header->s.size);
    free(block);
  }
  return ret;
}
