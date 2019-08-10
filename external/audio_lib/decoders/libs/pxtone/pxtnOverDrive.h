// '12/03/03

#ifndef pxtnOverDrive_H
#define pxtnOverDrive_H

#include "./pxtn.h"

#include "./pxtnDescriptor.h"

#define TUNEOVERDRIVE_CUT_MAX      99.9f
#define TUNEOVERDRIVE_CUT_MIN      50.0f
#define TUNEOVERDRIVE_AMP_MAX       8.0f
#define TUNEOVERDRIVE_AMP_MIN       0.1f
#define TUNEOVERDRIVE_DEFAULT_CUT  90.0f
#define TUNEOVERDRIVE_DEFAULT_AMP   2.0f

class pxtnOverDrive
{
private:
	void operator = (const pxtnOverDrive& src){}
	pxtnOverDrive   (const pxtnOverDrive& src){}

	bool  _b_played;

	int32_t   _group   ;
	float _cut_f   ;
	float _amp_f   ;

	int32_t   _cut_16bit_top;
public :

	 pxtnOverDrive();
	~pxtnOverDrive();

	void Tone_Ready();
	void Tone_Supple( int32_t *group_smps ) const;

	bool    Write( pxtnDescriptor *p_doc ) const;
	pxtnERR Read ( pxtnDescriptor *p_doc );


	float   get_cut  ()const;
	float   get_amp  ()const;
	int32_t get_group()const;

	void    Set( float cut, float amp, int32_t group );

	bool    get_played()const;
	void    set_played( bool b );
	bool    switch_played();
};

#endif
