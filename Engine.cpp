// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdarg.h>
#include "UniPatSys.h"
#include "UniPatSysCard.h"
#include "Engine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine (CCardGame* cardgame)  { m_cardgame = cardgame; }

CEngine::~CEngine () { }

int CEngine::Evaluate (void* pcode, int nargs, ...) {
   va_list arglist;
   long args [10];
   va_start (arglist, nargs);
   for (int n = 0; n < nargs; n++) {
      args [n] = va_arg (arglist, long);
   }
   va_end (arglist);
   long stk [10];
   long* ps = stk;
   int ret = 0;
   bool skip = false;
   aCode* pca = NULL;
   for (aCode* pc = (aCode*)pcode; *pc != XEND; ) {
      ps = stk;
      //for (aCode* pca = pc + 1; OPTYPE (*pca) == OPARG; *pca++) { --vs8: C2065
      for (pca = pc + 1; OPTYPE (*pca) == OPARG; *pca++) {
	 switch (*pca & OPMASK) {
	 case 0:
	    *ps++ = *pca & ~OPMASK;
	    break;
	 case XSTK1:
	 case XNUM1:
	    *ps++ = args [0];
	    break;
	 case XSTK2:
	 case XNUM2:
	    *ps++ = args [1];
	    break;
	 case XST:
	    *ps++ = (long)m_cardgame->Stack ((estacktype)BY1(*pca), BY0(*pca));
	    break;
	 }
      }
      switch (*pc & OPMASK) {
      case XRETURN:
	 return stk [0];
      case XDEAL: // deal to pile, face down, face up
	 m_cardgame->DealCards ((CStack*)stk[0], stk[1], stk[2]);
	 break;
      case XNEWSTK: // create [1] new stacks of type [0]
	 //m_cardgame->NewStacks((estacktype)stk[0], stk[1]); // not yet used
         break;
      case XNEWCRD: // create [0] new cards
	 //m_cardgame->NewCards(stk[0]);  // not yet used
         break;
      case XGATHER:
	 m_cardgame->Stack (ST_DECK, 1)->MoveCards ((CStack*)stk[0], stk[1], CF_FACEDOWN);
	 break;
      case XMOVES:
	 ((CStack*)stk[1])->MoveStack ((CStack*)stk[0], stk[2]);
	 break;
      case XDEALC:
	 m_cardgame->DealCard ((CStack*)stk[0], (esuit)stk[1], (erank)stk[2]);
	 break;
      case XCOMPARE:	
	 skip = !Compare (stk[0], (erelop)stk[1], stk[2]);
	 break;
      case XSCOUNT:	
	 skip = !TestCount ((CStack*)stk[0], (erelop)stk[1], stk[2]);
	 break;
      case XSCOUNTS: // count: stack relop value
	 skip = !TestCount ((CStack*)stk[0], (erelop)stk[1], stk[2]);
	 break;
      case XSTYPE:	
	 skip = !TestType ((CStack*)stk[0], (erelop)stk[1], stk[2]);
	 break;
      case XSHEAD: // test head???
	 skip = !TestTail ((CStack*)stk[0], (ecardop)stk[1], stk[2]);
	 break;
      case XSTAIL:	
	 skip = !TestTail ((CStack*)stk[0], (ecardop)stk[1], stk[2]);
	 break;
      case XSJOIN: // test that adjacent cards of two stacks conform to SQ_ASCENDING etc
	 skip = !TestSeqC ((CStack*)stk[0], (eseq)stk[1], (CStack*)stk[2]);
	 break;
      case XSSUM: // test the sum of the card values RELOP value
	 skip = !TestSum ((CStack*)stk[0], (erelop)stk[1], stk[2]);
	 break;
      case XSSEQ:
	 skip = !TestSeqS ((CStack*)stk[0], (eseq)stk[1], (int)stk[2]);
	 break;
      case XSPLACE:
	 skip = !TestPlace((CStack*)stk[0], (eplace)stk[1], (CStack*)stk[2]);
	 break;
      default: ;
	 ASSERT (FALSE);
      }
      pc = pca;
      if (skip) {
	 while (OPTYPE (*pc) != OPACTION && *pc != XEND)
	    pc++;
	 while ((OPTYPE (*pc) == OPACTION || OPTYPE (*pc) == OPARG) && *pc != XEND)
	    pc++;
	 skip = false;
      } 
   }
   return ret;
}

bool CEngine::Compare (int val1, erelop relop, int val2) {
   switch (relop) {
   case EQ: return val1 == val2;
   case NE: return val1 != val2;
   case GT: return val1 >  val2;
   case GE: return val1 >= val2;
   case LT: return val1 <  val2;
   case LE: return val1 <= val2;
   }
   ASSERT (FALSE);
   return false;
}

bool CEngine::CompareCard (CCard* pc, ecardop cardop, int val) {
   if (!pc)
      return false;
   switch (cardop) {
   case RANK_EQ:  return pc->Rank () == val;
   case RANK_NE:  return pc->Rank () != val;
   case SUIT_EQ:  return pc->Suit () == val;
   case SUIT_NE:  return pc->Suit () != val;
   case RED_EQ:   return pc->Colour () == val;
   case RED_NE:   return pc->Colour () != val;
   case BLACK_EQ: return pc->Colour () == val;
   case BLACK_NE: return pc->Colour () != val;
   }
   ASSERT (FALSE);
   return false;
}

bool CEngine::TestCountS (estacktype st, int ord, erelop relop, int val) {
   return Compare (GetCount (m_cardgame->Stack (st, ord)), relop, val); 
}




