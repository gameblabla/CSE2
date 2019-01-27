#include "Sound.h"
#include "MyChar.h"
#include "NpChar.h"
#include "ValueView.h"
#include "TextScr.h"

void DamageMyChar(int damage)
{
	if (!gMC.shock)
	{
		//Damage player
		PlaySoundObject(16, 1);
		gMC.cond &= ~1;
		gMC.shock = 128;
		if (gMC.unit != 1)
			gMC.ym = -0x400;
		gMC.life -= damage;
		
		//Lose a whimsical star
		if (gMC.equip & 0x80 && gMC.star > 0)
			--gMC.star;
		
		//Lose experience
		/*
		if ( unk_81C8598 & 4 )
			v1 = gArmsData[gSelectedArms].exp - damage;
		else
			v1 = gArmsData[gSelectedArms].exp - 2 * damage;
		gArmsData[gSelectedArms].exp = v1;
		while ( gArmsData[gSelectedArms].exp < 0 )
		{
		if ( gArmsData[gSelectedArms].level <= 1 )
		{
		gArmsData[gSelectedArms].exp = 0;
		}
		else
		{
		gArmsData[gSelectedArms].exp += gArmsLevelTable[0].exp[--gArmsData[gSelectedArms].level
		- 1
		+ 3 * gArmsData[gSelectedArms].code];
		if ( word_81C8614 > 0 && gArmsData[gSelectedArms].code != 13 )
		SetCaret(x, y, 10, 2);
		}
		}
		*/
		
		//Tell player how much damage was taken
		SetValueView(&gMC.x, &gMC.y, -damage);
		
		//Death
		if (gMC.life <= 0)
		{
			PlaySoundObject(17, 1);
			gMC.cond = 0;
			SetDestroyNpChar(gMC.x, gMC.y, 0x1400, 0x40);
			StartTextScript(40);
		}
	}
}