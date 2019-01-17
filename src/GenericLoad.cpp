#include "CommonDefines.h"
#include "Draw.h"

bool LoadGenericData()
{
	MakeSurface_File("Pixel", 1);
	
	bool bError = false;
	if ( !MakeSurface_File("MyChar", 16) )
		bError = true;
	if ( !MakeSurface_File("Title", 0) )
		bError = true;
	if ( !MakeSurface_File("ArmsImage", 12) )
		bError = true;
	if ( !MakeSurface_File("Arms", 11) )
		bError = true;
	if ( !MakeSurface_File("ItemImage", 8) )
		bError = true;
	if ( !MakeSurface_File("StageImage", 14) )
		bError = true;
	if ( !MakeSurface_File("Npc/NpcSym", 20) )
		bError = true;
	if ( !MakeSurface_File("Npc/NpcRegu", 23) )
		bError = true;
	if ( !MakeSurface_File("TextBox", 26) )
		bError = true;
	if ( !MakeSurface_File("Caret", 19) )
		bError = true;
	if ( !MakeSurface_File("Bullet", 17) )
		bError = true;
	if ( !MakeSurface_File("Face", 27) )
		bError = true;
	if ( !MakeSurface_File("Fade", 6) )
		bError = true;
	
	MakeSurface_File("Credit/Credit01", 36);
	
	if (bError)
	{
		return false;
	}
	else
	{
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_SCREEN_GRAB);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_LEVEL_BACKGROUND);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_MAP);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, 37); //Unknown?
		MakeSurface_Generic(256, 256, SURFACE_ID_LEVEL_TILESET);
		MakeSurface_Generic(160, 16, SURFACE_ID_ROOM_NAME);
		MakeSurface_Generic(40, 240, 29); //Unknown?
		MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_1);
		MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_2);
		MakeSurface_Generic(WINDOW_WIDTH, 240, SURFACE_ID_CREDIT_CAST);
		return true;
	}
}
