#include "type.h"

A_stm A_CompoundStm(A_stm stm1,A_stm stm2)
{
  A_stm s = checked_malloc(sizeof(*s));
  s->kind = A_compoundStm;
  s->u.compound.stm1=stm1;
  s->u.compound.stm2=stm2;
  return s;
}
