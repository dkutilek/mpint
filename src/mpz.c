/*
 * mpz.c
 *
 *  Created on: Sep 27, 2011
 *      Author: drewkutilek
 */

#include "../include/gmp.h"
#include <stdlib.h>
#include <math.h>

// Initialization
void mpz_init(mpz_t x)
{
  x.value = malloc(sizeof (uintmax_t));
  x.len = 1;
}
void mpz_clear(mpz_t x)
{
  
}

// Assignment
void mpz_set(mpz_t rop, mpz_t op)
{

}
void mpz_set_ui(mpz_t rop, unsigned long int op)
{

}
void mpz_set_si(mpz_t rop, signed long int op)
{

}
void mpz_set_d(mpz_t top, double op)
{

}
int mpz_set_str(mpz_t rop, char * str, int base)
{
  int result = 0;

  return result;
}
void mpz_swap(mpz_t rop1, mpz_t rop2)
{

}

// Initialization + Assignment
void mpz_init_set(mpz_t rop, mpz_t op)
{

}
void mpz_init_set_ui(mpz_t rop, unsigned long int op)
{

}
void mpz_init_set_si(mpz_t rop, signed long int op)
{

}
void mpz_init_set_d(mpz_t rop, double op)
{

}
int mpz_init_set_str(mpz_t rop, char * str, int base)
{
  int result = 0;

  return result;
}

// Conversion
unsigned long int mpz_get_ui(mpz_t op)
{
  unsigned long int result = 0;

  return result;
}
signed long int mpz_get_si(mpz_t op)
{
  signed long int result = 0;

  return result;
}
double mpz_get_d(mpz_t op)
{
  double result = 0;

  return result;
}
char * mpz_get_string(char * str, int base, mpz_t op)
{
  mpz_t x, div, q, r;
  int i;
  size_t size;
  char * beg = str;

  // + 1 for the ternimating zero bit
  size = mpz_sizeinbase(op, base) + 1;
  // TODO check for negative number
  // allocate a char for the negative sign if necessary
  str = malloc(size);
  str += size;
  str = 0;
  str--;
  
  mpz_init_set(x, op);
  mpz_init_set_si(div, base);
  mpz_init(q);
  mpz_init(r);
  
  for (i = 0; i < size; i++)
    {
      mpz_tdiv_qr(q, r, x, div);
      *str = to_digit(r, base);
      str--;
      mpz_set(x, q);
    }

  mpz_clear(q);
  mpz_clear(r);
  mpz_clear(x);
  mpz_clear(div);

  return beg;
}

// Arithmetic;
void mpz_add(mpz_t rop, mpz_t op2)
{

}
void mpz_add_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
{

}
void mpz_sub(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_sub_ui(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_ui_sub(mpz_t rop, unsigned long int op1, mpz_t op2)
{

}
void mpz_mul(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_mul_si(mpz_t rop, mpz_t op1, signed long int op2)
{

}
void mpz_mul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
{

}
void mpz_addmul(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_addmul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
{

}
void mpz_submul(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_submul_ui(mpz_t rop, mpz_t op1, unsigned long int op2)
{

}
void mpz_neg(mpz_t rop, mpz_t op)
{

}
void mpz_abs(mpz_t rop, mpz_t op)
{

}

// Division
void mpz_cdiv_q(mpz_t q, mpz_t n, mpz_t d)
{

}
void mpz_cdiv_r(mpz_t r, mpz_t n, mpz_t d)
{

}
void mpz_cdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int mpz_cdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_cdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_cdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_cdiv_ui(mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void mpz_cdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void mpz_cdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}
void mpz_fdiv_q(mpz_t q, mpz_t n, mpz_t d)
{

}
void mpz_fdiv_r(mpz_t r, mpz_t n, mpz_t d)
{

}
void mpz_fdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int mpz_fdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_fdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_fdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_fdiv_ui(mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void mpz_fdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void mpz_fdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}
void mpz_tdiv_q(mpz_t q, mpz_t n, mpz_t d)
{

}
void mpz_tdiv_r(mpz_t r, mpz_t n, mpz_t d)
{

}
void mpz_tdiv_qr(mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int mpz_tdiv_q_ui(mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_tdiv_r_ui(mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_tdiv_qr_ui(mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int mpz_tdiv_ui(mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void mpz_tdiv_q_2exp(mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void mpz_tdiv_r_2exp(mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}

// Modulo
void mpz_mod(mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int mpz_mod_ui(mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}

// Comparison
int mpz_cmp(mpz_t op1, mpz_t op2)
{
  int result = 0;

  return result;
}
int mpz_cmp_d(mpz_t op1, double op2)
{
  int result = 0;

  return result;
}
int mpz_cmp_si(mpz_t op1, signed long int op2)
{
  int result = 0;

  return result;
}
int mpz_cmp_ui(mpz_t op1, unsigned long int op2)
{
  int result = 0;

  return result;
}
int mpz_cmpabs(mpz_t op1, mpz_t op2)
{
  int result = 0;

  return result;
}
int mpz_cmpabs_d(mpz_t op1, double op2)
{
  int result = 0;

  return result;
}
int mpz_cmpabs_ui(mpz_t op1, unsigned long int op2)
{
  int result = 0;

  return result;
}
int mpz_sgn(mpz_t op)
{
  int result = 0;

  return result;
}

// Binary
void mpz_and(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_ior(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_xor(mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void mpz_com(mpz_t rop, mpz_t op)
{

}

// I/O
size_t mpz_out_str(FILE * stream, int base, mpz_t op)
{
  size_t result = 0;

  return result;
}
size_t mpz_inp_str(mpz_t rop, FILE * stream, int base)
{
  size_t result = 0;

  return result;
}

// Misc.
int mpz_fits_ulong_p(mpz_t op)
{
  int result = 0;

  return result;
}
int mpz_fits_slong_p(mpz_t op)
{
  int result = 0;

  return result;
}
int mpz_fits_uint_p(mpz_t op)
{
  int result = 0;

  return result;
}
int mpz_fits_sint_p(mpz_t op)
{
  int result = 0;

  return result;
}
int mpz_fits_ushort_p(mpz_t op)
{
  int result = 0;

  return result;
}
int mpz_fits_sshort_p(mpz_t op)
{
  int result = 0;

  return result;
}
size_t mpz_sizeinbase(mpz_t op, int base)
{
  size_t result = 0;

  return result;
}

char to_digit(uintmax_t x)
{
  if (x < 10)
    {
      return x + '0';
    }
  else
    {
      return 0;
    }
}
