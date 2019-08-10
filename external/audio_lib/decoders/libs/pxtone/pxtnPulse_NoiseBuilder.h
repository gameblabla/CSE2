#ifndef pxtnPulse_NoiseBuilder_H
#define pxtnPulse_NoiseBuilder_H

#include "./pxtn.h"

#include "./pxtnPulse_Noise.h"

class pxtnPulse_NoiseBuilder
{
private:
	void operator =       (const pxtnPulse_NoiseBuilder& src){}
	pxtnPulse_NoiseBuilder(const pxtnPulse_NoiseBuilder& src){}

	bool    _b_init;
	short*  _p_tables[ pxWAVETYPE_num ];
	int32_t _rand_buf [ 2 ];

	void  _random_reset();
	short _random_get  ();

	pxtnPulse_Frequency* _freq;

public :

	 pxtnPulse_NoiseBuilder();
	~pxtnPulse_NoiseBuilder();

	bool Init();

	pxtnPulse_PCM *BuildNoise( pxtnPulse_Noise *p_noise, int32_t ch, int32_t sps, int32_t bps ) const;
};

#endif
