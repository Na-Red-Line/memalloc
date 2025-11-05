#include <assert.h>
#include <stdio.h>

#include "memalloc.c"

#define BASE_SIZE 10;

void test_basic_malloc_free() {
  int length = BASE_SIZE;
  int* ptr = (int*)malloc(sizeof(int) * length);
  assert(ptr != NULL);

  for (int i = 0; i < length; i++) ptr[i] = i * 2;

  for (int i = 0; i < length; i++) assert(ptr[i] == i * 2);

  free(ptr);
  printf("test_basic_malloc_free OK\n");
}

void test_multiple_allocations() {
  void* p1 = malloc(100);
  void* p2 = malloc(200);
  void* p3 = malloc(300);

  assert(p1 != NULL);
  assert(p2 != NULL);
  assert(p3 != NULL);
  assert(p1 != p2 && p2 != p3 && p1 != p3);

  free(p2);
  free(p1);
  free(p3);

  printf("test_multiple_allocations OK\n");
}

void test_reuse_freed_blocks() {
  void* p1 = malloc(100);
  assert(p1 != NULL);

  free(p1);

  void* p2 = malloc(100);
  assert(p2 != NULL);
  assert(p1 == p2);

  free(p2);

  printf("test_reuse_freed_blocks OK\n");
}

void test_calloc() {
  int length = BASE_SIZE;
  int* arr = (int*)calloc(length, sizeof(int));
  assert(arr != NULL);

  for (int i = 0; i < length; i++) assert(arr[i] == 0);

  free(arr);

  printf("test_calloc OK\n");
}

void test_realloc() {
  int length = BASE_SIZE;
  int* arr = (int*)malloc(sizeof(int) * (length / 2));
  assert(arr != NULL);

  for (int i = 0; i < 5; i++) arr[i] = i + 1;

  arr = (int*)realloc(arr, sizeof(int) * length);
  assert(arr != NULL);

  for (int i = 0; i < length / 2; i++) assert(arr[i] == i + 1);

  for (int i = length / 2; i < length; i++) arr[i] = i + 1;

  free(arr);

  printf("test_realloc OK\n");
}

void test_null_cases() {
  void* p1 = malloc(0);
  assert(p1 == NULL);

  free(NULL);

  void* p2 = calloc(0, 10);
  assert(p2 == NULL);

  void* p3 = realloc(NULL, 100);
  assert(p3 != NULL);
  free(p3);

  printf("test_null_cases OK\n");
}

void test_large_allocation() {
  size_t size = 1024 * 1024;
  char* large = (char*)malloc(size);
  assert(large != NULL);

  large[0] = 'A';
  large[size - 1] = 'Z';

  assert(large[0] == 'A');
  assert(large[size - 1] == 'Z');

  free(large);

  printf("test_large_allocation OK\n");
}

void test_string_operations() {
  char* str = (char*)malloc(20);
  assert(str != NULL);

  strcpy(str, "Hello, World!");
  assert(strcmp(str, "Hello, World!") == 0);

  free(str);

  printf("test_string_operations OK\n");
}

int main() {
  test_basic_malloc_free();
  test_multiple_allocations();
  test_reuse_freed_blocks();
  test_calloc();
  test_realloc();
  test_null_cases();
  test_large_allocation();
  test_string_operations();

  printf("all test OK\n");

  return 0;
}
