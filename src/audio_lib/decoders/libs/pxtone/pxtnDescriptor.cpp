
#include "./pxtn.h"

#include "./pxtnDescriptor.h"

pxtnDescriptor::pxtnDescriptor()
{
	_p_desc = NULL ;
	_size   =     0;
	_b_file = false;
	_b_read = false;
	_cur    =     0;
}

int pxtnDescriptor::get_size_bytes() const { return _size; }

bool pxtnDescriptor::set_memory_r( void *p_mem, int size )
{
	if( !p_mem || size < 1 ) return false;
	_p_desc = p_mem;
	_size   = size ;
	_b_file = false;
	_b_read = true ;
	_cur    =     0;
	return true;
}

bool pxtnDescriptor::set_file_r  ( FILE *fd )
{
	if( !fd ) return false;

	fpos_t sz;
	if( fseek  ( fd, 0, SEEK_END ) ) return false;
	if( fgetpos( fd, &sz         ) ) return false;
	if( fseek  ( fd, 0, SEEK_SET ) ) return false;
	_p_desc = fd  ;

#ifdef pxSCE
	_size   = (int32_t)sz._Off;
#else
	_size   = (int32_t)sz;
#endif

	_b_file = true;
	_b_read = true;
	_cur    =    0;
	return true;
}

bool pxtnDescriptor::set_file_w  ( FILE *fd )
{
	if( !fd ) return false;

	_p_desc = fd   ;
	_size   =    0 ;
	_b_file = true ;
	_b_read = false;
	_cur    =    0 ;
	return true;
}

bool pxtnDescriptor::seek( pxtnSEEK mode, int val )
{
	if( _b_file )
	{
		int seek_tbl[ pxtnSEEK_num ] = {SEEK_SET, SEEK_CUR, SEEK_END};
		if( fseek( (FILE*)_p_desc, val, seek_tbl[ mode ] ) ) return false;
	}
	else
	{
		switch( mode )
		{
		case pxtnSEEK_set:
			if( val >=         _size ) return false;
			if( val <              0 ) return false;
			_cur = val;
			break;
		case pxtnSEEK_cur:
			if( _cur  + val >= _size ) return false;
			if( _cur  + val <      0 ) return false;
			_cur += val;
			break;
		case pxtnSEEK_end:
			if( _size + val >= _size ) return false;
			if( _size + val <      0 ) return false;
			_cur = _size + val;
			break;
		}
	}
	return true;
}

bool pxtnDescriptor::w_asfile( const void *p, int size, int num )
{
	bool b_ret = false;

	if( !_p_desc || !_b_file || _b_read ) goto End;
	
	if( fwrite( p, size, num, (FILE*)_p_desc ) != num ) goto End;
	_size += size * num;
	
	b_ret = true;
End:
	return b_ret;
}

bool pxtnDescriptor::r(       void *p, int size, int num )
{
	if( !_p_desc ) return false;
	if( !_b_read ) return false;

	bool b_ret = false;

	if( _b_file )
	{
		if( fread( p, size, num, (FILE*)_p_desc ) != num ) goto End;
	}
	else
	{
		for( int  i = 0; i < num; i++ )
		{
			if( _cur + size > _size ) goto End;
			memcpy( &((char*)p)[ i ], (uint8_t*)_p_desc + _cur, size );
			_cur += size;
		}
	}
	
	b_ret = true;
End:
	return b_ret;
}


int  pxtnDescriptor_v_chk( int val )
{
	uint32_t  us;

	us = (uint32_t)val;
	if( us <        0x80 ) return 1;	// 1byte( 7bit)
	if( us <      0x4000 ) return 2;	// 2byte(14bit)
	if( us <    0x200000 ) return 3;	// 3byte(21bit)
	if( us <  0x10000000 ) return 4;	// 4byte(28bit)
//	if( value < 0x800000000 ) return 5;	// 5byte(35bit)
	if( us <= 0xffffffff ) return 5;

	return 6;
}



// ..uint32_t
int  pxtnDescriptor::v_w_asfile( int val, int *p_add )
{
	if( !_p_desc ) return 0;
	if( !_b_file ) return 0;
	if(  _b_read ) return 0;

	uint8_t  a[ 5 ] = {0};
	uint8_t  b[ 5 ] = {0};
	uint32_t us     = (uint32_t )val;
	int32_t  bytes  = 0;
	
	a[ 0 ] = *( (uint8_t *)(&us) + 0 );
	a[ 1 ] = *( (uint8_t *)(&us) + 1 );
	a[ 2 ] = *( (uint8_t *)(&us) + 2 );
	a[ 3 ] = *( (uint8_t *)(&us) + 3 );
	a[ 4 ] = 0;

	// 1byte(7bit)
	if     ( us < 0x00000080 )
	{
		bytes = 1;
		b[0]  = a[0];
	}

	// 2byte(14bit)
	else if( us < 0x00004000 )
	{
		bytes = 2;
		b[0]  =             ((a[0]<<0)&0x7F) | 0x80;
		b[1]  = (a[0]>>7) | ((a[1]<<1)&0x7F);
	}

	// 3byte(21bit)
	else if( us < 0x00200000 )
	{
		bytes = 3;
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F);
	}

	// 4byte(28bit)
	else if( us < 0x10000000 )
	{
		bytes = 4;
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F) | 0x80;
		b[3] = (a[2]>>5) | ((a[3]<<3)&0x7F);
	}

	// 5byte(32bit)
	else
	{
		bytes = 5;
		b[0] =             ((a[0]<<0)&0x7F) | 0x80;
		b[1] = (a[0]>>7) | ((a[1]<<1)&0x7F) | 0x80;
		b[2] = (a[1]>>6) | ((a[2]<<2)&0x7F) | 0x80;
		b[3] = (a[2]>>5) | ((a[3]<<3)&0x7F) | 0x80;
		b[4] = (a[3]>>4) | ((a[4]<<4)&0x7F);
	}
	if( fwrite( b, 1, bytes, (FILE*)_p_desc )     != bytes ) return false;
	if( p_add ) *p_add += bytes;
	_size += bytes;
	return true;

	return false;
}

// 可変長読み込み（int32_t  までを保証）
bool pxtnDescriptor::v_r  ( int32_t *p  )
{
	if( !_p_desc ) return false;
	if( !_b_read ) return false;

	int          i;
	uint8_t a[ 5 ] = {0};
	uint8_t b[ 5 ] = {0};

	for( i = 0; i < 5; i++ )
	{
		if( !pxtnDescriptor::r( &a[i], 1, 1 ) ) return false;
		if( !(a[i] & 0x80) ) break;
	}
	switch( i )
	{
	case 0:
		b[0]    =  (a[0]&0x7F)>>0;
		break;
	case 1:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    =  (a[1]&0x7F)>>1;
		break;
	case 2:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    =  (a[2]&0x7F)>>2;
		break;
	case 3:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    = ((a[2]&0x7F)>>2) | (a[3]<<5);
		b[3]    =  (a[3]&0x7F)>>3;
		break;
	case 4:
		b[0]    = ((a[0]&0x7F)>>0) | (a[1]<<7);
		b[1]    = ((a[1]&0x7F)>>1) | (a[2]<<6);
		b[2]    = ((a[2]&0x7F)>>2) | (a[3]<<5);
		b[3]    = ((a[3]&0x7F)>>3) | (a[4]<<4);
		b[4]    =  (a[4]&0x7F)>>4;
		break;
	case 5:
		return false;
	}

	*p = *((int32_t*)b);

	return true;
}
