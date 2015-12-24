# File Name - Mks_source_decoders.mk
# Include file for specific decoders.


# Needed for any and all decoders
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/DecoderLib
DIR_SOURCE_APP += $(DIR_SBOX_ROOT)/libraries/DecoderLib/Source

MATRIX_SOURCES_APP += DecoderInterface  #used for all decoders

# Integrated Decoder
.IF $(DECODER_TYPE_ID) == 1
  CPP_DEF_EXTRA_APP += -DDECODER_ID=1
  
  MATRIX_SOURCES_APP += SwiftInterface
  
  LIBS_NOSOURCE+= $(DIR_SBOX_ROOT)/libraries/DecoderLib/opmatrix.a
.ELSE
  CPP_DEF_EXTRA_APP += -DDECODER_ID=0
.END

# Adaptus Decoder
.IF $(DECODER_TYPE_AD) == 1
  CPP_DEF_EXTRA_APP += -DDECODER_AD=1

  MATRIX_SOURCES_APP += AdaptusInterface

  LIBS_NOSOURCE+= $(DIR_SBOX_ROOT)/libraries/DecoderLib/hhpDecoder_gen6_ARM926EJ-S_prd.a
.ELSE
  CPP_DEF_EXTRA_APP += -DDECODER_AD=0
.END

# Rapidscan (FLD)
.IF $(DECODER_TYPE_FLD) == 1
  CPP_DEF_EXTRA_APP += -DDECODER_FLD=1
  
  MATRIX_SOURCES_APP += RapidScanInterface 
  
  LIBS_NOSOURCE+= $(DIR_SBOX_ROOT)/libraries/DecoderLib/rapidscan.a
  #MATRIX_SOURCES_APP += rapidscan #uncomment if build source instead of lib
.ELSE
  CPP_DEF_EXTRA_APP += -DDECODER_FLD=0
.END

# Unified Laser Decoder
.IF $(DECODER_TYPE_ULD) == 1
  CPP_DEF_EXTRA_APP += -DDECODER_ULD=1
  CPP_DEF_EXTRA_APP += -DREDUNDANCY_FILTER=1

  MATRIX_SOURCES_APP += UnifiedLaserInterface
  MATRIX_SOURCES_APP += RedundancyFilter

  LIBS_NOSOURCE+= $(DIR_SBOX_ROOT)/libraries/DecoderLib/unified_laser.o
.ELSE
  CPP_DEF_EXTRA_APP += -DDECODER_ULD=0
.END
## end
##
