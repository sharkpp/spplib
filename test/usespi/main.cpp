// usespi.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <windows.h>
#include <time.h>
#include "../../spplib/usespi.hpp"
#include "../../spplib/file.hpp"

#define GetPercent(a,b)	(int)((0==(DWORD)(b))?0:((DWORD)(a)*100/(DWORD)(b)))
#define GetPercentDecimal1(a,b)	(int)((0==(DWORD)(b))?0:(((DWORD)(a)*10000/(DWORD)(b)+5)/10%10))

DWORD GetBitmapBitsSizeAndOffset(const LPBITMAPINFO lpbi, DWORD & dwOffBits)
{
	dwOffBits   = sizeof(BITMAPINFOHEADER);
	LONG nLines = lpbi->bmiHeader.biWidth;

	switch (lpbi->bmiHeader.biBitCount) {
		case 1:
			dwOffBits += sizeof(RGBQUAD) * 2;
			//１ラインのサイズ(まずはバイト単位にそろえる)
			nLines = (nLines +7) >> 3;
			break;
		case 4:
			dwOffBits += sizeof(RGBQUAD) * 16;
			//１ラインのサイズ(まずはバイト単位にそろえる)
			nLines = (nLines +1) >> 1;
			break;
		case 8:
			dwOffBits += sizeof(RGBQUAD) * 256;
			//nLines = nLines;
			break;
		case 16:
			//マスクがあるか
			if(BI_BITFIELDS == lpbi->bmiHeader.biCompression)
				dwOffBits += sizeof(DWORD)*3;
			nLines *= 2;
			break;
		case 24:
			nLines *= 3;
			break;
		case 32:
			//マスクがあるか
			if(BI_BITFIELDS == lpbi->bmiHeader.biCompression)
				dwOffBits += sizeof(DWORD)*3;
			nLines *= 4;
			break;
		default:	//知らない
			;
	}

	return ((!lpbi->bmiHeader.biSizeImage) ?
			((nLines +3) & ~3) * abs(lpbi->bmiHeader.biHeight) : /* 4バイト境界 */
			lpbi->bmiHeader.biSizeImage);
}

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace spplib;

	if( argc < 2 )
	{
		printf("usage: usespi filename\n");
		return 0;
	}

	spi::loader ldr;

	unsigned int  size = 0;
	unsigned char *buff= NULL;

	if( !file::get(argv[1], &buff, &size) )
	{
		printf("can't open file '%s'\n", argv[1]);
		return 1;
	}

	spi::plugin * plg = ldr.IsSupported(argv[1], (DWORD)buff);

	if( !plg )
	{
		printf("plugin: not found\n");
	}
	else
	{
		spi::SPI_ERROR_TYPE err;
		TCHAR szPath[256];
		::GetModuleFileName(*plg, szPath, 255);

		printf("plugin: '%s'\n", szPath);

		switch( plg->GetPluginVersion() )
		{
		case spi::SPI_TYPE_00IN: {
			printf("type: 00IN\n");
			spi::picture * pictplg = static_cast<spi::picture*>( plg );

			spi::PICTUREINFO pictinfo = { 0 };
			err = pictplg->GetPictureInfo((LPSTR)buff, size,
			                              spi::SPI_INPUT_MEMORY, &pictinfo);
			if( spi::SPI_ERROR_SUCCEED != err )
			{
				printf("GetPictureInfo() was error result %d\n", err);
			}
			else
			{
				printf("pictinfo\n");
				printf("  .left:       %d\n", pictinfo.left);
				printf("  .top:        %d\n", pictinfo.top);
				printf("  .width:      %d\n", pictinfo.width);
				printf("  .height:     %d\n", pictinfo.height);
				printf("  .x_density:  %d\n", pictinfo.x_density);
				printf("  .y_density:  %d\n", pictinfo.y_density);
				printf("  .colorDepth: %d\n", pictinfo.colorDepth);
				printf("  .hInfo:      \"%s\"\n", pictinfo.hInfo ? ::LocalLock(pictinfo.hInfo) : "");
				
				::LocalUnlock(pictinfo.hInfo);
				::LocalFree(pictinfo.hInfo);
			}

			HLOCAL  hpbi = NULL, hpDIBits = NULL;
			err = pictplg->GetPicture((LPSTR)buff, size, spi::SPI_INPUT_MEMORY,
			                          &hpbi, &hpDIBits, NULL, 0);
			if( spi::SPI_ERROR_SUCCEED != err )
			{
				printf("GetPicture() was error result %d\n", err);
			}
			else
			{
				BITMAPINFO* lpbi   = (BITMAPINFO *)::LocalLock(hpbi);
				LPBYTE      lpbits = (LPBYTE)::LocalLock(hpDIBits);
				
				DWORD dwOffset = 0;
				DWORD dwOffBits= 0;
				DWORD dwSize   = GetBitmapBitsSizeAndOffset(lpbi, dwOffBits);
				BITMAPFILEHEADER bmfi
					= { 0x4D42, sizeof(BITMAPFILEHEADER) + dwOffBits + dwSize,
						0, 0, sizeof(BITMAPFILEHEADER) + dwOffBits };
				file::put("test.bmp", &bmfi, sizeof(bmfi), dwOffset); dwOffset += sizeof(bmfi);
				file::put("test.bmp", lpbi, dwOffBits, dwOffset);     dwOffset += dwOffBits;
				file::put("test.bmp", lpbits, dwSize, dwOffset);      dwOffset += dwSize;

				::LocalUnlock(hpbi);
				::LocalUnlock(hpDIBits);
				::LocalFree(hpbi);
				::LocalFree(hpDIBits);
			}

			break; }
		case spi::SPI_TYPE_00AM: {
			printf("type: 00AM\n");
			spi::archive * arcplg = static_cast<spi::archive*>( plg );

			HLOCAL  lphInfo = NULL;
			err = arcplg->GetArchiveInfo((LPSTR)buff, size,
			                             spi::SPI_INPUT_MEMORY, &lphInfo);
			if( spi::SPI_ERROR_SUCCEED != err )
			{
				printf("GetPicture() was error result %d\n", err);
			}
			else
			{
				spi::FILEINFO* lpInfo = (spi::FILEINFO *)::LocalLock(lphInfo);

				printf("Original   Packed     Position   Ratio   Date       Time     Type     CRC       Name\n");
				printf("---------- ---------- ---------- ------- ---------- -------- -------- --------  ------------------------------");

				for(; lpInfo->method[0]; ++lpInfo )
				{
					struct tm *ptm = ::_localtime32(&lpInfo->timestamp);
					printf("\n%10d %10d %10d %4d.%d%% %04d-%02d-%02d %02d:%02d:%02d %-8s %08X  %s%s",
							lpInfo->filesize,
							lpInfo->compsize,
							lpInfo->position,
							GetPercent(lpInfo->compsize, lpInfo->filesize),
							GetPercentDecimal1(lpInfo->compsize, lpInfo->filesize),
							(ptm->tm_year+1900), ptm->tm_mon+1, ptm->tm_mday,
							ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
							lpInfo->method, lpInfo->crc,
							lpInfo->path, lpInfo->filename
						);
				}
				printf("\n");

				::LocalUnlock(lphInfo);
				::LocalFree(lphInfo);
			}

			break; }
		}
	}

	delete [] buff;

	return 0;
}

