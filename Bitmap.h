#ifndef __PASER_BMP_H__
#define __PASER_BMP_H__

#pragma once
#include <Windows.h>
#include<stdio.h>

class Size
{
public:
	int _cx, _cy;
public:
	Size(int x, int y) { _cx = x; _cy = y; }
};
class Bitmap
{
public:
	LPVOID				_colorTable;
	LPBYTE				_image;
	LPBITMAPINFOHEADER	_header;
	DWORD				_size;
	int					_colorTableEntries;
	BOOL				_isCompressed;
public:
	Bitmap(void);
	~Bitmap(void);
public:
	inline int		getSize(void) { return _size; }
	inline int		getHeaderSize(void) { return sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * _colorTableEntries; }
public:
	DWORD	bpi(void);
	BOOL	open(const char *filename);
	void	computePaletteSize(int iBitCount);
	void	empty(void);
	int		getWidth(void);
	int		getHeight(void);
	int		computeMetrics(const Size& szImage, int iBitCount);
};

#endif