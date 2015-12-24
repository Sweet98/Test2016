//======================================================================================
// HONPlugin.cpp
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/HONPlugin.cpp $
// $Revision: 1.15 $
// $Date: 2011/05/30 20:53:25EDT $
//======================================================================================

//======================================================================================


//---------------------------------------------------------------------------
//
//  Module Interface Description:
//    This file contains the plugin object the plugin.
//
//---------------------------------------------------------------------------


#include "HONPlugin.h"
#include "HONPluginConfiguration.h"
#include "db_chrio.h"
#include "hstring.h"
#include "ProductConfiguration.h"
#include "menu.h"


#ifndef ARCH_SHF_SMALL
#define ARCH_SHF_SMALL 0
#endif

extern unsigned long MatrixSyscallAdd;

#define CACHE_DLINESIZE 32
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

#define GetPluginName(Plugin)			(Plugin ? Plugin->PluginName : "kernel")


/* If this is set, the section belongs in the init part of the plugin */
#define INIT_OFFSET_MASK (1UL << (BITS_PER_LONG-1))

extern "C" void MMU_CleanAndInvalidateBothCaches(void);

inline void PluginFree(void **region)
{
	if(*region != NULL){
		free(*region);
		*region = NULL;
	}
}


HONPlugin::HONPlugin()
{
	PluginRawData = NULL;
	PluginCoreCode = NULL;
	PluginInitCode = NULL;
	ExportedSyms = NULL;
	RawDataLen = 0;
	IsMenuLocked = false;

	RawInfo = NULL;
	ClassType = HON_PLUGIN_TYPE_UNKNOW;
	PluginStatus = HON_PLUGIN_STATUS_UNKNOWN;
	PluginIdentifier = NULL;

	MainRoutineOrder = HON_PLUGIN_MAIN_ROUTINE_ORDER_BEFORE;
	BarcodeInterceptMode = false;
	EntryDataState = HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL;
	ChainOnExit = HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE;
	
}


HONPlugin::HONPlugin(const char *FileName)
{
	PluginFileName = FileName;
	PluginRawData = NULL;
	PluginCoreCode = NULL;
	PluginInitCode = NULL;
	ExportedSyms = NULL;
	RawDataLen = 0;
	IsMenuLocked = false;

	RawInfo = NULL;
	ClassType = HON_PLUGIN_TYPE_UNKNOW;
	PluginStatus = HON_PLUGIN_STATUS_UNKNOWN;
	PluginIdentifier = NULL;

	MainRoutineOrder = HON_PLUGIN_MAIN_ROUTINE_ORDER_BEFORE;
	BarcodeInterceptMode = false;
	EntryDataState = HON_PLUGIN_ENTRY_DATA_STATE_ORIGINAL;
	ChainOnExit = HON_PLUGIN_CHAIN_ON_EXIT_CHAINIFFAILURE;
	
}


// Destructor
HONPlugin::~HONPlugin()
{
	ExitPlugin();
	ClearPlugin();
}

void HONPlugin::ClearPlugin()
{
	PluginFree((void**)(&PluginRawData));
	RawDataLen = 0;

	PluginFree((void**)(&PluginCoreCode));
	PluginFree((void**)(&PluginInitCode));


	RawInfo = NULL;
	ClassType = HON_PLUGIN_TYPE_UNKNOW;
	//PluginStatus = HON_PLUGIN_STATUS_UNKNOWN;
	PluginIdentifier = NULL;

}

bool HONPlugin::AddEndPlugin(HONPlugin **FirstPlugin, HONPlugin **LastPlugin)
{
	if(!*FirstPlugin)	//No plugin
	{
		*FirstPlugin = this;
		this->Prev = NULL;
		this->Next = NULL;
		*LastPlugin = this;
	}
	else
	{
		this->Prev = *LastPlugin;
		(*LastPlugin)->Next = this;
		*LastPlugin = this;
		this->Next = NULL;
	}
	return true;
}

bool HONPlugin::PluginSanityCheck()
{
	
	if (memcmp(PluginElfHeader->e_ident, ELFMAG, 4) != 0 
		|| PluginElfHeader->e_type != ET_REL 
		|| !ISArmArch(PluginElfHeader) 
		|| PluginElfHeader->e_shentsize != sizeof(ElfSectionHeader)) 
	{
		ClearPlugin();
		return false;
	}

	if (RawDataLen < PluginElfHeader->e_shoff + PluginElfHeader->e_shnum * sizeof(ElfSectionHeader))
	{
		PLUGIN_TRACE(("Plugin length %lu truncated\n", RawDataLen))
		ClearPlugin();
		return false;
	}

	return true;
}

bool HONPlugin::LoadSections()
{
	int i;
	
	for (i = 1; i < PluginElfHeader->e_shnum; i++)
	{
		if (PluginElfSectionHeaders[i].sh_type != SHT_NOBITS
		    && RawDataLen < PluginElfSectionHeaders[i].sh_offset + PluginElfSectionHeaders[i].sh_size)
		{
		       this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC);

			PLUGIN_TRACE(("Plugin length %lu truncated\n", RawDataLen))
			ClearPlugin();
			return false;
		}

		/* Mark all sections sh_addr with their address in the temporary image. */
		PluginElfSectionHeaders[i].sh_addr = (unsigned int)PluginElfHeader + PluginElfSectionHeaders[i].sh_offset;

		/* Internal symbols and strings. */
		if (PluginElfSectionHeaders[i].sh_type == SHT_SYMTAB)
		{
			SymbolTableIndex = i;
			StringTableIndex = PluginElfSectionHeaders[i].sh_link;
			StringTable = (char *)PluginElfHeader + PluginElfSectionHeaders[StringTableIndex].sh_offset;
		}

		/* Don't load .exit sections */
		if (strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".exit", 5) == 0)
			PluginElfSectionHeaders[i].sh_flags &= ~(unsigned long)SHF_ALLOC;
	}

	PluginInfoIndex = FindSection(PluginElfHeader, PluginElfSectionHeaders, SectionNameStrings, ".this_plugin");
	
	if (!PluginInfoIndex)
	{
	       this->SetPluginStatus(HON_PLUGIN_STATUS_CONFIG_NOPLUGIN);
		PLUGIN_TRACE(("No plug-in found in object\n"))
		ClearPlugin();
		return false;
	}
	
	RawInfo = (HONPluginRawInfo*)PluginElfSectionHeaders[PluginInfoIndex].sh_addr;

	RawInfo->PluginApis = NULL;
	PluginInitCode = false;
	PluginCoreCode = false;

	if (SymbolTableIndex == 0)
	{
	       this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC);

		PLUGIN_TRACE(("%s: plugin has no symbols (stripped?)\n", (char*)RawInfo->PluginRawName))
		ClearPlugin();
		return false;
	}

	/* Optional sections */
	ExportFunctionIndex = FindSection(PluginElfHeader, PluginElfSectionHeaders, SectionNameStrings, "matrix_symtab");

	/* Keep symbol and string tables for decoding later. */
	PluginElfSectionHeaders[SymbolTableIndex].sh_flags |= SHF_ALLOC;
	PluginElfSectionHeaders[StringTableIndex].sh_flags |= SHF_ALLOC;

	return true;
}

void HONPlugin::LayoutSections()
{
	unsigned long const Masks[][2] = {
		/* NOTE: all executable code must be the first section
		 * in this array; otherwise modify the text_size
		 * finder in the two loops below */
		{ SHF_EXECINSTR | SHF_ALLOC, ARCH_SHF_SMALL },
		{ SHF_ALLOC, SHF_WRITE | ARCH_SHF_SMALL },
		{ SHF_WRITE | SHF_ALLOC, ARCH_SHF_SMALL },
		{ ARCH_SHF_SMALL | SHF_ALLOC, 0 }
	};
	unsigned int m, i;
	
	CoreCodeSize = 0;
	InitCodeSize = 0;

	for (i = 0; i < PluginElfHeader->e_shnum; i++)
		PluginElfSectionHeaders[i].sh_entsize = ~0UL;
	PLUGIN_TRACE(("Core section allocation order:\r\n"))
	for (m = 0; m < ARRAY_SIZE(Masks); ++m)
	{
		for (i = 0; i < PluginElfHeader->e_shnum; ++i)
		{
			ElfSectionHeader *Section = &PluginElfSectionHeaders[i];

			if ((Section->sh_flags & Masks[m][0]) != Masks[m][0]
				|| (Section->sh_flags & Masks[m][1])
				|| Section->sh_entsize != ~0UL
				|| strncmp(SectionNameStrings + Section->sh_name, ".init", 5) == 0
				|| Section->sh_type == SHT_ARM_EXIDX
				|| strncmp(SectionNameStrings + Section->sh_name, ".rel.ARM.exidx", 14) == 0
				|| strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".rel.ARM.extab", 14) == 0
				|| strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".ARM.extab", 10) == 0)
				continue;
			
			Section->sh_entsize = SectionOffset(&CoreCodeSize, Section);
			PLUGIN_TRACE(("\t%s\r\n", SectionNameStrings + Section->sh_name))
		}
		if (m == 0)
			CoreTextSize = CoreCodeSize;
	}

	PLUGIN_TRACE(("Init section allocation order:\r\n"))

	for (m = 0; m < ARRAY_SIZE(Masks); ++m)
	{
		for (i = 0; i < PluginElfHeader->e_shnum; ++i)
		{
			ElfSectionHeader *Section = &PluginElfSectionHeaders[i];

			if ((Section->sh_flags & Masks[m][0]) != Masks[m][0]
				|| (Section->sh_flags & Masks[m][1])
				|| Section->sh_entsize != ~0UL
				|| strncmp(SectionNameStrings + Section->sh_name, ".init", 5) != 0)
				continue;
			Section->sh_entsize = (SectionOffset(&InitCodeSize, Section)
					 | INIT_OFFSET_MASK);
			PLUGIN_TRACE(("\t%s\r\n", SectionNameStrings + Section->sh_name))
		}
		if (m == 0)
			InitTextSize = InitCodeSize;
	}
}

bool HONPlugin::SimplifySymbols()
{
	ElfSymbol *SymbolTable = (ElfSymbol *)PluginElfSectionHeaders[SymbolTableIndex].sh_addr;
	unsigned long SectionBase;
	unsigned int i, n = PluginElfSectionHeaders[SymbolTableIndex].sh_size / sizeof(ElfSymbol);
	bool Ret = true;
	HONPlugin *Owner;

	for (i = 1; i < n; i++)
	{		
		switch (SymbolTable[i].st_shndx)
		{
			case SHN_COMMON:
				/* We compiled with -fno-common.  These are not supposed to happen.  */
				PLUGIN_TRACE(("Common symbol: %s\r\n", StringTable + SymbolTable[i].st_name))
				PLUGIN_TRACE(("%s: please compile with -fno-common\r\n", RawInfo->PluginRawName))
				Ret = false;
				break;

			case SHN_ABS:
				/* Don't need to do anything */
				break;

			case SHN_UNDEF:
				SymbolTable[i].st_value =  FindSymbol(StringTable + SymbolTable[i].st_name, &Owner);

				/* Ok if resolved.  */
				if (SymbolTable[i].st_value != 0)
					break;
				/* Ok if weak.  */
				if (ELF_ST_BIND(SymbolTable[i].st_info) == STB_WEAK)
					break;
				PLUGIN_TRACE(("Unknown symbol %s\r\n", StringTable + SymbolTable[i].st_name))
				Ret = false;

				break;

			default:
				SectionBase = (PluginElfSectionHeaders[SymbolTable[i].st_shndx].sh_addr);
				SymbolTable[i].st_value += SectionBase;
				break;
		}
	}
	
	return Ret;
}


bool HONPlugin::DoRelocation()
{
	int i;
	bool Err = true;
	
	for (i = 1; i < PluginElfHeader->e_shnum; i++) {
		const char *strtab = (char *)PluginElfSectionHeaders[StringTableIndex].sh_addr;
		unsigned int info = PluginElfSectionHeaders[i].sh_info;

		/* Not a valid relocation section? */
		if (info >= PluginElfHeader->e_shnum)
			continue;

		/* Don't bother with non-allocated sections */
		if (!(PluginElfSectionHeaders[info].sh_flags & SHF_ALLOC))
			continue;

		if (PluginElfSectionHeaders[i].sh_type == SHT_REL)
			Err = ApplyRelocate(PluginElfSectionHeaders, strtab, SymbolTableIndex, i,this);
		else if (PluginElfSectionHeaders[i].sh_type == SHT_RELA)
			Err = ApplyRelocateAdd(PluginElfSectionHeaders, strtab, SymbolTableIndex, i, this);
		if(Err == false)
			return Err;
	}
	return Err;
}


/*
 * Ensure that an exported symbol does not already exist
 * in the system list or in some other plugins exported symbol table.
 */
bool HONPlugin::VerifyExportSymbols()
{
	const char *Name = NULL;
	unsigned long i;
	bool	Ret = true;
	HONPlugin *Plugin;

	/* Now try symbols in other plugins. Search in all class type of plug-ins */
	for (i = 0; i < ExportedSymsNum; i++)
	{
		if (FindSymbol(ExportedSyms[i].Name, &Plugin))
		{
			Name = ExportedSyms[i].Name;
			Ret = false;
		}
	}

	if (!Ret)
	{
		PLUGIN_TRACE(("%s: exports duplicate symbol %s (owned by %s)\r\n", PluginName, Name, GetPluginName(Plugin)))
	}

	return Ret;
		
}

long HONPlugin::SectionOffset(unsigned long *Size, ElfSectionHeader *SectionHeader)
{
	long ret;

	ret = ALIGN(*Size, SectionHeader->sh_addralign ? SectionHeader->sh_addralign : 1);
	*Size = ret + SectionHeader->sh_size;
	return ret;
}

/* lookup symbol in plugins (Not used currently) */
const HONPlugin::HONPluginSymbol* HONPlugin::LookupExternalSymbol(const char *Name,
		const HONPlugin::HONPluginSymbol *Start,
		const HONPlugin::HONPluginSymbol *Stop)
{
	const HONPluginSymbol *MatrixSymbol = Start;
	char MatrixName[256];		//symbols are automatically added "matrix_" prefix
	strcpy(MatrixName, "matrix_");
	strcat(MatrixName, Name);
	
	for (; MatrixSymbol < Stop; MatrixSymbol++)
		if (strcmp(MatrixSymbol->Name, MatrixName) == 0)
			return MatrixSymbol;

	return NULL;
}


/* Find a symbol, return value and plugin which owns it */
unsigned long HONPlugin::FindSymbol(const char *Name, HONPlugin **Owner)
{
	HONPlugin*Plugin;
	const HONPluginSymbol *MatrixSymbol;
	int i;

	/* Matrix internal exported symbols first. */ 
	*Owner = NULL;
	
	if (strcmp("matrix_syscall", Name) == 0)
		return MatrixSyscallAdd;


	/* Now try symbols in other plugins. Search in all class type of plug-ins */
	for(i = 0; i < 3; i++)
	{
		if(g_PluginConfigurations[i])
		{
			for(Plugin = g_PluginConfigurations[i]->GetFirstPlugin(); Plugin!=NULL; Plugin = Plugin->Next)
			{
				if(Plugin->ExportedSymsNum && Plugin->ExportedSyms)
				{
					MatrixSymbol = LookupExternalSymbol(Name, Plugin->ExportedSyms, Plugin->ExportedSyms + Plugin->ExportedSymsNum);
					if (MatrixSymbol)
					{
						*Owner = Plugin;
						return MatrixSymbol->Value;
					}
				}
			}
		
		}
	}
	
	//PLUGIN_TRACE("Failed to find symbol %s\r\n", Name);

 	return 0;
}

bool HONPlugin::ApplyRelocate(ElfSectionHeader *SectionHeaders, 
		const char *StringTable, 
		unsigned int SymbolIndex,
	       unsigned int RelocateIndex, 
	       HONPlugin *Plugin)
{
	ElfSectionHeader *SymbolSection = SectionHeaders + SymbolIndex;
	ElfSectionHeader *RelocateSection = SectionHeaders + RelocateIndex;
	ElfSectionHeader *DestSection = SectionHeaders + RelocateSection->sh_info;
	ElfRelocation *Relocation = (ElfRelocation *)RelocateSection->sh_addr;
	unsigned int i;

	for (i = 0; i < RelocateSection->sh_size / sizeof(Elf32_Rel); i++, Relocation++)
	{
		unsigned long Location;
		ElfSymbol *Symbol;
		signed int Offset;

		Offset = ELF32_R_SYM(Relocation->r_info);
		if (Offset < 0 || Offset > (SymbolSection->sh_size / sizeof(ElfSymbol)))
		{
			PLUGIN_TRACE(("%s: bad relocation, section %d reloc %d\r\n", Plugin->PluginName, RelocateIndex, i))
			return false;
		}

		Symbol = ((ElfSymbol *)SymbolSection->sh_addr) + Offset;


		if ((int)Relocation->r_offset < 0 || Relocation->r_offset > DestSection->sh_size - sizeof(unsigned int))
		{
			PLUGIN_TRACE(("%s: out of bounds relocation, section %d reloc %d Offset %d size %d\r\n", Plugin->PluginName, RelocateIndex, i, Relocation->r_offset, DestSection->sh_size))
			return false;
		}

		Location = DestSection->sh_addr + Relocation->r_offset;

		switch (ELF32_R_TYPE(Relocation->r_info))
		{
			case R_ARM_ABS32:
			case R_ARM_TARGET1:		// R_ARM_TARGET1 treated as absolute relocation
				*(unsigned int *)Location += Symbol->st_value;
				break;

			case R_ARM_PC24:
			case R_ARM_CALL:
			case R_ARM_JUMP24:
				Offset = (*(unsigned int *)Location & 0x00ffffff) << 2;
				if (Offset & 0x02000000)
					Offset -= 0x04000000;

				Offset += Symbol->st_value - Location;
				if (Offset & 3 || Offset <= (signed int)0xfc000000 || Offset >= (signed int)0x04000000)
				{
					PLUGIN_TRACE(("%s: relocation out of range, section %d reloc %d Symbol '%s'\r\n", Plugin->PluginName,
					       RelocateIndex, i, StringTable + Symbol->st_name));
					return false;
				}

				Offset >>= 2;

				*(unsigned int *)Location &= 0xff000000;
				*(unsigned int *)Location |= Offset & 0x00ffffff;
				
				break;
				
			case R_ARM_V4BX:
			case R_ARM_PLT32:	/* Deprecated. Leave it alone */
				break;
				
			default:
				PLUGIN_TRACE(("%s: unknown relocation: %u\r\n", Plugin->RawInfo->PluginRawName, ELF32_R_TYPE(Relocation->r_info)));
				return false;
		}

	}

	return true;
}

bool HONPlugin::ApplyRelocateAdd(ElfSectionHeader *SectionHeaders, 
		const char *StringTable, 
		unsigned int SymbolIndex,
	       unsigned int RelocateIndex, 
	       HONPlugin *Plugin)
{
	PLUGIN_TRACE(("plugin %s: ADD RELOCATION unsupported\n", Plugin->PluginName))
	return false;
}

/* Find a plugin section: 0 means not found. */
unsigned int HONPlugin::FindSection(ElfHeader *Header, 
		ElfSectionHeader *SectionHeaders, 
		const char *NameStrings, 
		const char *SectionName)
{
	unsigned int i;

	for (i = 1; i < Header->e_shnum; i++)
	{
		if ((SectionHeaders[i].sh_flags & SHF_ALLOC) && strcmp(NameStrings + SectionHeaders[i].sh_name, SectionName) == 0)
			return i;
	}
	return 0;
}


bool HONPlugin::LoadPlugin()
{
	if(!PluginFileName){
		return false;
	}

	return LoadPlugin(PluginFileName);
}

int InitArrayIndex = 0;
unsigned long *Ctor = 0;
int CallNum = 0;

bool HONPlugin::LoadPlugin(const char *FileName)
{
	int i;
	unsigned long ReadSize;
	F_FILE *PluginFileHandler;
	char FName[1024];

	if(!FileName)
		return false;

	PluginFileName = FileName;

	sprintf(FName, "/PluginFolder/%s", PluginFileName);

	RawDataLen = f_filelength(FName);
	
	
	if (RawDataLen < sizeof(*PluginElfHeader)){
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		return false;
	}

	if (RawDataLen > (2 * 1024 * 1024)){
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		return false;
	}

	PluginRawData =  (unsigned char *)malloc(RawDataLen);
	if(!PluginRawData)
	{
		ClearPlugin();
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		return false;
	}


	PluginFileHandler = f_open(FName, "r");

	if(!PluginFileHandler){
		this->SetPluginStatus(HON_PLUGIN_STATUS_MISSING_FILENOTFOUND);
		return false;
	}

	/* Read the content to work buffer */
	ReadSize = f_read(PluginRawData, 1, RawDataLen, PluginFileHandler);

	if(ReadSize != RawDataLen)
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		ClearPlugin();
		f_close(PluginFileHandler);
		return false;
	}

	f_close(PluginFileHandler);

	
	PluginElfHeader = (ElfHeader*)PluginRawData;
	
	/* Plugin Snaity Check */
	if(!PluginSanityCheck())
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC);
		ClearPlugin();
		return false;
	}


	PluginElfSectionHeaders = (ElfSectionHeader*)((unsigned char *)PluginElfHeader + PluginElfHeader->e_shoff);
	SectionNameStrings = (char *)((unsigned char *)PluginElfHeader + PluginElfSectionHeaders[PluginElfHeader->e_shstrndx].sh_offset);
	PluginElfSectionHeaders[0].sh_addr = 0;

	/* Load Sections */
	if(!LoadSections())
	{
		//this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_RELOCATIONUNKNOWN);
		ClearPlugin();
		return false;
	}

	InitArrayIndex = FindSection(PluginElfHeader, PluginElfSectionHeaders, SectionNameStrings, ".init_array");
	CallNum = PluginElfSectionHeaders[InitArrayIndex].sh_size/sizeof(unsigned long);

	/* Layout Sections */
	LayoutSections();

	if((CoreCodeSize + InitCodeSize) > (3*1024*1024)){
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		ClearPlugin();
		return false;
	}

	PluginCoreCode = malloc(CoreCodeSize);

	if (!PluginCoreCode)
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		ClearPlugin();
		return false;
	}

	PluginInitCode = malloc(InitCodeSize);
	if (!PluginInitCode && InitCodeSize)
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_NORESOURCE_MALLOC);
		free(PluginCoreCode);
		ClearPlugin();
		return false;
	}

	memset(PluginCoreCode, 0, CoreCodeSize);
	memset(PluginInitCode, 0, InitCodeSize);


	PLUGIN_TRACE(("final section addresses:\r\n"))
	/* Transfer each section which specifies SHF_ALLOC */
	for (i = 0; i < PluginElfHeader->e_shnum; i++)
	{
		void *Dest;

		if (!(PluginElfSectionHeaders[i].sh_flags & SHF_ALLOC)
				|| PluginElfSectionHeaders[i].sh_type == SHT_ARM_EXIDX
				|| strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".rel.ARM.exidx", 14) == 0
				|| strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".rel.ARM.extab", 14) == 0
				|| strncmp(SectionNameStrings + PluginElfSectionHeaders[i].sh_name, ".ARM.extab", 10) == 0)
			continue;

		if (PluginElfSectionHeaders[i].sh_entsize & INIT_OFFSET_MASK)
			Dest = ((unsigned char*)PluginInitCode + (PluginElfSectionHeaders[i].sh_entsize & ~INIT_OFFSET_MASK));
		else
			Dest = ((unsigned char*)PluginCoreCode + PluginElfSectionHeaders[i].sh_entsize);

		if (PluginElfSectionHeaders[i].sh_type != SHT_NOBITS)
			memcpy(Dest, (void *)PluginElfSectionHeaders[i].sh_addr, PluginElfSectionHeaders[i].sh_size);
		
		/* Update sh_addr to point to copy in image. */
		PluginElfSectionHeaders[i].sh_addr = (unsigned long)Dest;

		PLUGIN_TRACE(("\t0x%lx %s\r\n", PluginElfSectionHeaders[i].sh_addr, SectionNameStrings + PluginElfSectionHeaders[i].sh_name))
	}
	
	/* Update Plugin info. */
	RawInfo = (HONPluginRawInfo *)PluginElfSectionHeaders[PluginInfoIndex].sh_addr;
	Ctor = (unsigned long*)PluginElfSectionHeaders[InitArrayIndex].sh_addr;


	/* Fix up syms, so that st_value is a pointer to location. */
	if(!SimplifySymbols())
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC);
		ClearPlugin();
		return false;
	}

	/* Set up EXPORTed symbols (section 0 is 0 length) */
	ExportedSymsNum = PluginElfSectionHeaders[ExportFunctionIndex].sh_size / sizeof(*ExportedSyms);
	ExportedSyms = (HONPluginSymbol *)PluginElfSectionHeaders[ExportFunctionIndex].sh_addr;

	/* Now do relocations. */
	if(!DoRelocation())
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_RELOCATIONFAIL);
		ClearPlugin();
		return false;
	}

#if 1
	/* See if there any duplicate symbols */
	if(!VerifyExportSymbols())
	{
		this->SetPluginStatus(HON_PLUGIN_STATUS_CORRUPT_SYMBOLREFERENC);
		ClearPlugin();
		return false;
	}
#endif
	/* flush the icache in correct context */
	MMU_CleanAndInvalidateBothCaches();

	/* Free raw data buffer */
	PluginFree((void**)(&PluginRawData));
	RawDataLen = 0;
	this->SetPluginStatus(HON_PLUGIN_STATUS_SUCCESS);
	return true;
	
}
// Remove Plugin
void HONPlugin::RemovePlugin(HONPlugin **FirstPlugin, HONPlugin **LastPlugin)
{
	/* First delete plugin from the sequence */
	if(!Prev)
		*FirstPlugin = Next;
	else
		Prev->Next = Next;
	
	if(!Next)
		*LastPlugin = Prev;
	else
		Next->Prev = Prev;

	(*FirstPlugin)->Prev = NULL;
	(*LastPlugin)->Next = NULL;

	/* Cleanup plugin by calling its own exit routine */
	ExitPlugin();

	/* Clear Plugin */
	ClearPlugin();


}

// Initialize Plugin by calling the plugin internal routine
bool HONPlugin::InitializePlugin()
{
	int Ret = 0;

#if 1
	PLUGIN_TRACE(("%d, 0x%x, %d\r\n", InitArrayIndex, Ctor, CallNum))	
	// Constructors
	if(InitArrayIndex){
		unsigned long *call;
		int i = 0;
		for(call = Ctor; i < CallNum; i++, call++){
			((void (*) (void)) (*call)) ();
		}
	}
#endif
	// Get serial number and GroupID
	HSTRING serialNumber(15,true);
	int GroupID;
	char cGroupID[7];
	serialNumber.Copy(GET_CONFIG_SETTING(SerNum));
	serialNumber.UniCodeToSimpleAsciiFormat();
	GroupID = GET_CONFIG_SETTING(iGroupIdentifier);
	//PLUGIN_TRACE(("Group ID is %d\r\n", GroupID));

	if(GroupID){
		int i;
		int tmp;
		for(i = 0; i < 4; i++){
			tmp = GroupID & (0xf << (i*4));
			//PLUGIN_TRACE(("tmp is %x\r\n", tmp));
			if((tmp>>(i*4)) <= 9)
				cGroupID[3-i] = '0' + (tmp >> (i*4));
			else
				cGroupID[3-i] = 'A' + ((tmp >> (i*4)) - 10);
		}
		cGroupID[4] = 'X';
		cGroupID[5] = 'X';
		cGroupID[6] = 0;
		//PLUGIN_TRACE(("Group ID is %s\r\n", cGroupID));
	}

	if(PluginStatus == HON_PLUGIN_STATUS_SUCCESS)
	{
		if(RawInfo->PluginInitRoutine)
			Ret = RawInfo->PluginInitRoutine(RawInfo);
	}
	else
		return false;

	if(Ret)
		return false;

	// Then check the license
	switch(RawInfo->PluginRawClassType)
	{
		case HON_PLUGIN_FORMAT:
			{
				DataEditApi *API = (DataEditApi*)RawInfo->PluginApis;
				if(API && API->CheckLicense){
					if(API->CheckLicense((char *)serialNumber.Char))
					{		// Not licensed by SN, try groupID
						if(GroupID){	// 0x0000 means we don't have a group ID
							if(API->CheckLicense((char*)cGroupID))
								PluginLicensed = false;
							else
								PluginLicensed = true;
						}else
							PluginLicensed = false;
					}
					else
					{		// Licensed
						PluginLicensed = true;
					}
				}
			}
			break;
		case HON_PLUGIN_IMAGE:
			break;
		case HON_PLUGIN_DECODE:
			{
				DecodeApi *API = (DecodeApi*)RawInfo->PluginApis;
				if(API && API->CheckLicense){
					if(API->CheckLicense((char *)serialNumber.Char))
					{		// Not licensed by SN, try groupID
						if(GroupID){	// 0x0000 means we don't have a group ID
							if(API->CheckLicense((char*)cGroupID))
								PluginLicensed = false;
							else
								PluginLicensed = true;
						}else
							PluginLicensed = false;
					}
					else
					{		// Licensed
						PluginLicensed = true;
					}
				}
			}
			break;
		default:
			break;
	}

	return true;	//still return true even the check license failed.
}


// Cleanup plugin by calling plugin's exit routine
void HONPlugin::ExitPlugin()
{
	if(PluginStatus == HON_PLUGIN_STATUS_SUCCESS)
		if(RawInfo->PluginExitRoutine)
			RawInfo->PluginExitRoutine();
}


bool HONPlugin::ReloadPlugin(const char *FileName)
{
	/* Close Plugin first */
	ExitPlugin();
	ClearPlugin();

	return LoadPlugin(FileName);
}


/* Debug */
void HONPlugin::DumpPlugin()
{
	const char *LoadStatus[] = {
		"UNKNOWN", "SUCCESS", "INACTIVE", 
		"UNLICENSED", "NORESOURCE", "MAINNOTFOUND",
		"HELPERNOTFOUND", "CPRRUPT"};

	if (PluginIdentifier){	
		PLUGIN_TRACE(("<%s>\r\n", PluginIdentifier))
	}
	if(PluginName){
		PLUGIN_TRACE(("\t[Name]:\t\t\t"))
		PLUGIN_TRACE((PluginName))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginCompanyName){
		PLUGIN_TRACE(("\t[Company Name]:\t\t"))
		PLUGIN_TRACE((PluginCompanyName))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginMajorRevision){
		PLUGIN_TRACE(("\t[Major Number]:\t\t"))
		PLUGIN_TRACE((PluginMajorRevision))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginMinorRevision){
		PLUGIN_TRACE(("\t[Minor Number]:\t\t"))
		PLUGIN_TRACE((PluginMinorRevision))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginBuild){
		PLUGIN_TRACE(("\t[Build]:\t\t"))
		PLUGIN_TRACE((PluginBuild))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginCertificate){
		PLUGIN_TRACE(("\t[Certificate]:\t\t"))
		PLUGIN_TRACE((PluginCertificate))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginCertificateTime){
		PLUGIN_TRACE(("\t[Certificate Time]:\t"))
		PLUGIN_TRACE((PluginCertificateTime))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginGUID){
		PLUGIN_TRACE(("\t[GUID]:\t\t\t"))
		PLUGIN_TRACE((PluginGUID))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginFileName){
		PLUGIN_TRACE(("\t[File Name]:\t\t"))
		PLUGIN_TRACE((PluginFileName))
		PLUGIN_TRACE(("\r\n"))
	}
	if(PluginStatus < 8 ){
		PLUGIN_TRACE(("\t[Load Status]:\t\t"))
		PLUGIN_TRACE((LoadStatus[PluginStatus]))
		PLUGIN_TRACE(("\r\n"))
	}
	PLUGIN_TRACE(("\r\n"))


}

//=================================================================================
//                       File Modification History
//===================================================================================



