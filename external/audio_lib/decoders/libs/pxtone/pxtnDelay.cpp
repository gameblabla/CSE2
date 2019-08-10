
#include "./pxtn.h"

#include "./pxtnMax.h"
#include "./pxtnMem.h"

#include "./pxtnDelay.h"

pxtnDelay::pxtnDelay()
{
	_b_played = true; 
	_unit     = DELAYUNIT_Beat;    
	_group    =    0;
	_rate     = 33.0;
	_freq     =  3.f;
	_smp_num  =    0;  
	_offset   =    0;  
	_rate_s32 =  100;

	memset( _bufs, 0, sizeof(_bufs) );
}

pxtnDelay::~pxtnDelay()
{
	Tone_Release();
}

DELAYUNIT pxtnDelay::get_unit ()const { return _unit ; }
int32_t   pxtnDelay::get_group()const { return _group; }
float     pxtnDelay::get_rate ()const { return _rate ; }
float     pxtnDelay::get_freq ()const { return _freq ; }

void      pxtnDelay::Set( DELAYUNIT unit, float freq, float rate, int32_t group )
{
	_unit  = unit ;
	_group = group;
	_rate  = rate ;
	_freq  = freq ;
}

bool pxtnDelay::get_played()const{ return _b_played; }
void pxtnDelay::set_played( bool b ){ _b_played = b; }
bool pxtnDelay::switch_played(){ _b_played = _b_played ? false : true; return _b_played; }



void pxtnDelay::Tone_Release()
{
	for( int32_t i = 0; i < pxtnMAX_CHANNEL; i ++ ) pxtnMem_free( (void**)&_bufs[ i ] );
	_smp_num = 0;
}

pxtnERR pxtnDelay::Tone_Ready( int32_t beat_num, float beat_tempo, int32_t sps )
{
	Tone_Release();

	pxtnERR res = pxtnERR_VOID;

	if( _freq && _rate )
	{
		_offset   = 0;
		_rate_s32 = (int32_t)_rate; // /100;

		switch( _unit )
		{
		case DELAYUNIT_Beat  : _smp_num = (int32_t)( sps * 60            / beat_tempo / _freq ); break;
		case DELAYUNIT_Meas  : _smp_num = (int32_t)( sps * 60 * beat_num / beat_tempo / _freq ); break;
		case DELAYUNIT_Second: _smp_num = (int32_t)( sps                              / _freq ); break;
		}

		for( int32_t c = 0; c < pxtnMAX_CHANNEL; c++ )
		{
			if( !pxtnMem_zero_alloc( (void**)&_bufs[ c ], _smp_num * sizeof(int32_t) ) ){ res = pxtnERR_memory; goto term; }
		}
	}

	res = pxtnOK;
term:

	if( res != pxtnOK ) Tone_Release();

	return res;
}

void pxtnDelay::Tone_Supple( int32_t ch, int32_t *group_smps )
{
	if( !_smp_num ) return;
	int32_t a = _bufs[ ch ][ _offset ] * _rate_s32/ 100;
	if( _b_played ) group_smps[ _group ] += a;
	_bufs[ ch ][ _offset ] =  group_smps[ _group ];
}

void pxtnDelay::Tone_Increment()
{
	if( !_smp_num ) return;
	if( ++_offset >= _smp_num ) _offset = 0;
}

void  pxtnDelay::Tone_Clear()
{
	if( !_smp_num ) return;
	int32_t def = 0; // ..
	for( int32_t i = 0; i < pxtnMAX_CHANNEL; i ++ ) memset( _bufs[ i ], def, _smp_num * sizeof(int32_t) );
}


// (12byte) =================
typedef struct
{
	uint16_t unit ;
	uint16_t group;
	float    rate ;
	float    freq ;
}
_DELAYSTRUCT;

bool pxtnDelay::Write( pxtnDescriptor *p_doc ) const
{
	_DELAYSTRUCT    dela;
	int32_t            size;

	memset( &dela, 0, sizeof( _DELAYSTRUCT ) );
	dela.unit  = (uint16_t)_unit ;
	dela.group = (uint16_t)_group;
	dela.rate  = _rate;
	dela.freq  = _freq;

	// dela ----------
	size = sizeof( _DELAYSTRUCT );
	if( !p_doc->w_asfile( &size, sizeof(int32_t), 1 ) ) return false;
	if( !p_doc->w_asfile( &dela, size,            1 ) ) return false;

	return true;
}

pxtnERR pxtnDelay::Read( pxtnDescriptor *p_doc )
{
	_DELAYSTRUCT dela = {0};
	int32_t      size =  0 ;

	if( !p_doc->r( &size, 4,                    1 ) ) return pxtnERR_desc_r     ;
	if( !p_doc->r( &dela, sizeof(_DELAYSTRUCT), 1 ) ) return pxtnERR_desc_r     ;
	if( dela.unit >= DELAYUNIT_num                  ) return pxtnERR_fmt_unknown;

	_unit  = (DELAYUNIT)dela.unit;
	_freq  = dela.freq ;
	_rate  = dela.rate ;
	_group = dela.group;

	if( _group >= pxtnMAX_TUNEGROUPNUM ) _group = 0;

	return pxtnOK;
}
