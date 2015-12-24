#ifndef _MocfTags_h_
#define _MocfTags_h_

//#define DEBUG_TAGS // uncomment to have different tags, padding bytes, etc.  This should aid viewing data with a hex editor.



enum Gen6CompatibleProductsId_t {cpGen6BootFw = 0, cpGen6CordedScannerAppFw = 1, cpGen6RfScannerAppFw = 2, cpGen6RfBaseAppFw = 3
//#ifdef VUQUEST3310
//							, cpVuquest3310AppFw = 4, cpGen61500BootFw = 5, cpGen6Corded1500AppFw = 6, cpVuquest3310BootFw = 7
//#endif
							, cpVuquest3310AppFw = 4, cpGen61500BootFw = 5, cpGen6Corded1500AppFw = 6, cpVuquest3310BootFw = 7
							, cpVoyager1400BootFw = 0xA0, cpVoyager1400AppFw = 0xA1
							, cpYoujie4600BootFw = 0xE0, cpYoujie4600AppFw = 0xE1
//#ifdef XENON_LITE_1500
//							, cpGen61500BootFw = 5, cpGen6Corded1500AppFw = 6
//#endif
};


namespace Mocf
{
#ifdef DEBUG_TAGS
	// These tags make it easier to look at the binary data via a hex editor.
	enum MocfTags_t {tagMocfId = 0x46434f4d, tagMocfVersion = 0x12345678, tagMocfHash = 0x78563412, tagMocfCompatibleProducts = 0x11111111};
#else
	enum MocfTags_t {tagMocfId = 0x46434f4d, tagMocfVersion = 0x00, tagMocfHash = 0x01, tagMocfCompatibleProducts = 0x02};
#endif

	enum ObjectTags_t {tagMocfObject = 0x50000000};

}



namespace Gen6
{
	enum Gen6MocfObjectTag_t {tagMocf = 0x50000000, tagBootFirmware = 0x60000000, tagAppFirmware = 0x60000001, tagUserFile = 0x60000002,
							tagSoftwareKeys = 0x60000003, tagCustomDefMenuSettings = 0x60000004, tagMenuSettings = 0x60000005,
							tagRomImage = 0x60000006, tagExecuteMenuSettings = 0x60000007};
	
	// These Tags are used in compound Records (Records that are themselves containers of other Records/Objects
	enum Gen6CompoundObjectTag_t {tagFileName = 0, tagData = 1, tagSwVersionStr = 2, tagCompatProd = 3};
}



#endif // endof #ifndef _MocfTags_h_


