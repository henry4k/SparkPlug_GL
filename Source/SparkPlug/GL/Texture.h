#ifndef __SPARKPLUG_TEXTURE__
#define __SPARKPLUG_TEXTURE__ 

#include <SparkPlug/Reference.h>
#include <SparkPlug/GL/OpenGL.h>
#include <SparkPlug/GL/Enums.h>
#include <SparkPlug/GL/Object.h>




namespace SparkPlug
{

class Image;
class PixelFormat;

namespace GL
{

class SamplerBase : public Object
{
public:
	SamplerBase( Context* context ) : Object(context) {}
	virtual ~SamplerBase() {}

	TextureFilter filter() const;
	virtual void setFilter( TextureFilter filter );
	
	TextureAddressMode addressMode() const;
	virtual void setAddressMode( TextureAddressMode mode );
	
	float maxAnisotropic() const;
	virtual void setMaxAnisotropic( float level );

private:
	TextureFilter      m_Filter;
	TextureAddressMode m_AddressMode;
	float              m_MaxAnisotropic;
};

class Texture : public SamplerBase
{
public:
	static StrongRef<Texture> Create( Context* context, TextureType type );
	static StrongRef<Texture> CreateFromImage( Context* context, TextureType type, const Image& image );
	virtual ~Texture();
	
	TextureType type() const;
	bool isPowerOfTwo() const;
	
	bool hasMipMaps() const;
	int mipMapLevels() const;
	
	void setFilter( TextureFilter f );
	void setAddressMode( TextureAddressMode m );
	void setMaxAnisotropic( float level );
	
private:
	Texture( Context* context, TextureType type );
	static bool UploadTextureRaw( TextureType type, bool proxy, int level, const PixelFormat& format, bool sRGB, int width, int height, int depth, bool border, const void* data );
	static bool TestTextureCreation( TextureType type, int width, int height, int depth, PixelFormat format, bool sRGB );
	
	TextureType m_Type;
	int         m_MipMapLevels;
	int         m_Width;
	int			m_Height;
    int			m_Depth;
};

}
}

#endif
