#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned long int count;
int cache_hits = 0;
int cache_misses = 0;
int memory_reads = 0;
int memory_writes = 0;
int cache_size = 0;
int block_size = 0;
int assoc;
char* cache_policy;
struct cache_line** cache;

typedef struct cache_line {
  unsigned long int tag;
  int valid;
  int time;
}cache_line;

cache_line** createCache(int num_set, int assoc) {
  cache = (cache_line**)malloc(sizeof(cache_line*) * num_set);
  for (int i = 0; i < num_set; i++) {
    cache[i] = (cache_line*)malloc(sizeof(cache_line) * assoc);
  }

  for (int i = 0; i < num_set; i++) {
    for (int j = 0; j < assoc; j++) {
      cache[i][j].valid = 0;
    }
  }
  return cache;
}

int num_sets(char* argv, int setnum, int n) {
  if (strcmp(argv, "direct") == 0) {
    assoc = 1;
    setnum = cache_size / block_size;
  }
  else if (strcmp(argv, "assoc") == 0) {
    setnum = 1;
    assoc = cache_size / block_size;
  }
  else {
    sscanf(argv, "assoc:%d", &n);
    assoc = n;
    setnum = cache_size / (block_size * n);
  }
  return setnum;
}

void read_to_cache(unsigned long int tag_index, unsigned long int setIndex, int assoc) {
  int min;

  for (int i = 0; i < assoc; i++) {
    if (cache[setIndex][i].valid == 0) {
      cache_misses++;
      memory_reads++;
      count++;
      cache[setIndex][i].valid = 1;
      cache[setIndex][i].tag = tag_index;
      cache[setIndex][i].time = count;
      return;
    }
    else {
      if (cache[setIndex][i].tag == tag_index) {
        cache_hits++;
        return;
      }
      if (i == (assoc - 1)) {
        cache_misses++;
        memory_reads++;
        min = 0;
        for (int j = 0; j < assoc; j++) {
          if (cache[setIndex][i].time <= cache[setIndex][min].time) {
            min = j;
          }
        }
        cache[setIndex][min].valid = 1;
        cache[setIndex][min].tag = tag_index;
        count++;
        cache[setIndex][min].time = count;
        return;
      }
    }
  }
  return;
}

void write_to_cache(unsigned long int tag_index, unsigned long int setIndex, int assoc) {
  int min;

  for (int i = 0; i < assoc; i++) {
    if (cache[setIndex][i].valid == 0) {
      cache_misses++;
      memory_reads++;
      memory_writes++;
      count++;
      cache[setIndex][i].valid = 1;
      cache[setIndex][i].tag = tag_index;
      cache[setIndex][i].time = count;
      return;
    }
    else {
      if (cache[setIndex][i].tag == tag_index) {
        cache_hits++;
        memory_writes++;
        return;
      }
      if (i == (assoc - 1)) {
        cache_misses++;
        memory_reads++;
        memory_writes++;
        min = 0;
        for (int j = 0; j < assoc; j++) {
          if (cache[setIndex][j].time <= cache[setIndex][min].time) {
            min = j;
          }
        }
        cache[setIndex][min].valid = 1;
        cache[setIndex][min].tag = tag_index;
        count++;
        cache[setIndex][min].time = count;
        return;
      }
    }
  }
  return;
}

int main(int argc, char** argv) {

  int setnum = 0;
  int n = 0;
  unsigned long int tagIndex;
  unsigned long int setIndex;
  unsigned long long int addr;

  if (argc != 6) {
    printf("Error. Invalid arguments\n");
    return 0;
  }

  cache_size = atoi(argv[1]);
  block_size = atoi(argv[2]);
  cache_policy = argv[3];
  assoc = 0;

  FILE* file = fopen(argv[5], "r");
  if (file == NULL) {
    printf("Error. Cannot open file\n");
    return 0;
  }

  int numOfSets = num_sets(argv[4], setnum, n);
  int b = log2(block_size);
  int s = log2(numOfSets);

  cache = createCache(numOfSets, assoc);

  char rw;
  while(fscanf(file, " %c %llx ", &rw, &addr) != EOF) {

    if (rw == '#')
      break;

    setIndex = (int)(addr >> b) & (numOfSets - 1);
    tagIndex = addr >> (b+s);

    if (rw == 'R') {
      read_to_cache(tagIndex, setIndex, assoc);
    }
    else if (rw == 'W'){
      write_to_cache(tagIndex, setIndex, assoc);
    }
  }

  printf("Memory reads: %d\n", memory_reads);
  printf("Memory writes: %d\n", memory_writes);
  printf("Cache hits: %d\n", cache_hits);
  printf("Cache misses: %d\n", cache_misses);
  printf("\n");
  fclose(file);

  return 0;
}
