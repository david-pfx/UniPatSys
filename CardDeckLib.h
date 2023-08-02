// CardDeckLib.h: interface of the CardDeckLib class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCardDeckLib {
public:
   static CCardDeckLib* CardDeckLib;
   static void Create();
   char* GetLibName() { return _libname; }
   CSize GetCardSize() { return _cardsize; }
   HINSTANCE GetHInst() { return _hinst; }
   vector<char*> GetFaceIds() { return _faceids; }
   vector<char*> GetBackIds() { return _backids; }
private:
   CCardDeckLib(void);
   virtual ~CCardDeckLib(void);
   char* _libname;
   CSize _cardsize;
   HINSTANCE _hinst;
   vector<char*> _faceids;
   vector<char*> _backids;
};
