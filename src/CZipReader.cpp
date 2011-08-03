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
 
#include <string.h>
#include "CZipReader.h"
#include "os.h"

#include "engineCompileConfig.h"
#ifdef _engine_COMPILE_WITH_ZLIB_
    #ifndef _engine_USE_NON_SYSTEM_ZLIB_
    #include <zlib.h> // use system lib
    #else // _engine_USE_NON_SYSTEM_ZLIB_
    #include "zlib/zlib.h"
    #endif // _engine_USE_NON_SYSTEM_ZLIB_
#endif // _engine_COMPILE_WITH_ZLIB_

namespace engine
{
namespace io
{


CZipReader::CZipReader(IReadFile* file, bool ignoreCase, bool ignorePaths)
	: File(file), IgnoreCase(ignoreCase), IgnorePaths(ignorePaths)
{
	#ifdef _DEBUG
	setDebugName("CZipReader");
	#endif

	if (File)
	{
		File->grab();

		// scan local headers
		while (scanLocalHeader()) ;

		// prepare file index for binary search
		FileList.sort();
	}
}

CZipReader::~CZipReader()
{
	if (File)
		File->drop();
}



//! splits filename from zip file into useful filenames and paths
void CZipReader::extractFilename(SZipFileEntry* entry)
{
	s32 lorfn = entry->header.FilenameLength; // length of real file name

	if (!lorfn)
		return;

	if (IgnoreCase)
		entry->zipFileName.make_lower();

	const c8* p = entry->zipFileName.c_str() + lorfn;

	// suche ein slash oder den anfang.

	while (*p!='/' && p!=entry->zipFileName.c_str())
	{
		--p;
		--lorfn;
	}

	bool thereIsAPath = p != entry->zipFileName.c_str();

	if (thereIsAPath)
	{
		// there is a path
		++p;
		++lorfn;
	}

	entry->simpleFileName = p;
	entry->path = "";

	// pfad auch kopieren
	if (thereIsAPath)
	{
		lorfn = (s32)(p - entry->zipFileName.c_str());
		entry->path.append(entry->zipFileName, lorfn);
	}

	if (!IgnorePaths)
		entry->simpleFileName = entry->zipFileName; // thanks to Pr3t3nd3r for this fix
}



//! scans for a local header, returns false if there is no more local file header.
bool CZipReader::scanLocalHeader()
{
	c8 tmp[1024];

	SZipFileEntry entry;
	entry.fileDataPosition = 0;
	memset(&entry.header, 0, sizeof(SZIPFileHeader));

	File->read(&entry.header, sizeof(SZIPFileHeader));

#ifdef PPC
	entry.header.Sig = OSReadSwapInt32(&entry.header.Sig,0);
	entry.header.VersionToExtract = OSReadSwapInt16(&entry.header.VersionToExtract,0);
	entry.header.GeneralBitFlag = OSReadSwapInt16(&entry.header.GeneralBitFlag,0);
	entry.header.CompressionMethod = OSReadSwapInt16(&entry.header.CompressionMethod,0);
	entry.header.LastModFileTime = OSReadSwapInt16(&entry.header.LastModFileTime,0);
	entry.header.LastModFileDate = OSReadSwapInt16(&entry.header.LastModFileDate,0);
	entry.header.DataDescriptor.CRC32 = OSReadSwapInt32(&entry.header.DataDescriptor.CRC32,0);
	entry.header.DataDescriptor.CompressedSize = OSReadSwapInt32(&entry.header.DataDescriptor.CompressedSize,0);
	entry.header.DataDescriptor.UncompressedSize = OSReadSwapInt32(&entry.header.DataDescriptor.UncompressedSize,0);
	entry.header.FilenameLength = OSReadSwapInt16(&entry.header.FilenameLength,0);
	entry.header.ExtraFieldLength = OSReadSwapInt16(&entry.header.ExtraFieldLength,0);
#endif

	if (entry.header.Sig != 0x04034b50)
		return false; // local file headers end here.

	// read filename
	entry.zipFileName.reserve(entry.header.FilenameLength+2);
	File->read(tmp, entry.header.FilenameLength);
	tmp[entry.header.FilenameLength] = 0x0;
	entry.zipFileName = tmp;

	extractFilename(&entry);

	// move forward length of extra field.

	if (entry.header.ExtraFieldLength)
		File->seek(entry.header.ExtraFieldLength, true);

	// if bit 3 was set, read DataDescriptor, following after the compressed data
	if (entry.header.GeneralBitFlag & ZIP_INFO_IN_DATA_DESCRITOR)
	{
		// read data descriptor
		File->read(&entry.header.DataDescriptor, sizeof(entry.header.DataDescriptor));
#ifdef PPC
		entry.header.DataDescriptor.CRC32 = OSReadSwapInt32(&entry.header.DataDescriptor.CRC32,0);
		entry.header.DataDescriptor.CompressedSize = OSReadSwapInt32(&entry.header.DataDescriptor.CompressedSize,0);
		entry.header.DataDescriptor.UncompressedSize = OSReadSwapInt32(&entry.header.DataDescriptor.UncompressedSize,0);
#endif
	}

	// store position in file
	entry.fileDataPosition = File->getPos();

	// move forward length of data
	File->seek(entry.header.DataDescriptor.CompressedSize, true);

	#ifdef _DEBUG
	//os::Debuginfo::print("added file from archive", entry.simpleFileName.c_str());
	#endif

	FileList.push_back(entry);

	return true;
}



//! opens a file by file name
IReadFile* CZipReader::openFile(const c8* filename)
{
	s32 index = findFile(filename);

	if (index != -1)
		return openFile(index);

	return 0;
}



//! opens a file by index
IReadFile* CZipReader::openFile(s32 index)
{
	//0 - The file is stored (no compression)
	//1 - The file is Shrunk
	//2 - The file is Reduced with compression factor 1
	//3 - The file is Reduced with compression factor 2
	//4 - The file is Reduced with compression factor 3
	//5 - The file is Reduced with compression factor 4
	//6 - The file is Imploded
	//7 - Reserved for Tokenizing compression algorithm
	//8 - The file is Deflated
	//9 - Reserved for enhanced Deflating
	//10 - PKWARE Date Compression Library Imploding

	switch(FileList[index].header.CompressionMethod)
	{
	case 0: // no compression
	{
		File->seek(FileList[index].fileDataPosition);
		return createLimitReadFile(FileList[index].simpleFileName.c_str(), File, FileList[index].header.DataDescriptor.UncompressedSize);
	}
	case 8:
	{
		        #ifdef _engine_COMPILE_WITH_ZLIB_

		u32 uncompressedSize = FileList[index].header.DataDescriptor.UncompressedSize;
		u32 compressedSize = FileList[index].header.DataDescriptor.CompressedSize;

		void* pBuf = new c8[ uncompressedSize ];
		if (!pBuf)
		{
			os::Printer::log("Not enough memory for decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
			return 0;
		}

		        #define MAX_BUFSIZE 128*1024

		c8 *pcData = new c8[ MAX_BUFSIZE ];
		if (!pcData)
		{
			os::Printer::log("Not enough memory for decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
			return 0;
		}

		//memset(pcData, 0, compressedSize );
		File->seek(FileList[index].fileDataPosition);
		File->read(pcData, MAX_BUFSIZE );

		// Setup the inflate stream.
		z_stream stream;
		s32 err;

		stream.next_in = (Bytef*)pcData;
		stream.avail_in = (uInt)MAX_BUFSIZE;
		stream.next_out = (Bytef*)pBuf;
		stream.avail_out = uncompressedSize;
		stream.zalloc = (alloc_func)0;
		stream.zfree = (free_func)0;

		// Perform inflation. wbits < 0 indicates no zlib header inside the data.
		err = inflateInit2(&stream, -MAX_WBITS);
		if (err == Z_OK)
		{

			while (err != Z_STREAM_END)
			{

				if (err == Z_BUF_ERROR || stream.avail_in == 0)
				{
					int rByte = File->read(pcData, MAX_BUFSIZE );
					stream.avail_in = rByte;
					stream.next_in = (Bytef*)pcData;
				}
				err = inflate(&stream,  Z_SYNC_FLUSH);
			}

			inflateEnd(&stream);
			if (err == Z_STREAM_END)
				err = Z_OK;

			err = Z_OK;
			inflateEnd(&stream);
		}


		delete[] pcData;

		if (err != Z_OK)
		{
			os::Printer::log("Error decompressing", FileList[index].simpleFileName.c_str(), ELL_ERROR);
			delete [] (c8*)pBuf;
			return 0;
		}
		else
			return io::createMemoryReadFile ( pBuf, uncompressedSize, FileList[index].simpleFileName.c_str(), true);

		        #else
		return 0;         // zlib not compiled, we cannot decompress the data.
		        #endif
	}
		break;
	default:
		os::Printer::log("file has unsupported compression method.", FileList[index].simpleFileName.c_str(), ELL_ERROR);
		return 0;
	};
}



//! returns count of files in archive
s32 CZipReader::getFileCount()
{
	return FileList.size();
}



//! returns data of file
const SZipFileEntry* CZipReader::getFileInfo(s32 index) const
{
	return &FileList[index];
}



//! deletes the path from a filename
void CZipReader::deletePathFromFilename(core::stringc& filename)
{
	// delete path from filename
	const c8* p = filename.c_str() + filename.size();

	// suche ein slash oder den anfang.

	while (*p!='/' && *p!='\\' && p!=filename.c_str())
		--p;

	core::stringc newName;

	if (p != filename.c_str())
	{
		++p;
		filename = p;
	}
}



//! returns fileindex
s32 CZipReader::findFile(const c8* simpleFilename)
{
	SZipFileEntry entry;
	entry.simpleFileName = simpleFilename;

	if (IgnoreCase)
		entry.simpleFileName.make_lower();

	if (IgnorePaths)
		deletePathFromFilename(entry.simpleFileName);

	s32 res = FileList.binary_search(entry);

	#ifdef _DEBUG
	if (res == -1)
	{
		for (u32 i=0; i<FileList.size(); ++i)
			if (FileList[i].simpleFileName == entry.simpleFileName)
			{
				os::Printer::log("File in archive but not found.", entry.simpleFileName.c_str(), ELL_ERROR);
				break;
			}
	}
	#endif

	return res;
}



} // end namespace io
} // end namespace engine

