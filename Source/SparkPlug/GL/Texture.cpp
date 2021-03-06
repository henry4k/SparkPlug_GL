#include <SparkPlug/Common.h>
#include <SparkPlug/Math.h>
#include <SparkPlug/Pixel.h>
#include <SparkPlug/Image.h>
#include <SparkPlug/GL/Pixel.h>
#include <SparkPlug/GL/OpenGL.h>
#include <SparkPlug/GL/Context.h>
#include <SparkPlug/GL/Texture.h>

namespace SparkPlug
{
namespace GL
{

TextureFilter SamplerBase::filter() const
{
	return m_Filter;
}

void SamplerBase::setFilter( TextureFilter filter )
{
	m_Filter = filter;
}

TextureAddressMode SamplerBase::addressMode() const
{
	return m_AddressMode;
}

void SamplerBase::setAddressMode( TextureAddressMode mode )
{
	m_AddressMode = mode;
}

float SamplerBase::maxAnisotropic() const
{
	return m_MaxAnisotropic;
}

void SamplerBase::setMaxAnisotropic( float level )
{
	m_MaxAnisotropic = level;
}

bool Texture::UploadTextureRaw( TextureType type, bool proxy, int level, const PixelFormat& format, bool sRGB, int width, int height, int depth, bool border, const void* data )
{
	GLenum typeGL   = proxy ? ConvertToProxyGL(type) : ConvertToGL(type);
	GLenum formatGL = ConvertToGL(format, sRGB);
	int borderGL = border ? 1 : 0;
	GLenum semanticGL      = ConvertToGL(format.semantic());
	GLenum componentTypeGL = ConvertToGL(format.componentType());

	switch(type)
	{
		case TextureType_1D:
			glTexImage1D(typeGL, level, formatGL, width, borderGL, semanticGL, componentTypeGL, data);
			break;
		
		case TextureType_3D:
			glTexImage3D(typeGL, level, formatGL, width, height, depth, borderGL, semanticGL, componentTypeGL, data);
			break;
		
		case TextureType_2D:
		case TextureType_Rect:
		case TextureType_CubeMap:
			glTexImage2D(typeGL, level, formatGL, width, height, borderGL, semanticGL, componentTypeGL, data);
			break;
		
		default:
			FatalError("Invalid texture type: %u", type);
	}
	
	GLint realWidth;
	glGetTexLevelParameteriv(typeGL, 0, GL_TEXTURE_WIDTH, &realWidth);
	
	if(realWidth == 0)
	{
		LogError("Cannot create %s texture:", AsString(type));
		LogError("Size: %d, %d, %d", width, height, depth);
		LogError("Format: %s (sRGB=%d)", format.asString().c_str(), (int)sRGB);
		return false;
	}
	return true;
}

bool Texture::TestTextureCreation( TextureType type, int width, int height, int depth, PixelFormat format, bool sRGB )
{
	return UploadTextureRaw(
			type,
			true, // proxy
			0,    // level
			format,
			sRGB,
			width, height, depth,
			false, // border
			NULL
	);
}

StrongRef<Texture> Texture::Create( Context* context, TextureType type )
{
	Texture* texture = new Texture(context, type);
	return StrongRef<Texture>(texture);
}

StrongRef<Texture> Texture::CreateFromImage( Context* context, TextureType type, const Image& image )
{
	bool sRGB = false;
	bool border = false;
	StrongRef<Texture> texture = new Texture(context, type);
	
	TextureBinding binding(context, context->activeTextureUnit(), texture);
	
	if(!TestTextureCreation(type, image.width(), image.height(), image.depth(), image.format(), sRGB))
	{
		return NULL;
	}
	
	GLenum texType = ConvertToGL(type);
	
	
	if(!UploadTextureRaw(
			type,
			false, // proxy
			0,     // level
			image.format(),
			sRGB,
			image.width(), image.height(), image.depth(),
			border,
			image.pixels()
	))
	{
		// Meep something went wrong :(
		return NULL;
	}
	
	CheckGl();
	
	return texture;
}

Texture::Texture( Context* context, TextureType type ) :
	SamplerBase(context),
	m_Type(type)
{
	enterImmortalSection();
	
	glGenTextures(1, &m_Handle);
	
	setAddressMode(TextureAddressMode_Clamp);
	setFilter(TextureFilter_Trilinear);
	
	{
		TextureBinding binding(context, context->activeTextureUnit(), this);
		glTexParameteri(ConvertToGL(type), GL_GENERATE_MIPMAP, GL_TRUE);
	}
	
	leaveImmortalSection();
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_Handle);
}

TextureType Texture::type() const
{
	return m_Type;
}

bool Texture::isPowerOfTwo() const
{
	return
		((m_Width == 0) || IsPowerOfTwo(m_Width)) &&
		((m_Height == 0) || IsPowerOfTwo(m_Height)) &&
		((m_Depth == 0) || IsPowerOfTwo(m_Depth));
}

bool Texture::hasMipMaps() const
{
	return m_MipMapLevels > 1;
}

int Texture::mipMapLevels() const
{
	return m_MipMapLevels;
}

void Texture::setFilter( TextureFilter f )
{
	if(f == filter())
		return;
	TextureBinding binding(context(), context()->activeTextureUnit(), this);
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_MIN_FILTER, ConvertToGL(f, hasMipMaps()));
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_MAG_FILTER, ConvertToGL(f, false));
	SamplerBase::setFilter(f);
}

void Texture::setAddressMode( TextureAddressMode m )
{
	if(m == addressMode())
		return;
	TextureBinding binding(context(), context()->activeTextureUnit(), this);
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_WRAP_S, ConvertToGL(m));
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_WRAP_T, ConvertToGL(m));
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_WRAP_R, ConvertToGL(m));
	SamplerBase::setAddressMode(m);
}

void Texture::setMaxAnisotropic( float level )
{
	if(level == maxAnisotropic())
		return;
	glTexParameteri(ConvertToGL(type()), GL_TEXTURE_MAX_ANISOTROPY_EXT, level);
	SamplerBase::setMaxAnisotropic(level);
}

}
}
