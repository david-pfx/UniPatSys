#pragma once

// CGameProps command target

class CGameProps : public CObject {
public:
   bool _automove;
   bool _bestmove_sclick;
   int _cardback;
   CString _game_name;
   int _seed;
   //
   static CGameProps GameProps;
   static const char* profSect;

   static void Load(const char* section);
   static void Save();

private:
   CGameProps();
   virtual ~CGameProps();
};


