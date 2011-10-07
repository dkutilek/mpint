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
void * _realloc (void * ptr, size_t size);
void _mem_check (void * ptr);
int digit_to_int (char ch, int base);
char int_to_digit (mpz_t op, int base);

// Initialization
void
mpz_init (mpz_t x)
{
  x[0].value = _malloc (sizeof (uintmax_t));
  x[0].len = 1;
}
void
mpz_clear (mpz_t x)
{
  free (x[0].value);
  x[0].len = 0;
}

// Assignment
void
mpz_set (mpz_t rop, mpz_t op)
{
  rop[0].value = _malloc (sizeof (uintmax_t) * abs (op[0].len));
  memcpy ((void *) rop[0].value, (void *) op[0].value,
      sizeof (uintmax_t) * abs(op[0].len));
  rop[0].len = op[0].len;
}
void
mpz_set_ui (mpz_t rop, unsigned long int op)
{
  _realloc(rop[0].value, sizeof (uintmax_t));
  rop[0].len = 1;
  *rop[0].value = op;
}
void
mpz_set_si (mpz_t rop, signed long int op)
{
  _realloc(rop[0].value, sizeof (uintmax_t));
  rop[0].len = op < 0 ? -1 : 1;
  *rop[0].value = abs (op);
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
  intmax_t len;
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
  return op[0].len < 0 ? - result : result;
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
  intmax_t i;
  int result;
  char * ch;

  if (str == NULL)
    {
      str = _malloc (sizeof (char) * ((abs(op[0].len) << 2) + abs(op[0].len)) + 1);
    }

  ch = str;

  if (op[0].len < 0)
    {
      // TODO REALLOC
      *str = '-';
      str++;
    }
  for (i = abs (op[0].len)-1; 0 <= i; i--)
    {
      result = sprintf(str, "%.16jx ", op[0].value[i]);
      printf("%.16jx\n", op[0].value[i]);
      str += 16;
    }

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
  int carry = 0;
  intmax_t len1, len2, i;
  mpz_t x;

  /* If only one of the numbers is negative, use subtract */
  if (op1[0].len < 0 && 0 < op2[0].len)
    {
      mpz_init_set (x, op1);
      x[0].len *= -1;
      mpz_sub (rop, op2, x);
      mpz_clear (x);
    }
  else if (0 < op1[0].len && op2[0].len < 0)
    {
      mpz_init_set (x, op2);
      x[0].len *= -1;
      mpz_sub (rop, op1, x);
      mpz_clear (x);
    }
  /* Otherwise, add */
  else
    {
      len1 = abs (op1[0].len);
      len2 = abs (op2[0].len);
      rop[0].len = len1 < len2 ? len2 : len1;
      _realloc (rop[0].value, sizeof (uintmax_t) * rop[0].len);
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
          _realloc (rop[0].value, sizeof (uintmax_t) * rop[0].len);
          rop[0].value[rop[0].len-1] = carry;
        }
      if (op1[0].len < 0 || op2[0].len < 0)
        rop[0].len = -1 * rop[0].len;
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
_sub (uintmax_t * rop, uintmax_t op1, uintmax_t op2, int * borrow)
{
  if (*borrow)
    {
      if (0 < op1)
        op1--;
      else
        {
          op1 = UINTMAX_MAX;
          *borrow = 1;
        }
    }

  /* The resulting subtraction will require borrowing */
  if (op1 < op2)
    *borrow = 1;
  else
    *borrow = 0;

  *rop = op1 - op2;
}
void
mpz_sub (mpz_t rop, mpz_t op1, mpz_t op2)
{
  int borrow = 0;
  intmax_t len1, len2, i;
  mpz_t x;

  /* If only op2 is negative, use add */
  if (0 < op1[0].len && op2[0].len < 0)
    {
      mpz_init_set (x, op2);
      x[0].len *= -1;
      mpz_add (rop, x, op2);
      mpz_clear (x);
    }
  /* If only op1 is negative, use add */
  else if (op1[0].len < 0 && 0 < op2[0].len)
    {
      mpz_init_set (x, op2);
      x[0].len *= -1;
      mpz_add (rop, op1, x);
      mpz_clear (x);
    }
  /* Otherwise, subtract */
  else
    {
      /* If op1 < op2, swap and negate the result */
      if (mpz_cmp (op1, op2) < 0)
        {
          mpz_sub (rop, op2, op1);
          rop[0].len *= -1;
        }
      else
        {
          len1 = abs (op1[0].len);
          len2 = abs (op2[0].len);
          rop[0].len = abs (op1[0].len);
          _realloc (rop[0].value, sizeof (uintmax_t) * rop[0].len);
          for (i = 0; i < rop[0].len; i++)
            {
              if (i < len1 && i < len2)
                {
                  _sub (rop[0].value + i, op1[0].value[i], op2[0].value[i], &borrow);
                }
              else if (i < len1)
                {
                  rop[0].value[i] = *op1[0].value;
                  if (borrow)
                    {
                      if (0 < rop[0].value[i])
                        rop[0].value[i]--;
                      else
                        rop[0].value[i] = UINTMAX_MAX;
                      borrow = 0;
                    }
                }
              /* Should never reach this point if op2 < op1 */
              else if (i < len2)
                {
                  // TODO crash
                }
            }
          if (op1[0].len < 0)
            rop[0].len *= -1;
        }
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
  int shift = sizeof (uintmax_t) >> 1;
  uintmax_t top1, top2, bot1, bot2, rtop, rbot, mask = UINTMAX_MAX << shift;

  /* Split the two operands to ensure the result does not overflow.  Take the
     overflow of the bottom and add it to the result of the top.  Take the
     overflow of the top and return it to be used in the next
     multiplication. */

  top1 = op1 >> shift;
  top2 = op2 >> shift;
  bot1 = op1 & mask;
  bot2 = op2 & mask;

  rbot = bot1 * bot2 + carry;
  rtop = (top1 * top2) + (rbot >> shift);

  *rop = (rtop << shift) | (rbot & (~mask));
  return (rbot & mask) >> shift;
}
void
mpz_mul (mpz_t rop, mpz_t op1, mpz_t op2)
{
  int i, j, carry;
  intmax_t len1, len2;
  len1 = abs (op1[0].len);
  len2 = abs (op2[0].len);

  rop[0].len = len1 + len2;

  /* Initialize and set to 0 */
  _realloc (rop[0].value, sizeof (uintmax_t) * rop[0].len);
  for (j = 0; j < rop[0].len; j++)
    rop[0].value[j] = 0;

  /* Multiply */
  for (j = 0; j < len1; j++)
    for (i = carry = 0; i < len2; i++)
      carry = _mul (rop[0].value + i + j, op1[0].value[i], op2[0].value[j], carry);

  if (carry)
    {
      rop[0].len++;
      _realloc (rop[0].value, sizeof (uintmax_t) * rop[0].len);
      rop[0].value[rop[0].len-1] = carry;
    }

  /* Set the sign of the result */
  if (op1[0].len < 0 && op1[0].len < 0)
    rop[0].len = abs (rop[0].len);
  else if (op1[0].len < 0 || op1[0].len < 0)
    rop[0].len = -1 * abs (rop[0].len);
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
  mpz_set (rop, op);
  rop[0].len *= -1;
}
void
mpz_abs (mpz_t rop, mpz_t op)
{
  mpz_set (rop, op);
  rop[0].len = abs (rop[0].len);
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
void
mpz_tdiv_qr (mpz_t q, mpz_t r, mpz_t n, mpz_t d)
{

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
mpz_cmp (mpz_t op1, mpz_t op2)
{
  intmax_t i;

  if (op1[0].len < op2[0].len)
    return -1;
  else if (op2[0].len < op1[0].len)
    return 1;
  else
    {
      for (i = 0; i < abs (op1[0].len); i++)
        {
          if (op1[0].value[i] < op2[0].value[i])
            return -1;
          else if (op2[0].value[i] < op1[0].value[i])
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
_realloc (void * ptr, size_t size)
{
  void * result = realloc (ptr, size);
  _mem_check (result);
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
