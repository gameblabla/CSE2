#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include "pxtnService.h"
#include "pxtnError.h"

static bool _load_ptcop(pxtnService* pxtn, const char* path_src)
{
	bool success = false;

	pxtnDescriptor desc;
	FILE *fp = fopen(path_src, "rb");

	if (fp && desc.set_file_r(fp) && pxtn->read(&desc) == pxtnOK && pxtn->tones_ready() == pxtnOK)
		success = true;
	else
		pxtn->evels->Release();

	if(fp)
		fclose(fp);

	return success;
}

extern "C" pxtnService* PxTone_Open(const char *file_path, bool loop, unsigned int sample_rate, unsigned int channel_count)
{
	pxtnService *pxtn = new pxtnService();
	if (pxtn->init() == pxtnOK)
	{
		if (pxtn->set_destination_quality(channel_count, sample_rate))
		{
			if( _load_ptcop( pxtn, file_path ) )
			{
				pxtnVOMITPREPARATION prep = {};
				if (loop)
					prep.flags |= pxtnVOMITPREPFLAG_loop;
				prep.start_pos_float = 0;
				prep.master_volume = 0.80f;

				if( pxtn->moo_preparation( &prep ) )
				{
					return pxtn;
				}
			}
		}
	}

	delete pxtn;

	return NULL;
}

extern "C" void PxTone_Close(pxtnService *pxtn)
{
	delete pxtn;
}

extern "C" void PxTone_Rewind(pxtnService *pxtn, bool loop)
{
	pxtnVOMITPREPARATION prep = {};
	if (loop)
		prep.flags |= pxtnVOMITPREPFLAG_loop;
	prep.start_pos_float = 0;
	prep.master_volume = 0.80f;

	pxtn->moo_preparation( &prep );
}

extern "C" unsigned long PxTone_GetSamples(pxtnService *pxtn, void *buffer, unsigned long bytes_to_do)
{
	pxtn->Moo(buffer, bytes_to_do);

	return bytes_to_do;
}
