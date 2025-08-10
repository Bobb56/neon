#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

int double_to_str(double number) {
  uint64_t sign_mask = 0x8000000000000000;
  uint64_t exp_mask = 0x7ff0000000000000;
  uint64_t frac_mask = 0x000fffffffffffff;
  uint64_t implicit_bit = 0x008000000000000;
  int frac_length = 52;
  int exp_length = 11;
  int total_length = 64;
  
  uint64_t n = *(uint64_t*)&number;
  
  bool sign = sign_mask & n;
  uint64_t exp = (exp_mask & n) >> frac_length;
  uint64_t frac = (frac_mask & n);
  
  // on ajoute d'abord le bit implicite
  frac = (frac >> 1) | implicit_bit;
  
  printf("frac : %lu\n", frac);
  
  exp = exp - 1023 + 1; // ajout de 1 à l'exposant à cause du bit implicite qu'on a ajouté
  
  // on récupère la partie entière de notre nombre
  uint64_t ent = frac >> (frac_length - exp);
  
  // et on récupère la partie décimale
  uint64_t decimal = frac << (exp + exp_length + 1);
  
  // on enlève les zéros en trop dans la partie décimale
  while (decimal % 2 == 0) {
    decimal = decimal >> 1;
  }
  
  printf("ent: %lu, decimal: %lu\n", ent, decimal);
  
  return sign;

}


int main(void) {

  double n = 3e30;
  printf("%d\n", double_to_str(n));

  return 0;
}
