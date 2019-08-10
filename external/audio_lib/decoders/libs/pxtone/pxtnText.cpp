// '12/03/03

#include "./pxtn.h"

#include "./pxtnText.h"

static bool _read4_malloc( char **pp, int32_t* p_buf_size, pxtnDescriptor *p_doc )
{
	if( !pp ) return false;
	if( !p_doc->r( p_buf_size, 4, 1 ) ) return false;
	if( *p_buf_size < 0 ) return false;

	bool  b_ret  = false;

	if( !( *pp = (char *)malloc( *p_buf_size + 1 ) ) ) return false;

	memset( *pp, 0, *p_buf_size + 1 );

	if( *p_buf_size )
	{
		if( !p_doc->r( *pp, sizeof(char), *p_buf_size ) ) goto term;
	}

	b_ret = true;
term:
	if( !b_ret ){ free( *pp ); *pp = NULL; }

	return b_ret;
}

static bool _write4( const char *p, int32_t buf_size, pxtnDescriptor *p_doc )
{
	if( !p_doc->w_asfile( &buf_size, 4,        1 ) ) return false;
	if( !p_doc->w_asfile(  p,        1, buf_size ) ) return false;
	return true;
}


bool pxtnText::set_name_buf( const char *name, int32_t buf_size )
{
	if( !name    ) return false;
	if( _p_name_buf ) free( _p_name_buf ); _p_name_buf = NULL;
	if( buf_size <= 0 ){ _name_size = 0; return true; }
	if( !(  _p_name_buf = (char *)malloc( buf_size + 1 ) ) ) return false;
	memcpy( _p_name_buf, name   ,         buf_size );
	_p_name_buf[ buf_size ] = '\0';
	_name_size = buf_size;
	return true;
}

bool pxtnText::set_comment_buf( const char *comment, int32_t buf_size )
{
	if( !comment ) return false;
	if( _p_comment_buf ) free( _p_comment_buf ); _p_comment_buf = NULL;
	if( buf_size <= 0 ){ _comment_size = 0; return true; }
	if( !(  _p_comment_buf = (char *)malloc( buf_size + 1 ) ) ) return false;
	memcpy( _p_comment_buf, comment,         buf_size );
	_p_comment_buf[ buf_size ] = '\0';
	_comment_size = buf_size;
	return true;
}

const char* pxtnText::get_name_buf( int32_t* p_buf_size ) const
{
	if( p_buf_size ) *p_buf_size = _name_size;
	return _p_name_buf;
}
const char* pxtnText::get_comment_buf( int32_t* p_buf_size ) const
{
	if( p_buf_size ) *p_buf_size = _comment_size;
	return _p_comment_buf;
}
bool pxtnText::is_name_buf   () const{ if( _name_size    > 0 ) return true; return false; }
bool pxtnText::is_comment_buf() const{ if( _comment_size > 0 ) return true; return false; }


pxtnText::pxtnText()
{
	_p_comment_buf = NULL;
	_p_name_buf    = NULL;
	_comment_size  =    0;
	_name_size     =    0;
}

pxtnText::~pxtnText()
{
	if( _p_comment_buf ) free( _p_comment_buf ); _p_comment_buf = NULL; _comment_size = 0;
	if( _p_name_buf    ) free( _p_name_buf    ); _p_name_buf    = NULL; _name_size    = 0;
}


bool pxtnText::Comment_w( pxtnDescriptor *p_doc )
{
	if( !_p_comment_buf ) return false;
	return _write4( _p_comment_buf, _comment_size, p_doc );
}

bool pxtnText::Name_w( pxtnDescriptor *p_doc )
{
	if( !_p_name_buf ) return false;
	return _write4( _p_name_buf, _name_size, p_doc );
}

bool pxtnText::Comment_r( pxtnDescriptor* p_doc )
{
	return _read4_malloc( &_p_comment_buf, &_comment_size, p_doc );
}

bool pxtnText::Name_r(  pxtnDescriptor* p_doc )
{
	return _read4_malloc( &_p_name_buf, &_name_size, p_doc );
}
