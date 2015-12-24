#ifndef __SAMPLING_H_81D7B7F1_2A99_432f_BD26_40CAD75F4859
#define __SAMPLING_H_81D7B7F1_2A99_432f_BD26_40CAD75F4859

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

#define PIXELS(pImage) (pImage)

class Pixel_sampler_abs
{
public:
	Pixel_sampler_abs() : pImage(0) { }
	void setImage(void * _pImage) { pImage = (unsigned char *)_pImage; }
	void * getImage() const       { return pImage; }

    virtual void reset(unsigned _width, unsigned _height) = 0;
    virtual void uniform_sample(unsigned nRow) = 0;
	virtual void center_sample(unsigned nRows) = 0;
	virtual void center_weight_sample(unsigned nRows) = 0;

    unsigned char * pImage;
};


template <class ACCUM_PIXELS>
class Pixel_sampler : public Pixel_sampler_abs, public ACCUM_PIXELS
{
public:
	Pixel_sampler()
		: width(0), height(0), 
		  center_top(0), center_bottom(0), center_left(0), center_right(0)
		{
		}
	void setDim(unsigned _width, unsigned _height)
	{
		if (width == _width && height == _height)
			return;

		(unsigned &)width = _width;
		(unsigned &)height = _height;

		// TODO: Jade code calculates left and right like this:
		// center_left    = (width * 3 >> 3) + 3 & ~3;
		// center_right   = (width * 5 >> 3) + 3 & ~3;


		unsigned nTemp = (width+7)>>3;
		center_left    = (nTemp * 3)&0xfffffffc;
		center_right   = ((nTemp * 5) + 3)&0xfffffffc;
		center_top     = (height * 3)>>3;
		center_bottom  = (height * 5)>>3;
	}

    virtual void reset(unsigned _width, unsigned _height)
		{
			setDim(_width, _height);
			setImage(0);
			ACCUM_PIXELS::reset();
			rows_serviced = 0;
		}

    virtual void uniform_sample(unsigned nRow)
		{
			while (rows_serviced < nRow)
			{
				if (uniform_is_sample_row(rows_serviced))
					uniform_sample_row(rows_serviced);
				++rows_serviced;
			}
		}

	virtual void center_sample(unsigned nRows)
		{
			while (rows_serviced < nRows)
			{
				if (center_is_sample_row(rows_serviced))
					center_sample_row(rows_serviced);
				++rows_serviced;
			}
		}

	virtual void center_weight_sample(unsigned nRows)
		{
			while (rows_serviced < nRows)
			{
				if (uniform_is_sample_row(rows_serviced))
					uniform_sample_row(rows_serviced);
				if (center_is_sample_row(rows_serviced))
					center_sample_row(rows_serviced);
				++rows_serviced;
			}
		}

    unsigned rows_serviced;
	// Declared const so that no one mistakenly gets these out of sync.
	// Must use setDim to set these values.  
    const unsigned width, height;


private:
	bool center_is_sample_row(unsigned nRow)
		{
			return nRow >= center_top && nRow <= center_bottom && (nRow&0xf)==0;
		}

    void center_sample_row(unsigned nRow)
		{
			unsigned char *pCurrentRow;
			pCurrentRow = PIXELS(pImage) + (nRow * width) + center_left;
			unsigned cnt = (center_right - center_left) / 16;
			unsigned char *pEndCurrentRow = pCurrentRow + cnt * 16;
			DWORD pixelGroup;

			while(pCurrentRow < pEndCurrentRow)
			{
				pixelGroup = *((DWORD *)pCurrentRow);
				ACCUM_PIXELS::accumulate(pixelGroup);
				pCurrentRow += 16;
			}
		}



    bool uniform_is_sample_row(unsigned nRow) { return (nRow &0x3f)==0x20; }
    void uniform_sample_row(unsigned nRow)
		{
			unsigned char *pCurrentRow;
			pCurrentRow = PIXELS(pImage) + nRow * width + 32;
			unsigned cnt = (width + 31) / 64;
			unsigned char *pEndCurrentRow = pCurrentRow + cnt * 64;
			DWORD pixelGroup;
			
			while (pCurrentRow < pEndCurrentRow)
			{
				pixelGroup = *((DWORD *)pCurrentRow);
				ACCUM_PIXELS::accumulate(pixelGroup);
				pCurrentRow += 64;
			}
		}

	unsigned center_top;
	unsigned center_bottom;
	unsigned center_left;
	unsigned center_right;
};

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif

#endif // #ifndef __SAMPLING_H_81D7B7F1_2A99_432f_BD26_40CAD75F4859
