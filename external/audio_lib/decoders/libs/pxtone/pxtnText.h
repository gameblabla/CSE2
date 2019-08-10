// '12/03/03

#ifndef pxtnText_H
#define pxtnText_H

#include "./pxtn.h"

#include "./pxtnDescriptor.h"

class pxtnText
{
private:
	void operator = (const pxtnText& src){}
	pxtnText        (const pxtnText& src){}

	char*   _p_comment_buf;
	int32_t _comment_size ;

	char*   _p_name_buf   ;
	int32_t _name_size    ;

public :
	 pxtnText();
	~pxtnText();

	bool        set_comment_buf( const char *p_comment, int32_t    buf_size );
	const char* get_comment_buf(                        int32_t* p_buf_size ) const;
	bool        is_comment_buf () const;

	bool        set_name_buf   ( const char *p_name   , int32_t    buf_size );
	const char* get_name_buf   (                        int32_t* p_buf_size ) const;
	bool        is_name_buf    () const;

	bool Comment_r( pxtnDescriptor *p_doc );
	bool Comment_w( pxtnDescriptor *p_doc );
	bool Name_r   ( pxtnDescriptor *p_doc );
	bool Name_w   ( pxtnDescriptor *p_doc );
};

#endif
