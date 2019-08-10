
#include "./pxtn.h"

#include "./pxtnMem.h"
#include "./pxtnPulse_Noise.h"


void pxtnPulse_Noise::set_smp_num_44k( int32_t num )
{
	_smp_num_44k = num;
}

int32_t pxtnPulse_Noise::get_unit_num() const
{
	return _unit_num;
}

int32_t pxtnPulse_Noise::get_smp_num_44k() const
{
	return _smp_num_44k;
}

float pxtnPulse_Noise::get_sec() const
{
	return (float)_smp_num_44k / 44100;
}


pxNOISEDESIGN_UNIT *pxtnPulse_Noise::get_unit( int32_t u )
{
	if( !_units || u < 0 || u >= _unit_num ) return NULL;
	return &_units[ u ];
}


pxtnPulse_Noise::pxtnPulse_Noise()
{
	_units       = NULL;
	_unit_num    =    0;
	_smp_num_44k =    0;
}

pxtnPulse_Noise::~pxtnPulse_Noise()
{
	Release();
}

#define NOISEDESIGNLIMIT_SMPNUM (48000 * 10)
#define NOISEDESIGNLIMIT_ENVE_X ( 1000 * 10)
#define NOISEDESIGNLIMIT_ENVE_Y (  100     )
#define NOISEDESIGNLIMIT_OSC_FREQUENCY 44100.0f
#define NOISEDESIGNLIMIT_OSC_VOLUME      200.0f
#define NOISEDESIGNLIMIT_OSC_OFFSET      100.0f

static void _FixUnit( pxNOISEDESIGN_OSCILLATOR *p_osc )
{
	if( p_osc->type   >= pxWAVETYPE_num                 ) p_osc->type   = pxWAVETYPE_None;
	if( p_osc->freq   >  NOISEDESIGNLIMIT_OSC_FREQUENCY ) p_osc->freq   = NOISEDESIGNLIMIT_OSC_FREQUENCY;
	if( p_osc->freq   <= 0                              ) p_osc->freq   = 0;
	if( p_osc->volume >  NOISEDESIGNLIMIT_OSC_VOLUME    ) p_osc->volume = NOISEDESIGNLIMIT_OSC_VOLUME;
	if( p_osc->volume <= 0                              ) p_osc->volume = 0;
	if( p_osc->offset >  NOISEDESIGNLIMIT_OSC_OFFSET    ) p_osc->offset = NOISEDESIGNLIMIT_OSC_OFFSET;
	if( p_osc->offset <= 0                              ) p_osc->offset = 0;
}

void pxtnPulse_Noise::Fix()
{
	pxNOISEDESIGN_UNIT *p_unit;
	int32_t i, e;

	if( _smp_num_44k > NOISEDESIGNLIMIT_SMPNUM ) _smp_num_44k = NOISEDESIGNLIMIT_SMPNUM;

	for( i = 0; i < _unit_num; i++ )
	{
		p_unit = &_units[ i ];
		if( p_unit->bEnable )
		{
			for( e = 0; e < p_unit->enve_num; e++ )
			{
				if( p_unit->enves[ e ].x > NOISEDESIGNLIMIT_ENVE_X ) p_unit->enves[ e ].x = NOISEDESIGNLIMIT_ENVE_X;
				if( p_unit->enves[ e ].x <                       0 ) p_unit->enves[ e ].x =                       0;
				if( p_unit->enves[ e ].y > NOISEDESIGNLIMIT_ENVE_Y ) p_unit->enves[ e ].y = NOISEDESIGNLIMIT_ENVE_Y;
				if( p_unit->enves[ e ].y <                       0 ) p_unit->enves[ e ].y =                       0;
			}
			if( p_unit->pan < -100 ) p_unit->pan = -100;
			if( p_unit->pan >  100 ) p_unit->pan =  100;
			_FixUnit( &p_unit->main );
			_FixUnit( &p_unit->freq );
			_FixUnit( &p_unit->volu );
		}
	}
}

#define MAX_NOISEEDITUNITNUM     4
#define MAX_NOISEEDITENVELOPENUM 3

#define NOISEEDITFLAG_XX1       0x0001
#define NOISEEDITFLAG_XX2       0x0002
#define NOISEEDITFLAG_ENVELOPE  0x0004
#define NOISEEDITFLAG_PAN       0x0008
#define NOISEEDITFLAG_OSC_MAIN  0x0010
#define NOISEEDITFLAG_OSC_FREQ  0x0020
#define NOISEEDITFLAG_OSC_VOLU  0x0040
#define NOISEEDITFLAG_OSC_PAN   0x0080

#define NOISEEDITFLAG_UNCOVERED 0xffffff83

//                          01234567
static const char *_code = "PTNOISE-";
//                    _ver =  20051028 ; -v.0.9.2.3
static const uint32_t _ver =  20120418 ; // 16 wave types.

static bool _WriteOscillator( const pxNOISEDESIGN_OSCILLATOR *p_osc, pxtnDescriptor *p_doc, int32_t *p_add )
{
	int32_t work;
	work = (int32_t) p_osc->type        ; if( !p_doc->v_w_asfile( work, p_add ) ) return false;
	work = (int32_t) p_osc->b_rev       ; if( !p_doc->v_w_asfile( work, p_add ) ) return false;
	work = (int32_t)(p_osc->freq   * 10); if( !p_doc->v_w_asfile( work, p_add ) ) return false;
	work = (int32_t)(p_osc->volume * 10); if( !p_doc->v_w_asfile( work, p_add ) ) return false;
	work = (int32_t)(p_osc->offset * 10); if( !p_doc->v_w_asfile( work, p_add ) ) return false;
	return true;
}

static pxtnERR _ReadOscillator( pxNOISEDESIGN_OSCILLATOR *p_osc, pxtnDescriptor *p_doc )
{
	int32_t work;
	if( !p_doc->v_r( &work )          ) return pxtnERR_desc_r     ; p_osc->type     = (pxWAVETYPE)work;
	if( p_osc->type >= pxWAVETYPE_num ) return pxtnERR_fmt_unknown;
	if( !p_doc->v_r( &work )          ) return pxtnERR_desc_r     ; p_osc->b_rev    = work ? true : false;
	if( !p_doc->v_r( &work )          ) return pxtnERR_desc_r     ; p_osc->freq     = (float)work / 10;
	if( !p_doc->v_r( &work )          ) return pxtnERR_desc_r     ; p_osc->volume   = (float)work / 10;
	if( !p_doc->v_r( &work )          ) return pxtnERR_desc_r     ; p_osc->offset   = (float)work / 10;

	return pxtnOK;
}

static uint32_t _MakeFlags( const pxNOISEDESIGN_UNIT *pU )
{
	uint32_t flags = 0;
	flags |= NOISEEDITFLAG_ENVELOPE;
	if( pU->pan                          ) flags |= NOISEEDITFLAG_PAN     ;
	if( pU->main.type != pxWAVETYPE_None ) flags |= NOISEEDITFLAG_OSC_MAIN;
	if( pU->freq.type != pxWAVETYPE_None ) flags |= NOISEEDITFLAG_OSC_FREQ;
	if( pU->volu.type != pxWAVETYPE_None ) flags |= NOISEEDITFLAG_OSC_VOLU;
	return flags;
}

bool pxtnPulse_Noise::write( pxtnDescriptor *p_doc, int32_t *p_add ) const
{
	bool  b_ret = false;
	int32_t   u, e, seek, num_seek, flags;
	char  byte;
	char  unit_num = 0;
	const pxNOISEDESIGN_UNIT *pU;

//	Fix();

	if( p_add ) seek = *p_add;
	else        seek =      0;

	if( !p_doc->w_asfile( _code, 1, 8 ) ) goto End;
	if( !p_doc->w_asfile( &_ver, 4, 1 ) ) goto End;
	seek += 12;
	if( !p_doc->v_w_asfile( _smp_num_44k, &seek ) ) goto End;

	if( !p_doc->w_asfile( &unit_num , 1, 1 ) ) goto End;
	num_seek = seek;
	seek += 1;

	for( u = 0; u < _unit_num; u++ )
	{
		pU = &_units[ u ];
		if( pU->bEnable )
		{
			// フラグ
			flags = _MakeFlags( pU );
			if( !p_doc->v_w_asfile( flags, &seek ) ) goto End;
			if( flags & NOISEEDITFLAG_ENVELOPE )
			{
				if( !p_doc->v_w_asfile( pU->enve_num, &seek ) ) goto End;
				for( e = 0; e < pU->enve_num; e++ )
				{
					if( !p_doc->v_w_asfile( pU->enves[ e ].x, &seek ) ) goto End;
					if( !p_doc->v_w_asfile( pU->enves[ e ].y, &seek ) ) goto End;
				}
			}
			if( flags & NOISEEDITFLAG_PAN      )
			{
				byte = (char)pU->pan;
				if( !p_doc->w_asfile( &byte, 1, 1 ) ) goto End;
				seek++;
			}
			if( flags & NOISEEDITFLAG_OSC_MAIN ){ if( !_WriteOscillator( &pU->main, p_doc, &seek ) ) goto End; }
			if( flags & NOISEEDITFLAG_OSC_FREQ ){ if( !_WriteOscillator( &pU->freq, p_doc, &seek ) ) goto End; }
			if( flags & NOISEEDITFLAG_OSC_VOLU ){ if( !_WriteOscillator( &pU->volu, p_doc, &seek ) ) goto End; }
			unit_num++;
		}
	}

	// update unit_num.
	p_doc->seek( pxtnSEEK_cur, num_seek - seek );
	if( !p_doc->w_asfile( &unit_num, 1, 1 ) ) goto End;
	p_doc->seek( pxtnSEEK_cur, seek - num_seek -1 );
	if( p_add ) *p_add = seek;

	b_ret = true;
End:

	return b_ret;
}

pxtnERR pxtnPulse_Noise::read( pxtnDescriptor *p_doc )
{
	pxtnERR  res       = pxtnERR_VOID;
	uint32_t flags     =            0;
	char     unit_num  =            0;
	char     byte      =            0;
	uint32_t ver       =            0;

	pxNOISEDESIGN_UNIT* pU = NULL;

	char       code[ 8 ] = {0};

	Release();
	
	if( !p_doc->r( code, 1, 8 )         ){ res = pxtnERR_desc_r     ; goto term; }
	if( memcmp( code, _code, 8 )        ){ res = pxtnERR_inv_code   ; goto term; }
	if( !p_doc->r( &ver     , 4, 1 )    ){ res = pxtnERR_desc_r     ; goto term; }	
	if( ver > _ver                      ){ res = pxtnERR_fmt_new    ; goto term; }	
	if( !p_doc->v_r( &_smp_num_44k )    ){ res = pxtnERR_desc_r     ; goto term; }
	if( !p_doc->r( &unit_num, 1, 1 )    ){ res = pxtnERR_desc_r     ; goto term; }
	if( unit_num < 0                    ){ res = pxtnERR_inv_data   ; goto term; }
	if( unit_num > MAX_NOISEEDITUNITNUM ){ res = pxtnERR_fmt_unknown; goto term; }
	_unit_num = unit_num;			    

	if( !pxtnMem_zero_alloc( (void**)&_units, sizeof(pxNOISEDESIGN_UNIT) * _unit_num ) ){ res = pxtnERR_memory; goto term; }

	for( int32_t u = 0; u < _unit_num; u++ )
	{
		pU = &_units[ u ];
		pU->bEnable = true;

		if( !p_doc->v_r( (int32_t*)&flags ) ){ res = pxtnERR_desc_r     ; goto term; }
		if( flags & NOISEEDITFLAG_UNCOVERED ){ res = pxtnERR_fmt_unknown; goto term; }

		// envelope
		if( flags & NOISEEDITFLAG_ENVELOPE )
		{
			if( !p_doc->v_r( &pU->enve_num ) ){ res = pxtnERR_desc_r; goto term; }
			if( pU->enve_num > MAX_NOISEEDITENVELOPENUM ){ res = pxtnERR_fmt_unknown; goto term; }
			if( !pxtnMem_zero_alloc( (void**)&pU->enves, sizeof(pxtnPOINT) * pU->enve_num ) ){ res = pxtnERR_memory; goto term; }
			for( int32_t e = 0; e < pU->enve_num; e++ )
			{
				if( !p_doc->v_r( &pU->enves[ e ].x ) ){ res = pxtnERR_desc_r; goto term; }
				if( !p_doc->v_r( &pU->enves[ e ].y ) ){ res = pxtnERR_desc_r; goto term; }
			}
		}
		// pan
		if( flags & NOISEEDITFLAG_PAN )
		{
			if( !p_doc->r( &byte, 1, 1 ) ){ res = pxtnERR_desc_r; goto term; }
			pU->pan = byte;
		}
		
		if( flags & NOISEEDITFLAG_OSC_MAIN ){ res = _ReadOscillator( &pU->main, p_doc ); if( res != pxtnOK ) goto term; }
		if( flags & NOISEEDITFLAG_OSC_FREQ ){ res = _ReadOscillator( &pU->freq, p_doc ); if( res != pxtnOK ) goto term; }
		if( flags & NOISEEDITFLAG_OSC_VOLU ){ res = _ReadOscillator( &pU->volu, p_doc ); if( res != pxtnOK ) goto term; }
	}

	res = pxtnOK;
term:
	if( res != pxtnOK ) Release();

	return res;
}

void pxtnPulse_Noise::Release()
{
	if( _units )
	{
		for( int32_t u = 0; u < _unit_num; u++ )
		{
			if( _units[ u ].enves ) pxtnMem_free( (void**)&_units[ u ].enves );
		}
		pxtnMem_free( (void**)&_units );
		_unit_num = 0;
	}
}

bool pxtnPulse_Noise::Allocate( int32_t unit_num, int32_t envelope_num )
{
	bool b_ret = false;

	Release();

	_unit_num = unit_num;
	if( !pxtnMem_zero_alloc( (void**)&_units, sizeof(pxNOISEDESIGN_UNIT) * unit_num ) ) goto End;

	for( int32_t u = 0; u < unit_num; u++ )
	{
		pxNOISEDESIGN_UNIT *p_unit = &_units[ u ];
		p_unit->enve_num = envelope_num;
		if( !pxtnMem_zero_alloc( (void**)&p_unit->enves, sizeof(pxtnPOINT) * p_unit->enve_num ) ) goto End;
	}

	b_ret = true;
End:
	if( !b_ret ) Release();

	return b_ret;
}

bool pxtnPulse_Noise::Copy( pxtnPulse_Noise *p_dst ) const
{
	if( !p_dst ) return false;

	bool b_ret = false;

	p_dst->Release();
	p_dst->_smp_num_44k = _smp_num_44k;

	if( _unit_num )
	{
		int32_t enve_num = _units[ 0 ].enve_num;
		if( !p_dst->Allocate( _unit_num, enve_num ) ) goto End;
		for( int32_t u = 0; u < _unit_num; u++ )
		{
			p_dst->_units[ u ].bEnable  = _units[ u ].bEnable ;
			p_dst->_units[ u ].enve_num = _units[ u ].enve_num;
			p_dst->_units[ u ].freq     = _units[ u ].freq    ;
			p_dst->_units[ u ].main     = _units[ u ].main    ;
			p_dst->_units[ u ].pan      = _units[ u ].pan     ;
			p_dst->_units[ u ].volu     = _units[ u ].volu    ;
			if( !( p_dst->_units[ u ].enves = (pxtnPOINT*)malloc( sizeof(pxtnPOINT) * enve_num ) ) ) goto End;
			for( int32_t e = 0; e < enve_num; e++ ) p_dst->_units[ u ].enves[ e ] = _units[ u ].enves[ e ];
		}
	}

	b_ret = true;
End:
	if( !b_ret ) p_dst->Release();

	return b_ret;
}

static int32_t _CompareOsci( const pxNOISEDESIGN_OSCILLATOR *p_osc1, const pxNOISEDESIGN_OSCILLATOR *p_osc2 )
{
	if( p_osc1->type   != p_osc2->type   ) return 1;
	if( p_osc1->freq   != p_osc2->freq   ) return 1;
	if( p_osc1->volume != p_osc2->volume ) return 1;
	if( p_osc1->offset != p_osc2->offset ) return 1;
	if( p_osc1->b_rev  != p_osc2->b_rev  ) return 1;
	return 0;
}

int32_t pxtnPulse_Noise::Compare     ( const pxtnPulse_Noise *p_src ) const
{
	if( !p_src ) return -1;

	if( p_src->_smp_num_44k != _smp_num_44k ) return 1;
	if( p_src->_unit_num    != _unit_num    ) return 1;

	for( int32_t u = 0; u < _unit_num; u++ )
	{
		if( p_src->_units[ u ].bEnable  != _units[ u ].bEnable          ) return 1;
		if( p_src->_units[ u ].enve_num != _units[ u ].enve_num         ) return 1;
		if( p_src->_units[ u ].pan      != _units[ u ].pan              ) return 1;
		if( _CompareOsci( &p_src->_units[ u ].main, &_units[ u ].main ) ) return 1;
		if( _CompareOsci( &p_src->_units[ u ].freq, &_units[ u ].freq ) ) return 1;
		if( _CompareOsci( &p_src->_units[ u ].volu, &_units[ u ].volu ) ) return 1;

		for( int32_t e = 0; e < _units[ u ].enve_num; e++ )
		{
			if( _units[ u ].enves[ e ].x != _units[ u ].enves[ e ].x ) return 1;
			if( _units[ u ].enves[ e ].y != _units[ u ].enves[ e ].y ) return 1;
		}
	}
	return 0;
}
