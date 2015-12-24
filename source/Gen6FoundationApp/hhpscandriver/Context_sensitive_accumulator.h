#ifndef CONTEXT_SENSITIVE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375
#define CONTEXT_SENSITIVE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375


#ifdef SD_NAMESPACE
   namespace SD_NAMESPACE {
#endif

class Context_sensitive_accumulator
{
  public:
	Context_sensitive_accumulator()
    {
		reset();
    }

	int delta_buffer[2048];
	unsigned char pixel_buffer[2048];
	unsigned char min_delta, max_delta;
	unsigned n_buffered;

    void reset();
    void accumulate(DWORD pixelGroup);
	unsigned get_white_value() const { calc(); return white_value; }
	unsigned get_iq_score() const { calc(); return iq_score; }
	unsigned get_cs_pixel_cnt() const { calc(); return cs_pixel_cnt; }

    DWORD nHistogram[256];

private:
	void calc() const;
	mutable bool update; // If update is true, white_value and iq_score recalculating.
	mutable unsigned white_value;
	mutable unsigned iq_score;
	mutable unsigned cs_pixel_cnt;
};

#ifdef SD_NAMESPACE
   } // namespace SD_NAMESPACE {
#endif
#endif // #ifndef CONTEXT_SENSITIVE_ACCUMULATOR_8cd46372_a084_11de_b811_001eecfc0375
