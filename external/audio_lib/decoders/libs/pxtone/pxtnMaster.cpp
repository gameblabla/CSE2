// '12/03/03

#include "./pxtn.h"

#include "./pxtnMaster.h"
#include "./pxtnEvelist.h"

pxtnMaster::pxtnMaster()
{
	Reset();
}

pxtnMaster::~pxtnMaster()
{
}

void pxtnMaster::Reset()
{
	_beat_num    = EVENTDEFAULT_BEATNUM  ;
	_beat_tempo  = EVENTDEFAULT_BEATTEMPO;
	_beat_clock  = EVENTDEFAULT_BEATCLOCK;
	_meas_num    = 1;
	_repeat_meas = 0;
	_last_meas   = 0;
}

void  pxtnMaster::Set( int32_t    beat_num, float    beat_tempo, int32_t    beat_clock )
{
	_beat_num   = beat_num;
	_beat_tempo = beat_tempo;
	_beat_clock = beat_clock;
}

void  pxtnMaster::Get( int32_t *p_beat_num, float *p_beat_tempo, int32_t *p_beat_clock, int32_t* p_meas_num ) const
{
	if( p_beat_num   ) *p_beat_num   = _beat_num  ;
	if( p_beat_tempo ) *p_beat_tempo = _beat_tempo;
	if( p_beat_clock ) *p_beat_clock = _beat_clock;
	if( p_meas_num   ) *p_meas_num   = _meas_num  ;
}

int32_t   pxtnMaster::get_beat_num   ()const{ return _beat_num   ;}
float     pxtnMaster::get_beat_tempo ()const{ return _beat_tempo ;}
int32_t   pxtnMaster::get_beat_clock ()const{ return _beat_clock ;}
int32_t   pxtnMaster::get_meas_num   ()const{ return _meas_num   ;}
int32_t   pxtnMaster::get_repeat_meas()const{ return _repeat_meas;}
int32_t   pxtnMaster::get_last_meas  ()const{ return _last_meas  ;}

int32_t   pxtnMaster::get_last_clock ()const
{
	return _last_meas * _beat_clock * _beat_num;
}

int32_t   pxtnMaster::get_play_meas  ()const
{
	if( _last_meas ) return _last_meas;
	return _meas_num;
}

int32_t  pxtnMaster::get_this_clock( int32_t meas, int32_t beat, int32_t clock ) const
{
	return _beat_num * _beat_clock * meas + _beat_clock * beat + clock;
}

void pxtnMaster::AdjustMeasNum( int32_t clock )
{
	int32_t m_num;
	int32_t b_num;

	b_num   = ( clock + _beat_clock  - 1 ) / _beat_clock;
	m_num   = ( b_num + _beat_num    - 1 ) / _beat_num;
	if( _meas_num    <= m_num       ) _meas_num    = m_num;
	if( _repeat_meas >= _meas_num   ) _repeat_meas = 0;
	if( _last_meas   >  _meas_num   ) _last_meas   = _meas_num;
}

void pxtnMaster::set_meas_num( int32_t meas_num )
{
	if( meas_num < 1                ) meas_num = 1;
	if( meas_num <= _repeat_meas    ) meas_num = _repeat_meas + 1;
	if( meas_num <  _last_meas      ) meas_num = _last_meas;
	_meas_num = meas_num;
}

void  pxtnMaster::set_repeat_meas( int32_t meas ){ if( meas < 0 ) meas = 0; _repeat_meas = meas; }
void  pxtnMaster::set_last_meas  ( int32_t meas ){ if( meas < 0 ) meas = 0; _last_meas   = meas; }

void  pxtnMaster::set_beat_clock ( int32_t beat_clock ){ if( beat_clock < 0 ) beat_clock = 0; _beat_clock = beat_clock; }





bool pxtnMaster::io_w_v5( pxtnDescriptor *p_doc, int32_t rough ) const
{

	uint32_t   size   =          15;
	int16_t   bclock = _beat_clock / rough;
	int32_t   clock_repeat = bclock * _beat_num * get_repeat_meas();
	int32_t   clock_last   = bclock * _beat_num * get_last_meas  ();
	int8_t    bnum   = _beat_num  ;
	float btempo = _beat_tempo;
	if( !p_doc->w_asfile( &size        , sizeof(uint32_t  ), 1 ) ) return false;
	if( !p_doc->w_asfile( &bclock      , sizeof(int16_t  ), 1 ) ) return false;
	if( !p_doc->w_asfile( &bnum        , sizeof(int8_t   ), 1 ) ) return false;
	if( !p_doc->w_asfile( &btempo      , sizeof(float), 1 ) ) return false;
	if( !p_doc->w_asfile( &clock_repeat, sizeof(int32_t  ), 1 ) ) return false;
	if( !p_doc->w_asfile( &clock_last  , sizeof(int32_t  ), 1 ) ) return false;

	return true;
}

pxtnERR pxtnMaster::io_r_v5( pxtnDescriptor *p_doc )
{
	pxtnERR   res = pxtnERR_VOID;
	int16_t   beat_clock   = 0;
	int8_t    beat_num     = 0;
	float     beat_tempo   = 0;
	int32_t   clock_repeat = 0;
	int32_t   clock_last   = 0;

	uint32_t  size         = 0;

	if( !p_doc->r( &size, sizeof(uint32_t), 1 ) ) return pxtnERR_desc_r;
	if( size != 15                              ) return pxtnERR_fmt_unknown;

	if( !p_doc->r( &beat_clock  ,sizeof(int16_t), 1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &beat_num    ,sizeof(int8_t ), 1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &beat_tempo  ,sizeof(float  ), 1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &clock_repeat,sizeof(int32_t), 1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &clock_last  ,sizeof(int32_t), 1 ) ) return pxtnERR_desc_r;

	_beat_clock = beat_clock;
	_beat_num   = beat_num  ;
	_beat_tempo = beat_tempo;

	set_repeat_meas( clock_repeat / ( beat_num * beat_clock ) );
	set_last_meas  ( clock_last   / ( beat_num * beat_clock ) );

	return pxtnOK;
}

int32_t pxtnMaster::io_r_v5_EventNum( pxtnDescriptor *p_doc )
{
	uint32_t size;
	if( !p_doc->r( &size, sizeof(uint32_t),  1 ) ) return 0;
	if( size != 15                               ) return 0;
	int8_t buf[ 15 ];
	if( !p_doc->r(  buf , sizeof(int8_t ), 15 )  ) return 0;
	return 5;
}

/////////////////////////////////
// file io
/////////////////////////////////

// master info(8byte) ================
typedef struct
{
	uint16_t data_num ;        // data-num is 3 ( clock / status / volume ）
	uint16_t rrr      ;
	uint32_t event_num;
}
_x4x_MASTER;

// read( project )
pxtnERR pxtnMaster::io_r_x4x( pxtnDescriptor *p_doc )
{
	_x4x_MASTER mast     ={0};
	int32_t     size     = 0;
	int32_t     e        = 0;
	int32_t     status   = 0;
	int32_t     clock    = 0;
	int32_t     volume   = 0;
	int32_t     absolute = 0;

	int32_t     beat_clock, beat_num, repeat_clock, last_clock;
	float       beat_tempo = 0;

	if( !p_doc->r( &size,                     4, 1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &mast, sizeof( _x4x_MASTER ), 1 ) ) return pxtnERR_desc_r;

	// unknown format
	if( mast.data_num != 3 ) return pxtnERR_fmt_unknown;
	if( mast.rrr           ) return pxtnERR_fmt_unknown;

	beat_clock   = EVENTDEFAULT_BEATCLOCK;
	beat_num     = EVENTDEFAULT_BEATNUM;
	beat_tempo   = EVENTDEFAULT_BEATTEMPO;
	repeat_clock = 0;
	last_clock   = 0;

	absolute = 0;

	for( e = 0; e < (int32_t)mast.event_num; e++ )
	{
		if( !p_doc->v_r( &status ) ) break;
		if( !p_doc->v_r( &clock  ) ) break;
		if( !p_doc->v_r( &volume ) ) break;
		absolute += clock;
		clock     = absolute;

		switch( status )
		{
		case EVENTKIND_BEATCLOCK: beat_clock   =  volume;                        if( clock  ) return pxtnERR_desc_broken; break;
		case EVENTKIND_BEATTEMPO: memcpy( &beat_tempo, &volume, sizeof(float) ); if( clock  ) return pxtnERR_desc_broken; break;
		case EVENTKIND_BEATNUM  : beat_num     =  volume;                        if( clock  ) return pxtnERR_desc_broken; break;
		case EVENTKIND_REPEAT   : repeat_clock =  clock ;                        if( volume ) return pxtnERR_desc_broken; break;
		case EVENTKIND_LAST     : last_clock   =  clock ;                        if( volume ) return pxtnERR_desc_broken; break;
		default: return pxtnERR_fmt_unknown;
		}
	}

	if( e != mast.event_num ) return pxtnERR_desc_broken;

	_beat_num   = beat_num  ;
	_beat_tempo = beat_tempo;
	_beat_clock = beat_clock;

	set_repeat_meas( repeat_clock / ( beat_num * beat_clock ) );
	set_last_meas  ( last_clock   / ( beat_num * beat_clock ) );

	return pxtnOK;
}

int32_t pxtnMaster::io_r_x4x_EventNum( pxtnDescriptor *p_doc )
{
	_x4x_MASTER mast;
	int32_t     size;
	int32_t     work;
	int32_t     e   ;

	memset( &mast, 0, sizeof( _x4x_MASTER ) );
	if( !p_doc->r( &size,                     4, 1 ) ) return 0;
	if( !p_doc->r( &mast, sizeof( _x4x_MASTER ), 1 ) ) return 0;

	if( mast.data_num != 3 ) return 0;

	for( e = 0; e < (int32_t)mast.event_num; e++ )
	{
		if( !p_doc->v_r( &work ) ) return 0;
		if( !p_doc->v_r( &work ) ) return 0;
		if( !p_doc->v_r( &work ) ) return 0;
	}

	return mast.event_num;
}
