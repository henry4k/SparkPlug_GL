#include <SparkPlug/Common.h>
#include <SparkPlug/GL/Context.h>


namespace SparkPlug
{
namespace GL
{

int GetInteger( GLenum property )
{
	int r = 0;
	glGetIntegerv(property, &r);
	return r;
}

float GetFloat( GLenum property )
{
	float r = 0.0f;
	glGetFloatv(property, &r);
	return r;
}

/// --- Limits ---
Limits::Limits( Context* context ) :
	m_Context(context)
{
	maxColorAttachments = GetInteger(GL_MAX_COLOR_ATTACHMENTS);
	maxDrawBuffers      = GetInteger(GL_MAX_DRAW_BUFFERS);
	
	maxVertexTextureUnits = GetInteger(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	maxFragmentTextureUnits = GetInteger(GL_MAX_TEXTURE_IMAGE_UNITS);
	maxCombinedTextureUnits = GetInteger(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	
	maxTextureSize[TextureType_1D] = GetInteger(GL_MAX_TEXTURE_SIZE);
	maxTextureSize[TextureType_2D] = GetInteger(GL_MAX_TEXTURE_SIZE);
	maxTextureSize[TextureType_3D] = GetInteger(GL_MAX_3D_TEXTURE_SIZE);
	maxTextureSize[TextureType_Rect] = GetInteger(GL_MAX_RECTANGLE_TEXTURE_SIZE);
	maxTextureSize[TextureType_CubeMap] = GetInteger(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
	
	maxTextureCoords    = GetInteger(GL_MAX_TEXTURE_COORDS);
	maxVertexAttributes = GetInteger(GL_MAX_VERTEX_ATTRIBS);
	
	if (GLEW_EXT_texture_filter_anisotropic)
		maxTextureAnisotropy = GetFloat(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
	else
		maxTextureAnisotropy = 1.f;
	
// 	print();
}

void Limits::print() const
{
#define LOG_INT(var) Log("%s = %d", #var, var)
	LOG_INT(maxColorAttachments);
	LOG_INT(maxDrawBuffers);
	LOG_INT(maxVertexTextureUnits);
	LOG_INT(maxFragmentTextureUnits);
	LOG_INT(maxCombinedTextureUnits);
	LOG_INT(maxTextureSize[TextureType_1D]);
	LOG_INT(maxTextureSize[TextureType_2D]);
	LOG_INT(maxTextureSize[TextureType_3D]);
	LOG_INT(maxTextureSize[TextureType_Rect]);
	LOG_INT(maxTextureSize[TextureType_CubeMap]);
	LOG_INT(maxTextureCoords);
	LOG_INT(maxVertexAttributes);
#undef LOG_INT
	Log("maxTextureAnisotropy = %f", maxTextureAnisotropy);
}

/// --- Bindings

TextureBinding::TextureBinding( Context* context, int unit, const StrongRef<Texture>& texture ) :
	m_Context(context),
	m_Unit(unit),
	m_Previous(m_Context->boundTexture(m_Unit))
{
	m_Context->bindTexture(m_Unit, texture);
}

TextureBinding::~TextureBinding()
{
	m_Context->bindTexture(m_Unit, m_Previous);
}



SamplerBinding::SamplerBinding( Context* context, int unit, const StrongRef<Sampler>& texture ) :
	m_Context(context),
	m_Unit(unit),
	m_Previous(m_Context->boundSampler(m_Unit))
{
	m_Context->bindSampler(m_Unit, texture);
}

SamplerBinding::~SamplerBinding()
{
	m_Context->bindSampler(m_Unit, m_Previous);
}



ProgramBinding::ProgramBinding( Context* context, const StrongRef<Program>& program ) :
	m_Context(context),
	m_Previous(m_Context->boundProgram())
{
	m_Context->bindProgram(program);
}

ProgramBinding::~ProgramBinding()
{
	m_Context->bindProgram(m_Previous);
}



/// --- Context ---
Context::Context() :
	m_Limits(NULL),
	m_ActiveTextureUnit(-1),
	m_Textures(NULL),
	m_Samplers(NULL)
{
}

Context::~Context()
{
	if(m_Limits)
		delete m_Limits;
	
	if(m_Textures)
		delete[] m_Textures;
	
	if(m_Samplers)
		delete[] m_Samplers;
}

void Context::postInit()
{
	m_Limits = new Limits(this);
	m_Textures = new StrongRef<Texture>[limits().maxCombinedTextureUnits];
	m_Samplers = new StrongRef<Sampler>[limits().maxCombinedTextureUnits];
	selectTextureUnit(0); // Cause -1 is illogical and introduces errors with some functions
}

const Limits& Context::limits() const
{
	assert(m_Limits);
	return *m_Limits;
}


/// Texture ///
int Context::activeTextureUnit() const
{
	return m_ActiveTextureUnit;
}

void Context::selectTextureUnit( int unit )
{
	if(m_ActiveTextureUnit == unit)
		return;

	glActiveTextureARB(GL_TEXTURE0_ARB+unit);
	m_ActiveTextureUnit = unit;
}

void Context::bindTexture( int unit, const StrongRef<Texture>& texture )
{
	assert(InsideArray(unit, limits().maxCombinedTextureUnits));
	
	if(m_Textures[unit] == texture)
		return;
	
	selectTextureUnit(unit);
	
	if(!texture)
	{
		glBindTexture(ConvertToGL(m_Textures[unit]->type()), 0);
		m_Textures[unit] = NULL;
	}
	else
	{
		if(!m_Textures[unit] || (m_Textures[unit]->type() != texture->type()))
		{
			if(m_Textures[unit])
				glDisable(ConvertToGL(m_Textures[unit]->type()));
			glEnable(ConvertToGL(texture->type()));
		}
		
		glBindTexture(ConvertToGL(texture->type()), texture->handle());
		m_Textures[unit] = texture;
	}
}

const StrongRef<Texture>& Context::boundTexture( int unit ) const
{
	assert(InsideArray(unit, limits().maxCombinedTextureUnits));
	return m_Textures[unit];
}


/// Sampler ///
void Context::bindSampler( int unit, const StrongRef<Sampler>& sampler )
{
	if(sampler)
		glBindSampler(unit, sampler->handle());
	else
		glBindSampler(unit, 0);
	m_Samplers[unit] = sampler;
}

const StrongRef<Sampler>& Context::boundSampler( int unit ) const
{
	assert(InsideArray(unit, limits().maxCombinedTextureUnits));
	return m_Samplers[unit];
}


/// Shader ///
void Context::bindProgram( const StrongRef<Program>& program )
{
	if(m_Program == program)
		return;
	
	if(!program)
	{
		if(!m_Program)
			return;

		glUseProgram(0);
		m_Program = NULL; 
	}

	glUseProgram(program->handle());
	m_Program = program;
}

const StrongRef<Program>& Context::boundProgram() const
{
	return m_Program;
}




}
}