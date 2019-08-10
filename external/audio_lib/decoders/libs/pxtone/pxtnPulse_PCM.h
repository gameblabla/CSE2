#ifndef pxtnPulse_PCM_H
#define pxtnPulse_PCM_H

#include "./pxtn.h"

#include "./pxtnDescriptor.h"

class pxtnPulse_PCM
{
private:
	void operator = (const pxtnPulse_PCM& src){}
	pxtnPulse_PCM   (const pxtnPulse_PCM& src){}

	int32_t _ch      ;
	int32_t _sps     ;
	int32_t _bps     ;
	int32_t _smp_head; // no use. 0
	int32_t _smp_body;
	int32_t _smp_tail; // no use. 0
	uint8_t  *_p_smp  ;

	bool _Convert_ChannelNum     ( int32_t new_ch  );
	bool _Convert_BitPerSample   ( int32_t new_bps );
	bool _Convert_SamplePerSecond( int32_t new_sps );

public:

	 pxtnPulse_PCM();
	~pxtnPulse_PCM();

	pxtnERR Create ( int32_t ch, int32_t sps, int32_t bps, int32_t sample_num );
	void    Release();

	pxtnERR read ( pxtnDescriptor* doc );
	bool    write( pxtnDescriptor* doc, const char* pstrLIST ) const;


	bool    Convert( int32_t  new_ch, int32_t new_sps, int32_t new_bps );
	bool    Convert_Volume( float v );
	pxtnERR Copy   ( pxtnPulse_PCM *p_dst ) const;
	bool    Copy_  ( pxtnPulse_PCM *p_dst, int32_t start, int32_t end ) const;

	void *Devolve_SamplingBuffer();

	float get_sec   () const;

	int32_t get_ch      () const;
	int32_t get_bps     () const;
	int32_t get_sps     () const;
	int32_t get_smp_body() const;
	int32_t get_smp_head() const;
	int32_t get_smp_tail() const;

	int32_t get_buf_size() const;

	const void *get_p_buf         () const;
	void       *get_p_buf_variable() const;

};

#endif





