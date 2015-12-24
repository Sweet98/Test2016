# File Name - source.mk
# Include file for building the Compression Library.


# TARGET_NAME and OBJDIR must be defined for the library
TARGET_NAME = Compression
OBJDIR      = Cmpr_LIB
#=================================================================
# extra CPP defines for compression.a
#=================================================================
CPP_DEF += -DLEMPEL_ZIV_VERSION_1_0_NOT_SUPPORTED=1
CPP_DEF += -DLZO_NOT_SUPPORTED=1
CPP_DEF += -DHUFFMAN_VERSION_1_0_NOT_SUPPORTED=1
CPP_DEF += -DLZO_NO_SYS_TYPES_H=1
CPP_DEF += -DJPEG_DECOMPRESSION_NOT_SUPPORTED=1
CPP_DEF += --diag_suppress=1294
C_AS_CPP=

#=================================================================
# source directories for compression.a
#=================================================================
DIR_SOURCE += ../StringAndData
DIR_SOURCE += ../OsKernel/db_shared
DIR_SOURCE += ../Communication
DIR_SOURCE += JPEG
DIR_SOURCE += LZO
DIR_SOURCE += zlibut

#=================================================================
# c/ cpp files for compression.a
#=================================================================
MATRIX_SOURCES+= adler32
MATRIX_SOURCES+= compression
MATRIX_SOURCES+= deflate
MATRIX_SOURCES+= infblock
MATRIX_SOURCES+= infcodes
MATRIX_SOURCES+= inffast
MATRIX_SOURCES+= inflate
MATRIX_SOURCES+= inftrees
MATRIX_SOURCES+= infutil
MATRIX_SOURCES+= jcapimin
MATRIX_SOURCES+= JCAPISTD
MATRIX_SOURCES+= jccoefct
MATRIX_SOURCES+= jccolor
MATRIX_SOURCES+= jcdctmgr
MATRIX_SOURCES+= jchuff
MATRIX_SOURCES+= jcinit
MATRIX_SOURCES+= jcmainct
MATRIX_SOURCES+= jcmarker
MATRIX_SOURCES+= jcmaster
MATRIX_SOURCES+= jcomapi
MATRIX_SOURCES+= JCPARAM
MATRIX_SOURCES+= jcphuff
MATRIX_SOURCES+= jcprepct
MATRIX_SOURCES+= jcsample
MATRIX_SOURCES+= jdapimin
MATRIX_SOURCES+= JDAPISTD
MATRIX_SOURCES+= Jdatadst
MATRIX_SOURCES+= Jdatasrc
MATRIX_SOURCES+= jdcoefct
MATRIX_SOURCES+= jdcolor
MATRIX_SOURCES+= jddctmgr
MATRIX_SOURCES+= jdhuff
MATRIX_SOURCES+= jdinput
MATRIX_SOURCES+= jdmainct
MATRIX_SOURCES+= jdmarker
MATRIX_SOURCES+= jdmaster
MATRIX_SOURCES+= jdmerge
MATRIX_SOURCES+= jdphuff
MATRIX_SOURCES+= jdpostct
MATRIX_SOURCES+= jdsample
MATRIX_SOURCES+= JERROR
MATRIX_SOURCES+= jfdctfst
MATRIX_SOURCES+= jfdctint
MATRIX_SOURCES+= jidctfst
MATRIX_SOURCES+= jidctint
MATRIX_SOURCES+= jidctred
MATRIX_SOURCES+= jmemmgr
MATRIX_SOURCES+= jmemnobs
MATRIX_SOURCES+= jquant1
MATRIX_SOURCES+= jquant2
MATRIX_SOURCES+= jutils
MATRIX_SOURCES+= trees
MATRIX_SOURCES+= zutil
