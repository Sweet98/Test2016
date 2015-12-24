#ifndef SCANDRIVERMISC_H_A71D5A2A_B6A5_4B0D_B769_A28C05253713
#define SCANDRIVERMISC_H_A71D5A2A_B6A5_4B0D_B769_A28C05253713

#undef min
#undef max
template <class T>
T min(const T & t1, const T & t2)
{
	return (t1 < t2) ? t1 : t2;
}

template <class T>
T max(const T & t1, const T & t2)
{
	return (t1 > t2) ? t1 : t2;
}

template <class T>
T bound(const T & lower, const T & t, const T & upper)
{
	if (t < lower)
		return lower;
	if (t > upper)
		return upper;
	return t;
}


#ifndef FLASH_LEDS
/// If FLASH_LEDS is not defined, it means code is compiling for
/// hardware that doesn't support flash mode.  So, stub these out to do
/// nothing.  @todo Remove stub SetILL_FlashMode and GetILL_FlashMode.
inline void SetILL_FlashMode(bool) { }
inline bool GetILL_FlashMode(void) { return false; }
#endif

#endif // #ifndef SCANDRIVERMISC_H_A71D5A2A_B6A5_4B0D_B769_A28C05253713
