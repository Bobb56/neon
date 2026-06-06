#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t* map;
  uint8_t capacity;
} boolmap;





void boolmap_init(boolmap* bm) {
  // On commence avec 64 booléens (2^3 * 8)
  bm->capacity = 3;
  bm->map = malloc(1 << bm->capacity);
  if (bm->map == NULL) {
    return;
  }

  memset(bm->map, 0, 1 << bm->capacity);
}

void boolmap_set(boolmap* bm, int index, bool value) {
  uint8_t byte_index = index / 8;
  uint8_t bit_offset = index % 8;
  uint8_t byte = bm->map[byte_index];
  uint8_t left_part = (byte << (bit_offset + 1)) >> (bit_offset + 1);
  uint8_t right_part = (byte >> (8 - bit_offset)) << (8 - bit_offset);
  bm->map[byte_index] = left_part | (value << (8 - bit_offset - 1)) | right_part;
}


bool boolmap_get(boolmap* bm, int index) {
  uint8_t byte_index = index / 8;
  uint8_t bit_offset = index % 8;
  uint8_t byte = bm->map[byte_index];
  return ((byte << bit_offset) >> bit_offset) >> (8 - bit_offset - 1);
}


int main(void) {
    boolmap bm;
    boolmap_init(&bm);
    boolmap_set(&bm, 1, 1);
    boolmap_set(&bm, 5, 1);
    boolmap_set(&bm, 13, 1);
    printf("%d %d\n", bm.map[0], bm.map[1]);
    printf("%d %d %d\n", boolmap_get(&bm, 1), boolmap_get(&bm, 5), boolmap_get(&bm, 13));
    return 0;
}
