// '12/03/03

#include "./pxtn.h"

#include "./pxtnOverDrive.h"

pxtnOverDrive::pxtnOverDrive()
{
	_b_played = true;
}

pxtnOverDrive::~pxtnOverDrive()
{
}

float   pxtnOverDrive::get_cut  ()const{ return _cut_f; }
float   pxtnOverDrive::get_amp  ()const{ return _amp_f; }
int32_t pxtnOverDrive::get_group()const{ return _group; }

void  pxtnOverDrive::Set( float cut, float amp, int32_t group )
{
	_cut_f = cut  ;
	_amp_f = amp  ;
	_group = group;
}

bool pxtnOverDrive::get_played()const{ return _b_played; }
void pxtnOverDrive::set_played( bool b ){ _b_played = b; }
bool pxtnOverDrive::switch_played(){ _b_played = _b_played ? false : true; return _b_played; }

void pxtnOverDrive::Tone_Ready()
{
	_cut_16bit_top  = (int32_t)( 32767 * ( 100 - _cut_f ) / 100 );
}

void pxtnOverDrive::Tone_Supple( int32_t *group_smps ) const
{
	if( !_b_played ) return;
	int32_t work = group_smps[ _group ];
	if(      work >  _cut_16bit_top ) work =   _cut_16bit_top;
	else if( work < -_cut_16bit_top ) work =  -_cut_16bit_top;
	group_smps[ _group ] = (int32_t)( (float)work * _amp_f );
}


// (8byte) =================
typedef struct
{
	uint16_t   xxx  ;
	uint16_t   group;
	float cut  ;
	float amp  ;
	float yyy  ;
}
_OVERDRIVESTRUCT;

bool pxtnOverDrive::Write( pxtnDescriptor *p_doc ) const
{
	_OVERDRIVESTRUCT over;
	int32_t              size;

	memset( &over, 0, sizeof( _OVERDRIVESTRUCT ) );
	over.cut   = _cut_f;
	over.amp   = _amp_f;
	over.group = (uint16_t)_group;

	// dela ----------
	size = sizeof( _OVERDRIVESTRUCT );
	if( !p_doc->w_asfile( &size, sizeof(uint32_t), 1 ) ) return false;
	if( !p_doc->w_asfile( &over, size,        1 ) ) return false;

	return true;
}

pxtnERR pxtnOverDrive::Read( pxtnDescriptor *p_doc )
{
	_OVERDRIVESTRUCT over = {0};
	int32_t          size =  0 ;

	memset( &over, 0, sizeof(_OVERDRIVESTRUCT) );
	if( !p_doc->r( &size, 4,                        1 ) ) return pxtnERR_desc_r;
	if( !p_doc->r( &over, sizeof(_OVERDRIVESTRUCT), 1 ) ) return pxtnERR_desc_r;

	if( over.xxx                         ) return pxtnERR_fmt_unknown;
	if( over.yyy                         ) return pxtnERR_fmt_unknown;
	if( over.cut > TUNEOVERDRIVE_CUT_MAX || over.cut < TUNEOVERDRIVE_CUT_MIN ) return pxtnERR_fmt_unknown;
	if( over.amp > TUNEOVERDRIVE_AMP_MAX || over.amp < TUNEOVERDRIVE_AMP_MIN ) return pxtnERR_fmt_unknown;

	_cut_f = over.cut  ;
	_amp_f = over.amp  ;
	_group = over.group;

	return pxtnOK;
}