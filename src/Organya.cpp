// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

// Some of the original source code for this file can be found here:
// https://github.com/shbow/organya/blob/master/source/OrgFile.cpp
// https://github.com/shbow/organya/blob/master/source/OrgPlay.cpp
// https://github.com/shbow/organya/blob/master/source/Sound.cpp
// https://github.com/shbow/organya/blob/master/source/WinTimer.cpp

#include "Organya.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Backends/Audio.h"
#include "Backends/Misc.h"
#include "Resource.h"
#include "Sound.h"

#define PANDUMMY 0xFF
#define VOLDUMMY 0xFF
#define KEYDUMMY 0xFF

#define ALLOCNOTE 4096

#define DEFVOLUME	200//255はVOLDUMMY。MAXは254
#define DEFPAN		6

//曲情報をセットする時のフラグ
#define SETALL		0xffffffff//全てをセット
#define SETWAIT		0x00000001
#define SETGRID		0x00000002
#define SETALLOC	0x00000004
#define SETREPEAT	0x00000008
#define SETFREQ		0x00000010
#define SETWAVE		0x00000020
#define SETPIPI		0x00000040

// Below are Organya song data structures
typedef struct NOTELIST
{
	NOTELIST *from;	// Previous address
	NOTELIST *to;	// Next address

	long x;	// Position
	unsigned char length;	// Sound length
	unsigned char y;	// Sound height
	unsigned char volume;	// Volume
	unsigned char pan;
} NOTELIST;

// Track data * 8
typedef struct TRACKDATA
{
	unsigned short freq;	// Frequency (1000 is default)
	unsigned char wave_no;	// Waveform No.
	signed char pipi;

	NOTELIST *note_p;
	NOTELIST *note_list;
} TRACKDATA;

// Unique information held in songs
typedef struct MUSICINFO
{
	unsigned short wait;
	unsigned char line;	// Number of lines in one measure
	unsigned char dot;	// Number of dots per line
	unsigned short alloc_note;	// Number of allocated notes
	long repeat_x;	// Repeat
	long end_x;	// End of song (Return to repeat)
	TRACKDATA tdata[MAXTRACK];
} MUSICINFO;

// メインクラス。このアプリケーションの中心。（クラスってやつを初めて使う） (Main class. The heart of this application. (Class is used for the first time))
typedef struct OrgData
{
	OrgData();	// コンストラクタ (Constructor)
//	~OrgData();	// デストラクタ (Destructor)
	MUSICINFO info;
	char track;
	char mute[MAXTRACK];
	unsigned char def_pan;
	unsigned char def_volume;
	void InitOrgData(void);
	void GetMusicInfo(MUSICINFO *mi);	// 曲情報を取得 (Get song information)
	// 曲情報を設定。flagは設定アイテムを指定 (Set song information. flag specifies the setting item)
	BOOL SetMusicInfo(MUSICINFO *mi,unsigned long flag);
	BOOL NoteAlloc(unsigned short note_num);	// 指定の数だけNoteDataの領域を確保 (Allocate the specified number of NoteData areas.)
	void ReleaseNote(void);	// NoteDataを開放 (Release NoteData)
	// 以下は再生 (The following is playback)
	void PlayData(void);
	void SetPlayPointer(long x);	// 再生ポインターを指定の位置に設定 (Set playback pointer to specified position)
	// 以下はファイル関係 (The following are related to files)
	BOOL InitMusicData(const char *path);
} ORGDATA;

AudioBackend_Sound *lpORGANBUFFER[8][8][2] = {NULL};

/////////////////////////////////////////////
//■オルガーニャ■■■■■■■■■■■■/////// (Organya)
/////////////////////

// Wave playing and loading
typedef struct
{
	short wave_size;
	short oct_par;
	short oct_size;
} OCTWAVE;

OCTWAVE oct_wave[8] =
{
	{ 256,  1,  4 }, // 0 Oct
	{ 256,  2,  8 }, // 1 Oct
	{ 128,  4, 12 }, // 2 Oct
	{ 128,  8, 16 }, // 3 Oct
	{  64, 16, 20 }, // 4 Oct
	{  32, 32, 24 }, // 5 Oct
	{  16, 64, 28 }, // 6 Oct
	{   8,128, 32 }, // 7 Oct
};

BOOL MakeSoundObject8(signed char *wavep, signed char track, signed char pipi)
{
	unsigned long i,j,k;
	unsigned long wav_tp;	// WAVテーブルをさすポインタ (Pointer to WAV table)
	unsigned long wave_size;	// 256;
	unsigned long data_size;
	unsigned char *wp;
	unsigned char *wp_sub;
	int work;

	if (!audio_backend_initialised)
		return FALSE;

	for (j = 0; j < 8; j++)
	{
		for (k = 0; k < 2; k++)
		{
			wave_size = oct_wave[j].wave_size;

			if (pipi)
				data_size = wave_size * oct_wave[j].oct_size;
			else
				data_size = wave_size;

			wp = (unsigned char*)malloc(data_size);

			if (wp == NULL)
				return FALSE;

			// Get wave data
			wp_sub = wp;
			wav_tp = 0;

			for (i = 0; i < data_size; i++)
			{
				work = *(wavep + wav_tp);
				work += 0x80;

				*wp_sub = (unsigned char)work;

				wav_tp += 0x100 / wave_size;
				if (wav_tp > 0xFF)
					wav_tp -= 0x100;

				wp_sub++;
			}

			lpORGANBUFFER[track][j][k] = AudioBackend_CreateSound(22050, wp, data_size);

			free(wp);

			if (lpORGANBUFFER[track][j][k] == NULL)
				return FALSE;

			AudioBackend_RewindSound(lpORGANBUFFER[track][j][k]);
		}
	}

	return TRUE;
}

short freq_tbl[12] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};

void ChangeOrganFrequency(unsigned char key, signed char track, long a)
{
	if (!audio_backend_initialised)
		return;

	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 2; i++)
			AudioBackend_SetSoundFrequency(lpORGANBUFFER[track][j][i], ((oct_wave[j].wave_size * freq_tbl[key]) * oct_wave[j].oct_par) / 8 + (a - 1000));	// 1000を+αのデフォルト値とする (1000 is the default value for + α)
}

BOOL g_mute[MAXTRACK];	// Used by the debug Mute menu
short pan_tbl[13] = {0, 43, 86, 129, 172, 215, 256, 297, 340, 383, 426, 469, 512};
unsigned char old_key[MAXTRACK] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// 再生中の音 (Sound being played)
unsigned char key_on[MAXTRACK];	// キースイッチ (Key switch)
unsigned char key_twin[MAXTRACK];	// 今使っているキー(連続時のノイズ防止の為に二つ用意) (Currently used keys (prepared for continuous noise prevention))

void ChangeOrganPan(unsigned char key, unsigned char pan, signed char track)	// 512がMAXで256がﾉｰﾏﾙ (512 is MAX and 256 is normal)
{
	if (!audio_backend_initialised)
		return;

	if (old_key[track] != KEYDUMMY)
		AudioBackend_SetSoundPan(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], (pan_tbl[pan] - 0x100) * 10);
}

void ChangeOrganVolume(int no, long volume, signed char track)	// 300がMAXで300がﾉｰﾏﾙ (300 is MAX and 300 is normal)
{
	if (!audio_backend_initialised)
		return;

	if (old_key[track] != KEYDUMMY)
		AudioBackend_SetSoundVolume(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], (volume - 0xFF) * 8);
}

// サウンドの再生 (Play sound)
void PlayOrganObject(unsigned char key, int mode, signed char track, long freq)
{
	if (!audio_backend_initialised)
		return;

	if (lpORGANBUFFER[track][key / 12][key_twin[track]] != NULL)
	{
		switch (mode)
		{
			case 0:	// 停止 (Stop)
				if (old_key[track] != 0xFF)
				{
					AudioBackend_StopSound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]);
					AudioBackend_RewindSound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]);
				}
				break;

			case 1: // 再生 (Playback)
				break;

			case 2:	// 歩かせ停止 (Stop playback)
				if (old_key[track] != 0xFF)
				{
					AudioBackend_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], FALSE);
					old_key[track] = 0xFF;
				}
				break;

			case -1:
				if (old_key[track] == 0xFF)	// 新規鳴らす (New sound)
				{
					ChangeOrganFrequency(key % 12, track, freq);	// 周波数を設定して (Set the frequency)
					AudioBackend_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], TRUE);
					old_key[track] = key;
					key_on[track] = 1;
				}
				else if (key_on[track] == 1 && old_key[track] == key)	// 同じ音 (Same sound)
				{
					// 今なっているのを歩かせ停止 (Stop playback now)
					AudioBackend_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], FALSE);
					key_twin[track]++;
					if (key_twin[track] > 1)
						key_twin[track] = 0;
					AudioBackend_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], TRUE);
				}
				else	// 違う音を鳴らすなら (If you make a different sound)
				{
					AudioBackend_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], FALSE);	// 今なっているのを歩かせ停止 (Stop playback now)
					key_twin[track]++;
					if (key_twin[track] > 1)
						key_twin[track] = 0;
					ChangeOrganFrequency(key % 12, track, freq);	// 周波数を設定して (Set the frequency)
					AudioBackend_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], TRUE);
					old_key[track] = key;
				}

				break;
		}
	}
}

// オルガーニャオブジェクトを開放 (Open Organya object)
void ReleaseOrganyaObject(signed char track)
{
	if (!audio_backend_initialised)
		return;

	for (int i = 0; i < 8; i++)
	{
		if (lpORGANBUFFER[track][i][0] != NULL)
		{
			AudioBackend_DestroySound(lpORGANBUFFER[track][i][0]);
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if (lpORGANBUFFER[track][i][1] != NULL)
		{
			AudioBackend_DestroySound(lpORGANBUFFER[track][i][1]);
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}

// 波形データをロード (Load waveform data)
signed char wave_data[100][0x100];

BOOL InitWaveData100(void)
{
	const unsigned long *lpdword;	// リソースのアドレス (Resource address)

	if (!audio_backend_initialised)
		return FALSE;

	// リソースの検索 (Search for resources)
	lpdword = (unsigned long*)FindResource("WAVE100", "WAVE", NULL);

	if (lpdword == NULL)
		return FALSE;

	memcpy(wave_data, lpdword, 100 * 0x100);

	return TRUE;
}

// 波形を１００個の中から選択して作成 (Select from 100 waveforms to create)
BOOL MakeOrganyaWave(signed char track, signed char wave_no, signed char pipi)
{
	if (!audio_backend_initialised)
		return FALSE;

	if (wave_no > 99)
		return FALSE;

	ReleaseOrganyaObject(track);
	MakeSoundObject8(wave_data[wave_no], track, pipi);

	return TRUE;
}

/////////////////////////////////////////////
//■オルガーニャドラムス■■■■■■■■/////// (Organya drums)
/////////////////////

void ChangeDramFrequency(unsigned char key, signed char track)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetSoundFrequency(lpSECONDARYBUFFER[150 + track], key * 800 + 100);
}

void ChangeDramPan(unsigned char pan, signed char track)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetSoundPan(lpSECONDARYBUFFER[150 + track], (pan_tbl[pan] - 0x100) * 10);
}

void ChangeDramVolume(long volume, signed char track)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetSoundVolume(lpSECONDARYBUFFER[150 + track], (volume - 0xFF) * 8);
}

// サウンドの再生 (Play sound)
void PlayDramObject(unsigned char key, int mode, signed char track)
{
	if (!audio_backend_initialised)
		return;

	if (lpSECONDARYBUFFER[150 + track] != NULL)
	{
		switch (mode)
		{
			case 0:	// 停止 (Stop)
				AudioBackend_StopSound(lpSECONDARYBUFFER[150 + track]);
				AudioBackend_RewindSound(lpSECONDARYBUFFER[150 + track]);
				break;

			case 1:	// 再生 (Playback)
				AudioBackend_StopSound(lpSECONDARYBUFFER[150 + track]);
				AudioBackend_RewindSound(lpSECONDARYBUFFER[150 + track]);
				ChangeDramFrequency(key, track);	// 周波数を設定して (Set the frequency)
				AudioBackend_PlaySound(lpSECONDARYBUFFER[150 + track], FALSE);
				break;

			case 2:	// 歩かせ停止 (Stop playback)
				break;

			case -1:
				break;
		}
	}
}

ORGDATA org_data;

static void OrganyaCallback(void)
{
	org_data.PlayData();
}

OrgData::OrgData(void)
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		info.tdata[i].note_list = NULL;
		info.tdata[i].note_p = NULL;
	}
}

void OrgData::InitOrgData(void)
{
	track = 0;
	info.alloc_note = ALLOCNOTE;	// とりあえず10000個確保 (For the time being, secure 10,000 pieces)
	info.dot = 4;
	info.line = 4;
	info.wait = 128;
	info.repeat_x = info.dot * info.line * 0;
	info.end_x = info.dot * info.line * 255;

	for (int i = 0; i < MAXTRACK; i++)
	{
		info.tdata[i].freq = 1000;
		info.tdata[i].wave_no = 0;
		info.tdata[i].pipi = 0;
	}

	NoteAlloc(info.alloc_note);
	SetMusicInfo(&info, SETALL);

	def_pan = DEFPAN;
	def_volume = DEFVOLUME;
}

// 曲情報を設定。flagはアイテムを指定 (Set song information. flag specifies an item)
BOOL OrgData::SetMusicInfo(MUSICINFO *mi, unsigned long flag)
{
	//char str[32];	// Leftover debug junk
	int i;

	if (flag & SETGRID)	// グリッドを有効に (Enable grid)
	{
		info.dot = mi->dot;
		info.line = mi->line;
	}

	if (flag & SETWAIT)
	{
		info.wait = mi->wait;
		//itoa(mi->wait, str, 10);	// Leftover debug junk
	}

	if (flag & SETREPEAT)
	{
		info.repeat_x = mi->repeat_x;
		info.end_x = mi->end_x;
	}

	if (flag & SETFREQ)
	{
		for (i = 0; i < MAXMELODY; i++)
		{
			info.tdata[i].freq = mi->tdata[i].freq;
			info.tdata[i].pipi = info.tdata[i].pipi;	 // Just sets info.tdata[i].pipi to itself (SETPIPI already sets pipi, so maybe this line shouldn't be here in the first place)
		}
	}

	if (flag & SETWAVE)
		for (i = 0; i < MAXTRACK; i++)
			info.tdata[i].wave_no = mi->tdata[i].wave_no;

	if (flag & SETPIPI)
		for (i = 0; i < MAXTRACK; i++)
			info.tdata[i].pipi = mi->tdata[i].pipi;

	return TRUE;
}

// 指定の数だけNoteDataの領域を確保(初期化) (Allocate the specified number of NoteData areas (initialization))
BOOL OrgData::NoteAlloc(unsigned short alloc)
{
	int i,j;

	for (j = 0; j < MAXTRACK; j++)
	{
		info.tdata[j].wave_no = 0;
		info.tdata[j].note_list = NULL;	// コンストラクタにやらせたい (I want the constructor to do it)
		info.tdata[j].note_p = (NOTELIST*)malloc(sizeof(NOTELIST) * alloc);

		if (info.tdata[j].note_p == NULL)
		{
			for (i = 0; i < MAXTRACK; i++)
			{
				if (info.tdata[i].note_p != NULL)
				{
					free(info.tdata[i].note_p);
				#ifdef FIX_BUGS
					info.tdata[i].note_p = NULL;
				#else
					info.tdata[j].note_p = NULL;	// Uses j instead of i
				#endif
				}
			}

			return FALSE;
		}

		for (i = 0; i < alloc; i++)
		{
			(info.tdata[j].note_p + i)->from = NULL;
			(info.tdata[j].note_p + i)->to = NULL;
			(info.tdata[j].note_p + i)->length = 0;
			(info.tdata[j].note_p + i)->pan = PANDUMMY;
			(info.tdata[j].note_p + i)->volume = VOLDUMMY;
			(info.tdata[j].note_p + i)->y = KEYDUMMY;
		}
	}

	for (j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);

	track = 0;	// 今はここに書いておく (Write here now)

	return TRUE;
}

// NoteDataを開放 (Release NoteData)
void OrgData::ReleaseNote(void)
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		if (info.tdata[i].note_p != NULL)
		{
			free(info.tdata[i].note_p);
			info.tdata[i].note_p = NULL;
		}
	}
}

char pass[7] = "Org-01";
char pass2[7] = "Org-02";	// Pipi

BOOL OrgData::InitMusicData(const char *path)
{
	#define READ_LE16(p) ((p[1] << 8) | p[0]); p += 2
	#define READ_LE32(p) ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]); p += 4

	NOTELIST *np;
	int i,j;
	char pass_check[6];
	char ver = 0;
	unsigned short note_num[MAXTRACK];

	const unsigned char *p = FindResource(path, "ORG", NULL);
	if (p == NULL)
		return FALSE;

	memcpy(&pass_check[0], p, 6);
	p += 6;

	if(memcmp(pass_check, pass, 6) == 0)
		ver = 1;
	if(memcmp(pass_check, pass2, 6) == 0)
		ver = 2;

	if(ver == 0)
		return FALSE;

	// 曲の情報を設定 (Set song information)
	info.wait = READ_LE16(p);
	info.line = *p++;
	info.dot = *p++;
	info.repeat_x = READ_LE32(p);
	info.end_x = READ_LE32(p);

	for (i = 0; i < MAXTRACK; i++)
	{
		info.tdata[i].freq = READ_LE16(p);

		info.tdata[i].wave_no = *p++;

		if (ver == 1)
			info.tdata[i].pipi = 0;
		else
			info.tdata[i].pipi = *p;

		++p;

		note_num[i] = READ_LE16(p);
	}

	// 音符のロード (Loading notes)
	for (j = 0; j < MAXTRACK; j++)
	{
		// 最初の音符はfromがNULLとなる (The first note has from as NULL)
		if (note_num[j] == 0)
		{
			info.tdata[j].note_list = NULL;
			continue;
		}

		// リストを作る (Make a list)
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;

		for (i = 1; i < note_num[j]; i++)
		{
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}

		// 最後の音符のtoはNULL (The last note to is NULL)
		np--;
		np->to = NULL;

		// 内容を代入 (Assign content)
		np = info.tdata[j].note_p;	// Ｘ座標 (X coordinate)
		for (i = 0; i < note_num[j]; i++)
		{
			np->x = READ_LE32(p);
			np++;
		}

		np = info.tdata[j].note_p;	// Ｙ座標 (Y coordinate)
		for (i = 0; i < note_num[j]; i++)
		{
			np->y = *p++;
			np++;
		}

		np = info.tdata[j].note_p;	// 長さ (Length)
		for (i = 0; i < note_num[j]; i++)
		{
			np->length = *p++;
			np++;
		}

		np = info.tdata[j].note_p;	// ボリューム (Volume)
		for (i = 0; i < note_num[j]; i++)
		{
			np->volume = *p++;
			np++;
		}

		np = info.tdata[j].note_p;	// パン (Pan)
		for (i = 0; i < note_num[j]; i++)
		{
			np->pan = *p++;
			np++;
		}
	}

	// データを有効に (Enable data)
	for (j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j,info.tdata[j].wave_no, info.tdata[j].pipi);

	// Pixel ripped out some code so he could use PixTone sounds as drums, but he left this dead code
	for (j = MAXMELODY; j < MAXTRACK; j++)
	{
		i = info.tdata[j].wave_no;
		//InitDramObject(dram_name[i], j - MAXMELODY);
	}

	SetPlayPointer(0);	// 頭出し (Cue)

	return TRUE;
}

// 曲情報を取得 (Get song information)
void OrgData::GetMusicInfo(MUSICINFO *mi)
{
	mi->dot = info.dot;
	mi->line = info.line;
	mi->alloc_note = info.alloc_note;
	mi->wait = info.wait;
	mi->repeat_x = info.repeat_x;
	mi->end_x = info.end_x;

	for (int i = 0; i < MAXTRACK; i++)
	{
		mi->tdata[i].freq = info.tdata[i].freq;
		mi->tdata[i].wave_no = info.tdata[i].wave_no;
		mi->tdata[i].pipi = info.tdata[i].pipi;
	}
}

// Play data
long PlayPos;	// Called 'play_p' in the source code release
NOTELIST *np[MAXTRACK];
long now_leng[MAXMELODY];

int Volume = 100;
int TrackVol[MAXTRACK];
BOOL bFadeout = FALSE;

void OrgData::PlayData(void)
{
	int i;

	// Handle fading out
	if (bFadeout && Volume)
		Volume -= 2;
	if (Volume < 0)
		Volume = 0;

	// メロディの再生 (Play melody)
	for (i = 0; i < MAXMELODY; i++)
	{
		if (np[i] != NULL && PlayPos == np[i]->x)
		{
			if (!g_mute[i] && np[i]->y != KEYDUMMY)	// 音が来た。 (The sound has come.)
			{
				PlayOrganObject(np[i]->y, -1, i, info.tdata[i].freq);
				now_leng[i] = np[i]->length;
			}

			if (np[i]->pan != PANDUMMY)
				ChangeOrganPan(np[i]->y, np[i]->pan, i);
			if (np[i]->volume != VOLDUMMY)
				TrackVol[i] = np[i]->volume;

			np[i] = np[i]->to;	// 次の音符を指す (Points to the next note)
		}

		if (now_leng[i] == 0)
			PlayOrganObject(0, 2, i, info.tdata[i].freq);

		if (now_leng[i] > 0)
			now_leng[i]--;

		if (np[i])
			ChangeOrganVolume(np[i]->y, TrackVol[i] * Volume / 0x7F, i);
	}

	// ドラムの再生 (Drum playback)
	for (i = MAXMELODY; i < MAXTRACK; i++)
	{
		if (np[i] != NULL && PlayPos == np[i]->x)	// 音が来た。 (The sound has come.)
		{
			if (np[i]->y != KEYDUMMY && !g_mute[i])	// ならす (Tame)
				PlayDramObject(np[i]->y, 1, i - MAXMELODY);

			if (np[i]->pan != PANDUMMY)
				ChangeDramPan(np[i]->pan, i - MAXMELODY);
			if (np[i]->volume != VOLDUMMY)
				TrackVol[i] = np[i]->volume;

			np[i] = np[i]->to;	// 次の音符を指す (Points to the next note)
		}

		if (np[i])
			ChangeDramVolume(TrackVol[i] * Volume / 0x7F, i - MAXMELODY);
	}

	// Looping
	PlayPos++;
	if (PlayPos >= info.end_x)
	{
		PlayPos = info.repeat_x;
		SetPlayPointer(PlayPos);
	}
}

void OrgData::SetPlayPointer(long x)
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		np[i] = info.tdata[i].note_list;
		while (np[i] != NULL && np[i]->x < x)
			np[i] = np[i]->to;	// 見るべき音符を設定 (Set note to watch)
	}

	PlayPos = x;
}

// Start and end organya
BOOL StartOrganya(const char *path_wave)	// The argument is ignored for some reason
{
	if (!audio_backend_initialised)
		return FALSE;

	if (!InitWaveData100())
		return FALSE;

	org_data.InitOrgData();

	AudioBackend_SetOrganyaCallback(OrganyaCallback);

	return TRUE;
}

// Load organya file
BOOL LoadOrganya(const char *name)
{
	if (!audio_backend_initialised)
		return FALSE;

	if (!org_data.InitMusicData(name))
		return FALSE;

	Volume = 100;
	bFadeout = 0;

#ifdef FIX_BUGS
	return TRUE;
#else
	return FALSE;	// Err... isn't this meant to be 'TRUE'?
#endif
}

void SetOrganyaPosition(unsigned int x)
{
	if (!audio_backend_initialised)
		return;

	org_data.SetPlayPointer(x);
	Volume = 100;
	bFadeout = FALSE;
}

unsigned int GetOrganyaPosition(void)
{
	if (!audio_backend_initialised)
		return 0;

	return PlayPos;
}

void PlayOrganyaMusic(void)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetOrganyaTimer(org_data.info.wait);
}

BOOL ChangeOrganyaVolume(signed int volume)
{
	if (!audio_backend_initialised)
		return FALSE;

	if (volume < 0 || volume > 100)
		return FALSE;

	Volume = volume;
	return TRUE;
}

void StopOrganyaMusic(void)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetOrganyaTimer(0);

	// Stop notes
	for (int i = 0; i < MAXMELODY; i++)
		PlayOrganObject(0, 2, i, 0);

	memset(old_key, 255, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));

	// Put the main thread to sleep for 100 milliseconds... but why?
	// Really, what's the point? All this does is cause an annoying
	// stutter when a new song loads.
	// I'd guess it avoids a race-condition with the Organya thread,
	// but the earlier QuitMMTimer call already disables it.
	Backend_Delay(100);
}

void SetOrganyaFadeout(void)
{
	bFadeout = TRUE;
}

void EndOrganya(void)
{
	if (!audio_backend_initialised)
		return;

	AudioBackend_SetOrganyaTimer(0);

	// Release everything related to org
	org_data.ReleaseNote();

	for (int i = 0; i < MAXMELODY; i++)
	{
		PlayOrganObject(0, 0, i, 0);
		ReleaseOrganyaObject(i);
	}
}
