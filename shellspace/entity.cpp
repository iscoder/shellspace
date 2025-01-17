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
#include "common.h"
#include "entity.h"
#include "command.h"
#include "file.h"
#include "reflist.h"
#include "registry.h"
#include <GlProgram.h>


struct SEntityGlobals
{
	OVR::GlProgram 	shader;
	SRef 			firstRoot;
};


SEntityGlobals	s_ent;


void Entity_Init()
{
	s_ent.firstRoot = S_NULL_REF;
}


void Entity_LoadShaders( const char *vertexName, const char *fragmentName )
{
	char	*vertexText;
	char	*fragmentText;

	vertexText = (char *)File_Read( vertexName, NULL );
	fragmentText = (char *)File_Read( fragmentName, NULL );

	if ( vertexText && fragmentText )
		s_ent.shader = OVR::BuildProgram( vertexText, fragmentText );

	if ( vertexText )
		free( vertexText );

	if ( fragmentText )
		free( fragmentText );
}


void Entity_DrawEntity( SEntity *entity, const OVR::Matrix4f &view )
{
	STexture 	*texture;
	SGeometry	*geometry;
	uint 		geometryIndex;
	uint 		textureIndex;
	GLuint 		texId;
	float 		uScale;
	float 		vScale;
	GLuint 		vertexArrayObject;
	int 		triCount;
	int 		indexOffset;
	int 		batchTriCount;
	int 		triCountLeft;

	Prof_Start( PROF_DRAW_ENTITY );

	assert( entity );

	OVR::GL_CheckErrors( "before Entity_DrawEntity" );

	geometry = Registry_GetGeometry( entity->geometryRef );
	assert( geometry );

	geometryIndex = geometry->drawIndex % BUFFER_COUNT;

	vertexArrayObject = geometry->vertexArrayObjects[geometryIndex];
	if ( !vertexArrayObject )
	{
		Prof_Stop( PROF_DRAW_ENTITY );
		return;
	}

	glUseProgram( s_ent.shader.program );

	glUniformMatrix4fv( s_ent.shader.uMvp, 1, GL_FALSE, view.Transposed().M[0] );

	glBindVertexArrayOES_( vertexArrayObject );

	glActiveTexture( GL_TEXTURE0 );

	if ( entity->textureRef != S_NULL_REF )
	{
		texture = Registry_GetTexture( entity->textureRef );
		assert( texture );

		textureIndex = texture->drawIndex % BUFFER_COUNT;
		texId = texture->texId[textureIndex];

		glBindTexture( GL_TEXTURE_2D, texId );

		if ( texId )
		{
			assert( texture->texWidth[textureIndex] );
			assert( texture->texHeight[textureIndex] );

			uScale = (float)texture->width / texture->texWidth[textureIndex];
			vScale = (float)texture->height / texture->texHeight[textureIndex];

			glUniform4f( s_ent.shader.uColor, uScale, vScale, 1.0f, 1.0f );
		}
		else
		{
			glUniform4f( s_ent.shader.uColor, 1.0f, 1.0f, 1.0f, 1.0f );
		}

		if ( texture->format == SxTextureFormat_R8G8B8A8 ||
			 texture->format == SxTextureFormat_R8G8B8A8_SRGB )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D, 0 );
		glUniform4f( s_ent.shader.uColor, 1.0f, 1.0f, 1.0f, 1.0f );
		glDisable( GL_BLEND );
	}

	indexOffset = 0;
	triCount = geometry->indexCounts[geometryIndex] / 3;

	triCountLeft = triCount;

	while ( triCountLeft )
	{
#if USE_SPLIT_DRAW
		batchTriCount = S_Min( triCountLeft, S_Max( 1, triCount / 10 ) );
#else // #if USE_SPLIT_DRAW
		batchTriCount = triCount;
#endif // #else // #if USE_SPLIT_DRAW

		glDrawElements( GL_TRIANGLES, batchTriCount * 3, GL_UNSIGNED_SHORT, (void *)indexOffset );

		indexOffset += batchTriCount * sizeof( ushort ) * 3;
		triCountLeft -= batchTriCount;
	}

	glBindVertexArrayOES_( 0 );

	glBindTexture( GL_TEXTURE_2D, 0 );

	glDisable( GL_BLEND );

	OVR::GL_CheckErrors( "after Entity_DrawEntity" );

	Prof_Stop( PROF_DRAW_ENTITY );
}


void Entity_DrawChildren( const OVR::Matrix4f &view, const SxTransform& xform, SRef first )
{
	SRef  			ref;
	SEntity 		*entity;
	SxTransform		entityXform;
	SxTransform		childXform;
	OVR::Matrix4f 	m;

	for ( ref = first; ref != S_NULL_REF; ref = entity->parentLink.next )
	{
		entity = Registry_GetEntity( ref );
		assert( entity );

		if ( entity->visibility <= 0.0f )
			continue;

		OrientationToTransform( entity->orientation, &entityXform );
		ConcatenateTransforms( xform, entityXform, &childXform );

		// if ( strstr( entity->id, "vnc" ) )
		// {		
		// 	S_Log( "entityXform %s:", entity->id );
		// 	S_Log( "xAxis: %f %f %f", entityXform.axes.x.x, entityXform.axes.x.y, entityXform.axes.x.z );
		// 	S_Log( "yAxis: %f %f %f", entityXform.axes.y.x, entityXform.axes.y.y, entityXform.axes.y.z );
		// 	S_Log( "zAxis: %f %f %f", entityXform.axes.z.x, entityXform.axes.z.y, entityXform.axes.z.z );
		// 	S_Log( "origin: %f %f %f", entityXform.origin.x, entityXform.origin.y, entityXform.origin.z );
		// 	S_Log( "scale: %f %f %f", entityXform.scale.x, entityXform.scale.y, entityXform.scale.z );
		// 	S_Log( "childXform %s:", entity->id );
		// 	S_Log( "xAxis: %f %f %f", childXform.axes.x.x, childXform.axes.x.y, childXform.axes.x.z );
		// 	S_Log( "yAxis: %f %f %f", childXform.axes.y.x, childXform.axes.y.y, childXform.axes.y.z );
		// 	S_Log( "zAxis: %f %f %f", childXform.axes.z.x, childXform.axes.z.y, childXform.axes.z.z );
		// 	S_Log( "origin: %f %f %f", childXform.origin.x, childXform.origin.y, childXform.origin.z );
		// 	S_Log( "scale: %f %f %f", childXform.scale.x, childXform.scale.y, childXform.scale.z );
		// }

		m = OVR::Matrix4f( 
			childXform.axes.x.x * childXform.scale.x, childXform.axes.x.y * childXform.scale.x, childXform.axes.x.z * childXform.scale.x, 0.0f,
			childXform.axes.y.x * childXform.scale.y, childXform.axes.y.y * childXform.scale.y, childXform.axes.y.z * childXform.scale.y, 0.0f,
			childXform.axes.z.x * childXform.scale.z, childXform.axes.z.y * childXform.scale.z, childXform.axes.z.z * childXform.scale.z, 0.0f,
			childXform.origin.x, childXform.origin.y, childXform.origin.z, 1.0f );

		Entity_DrawEntity( entity, view * m.Transposed() );

		if ( entity->firstChild != S_NULL_REF )
		{
			// S_Log( "%s has children", entity->id );
			Entity_DrawChildren( view, childXform, entity->firstChild );
		}
	}
}


void Entity_Draw( const OVR::Matrix4f &view )
{
	SxTransform		xform;

	IdentityTransform( &xform );

	Entity_DrawChildren( view, xform, s_ent.firstRoot );
}


void Entity_Register( SEntity *entity )
{
	entity->visibility = 1.0f;
	IdentityOrientation( &entity->orientation );

	entity->parentRef = S_NULL_REF;
	entity->parentLink.prev = S_NULL_REF;
	entity->parentLink.next = S_NULL_REF;
	entity->firstChild = S_NULL_REF;

	RefList_Insert( entity, offsetof( SEntity, parentLink ), &s_ent.firstRoot );
}


void Entity_Unregister( SEntity *entity )
{
	Entity_SetParent( entity, S_NULL_REF );

	RefList_Remove( entity, offsetof( SEntity, parentLink ), &s_ent.firstRoot );
}


void Entity_SetParent( SEntity *entity, SRef parentRef )
{
	SEntity *parent;

	if ( entity->parentRef != S_NULL_REF )
	{
		parent = Registry_GetEntity( entity->parentRef );
		assert( parent );

		RefList_Remove( entity, offsetof( SEntity, parentLink ), &parent->firstChild );
	}
	else
	{
		RefList_Remove( entity, offsetof( SEntity, parentLink ), &s_ent.firstRoot );
	}

	entity->parentRef = parentRef;

	if ( parentRef != S_NULL_REF )
	{
		parent = Registry_GetEntity( parentRef );
		assert( parent );

		RefList_Insert( entity, offsetof( SEntity, parentLink ), &parent->firstChild );
	}
	else
	{
		RefList_Insert( entity, offsetof( SEntity, parentLink ), &s_ent.firstRoot );
	}
}


sbool Entity_Command()
{
	if ( strcasecmp( Cmd_Argv( 0 ), "entity" ) == 0 )
	{
		if ( strcasecmp( Cmd_Argv( 1 ), "shaders" ) == 0 )
		{
			if ( Cmd_Argc() != 4 )
			{
				S_Log( "Usage: entity shaders <vertex> <pixel>" );
				return strue;
			}

			Entity_LoadShaders( Cmd_Argv( 2 ), Cmd_Argv( 3 ) );

			return strue;
		}
	}

	return sfalse;
}



