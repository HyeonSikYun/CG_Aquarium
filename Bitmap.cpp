#include "Bitmap.h"

Bitmap::Bitmap(void)
{
	_header = NULL;
	_size = 0;
	_image = NULL;
	_isCompressed = FALSE;
	empty();
}

Bitmap::~Bitmap(void)
{
	empty();
}

DWORD Bitmap::bpi(void)
{
	DWORD dwBytes = ((DWORD)_header->biWidth * _header->biBitCount) / 32;
	if (((DWORD)_header->biWidth * _header->biBitCount) % 32)
		dwBytes++;
	dwBytes *= 4;
	return dwBytes;
}

BOOL Bitmap::open(const char *filename)
{
	empty();
	int iCount, iSize;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	LPBYTE pbyBuffer;

	FILE* fp = NULL;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL) {
		printf("file open error\n");
		return FALSE;
	}

	iCount = fread((LPVOID)&bmfh, sizeof(BITMAPFILEHEADER), 1, fp);
	if (iCount != 1) {
		printf("file read error\n");
		return FALSE;
	}
	if (bmfh.bfType != 0x4d42) {
		printf("invalid file\n");
		return FALSE;
	}
	iSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
	pbyBuffer = new BYTE[iSize];
	iCount = fread(pbyBuffer, iSize, 1, fp); // info hdr & color table
	bmih = *(BITMAPINFOHEADER*)pbyBuffer;

	_size = bmih.biSizeImage;

	if (_size == 0)
		_size = computeMetrics(Size(bmih.biWidth, bmih.biHeight), bmih.biBitCount);
	computePaletteSize(bmih.biBitCount);

	_header = (LPBITMAPINFOHEADER)new unsigned char[sizeof(BITMAPINFOHEADER) + _colorTableEntries * sizeof(RGBQUAD)];
	memcpy(_header, pbyBuffer, iSize);
	delete[] pbyBuffer;
	_colorTable = (LPBYTE)_header + sizeof(BITMAPINFOHEADER);
	_image = new BYTE[_size];

	iCount = fread(_image, _size, 1, fp);
	fclose(fp);
	return TRUE;
}

void Bitmap::computePaletteSize(int iBitCount)
{
	if (_header == NULL || _header->biClrUsed == 0 || iBitCount > 8) {
		switch (iBitCount) {
		case 1:
			_colorTableEntries = 2;
			break;
		case 4:
			_colorTableEntries = 16;
			break;
		case 8:
			_colorTableEntries = 256;
			break;
		case 16:
		case 24:
		case 32:
			_colorTableEntries = 0;
			break;
		default:
			;
		}
	}
	else {
		_colorTableEntries = _header->biClrUsed;
	}
}

void Bitmap::empty(void)
{
	if (_header) {
		delete[] _header;
		_header = NULL;
	}

	if (_image) {
		delete[] _image;
		_image = NULL;
	}

	_colorTable = NULL;
	_colorTableEntries = 0;
	_size = 0;
	_isCompressed = FALSE;
}

int Bitmap::computeMetrics(const Size& szImage, int iBitCount)
{
	int iBytes = (szImage._cx * iBitCount) / 32;
	if ((szImage._cx * iBitCount) % 32) {
		iBytes++;
	}
	iBytes *= (4 * szImage._cy);

	return iBytes;
}

int Bitmap::getWidth(void)
{
	if (_header == NULL) return 0;
	return _header->biWidth;
}

int Bitmap::getHeight(void)
{
	if (_header == NULL) return 0;
	return _header->biHeight;
}