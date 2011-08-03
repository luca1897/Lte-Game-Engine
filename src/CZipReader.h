/*
 * LTE Game Engine
 * Copyright (C) 2006-2008 SiberianSTAR <haxormail@gmail.com>
 * http://www.ltestudios.com
 *  
 * The LTE Game Engine is based on Irrlicht 1.0
 * Irrlicht Engine is Copyright (C) 2002-2006 Nikolaus Gebhardt
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */
 
#ifndef __C_ZIP_READER_H_INCLUDED__
#define __C_ZIP_READER_H_INCLUDED__

#include "IUnknown.h"
#include "IReadFile.h"
#include "engineArray.h"
#include "engineString.h"

namespace engine
{
namespace io
{

const s16 ZIP_FILE_ENCRYPTED =                  0x0001;         // set if the file is encrypted
const s16 ZIP_INFO_IN_DATA_DESCRITOR =  0x0008;         // the fields crc-32, compressed size
// and uncompressed size are set to zero in the local
// header

#ifdef _MSC_VER
#       pragma pack( push, packing )
#       pragma pack( 1 )
#       define PACK_STRUCT
#elif defined ( __GNUC__ )
#       define PACK_STRUCT      __attribute__((packed))
#else
#       error compiler not supported
#endif


struct SZIPFileDataDescriptor
{
	s32 CRC32;
	s32 CompressedSize;
	s32 UncompressedSize;
} PACK_STRUCT;

struct SZIPFileHeader
{
	s32 Sig;
	s16 VersionToExtract;
	s16 GeneralBitFlag;
	s16 CompressionMethod;
	s16 LastModFileTime;
	s16 LastModFileDate;
	SZIPFileDataDescriptor DataDescriptor;
	s16 FilenameLength;
	s16 ExtraFieldLength;
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#       pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


struct SZipFileEntry
{
	core::stringc zipFileName;
	core::stringc simpleFileName;
	core::stringc path;
	s32 fileDataPosition;         // position of compressed data in file
	SZIPFileHeader header;

	bool operator < (const SZipFileEntry& other) const
	{
		return simpleFileName < other.simpleFileName;
	}


	bool operator == (const SZipFileEntry& other) const
	{
		return simpleFileName == other.simpleFileName;
	}
};



/*!
   	Zip file Reader written April 2002 by N.Gebhardt.
   	Doesn't decompress data, only reads the file and is able to
   	open uncompressed entries.
 */
class CZipReader : public IUnknown
{
public:

CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths);
virtual ~CZipReader();

//! opens a file by file name
virtual IReadFile* openFile(const c8* filename);

//! opens a file by index
IReadFile* openFile(s32 index);

//! returns count of files in archive
s32 getFileCount();

//! returns data of file
const SZipFileEntry* getFileInfo(s32 index) const;

//! returns fileindex
s32 findFile(const c8* filename);

private:

//! scans for a local header, returns false if there is no more local file header.
bool scanLocalHeader();

//! splits filename from zip file into useful filenames and paths
void extractFilename(SZipFileEntry* entry);

//! deletes the path from a filename
void deletePathFromFilename(core::stringc& filename);

IReadFile* File;

core::array<SZipFileEntry> FileList;

bool IgnoreCase;
bool IgnorePaths;
};

} // end namespace io
} // end namespace engine

#endif

