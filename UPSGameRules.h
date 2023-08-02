// UPSGameRules.h: rules for UPS card games
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UPSGAMERULES_H_INCLUDED_)
#define AFX_UPSGAMERULES_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef long aCode;
enum estacktype;
struct CCardLayout;
class CStack;
class CCardGame;
class CEngine;

/////////////////////////////////////////////////////////////////////////////
// basic parameters of a game
struct CGameParams {
   char* title;		   // string to identify game
   int ncards;		   // number of cards (52, 104 or special)
   estacktype autodest;	   // where to automatically move cards (don't if NUL)
   int maxround;	   // maximum number of rounds 0=not used
   int maxdeal;		   // maximum number of deals 0=not used
private:
   static CGameParams GameParams;
};

/////////////////////////////////////////////////////////////////////////////
// structure for collection of static data per game
struct CGameRulesInfo {
   CGameParams* m_gameparams;
   CCardLayout* m_cardlayout;
   aCode* m_validjoin;
   aCode* m_validsplit;
   aCode* m_dealcards;
   static CGameRulesInfo GameRulesInfo []; // rules for all defined games
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGameRules - implements the rules for the selected game

class CGameRules : public CObject {
// Attributes
private:
   CGameParams* m_gameparams;
   CCardLayout* m_cardlayout;
   aCode* m_validjoin;
   aCode* m_validsplit;
   aCode* m_dealcards;
   static CCardGame* m_cardgame;
   static vector<CGameRules*> m_games;
   static CEngine* m_engine;

// constructors
public:
   static void Create(CCardGame* game);
private:
   CGameRules(CGameRulesInfo* info);
   virtual ~CGameRules () { }

// properties
public:
   static CGameRules* GetGame(int n)  { return m_games[n]; }
   static CGameRules* FindGame(CString title);
   static int MaxGame()             { return m_games.size(); }
   CCardLayout* CardLayout()       { return m_cardlayout; }
   CGameParams* GameParams()       { return m_gameparams; }
   char* Title()                    { return m_gameparams->title; }

// operations
   void MakeLayout();
   int DealLayout();
   int DealRound();
   int DealRedeal();
   int PostDrop(CStack* psd);
   int ValidJoin(CStack* psd, CStack* pss);
   int ValidSplit(CStack* ps, int num);
   bool ValidTurnup (CStack* ps);
};

#endif

