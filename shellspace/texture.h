/*
    Shellspace - One tiny step towards the VR Desktop Operating System
    Copyright (C) 2015  Wade Brainerd

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "registry.h"

uint Texture_GetDataSize( uint width, uint height, SxTextureFormat format );

void Texture_Resize( STexture *texture, uint width, uint height, SxTextureFormat format );
void Texture_Update( STexture *texture, uint x, uint y, uint width, uint height, const void *data );
void Texture_Present( STexture *texture );
void Texture_Decommit( STexture *texture );

sbool Texture_DecompressJpeg( const void *jpegData, uint jpegSize, uint *width, uint *height, SxTextureFormat *format, void **data );
sbool Texture_LoadSvg( const char *svg, uint *widthOut, uint *heightOut, SxTextureFormat *formatOut, void **dataOut );
sbool Texture_LoadBitmap( SkBitmap *bitmap, uint *widthOut, uint *heightOut, SxTextureFormat *formatOut, void **dataOut );

#endif
