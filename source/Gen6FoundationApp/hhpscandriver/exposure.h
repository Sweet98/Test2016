inline bool is_autoexposure(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_AUTOEXPOSURE_USE_HHP 
		/*|| mode == HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP*/;
}

inline bool is_context_sensitive(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_CONTEXT_SENSITIVE_USE_HHP
		/*|| mode == HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP*/;
}

inline bool is_presentation(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_AUTOEXPOSURE_HHP_PRESENTATION;
}

inline bool is_dual_track(HHPScanDriverAutoExposureMode_t mode)
{
	return false;
//	return mode == HHPSD_AUTOEXPOSURE_DUAL_TRACK_USE_HHP
//		|| mode == HHPSD_CONTEXT_SENSITIVE_DUAL_TRACK_USE_HHP;
}

inline bool is_fixed(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_EXPOSURE_FIXED;
}

inline bool is_onchip(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_AUTOEXPOSURE_USE_ONCHIP;
}

inline bool is_openloop_gain(HHPScanDriverAutoExposureMode_t mode)
{
	return mode == HHPSD_OPENLOOP_GAIN;
}

