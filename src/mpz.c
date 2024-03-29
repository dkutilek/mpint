/*
 * mpz.c
 *
 *  Created on: Sep 27, 2011
 *      Author: drewkutilek
 */

#include "../include/gmp.h"
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

void * _malloc (size_t size);
void * _realloc (void * ptr, size_t prev_size, size_t new_size);
void _mem_check (void * ptr);
int digit_to_int (char ch, int base);
char int_to_digit (mpz_t op, int base);
int mpz_is_neg (mpz_t x);

size_t WORD_SIZE = sizeof (uintmax_t);

// Initialization
void
mpz_init (mpz_t x)
{
  x[0].value = _malloc (WORD_SIZE);
  x[0].len = 1;
}
void
mpz_clear (mpz_t x)
{
  free (x[0].value);
  x[0].value = NULL;
  x[0].len = 0;
}

// Assignment
void
mpz_set (mpz_t rop, mpz_t op)
{
  if (rop[0].len != op[0].len)
    {
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len,
          WORD_SIZE * op[0].len);
    }
  memcpy (rop[0].value, op[0].value, WORD_SIZE * op[0].len);
  rop[0].len = op[0].len;
}
void
mpz_set_ui (mpz_t rop, unsigned long int op)
{
  uintmax_t sign_bit = ~(UINTMAX_MAX >> 1);
  if (op & sign_bit)
    {
      rop[0].value = _realloc(rop[0].value, 0, WORD_SIZE * 2);
      rop[0].value[1] = 0;
      rop[0].len = 2;
    }
  else
    {
      rop[0].value = _realloc(rop[0].value, 0, WORD_SIZE);
      rop[0].len = 1;
    }
  *rop[0].value = op;
}
void
mpz_set_si (mpz_t rop, signed long int op)
{
  uintmax_t sign_bit = ~(UINTMAX_MAX >> 1);
  rop[0].value =_realloc(rop[0].value, 0, WORD_SIZE);
  rop[0].len = 1;
  *rop[0].value = op;
  if (op < 0)
    *rop[0].value &= sign_bit;
}
void
mpz_set_d (mpz_t top, double op)
{
  // TODO
}
int
mpz_set_str (mpz_t rop, char * str, int base)
{
  int result = 0;

  return result;
}
void
mpz_swap (mpz_t rop1, mpz_t rop2)
{
  uintmax_t len;
  uintmax_t * value;

  value = rop1[0].value;
  len = rop1[0].len;
  rop1[0].value = rop2[0].value;
  rop1[0].len = rop2[0].len;
  rop2[0].value = value;
  rop2[0].len = len;
}

// Initialization + Assignment
void
mpz_init_set (mpz_t rop, mpz_t op)
{
  mpz_init (rop);
  mpz_set (rop, op);
}
void
mpz_init_set_ui (mpz_t rop, unsigned long int op)
{
  mpz_init (rop);
  mpz_set_ui (rop, op);
}
void
mpz_init_set_si (mpz_t rop, signed long int op)
{
  mpz_init (rop);
  mpz_set_si (rop, op);
}
void
mpz_init_set_d (mpz_t rop, double op)
{
  mpz_init (rop);
  mpz_set_d (rop, op);
}
int
mpz_init_set_str (mpz_t rop, char * str, int base)
{
  char ch;
  int digit, negative = 0;
  mpz_t value;
  mpz_init (value);
  mpz_set_si (rop, 0);

  /* Skip whitespace */
  while (isspace (*str))
    str++;

  /* Check for negative sign */
  if (*str == '-')
    {
      negative = 1;
      str++;
    }

  /* Read in the base from the beginning of the string if it isn't specified.
   * 0x or 0X for hex
   * 0b or 0B for binary
   * 0 for octal
   * and 10 otherwise */
  if (base == 0)
    {
      if (*str == '0')
        {
          ch = tolower (*str++);
          if (ch == 'x')
            {
              str++;
              base = 16;
            }
          else if (ch == 'b')
            {
              str++;
              base = 2;
            }
          else
            {
              base = 8;
            }
        }
      else
        {
          base = 10;
        }
    }
  else if (base < 2 || 62 < base) {
      return -1;
  }

  /* Read the rest of the string */
  while ((ch = *str++) != 0)
    {
      /* Get the digit */
      digit = digit_to_int (ch, base);
      if (digit < 0)
        return -1;
      /* Multiply by the base to shift the number over, then add the digit. */
      mpz_mul_si (value, rop, base);
      mpz_add_ui (rop, value, digit);
    }

  return 0;
}

// Conversion
unsigned long int
mpz_get_ui (mpz_t op)
{
  return op[0].value[0];
}
signed long int
mpz_get_si (mpz_t op)
{
  signed long int result = op[0].value[0];
  return mpz_is_neg (op) ? - abs (result) : abs (result);
}
double
mpz_get_d (mpz_t op)
{
  double result = 0;
  // TODO
  return result;
}
char *
mpz_get_string (char * str, int base, mpz_t op)
{
  mpz_t x, div, q, r;
  int i;
  size_t size;
  char * beg = str;

  /* Add 1 for the terminating zero bit */
  size = mpz_sizeinbase (op, base) + 1;
  /* Add 1 for the negative sign */
  if (mpz_cmp_si (op, 0) < 0)
    size++;
  str = _malloc (size);
  if (mpz_cmp_si (op, 0) < 0)
    str = "-";
  /* Move to the end of the string */
  str += size;
  str = 0;
  str--;
  
  /* Initialize the multiple precision ints */
  mpz_init_set (x, op);
  mpz_init_set_si (div, base);
  mpz_init (q);
  mpz_init (r);
  
  /* Copy all digits to the string */
  for (i = 0; i < size; i++)
    {
      mpz_tdiv_qr (q, r, x, div);
      *str = int_to_digit (r, base);
      str--;
      mpz_set (x, q);
    }

  /* Clear the space associated with the ints */
  mpz_clear (q);
  mpz_clear (r);
  mpz_clear (x);
  mpz_clear (div);

  return beg;
}

char *
mpz_get_hex (char * str, mpz_t op)
{
  uintmax_t i;
  mpz_t x;
  int result;
  char * ch;
  mpz_init (x);

  if (str == NULL)
    {
      str = _malloc ((op[0].len << 4) + 2);
    }

  ch = str;

  if (mpz_is_neg (op))
    {
      //*str = '-';
      //str++;
      mpz_set (x, op);
    }
  else
    {
      mpz_set (x, op);
    }

  for (i = x[0].len; 0 < i; i--)
    {
      result = sprintf(str, "%.16jx ", x[0].value[i-1]);
      str += 16;
    }

  mpz_clear (x);
  return ch;
}

// Arithmetic;
int
_add (uintmax_t * rop, uintmax_t * op)
{
  uintmax_t result = *rop + *op;
  /* Check for overflow and return the carry. */
  if (result < (*rop < *op ? *op : *rop))
    {
      *rop = result;
      return 1;
    }
  else
    {
      *rop = result;
      return 0;
    }
}

void
mpz_add (mpz_t rop, mpz_t op1, mpz_t op2)
{
  int op1_is_neg, op2_is_neg, rop_is_neg, carry = 0;
  uintmax_t len1, len2, i, size;

  op1_is_neg = mpz_is_neg (op1);
  op2_is_neg = mpz_is_neg (op2);

  len1 = op1[0].len;
  len2 = op2[0].len;
  size = len1 < len2 ? len2 : len1;
  rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len,
      WORD_SIZE * size);
  rop[0].len = size;
  for (i = 0; i < rop[0].len; i++)
    {
      rop[0].value[i] = carry;
      carry = 0;
      if (i < len1)
        carry += _add (rop[0].value + i, op1[0].value + i);
      if (i < len2)
        carry += _add (rop[0].value + i, op2[0].value + i);
    }
  if (carry)
    {
      rop[0].len++;
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len - 1,
          WORD_SIZE * rop[0].len);
      rop[0].value[rop[0].len-1] = carry;
    }

  rop_is_neg = mpz_is_neg (rop);
  if (rop_is_neg && !op1_is_neg && !op2_is_neg)
    {
      rop[0].len++;
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len - 1,
          WORD_SIZE * rop[0].len);
      rop[0].value[rop[0].len-1] = 0;
    }
}
void
mpz_add_ui (mpz_t rop, mpz_t op1, unsigned long int op2)
{
  mpz_t x;
  mpz_init_set_ui (x, op2);
  mpz_add (rop, op1, x);
  mpz_clear (x);
}
void
mpz_sub (mpz_t rop, mpz_t op1, mpz_t op2)
{
  int op1_is_neg, x_is_neg, carry = 0;
  uintmax_t len1, len2, value1, value2, i;
  mpz_t x;

  mpz_init (x);
  mpz_neg (x, op2);
  op1_is_neg = mpz_is_neg (op1);
  x_is_neg = mpz_is_neg (x);

  /* Subtracting a negative number */
  if (!x_is_neg)
    {
      mpz_add (rop, op1, x);
      return;
    }

  if (op1_is_neg)
    value1 = UINTMAX_MAX;
  else
    value1 = 0;

  if (x_is_neg)
    value2 = UINTMAX_MAX;
  else
    value2 = 0;

  len1 = op1[0].len;
  len2 = x[0].len;
  rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len,
      WORD_SIZE * (len1 < len2 ? len2 : len1));
  rop[0].len = len1 < len2 ? len2 : len1;

  for (i = 0; i < rop[0].len; i++)
    {
      rop[0].value[i] = carry;
      carry = 0;

      if (i < len1)
        carry += _add (rop[0].value + i, op1[0].value + i);
      else
        carry += _add (rop[0].value + i, &value1);

      if (i < len2)
        carry += _add (rop[0].value + i, x[0].value + i);
      else
        carry += _add (rop[0].value + i, &value2);
    }
}
void
mpz_sub_ui (mpz_t rop, mpz_t op1, unsigned long int op2)
{
  mpz_t x;
  mpz_init_set_ui (x, op2);
  mpz_sub (rop, op1, x);
  mpz_clear (x);
}
void
mpz_ui_sub (mpz_t rop, unsigned long int op1, mpz_t op2)
{
  mpz_t x;
  mpz_init_set_ui (x, op1);
  mpz_sub (rop, x, op2);
  mpz_clear (x);
}
uintmax_t
_mul (uintmax_t * rop, uintmax_t op1, uintmax_t op2, uintmax_t carry) {
  int shift = WORD_SIZE << 2;
  uintmax_t bot_oflow, top_oflow, top1, top2, bot1, bot2, rtop, rbot, rcarry, mask = UINTMAX_MAX >> shift;
  bot_oflow = top_oflow = 0;

  /* Split the two operands to ensure the result does not overflow.  Take the
     overflow of the bottom and add it to the result of the top.  Take the
     overflow of the top and return it to be used in the next
     multiplication. */

  top1 = op1 >> shift;
  top2 = op2 >> shift;
  bot1 = op1 & mask;
  bot2 = op2 & mask;

  /* If we have a combination of maximum representable integers, then we have
     to pass some extra carry into the return value; */
  if (top1 == mask && bot1 == mask && top2 == mask && bot2 == mask)
    top_oflow++;

  rbot = (bot1 * bot2) + (carry & mask) + (*rop & mask);
  rtop = (top1 * bot2) + (bot1 * top2) + (rbot >> shift) + (*rop >> shift)
      + (carry >> shift);
  rcarry = (top1 * top2) + (rtop >> shift) + (top_oflow << (shift + 1));

  *rop = (rtop << shift) | (rbot & mask);
  return rcarry;
}
void
mpz_mul (mpz_t rop, mpz_t op1, mpz_t op2)
{
  int i, j;
  uintmax_t len1, len2, op1_is_neg, op2_is_neg, carry;
  mpz_t x, y, swap;

  mpz_init (x);
  mpz_init (y);

  len1 = op1[0].len;
  len2 = op2[0].len;

  op1_is_neg = mpz_is_neg (op1);
  op2_is_neg = mpz_is_neg (op2);

  if (op1_is_neg)
    mpz_neg (x, op1);
  else
    mpz_set (x, op1);

  if (op2_is_neg)
    mpz_neg (y, op2);
  else
    mpz_set (y, op2);

  /* Initialize and set to 0 */
  rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len,
      WORD_SIZE * (len1 + len2));
  rop[0].len = len1 + len2;
  for (j = 0; j < rop[0].len; j++)
    rop[0].value[j] = 0;

  /* Multiply */
  for (j = 0; j < len1; j++)
    {
      for (i = carry = 0; i < len2; i++)
        carry = _mul (rop[0].value + i + j, x[0].value[i], y[0].value[j], carry);
      if (carry)
        rop[0].value[i+j] += carry;
    }

  /* Clear extra padded zeros */
  for (j = rop[0].len; 1 < j; j--)
    if (rop[0].value[j-1] != 0)
      break;
  if (j < rop[0].len)
    {
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len,
          WORD_SIZE * j);
      rop[0].len = j;
    }

  if (carry)
    {
      rop[0].len++;
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len - 1,
          WORD_SIZE * rop[0].len);
      rop[0].value[rop[0].len-1] = carry;
    }

  /* Set the sign of the result */
  if ((op1_is_neg && !op2_is_neg) || (!op1_is_neg && op2_is_neg))
    {
      mpz_init (swap);
      mpz_neg (swap, rop);
      mpz_swap (rop, swap);
      mpz_clear (swap);
    }

  mpz_clear (x);
  mpz_clear (y);
}
void
mpz_mul_si (mpz_t rop, mpz_t op1, signed long int op2)
{
  mpz_t x;
  mpz_init_set_si (x, op2);
  mpz_mul (rop, op1, x);
  mpz_clear (x);
}
void
mpz_mul_ui (mpz_t rop, mpz_t op1, unsigned long int op2)
{
  mpz_t x;
  mpz_init_set_ui (x, op2);
  mpz_mul (rop, op1, x);
  mpz_clear (x);
}
void
mpz_neg (mpz_t rop, mpz_t op)
{
  uintmax_t i, sign_bit = ~(UINTMAX_MAX >> 1);
  mpz_set (rop, op);
  for (i = 0; i < rop[0].len-1; i++)
    rop[0].value[i] = (~rop[0].value[i]) + 1;

  /* If the most significant value is the smallest representable number,
     we have to extend it to be able to represent that as a positive number. */
  if (rop[0].value[rop[0].len-1] != sign_bit)
    rop[0].value[i] = (~rop[0].value[i]) + 1;
  else
    {
      rop[0].value[rop[0].len-1] = 0;
      rop[0].len++;
      rop[0].value = _realloc (rop[0].value, WORD_SIZE * rop[0].len - 1,
          WORD_SIZE * rop[0].len);
      rop[0].value[rop[0].len-1] = 1;
    }
}
void
mpz_abs (mpz_t rop, mpz_t op)
{
  mpz_t swap;
  mpz_set (rop, op);
  if (mpz_is_neg (rop))
    {
      mpz_init (swap);
      mpz_neg (swap, rop);
      mpz_swap (rop, swap);
      mpz_clear (swap);
    }
}

// Division
void
mpz_cdiv_q (mpz_t q, mpz_t n, mpz_t d)
{

}
void
mpz_cdiv_r (mpz_t r, mpz_t n, mpz_t d)
{

}
void
mpz_cdiv_qr (mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int
mpz_cdiv_q_ui (mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_cdiv_r_ui (mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_cdiv_qr_ui (mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_cdiv_ui (mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void
mpz_cdiv_q_2exp (mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void
mpz_cdiv_r_2exp (mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}
void
mpz_fdiv_q (mpz_t q, mpz_t n, mpz_t d)
{

}
void
mpz_fdiv_r (mpz_t r, mpz_t n, mpz_t d)
{

}
void
mpz_fdiv_qr (mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int
mpz_fdiv_q_ui (mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_fdiv_r_ui (mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_fdiv_qr_ui (mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_fdiv_ui (mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void
mpz_fdiv_q_2exp (mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void
mpz_fdiv_r_2exp (mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}
void
mpz_tdiv_q (mpz_t q, mpz_t n, mpz_t d)
{

}
void
mpz_tdiv_r (mpz_t r, mpz_t n, mpz_t d)
{

}
/* Compute the b's compliment of op, and place is in rop. */
void
_b_compliment (mpz_t rop, mpz_t op, mpz_t b)
{
  uintmax_t len = op[0].len < b[0].len ? b[0].len : op[0].len;
  mpz_add (rop, op, b);
  if (rop[0].len != len)
    {
      rop[0].value = _realloc (rop[0].value, rop[0].len, len);
      rop[0].len = len;
    }
}
/* Compute op1 exp (op2), and place it in rop.  This is used to
   find b to use in calculating b's compliment. */
void
_exp (mpz_t rop, mpz_t op1, uintmax_t op2)
{
  mpz_t temp1, temp2;
  uintmax_t i;
  mpz_init_set (temp1, op1);
  mpz_init_set (temp2, op1);

  for (i = 0; i < op2; i++)
    {
      mpz_mul (rop, temp1, temp2);
      mpz_set (temp1, rop);
    }

  mpz_clear (temp1);
  mpz_clear (temp2);
}
/* Recursive function that calculates the quotient q from the division
   of n by d, returning the remainder. */
uintmax_t
_div (mpz_t q, mpz_t n, uintmax_t d)
{
  mpz_t q_temp, n_temp;
  uintmax_t len, r;

  mpz_init (q_temp);
  mpz_init (n_temp);

  q[0].value = _realloc (q[0].value, q[0].len, n[0].len);
  len = q[0].len = n[0].len;

  if (len == 1)
    {
      q[0].value[0] = n[0].value[0] / d;
      return n[0].value[0] % d;
    }
  else if (1 < len)
    {
      q[0].value[len-1] = n[0].value[len-1] / d;
      r = n[0].value[len-1] % d;

      /* Expand n_temp if there is a remainder. */
      if (r)
        {
          n_temp[0].value = _realloc (n_temp[0].value, 0, WORD_SIZE * 2);
          n_temp[0].len = 2;
          n_temp[0].value[1] = r;
          n_temp[0].value[0] = n[0].value[len-2];
        }
      else
        {
          n_temp[0].value = _realloc (n_temp[0].value, 0, WORD_SIZE);
          n_temp[0].len = 1;
          n_temp[0].value[0] = n[0].value[len-2];
        }

      /* Recurse on the rest of the number, copy the rest of q, and return. */
      r = _div (q_temp, n_temp, d);
      memcpy (q[0].value + 1, q_temp[0].value, q_temp[0].len * WORD_SIZE);
      return r;
    }
  else
    {
      // CRASH
      printf("division failed.\n");
      exit (0);
    }
}
void
mpz_tdiv_qr (mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{
  mpz_t x, y, swap, q_temp, r_temp, n_temp, temp1, temp2, compliment;
  uintmax_t z, i, j, k, l, b;
  int n_is_neg, d_is_neg, temp_is_neg;

  mpz_init (x);
  mpz_init (y);
  mpz_init (q_temp);
  mpz_init (r_temp);
  mpz_init (n_temp);
  mpz_init (temp1);
  mpz_init (temp2);
  mpz_init (compliment);

  n_is_neg = mpz_is_neg (n);
  d_is_neg = mpz_is_neg (d);

  if (n_is_neg)
    mpz_neg (x, n);
  else
    mpz_set (x, n);

  if (d_is_neg)
    mpz_neg (y, d);
  else
    mpz_set (y, d);

  /* x = (x[k+l-1] ... x[1], x[0])_b and
     y = (y[l-1] ... y[1], y[0])_b
     where each sub represents a number in it's array,
     and b is the word size. */

  l = y[0].len;
  k = x[0].len - l;
  b = sizeof (uintmax_t) * CHAR_BIT - 1;

  /* set compliment to b ^ (l-1) */
  mpz_set_ui (temp1, b);
  _exp (compliment, temp1, l+1);

  /* If if the divisor is larger than what its dividing, we can't do it. */
  if (mpz_cmp(x, y) < 0)
    {
      // CRASH
      exit (0);
    }

  /* Normalization constant */
  z = b / y[0].value[l-1];

  /* Normalize values */
  mpz_init (swap);
  mpz_mul_ui (swap, x, z);
  mpz_swap (x, swap);
  mpz_mul_ui (swap, y, z);
  mpz_swap (y, swap);

  for (j = k; 0 < j; j--)
    {
      /* set q_temp = (x[j+l]*b + x[j+l-1]) / y[l-1] */
      mpz_set_ui (n_temp, b) ;
      mpz_mul_ui (swap, n_temp, x[0].value[j+l]);
      mpz_add_ui (n_temp, swap, x[0].value[j+l-1]);
      mpz_set_ui (r_temp, _div (q_temp, n_temp, y[0].value[l-1]));

      /* set n_temp to b*r_temp + x[j+l-2] */
      mpz_mul_ui (swap, r_temp, b);
      mpz_add_ui (n_temp, swap, x[0].value[j+l-2]);
      /* set swap to q_temp*y[l-2] */
      mpz_mul_ui (swap, q_temp, y[0].value[l-2]);

      do
        {
          /* test if q_temp == b or q_temp*y[l-2] > b*r_temp + x[j+l-2] */
          if (mpz_cmp_ui (q_temp, b) == 0 || 0 < mpz_cmp (swap, n_temp))
            {
              /* decrease q_temp by 1 */
              mpz_sub_ui (temp1, q_temp, 1);
              mpz_swap (q_temp, temp1);

              /* increase r_temp by y[l-1] */
              mpz_add_ui (temp1, r_temp, y[0].value[l-1]);
              mpz_swap (r_temp, temp1);
            }
          else
            break;
        }
      while (mpz_cmp_ui (r_temp, b) < 0);

      /* (x[j+l] ... x[j])_b -= q_temp*(y[l-1] ... y[1], y[0])_b */
      temp1[0].value = _realloc (temp1[0].value, 0, l);
      /* temp1 = (x[j+l] ... x[j])_b */
      for (i = 0; i < l+1; i++)
        temp1[0].value[i] = x[j+i];
      /* swap = q_temp*(y[l-1] ... y[1], y[0])_b */
      mpz_mul (swap, q_temp, y);
      /* temp2 = temp1 - swap */
      mpz_sub (temp2, temp1, swap);

      temp_is_neg = mpz_is_neg (temp2);
      if (temp_is_neg)
        {

        }
    }
}
unsigned long int
mpz_tdiv_q_ui (mpz_t q, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_tdiv_r_ui (mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_tdiv_qr_ui (mpz_t q, mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
unsigned long int
mpz_tdiv_ui (mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}
void
mpz_tdiv_q_2exp (mpz_t q, mpz_t n, mp_bitcnt_t b)
{

}
void
mpz_tdiv_r_2exp (mpz_t r, mpz_t n, mp_bitcnt_t b)
{

}

// Modulo
void
mpz_mod (mpz_t r, mpz_t n, mpz_t d)
{

}
unsigned long int
mpz_mod_ui (mpz_t r, mpz_t n, unsigned long int d)
{
  unsigned long int result = 0;

  return result;
}

// Comparison
int
mpz_is_neg (mpz_t op)
{
  uintmax_t sign_bit = ~(UINTMAX_MAX >> 1);
  if (op[0].value[op[0].len-1] & sign_bit)
    return 1;
  else
    return 0;
}
int
mpz_cmp (mpz_t op1, mpz_t op2)
{
  uintmax_t i;
  int op1_is_neg, op2_is_neg;

  op1_is_neg = mpz_is_neg (op1);
  op2_is_neg = mpz_is_neg (op2);

  /* If only one is negative */
  if (!op1_is_neg && op2_is_neg)
    return -1;
  else if (op1_is_neg && !op2_is_neg)
    return 1;
  /* Both positive or negative */
  else
    {
      /* Both negative, check the length for a shortcut */
      if (op1_is_neg && op1[0].len != op2[0].len)
        return op1[0].len < op2[0].len ? 1 : -1;

      /* Both positive, check the length for a shortcut */
      if (!op1_is_neg && op1[0].len != op2[0].len)
        return op1[0].len < op2[0].len ? -1 : 1;

      /* Scan from the most significant value */
      for (i = op1[0].len; 0 < i; i--)
        {
          if (op1[0].value[i-1] < op2[0].value[i-1])
            return -1;
          else if (op2[0].value[i-1] < op1[0].value[i-1])
            return 1;
        }
    }
  return 0;
}
int
mpz_cmp_d (mpz_t op1, double op2)
{
  int result = 0;

  return result;
}
int
mpz_cmp_si (mpz_t op1, signed long int op2)
{
  int result = 0;

  return result;
}
int
mpz_cmp_ui (mpz_t op1, unsigned long int op2)
{
  int result = 0;

  return result;
}
int
mpz_cmpabs (mpz_t op1, mpz_t op2)
{
  int result = 0;

  return result;
}
int
mpz_cmpabs_d (mpz_t op1, double op2)
{
  int result = 0;

  return result;
}
int
mpz_cmpabs_ui (mpz_t op1, unsigned long int op2)
{
  int result = 0;

  return result;
}
int
mpz_sgn (mpz_t op)
{
  int result = 0;

  return result;
}

// Binary
void
mpz_and (mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void
mpz_ior (mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void
mpz_xor (mpz_t rop, mpz_t op1, mpz_t op2)
{

}
void
mpz_com (mpz_t rop, mpz_t op)
{

}

// I/O
size_t
mpz_out_str (FILE * stream, int base, mpz_t op)
{
  size_t result = 0;

  return result;
}
size_t
mpz_inp_str (mpz_t rop, FILE * stream, int base)
{
  size_t result = 0;

  return result;
}

// Misc.
int
mpz_fits_ulong_p (mpz_t op)
{
  int result = 0;

  return result;
}
int
mpz_fits_slong_p (mpz_t op)
{
  int result = 0;

  return result;
}
int
mpz_fits_uint_p (mpz_t op)
{
  int result = 0;

  return result;
}
int
mpz_fits_sint_p (mpz_t op)
{
  int result = 0;

  return result;
}
int
mpz_fits_ushort_p (mpz_t op)
{
  int result = 0;

  return result;
}
int
mpz_fits_sshort_p (mpz_t op)
{
  int result = 0;

  return result;
}
size_t
mpz_sizeinbase (mpz_t op, int base)
{
  size_t result = 0;
  mpz_t temp, swap;
  mpz_init_set (temp, op);
  mpz_init (swap);
  while (0 < mpz_cmp_si(temp, 0))
    {
      mpz_tdiv_q_ui (swap, temp, base);
      mpz_swap(temp, swap);
      result++;
    }
  return result;
}

void *
_malloc (size_t size)
{
  void * result = malloc (size);
  _mem_check (result);
  return result;
}

void *
_realloc (void * ptr, size_t prev_size, size_t new_size)
{
  void * result = malloc (new_size);
  _mem_check (result);
  memcpy (result, ptr, prev_size < new_size ? prev_size : new_size);
  free (ptr);
  return result;
}

void
_mem_check (void * ptr)
{
  if (ptr == NULL)
    {
      exit(-1);
    }
}

int
digit_to_int (char ch, int base)
{
  if (base < 2 || 62 < base)
    return -1;

  if (isdigit (ch))
    return ch - '0';
  else if (isupper (ch))
    return ch - 'A' + 10;
  else if (islower (ch))
    {
      if (base < 37)
        return ch - 'a' + 10;
      else if (base < 63)
        return ch - 'a' + 36;
      else
        return -1;
    }
  else
    return -1;
}

char
int_to_digit (mpz_t op, int base)
{
  char x;

  if ((base < -36 || 62 < base) || (base < 2 && -2 < base))
      return 0;

  x = (char) mpz_get_ui (op);

  if (x > abs (base))
    return 0;

  if (x < 10)
    return x + '0';
  else if (x < 37)
    {
      if (base < 0)
        return x + 'A' - 10;
      else if (base < 37)
        return x + 'a' - 10;
      else if (base < 63)
        return x + 'A' - 10;
      else
        return 0;
    }
  else if (x < 63)
    {
      return x + 'a' - 36;
    }
  else
    return 0;
}
