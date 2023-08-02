// UpsGameProps.cpp : implementation file
//

#include "stdafx.h"
#include "UniPatSys.h"
#include "UpsGameProps.h"

// CGameProps
CGameProps CGameProps::GameProps;
const char* CGameProps::profSect;

CGameProps::CGameProps() {
   _automove = true;
   _bestmove_sclick = false;
   _cardback = 3;
   _game_name = "";
   srand((unsigned)time(NULL));
   _seed = rand();
}

CGameProps::~CGameProps() {
}

void CGameProps::Load(const char* sect) {
   profSect = sect;
   CWinApp* app = AfxGetApp();
   GameProps._automove = !!app->GetProfileInt(profSect, "Automove", 1);
   GameProps._bestmove_sclick = !!app->GetProfileInt(profSect, "SingleClick", 1);
   GameProps._cardback = app->GetProfileInt(profSect, "CardBack", 0);
   GameProps._game_name = app->GetProfileString(profSect, "Game", "Ultima");
   GameProps._seed = app->GetProfileInt(profSect, "Seed", 0);
}

// save properties back to same section as loaded
void CGameProps::Save() {
   CWinApp* app = AfxGetApp();
   app->WriteProfileInt(profSect, "Automove", GameProps._automove);
   app->WriteProfileInt(profSect, "SingleClick", GameProps._bestmove_sclick);
   app->WriteProfileInt(profSect, "CardBack", GameProps._cardback);
   app->WriteProfileString(profSect, "Game", GameProps._game_name);
   app->WriteProfileInt(profSect, "Seed", GameProps._seed);
}

// end CGameProps
