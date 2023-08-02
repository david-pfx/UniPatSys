// Engine.h: interface for the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENGINE_H__86D6A6A1_EF05_11D1_A8AE_00000100782D__INCLUDED_)
#define AFX_ENGINE_H__86D6A6A1_EF05_11D1_A8AE_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef long aCode;

#define HILO(a,b) (((a)<<16)|(b))
#define HI(x) (((x)>>16)&0xffff)
#define LO(x) ( (x)     &0xffff)
#define BY0(x) ( (x)     &0xff)
#define BY1(x) (((x)>> 8)&0xff)
#define BY2(x) (((x)>>16)&0xff)
#define BY3(x) (((x)>>24)&0xff)


enum excode {
   //  take action -- change state
   XEND        = 0x0101 << 16, 
   XRETURN     = 0x0102 << 16, 
   XDEAL       = 0x0103 << 16, 
   XGATHER     = 0x0104 << 16, 
   XMOVES      = 0x0105 << 16, 
   XDEALC      = 0x0106 << 16, 
   XNEWSTK     = 0x0107 << 16, 
   XNEWCRD     = 0x0108 << 16, 
   //  compute predicate, skip if false
   XCOMPARE    = 0x0201 << 16, 
   XSCOUNT     = 0x0202 << 16, 
   XSTYPE      = 0x0203 << 16, 
   XSJOIN      = 0x0204 << 16, 
   XSHEAD      = 0x0205 << 16, 
   XSTAIL      = 0x0206 << 16, 
   XSSUM       = 0x0207 << 16, 
   XSSEQ       = 0x0208 << 16, 
   XSCOUNTS    = 0x0209 << 16, 
   XSPLACE     = 0x020a << 16, 
   // argument operations (push on stack)
   XSTK1       = 0x0001 << 16, 
   XSTK2       = 0x0002 << 16, 
   XNUM1       = 0x0003 << 16, 
   XNUM2       = 0x0004 << 16, 
   XST         = 0x0005 << 16, 

};

#define OPTYPE(x) BY3(x)
#define OPACTION  1
#define OPPRED	  2
#define	OPARG	  0
#define	OPMASK	  0xffff0000

#define XSTK(a,b) XST|((a)<<8)|(b)

class CEngine {
public:
   CCardGame* m_cardgame;

public:
   CEngine (CCardGame* cardgame);
   virtual ~CEngine ();

   int Evaluate (void* pcode, int nargs, ...);
   bool Compare (int val1, erelop relop, int val2);
   bool CompareCard (CCard* pc, ecardop cardop, int val);
   int GetCount (CStack* ps) { return ps ? ps->GetCount () : 0; }
   bool TestCountS (estacktype st, int ord, erelop relop, int val);
   bool TestCount (CStack* ps, erelop relop, int val)
      { return Compare (ps->GetCount (), relop, val); }
   bool TestType (CStack* ps, erelop relop, int val)
      { return Compare (ps->Type (), relop, val); }
   bool TestHead (CStack* ps, ecardop cardop, int val)
      { return CompareCard (ps->HeadCard (), cardop, val); }
   bool TestTail (CStack* ps, ecardop cardop, int val)
      { return ps && CompareCard (ps->TailCard (), cardop, val); }
   bool TestSeqC (CStack* psd, eseq seq, CStack* pss)
      { return psd->HeadCard () && pss->TailCard () 
	       ? psd->HeadCard ()->TestSeq (pss->TailCard (), seq) : false; }
   bool TestSeqS (CStack* ps, eseq seq, int num)
      { return ps->TestSeq (seq, num); }
   bool TestSum (CStack* ps, erelop relop, int val)
      { return Compare (ps->Sum (), relop, val); }
   bool TestPlace(CStack* ps1, eplace place, CStack* ps2)
      { return ps1->TestPlace(place, ps2); }
};

#endif // !defined(AFX_ENGINE_H__86D6A6A1_EF05_11D1_A8AE_00000100782D__INCLUDED_)
