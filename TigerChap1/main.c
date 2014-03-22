#include "util.h"
#include "slp.h"
#include "prog1.h"
#include <stdio.h>

#define DEBUG

typedef struct table *Table_;
struct table {string id; int value; Table_ tail;};

typedef struct IntAndTable *TableInt_;
struct IntAndTable {int i; Table_ t;};

Table_ interpStm(A_stm a,Table_ t);
TableInt_ interpExp(A_exp e,Table_ t);
TableInt_ interpExpList(A_expList,Table_ t);

int lookup(Table_ t,string key)
{
   Table_ tmp=t->tail;
   while(tmp)
   {
      if(strcmp(key,tmp->id)==0)
        return tmp->value; 
      tmp=tmp->tail;
   }
}

void update(Table_ t,string id,int value)
{
  Table_ tnew = malloc(sizeof(*t));
  tnew->id=id; tnew->value=value; tnew->tail=t->tail;
  t->tail=tnew;
}

Table_ interpStm(A_stm a,Table_ t) //(id,value) should append to Table t
{
  //new table
  switch(a->kind)
  {
    case (A_compoundStm) : 
                         {
                         Table_ tmpt=NULL;
                         tmpt=interpStm(a->u.compound.stm1,t);
                         tmpt=interpStm(a->u.compound.stm2,tmpt);
                         return tmpt;
                         break;
                         } 
    case (A_assignStm) :  //id := Exp, (id,value_of_exp) add to the table;
                        //interpreting Exp can produce (id,value) tables,so we must save it;
                        {
                        TableInt_ tmptable=NULL;
                        Table_ tmpt=t;
                        tmptable = interpExp(a->u.assign.exp,t);
                        update(t,a->u.assign.id,tmptable->i); //t will never change !!!!!!!!
                        //tmpt->tail=tmptable->t;//link together 
                        return t;
                        break;
                        }

    case (A_printStm) :   //ExpList can produce (id.value) tables
                        // the value of ExpList should be printed
                        {
                        Table_ tmpt = NULL;
                        TableInt_ tmptable=NULL;
                        tmptable = interpExpList(a->u.print.exps,t);
                        return tmptable->t; 
                        }
                       
    default : break;
  }
}

TableInt_ interpExpList(A_expList list,Table_ t)
{
   if(list->kind==A_pairExpList)
   {
     TableInt_ tmptable=interpExp(list->u.pair.head,t);
     printf("%d\t",tmptable->i);
     TableInt_ tmptable2=interpExpList(list->u.pair.tail,tmptable->t);
     return tmptable2;
   }
   else if(list->kind==A_lastExpList)
   {
     TableInt_ tmptable=interpExp(list->u.last,t);
     printf("%d\t",tmptable->i);
     return tmptable;
   }
   return;
}

TableInt_ interpExp(A_exp e,Table_ t)
{
  switch(e->kind)
  {
    case (A_idExp) :   
                     {
                     int tmp=lookup(t,e->u.id);
                     TableInt_ tb = malloc(sizeof(*tb));
                     tb->t=t; tb->i=tmp;
                     return tb;
                     }
                   

    case (A_numExp) : 
                      {
                      Table_ t = malloc(sizeof(*t));
                      TableInt_ tb = malloc(sizeof(*tb));
                      tb->t=t; tb->i=e->u.num; 
                      return tb;
                      }
                      
    case (A_opExp) :
                       {
                       TableInt_ lhs=interpExp(e->u.op.left,t);
                       TableInt_ rhs=interpExp(e->u.op.right,t);
                       int ret_;
                       switch(e->u.op.oper)
                       {
                          case (A_plus) : ret_=lhs->i+rhs->i; break; 
                          case (A_minus): ret_=lhs->i-rhs->i; break;
                          case (A_times): ret_=lhs->i*rhs->i; break;
                          case (A_div)  : ret_=lhs->i/rhs->i; break;
                          default: break;                       
                       }
                       TableInt_ tb = malloc(sizeof(*tb));
                       tb->t=t; tb->i=ret_;
                       return tb;
                       }
     
    case (A_eseqExp) : //
                       {
                       Table_ tmpt=NULL;
                       tmpt=interpStm(e->u.eseq.stm,t);
                       TableInt_ tmptable=NULL;
                       tmptable=interpExp(e->u.eseq.exp,tmpt);
                       while(tmpt->tail)
                         tmpt=tmpt->tail;
                       tmpt->tail=tmptable->t;
                       tmptable->t=tmpt;
                       return tmptable;
                       }
   
    default : return;
  }
}

int main(int argc,char *argv[])
{                                             /* [head|0]-->NULL */
  Table_ t=malloc(sizeof(*t));
  string head=malloc(5*sizeof(char));
  head="head";
  t->value = 0;    
  t->tail=NULL; t->id=head;
  interpStm(prog(),t);
  return 0;
}
