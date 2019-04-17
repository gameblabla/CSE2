// '11/08/12 pxFile.h 
// '16/01/22 pxFile.h 
// '16/04/27 pxtnFile. (int32_t)
// '16/09/09 pxtnDescriptor.

#ifndef pxtnDescriptor_H
#define pxtnDescriptor_H

#include "./pxtn.h"

#include <stdio.h>

enum pxtnSEEK
{
	pxtnSEEK_set = 0,
	pxtnSEEK_cur,
	pxtnSEEK_end,
	pxtnSEEK_num
};

class pxtnDescriptor
{
private:

	void operator = (const pxtnDescriptor& src){}
	pxtnDescriptor  (const pxtnDescriptor& src){}

	enum
	{
		_BUFSIZE_HEEP = 1024,
		_TAGLINE_NUM  =  128,
	};
	
	void    *_p_desc;
	bool    _b_file ;
	bool    _b_read ;
	int32_t _size   ;
	int32_t _cur    ;
	
public:
	
	pxtnDescriptor();

	bool set_file_r  ( FILE *fp );
	bool set_file_w  ( FILE *fp );
	bool set_memory_r( void *p_mem, int len );
	bool seek        ( pxtnSEEK mode, int val );
	
	bool w_asfile( const void *p, int size, int num );
	bool r       (       void *p, int size, int num );

	int  v_w_asfile( int32_t val, int32_t *p_add );
	bool v_r       ( int32_t *p  );

	int get_size_bytes() const;
};

int  pxtnDescriptor_v_chk ( int val );

#endif
