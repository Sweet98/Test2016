#include "sampling.h"
#include "context_sensitive_accumulator.h"

#define elements(x) (sizeof(x)/sizeof(*x))

#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif


void 
Context_sensitive_accumulator ::
accumulate(DWORD cw)
{
  unsigned char p1 = cw & 0xFF;
  unsigned char p4 = p1;
  unsigned char p2;
  int d;
  update = true;
  ++nHistogram[p1];
  for (unsigned i = 3; i > 0; i--)
	{
	  cw >>= 8;
	  p2 = cw & 0xFF;
	  ++nHistogram[p2];

	  //if (p1 < 255 && p2 < 255){ //this check getting worse
		d = int(p1) - int(p2);
		if (d < 0)
			d = -d;

		if (d > max_delta)
			max_delta = d;
		if (d < min_delta)
			min_delta = d;
	  
		SDASSERT(n_buffered < sizeof(delta_buffer));
		//delta_histogram[d] += 1; //need initialization to zeros
		delta_buffer[n_buffered] = d;
		pixel_buffer[n_buffered] = (p1+p2)/2;
		++n_buffered;
	  //}
	  p1=p2;
	}
  //if(p1 < 255 && p4 < 255){
	d = int(p1) - int(p4);
	if (d < 0)
		d = -d;
	if (d > max_delta)
			max_delta = d;
	if (d < min_delta)
			min_delta = d;
	  
	SDASSERT(n_buffered < sizeof(delta_buffer));
	//delta_histogram[d] += 1; //need initialization to zeros
	delta_buffer[n_buffered] = d;
	pixel_buffer[n_buffered] = (p1+p4)/2;
	++n_buffered;
  //}

}


void
Context_sensitive_accumulator ::
reset()
{
	n_buffered = 0;
	max_delta = 0;
	min_delta = 255;

	update = true;
	white_value = ~0;
	iq_score = ~0;

	for (unsigned i = 0; i < elements(nHistogram); ++i)
		nHistogram[i] = 0;
}

void Context_sensitive_accumulator :: calc() const
{
	if (update == false)
		return;

	if (n_buffered == 0)
	{
		white_value = 0;
		iq_score = 0;
		cs_pixel_cnt = 0;
		return;
	}

	unsigned noise_th = (max_delta - min_delta) / 5 + min_delta;

// 		DWORD nSamplesToSkip = 20;
	//DWORD nSumation=0;
	//DWORD j=256;
	//while(nSumation < nSamplesToSkip)
	//	nSumation += delta_histogram[--j];
	//int noise_th = lspf_otsu_threshold(delta_buffer, n_buffered);
	//dbg_output("max_delta = %u\r\n", max_delta);
	//dbg_output("min_delta = %u\r\n", min_delta);
	//dbg_output("noise_th = %u\r\n", noise_th);
	unsigned pixel_sum = 0; //pixel white level sum
	unsigned delta_sum = 0; //pixel abs first derivative sum
	unsigned pixel_cnt = 0;
	iq_score = 0;
	for (unsigned i = 0; i < n_buffered; ++i)
	{
		if (delta_buffer[i] > noise_th)
		{
			pixel_sum += pixel_buffer[i];
			delta_sum += delta_buffer[i];
			pixel_cnt += 1;
		}
	}
	//dbg_output("pixel_cnt above threshold %u\r\n", pixel_cnt);
	cs_pixel_cnt = pixel_cnt;  

	if (pixel_cnt >= 10){
		white_value = pixel_sum / pixel_cnt;
		iq_score = unsigned(double(delta_sum)*100/pixel_cnt);}
	else
	{
		pixel_sum = 0;
		pixel_cnt = 0;
		for (unsigned i = 0; i < n_buffered; ++i)
		{
			pixel_sum += pixel_buffer[i];
			pixel_cnt += 1;
		}
		white_value = pixel_sum / pixel_cnt;
		iq_score = unsigned(double(delta_sum)*10); //lower iq_score when the pixel_cnt < 10
	}
//	dbg_output("pixel_cnt = %u, IQscore=%u white_value=%u\r\n", 
//			   pixel_cnt, iq_score, white_value);
	
	white_value = bound(1U, white_value, 255U);
	update = true;
}


#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif

