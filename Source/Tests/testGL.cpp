#include <SparkPlug/Common.h>
#include <SparkPlug/Image.h>
#include <SparkPlug/Time.h>
#include <SparkPlug/GL/Context.h>
#include <SparkPlug/GL/Shader.h>
#include <SparkPlug/GL/Texture.h>
#include <SparkPlug/GL/Buffer.h>
#include <SparkPlug/ImageIo/Loader.h>
#include <GL/glfw.h>

namespace sp = SparkPlug;

class GlfwContext : public sp::GL::Context
{
public:
	GlfwContext()
	{
		if(!glfwInit())
			sp::FatalError("GLFW init failed.");
		
		int major = 0, minor = 0, revision = 0;
		glfwGetVersion(&major, &minor, &revision);
		sp::Log("GLFW: %d.%d.%d", major, minor, revision);
		
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
		//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 0);
		glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		//glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		if(!glfwOpenWindow(1024, 1024, 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
		{
			sp::FatalError("Window creation failed.");
		}
		
		glfwSwapInterval(1);
		
		postInit();
		
		glViewport(0, 0, 1024, 1024);
	
		glClearColor(0.9f, 0.8f, 1.0f, 1.0f);
		glClearDepth(1);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glPointSize(4);
		glLineWidth(2);
		
		// glFrontFace(GL_CCW); // <- Ist eh default
		// glCullFace(GL_BACK);
	// 	glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		
// 		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	
	~GlfwContext()
	{
		glfwTerminate();
	}
	
	void swapBuffers()
	{
		glfwSwapBuffers();
	}
};

struct V3T2
{
	sp::vec3f v;
	sp::vec2f t;
	
	V3T2( const sp::vec3f& v, const sp::vec2f& t ) :
		v(v),
		t(t)
	{
	}
};

int main()
{
	GlfwContext ctx;
	
	sp::StrongRef<sp::GL::VertexBuffer> quadVertexBuffer = sp::GL::VertexBuffer::Create(&ctx, sp::GL::VertexFormat::V3T2, 4, SparkPlug::GL::BufferUsage_Static);
	V3T2* data = (V3T2*)quadVertexBuffer->map(sp::GL::BufferMapMode_WriteOnly);
		data[0].v = sp::vec3f(0,0,0);
		data[0].t = sp::vec2f(0,0);
		
		data[1].v = sp::vec3f(0,1,0);
		data[1].t = sp::vec2f(0,1);
		
		data[2].v = sp::vec3f(1,0,0);
		data[2].t = sp::vec2f(1,0);
		
		data[3].v = sp::vec3f(1,1,0);
		data[3].t = sp::vec2f(1,1);
	quadVertexBuffer->unmap();
	
	
	sp::GL::BufferBinding vboBinding(&ctx, quadVertexBuffer);
	ctx.setVertexFormat(quadVertexBuffer->format(), NULL);
	
	
	/// Texture
	sp::Image image;
	if(!sp::ImageIo::LoadFromFile(&image, "Resources/Images/Solid.png", sp::ImageIo::ImageFormat_Png))
		sp::FatalError("Can't load image from file.");
	
	sp::StrongRef<sp::GL::Texture> diffuse = sp::GL::Texture::CreateFromImage(
		&ctx,
		sp::GL::TextureType_2D,
		image
	);
	
	
	
	if(!sp::ImageIo::LoadFromFile(&image, "Resources/Images/Gradients/colorcorrection4.png", sp::ImageIo::ImageFormat_Png))
		sp::FatalError("Can't load image from file.");
	
	sp::StrongRef<sp::GL::Texture> gradient = sp::GL::Texture::CreateFromImage(
		&ctx,
		sp::GL::TextureType_1D,
		image
	);
	
	
	if(!sp::ImageIo::LoadFromFile(&image, "Resources/Images/Additive.png", sp::ImageIo::ImageFormat_Png))
		sp::FatalError("Can't load image from file.");
	
	sp::StrongRef<sp::GL::Texture> additive = sp::GL::Texture::CreateFromImage(
		&ctx,
		sp::GL::TextureType_2D,
		image
	);
	additive->setAddressMode(sp::GL::TextureAddressMode_Repeat);
	
	
	/// Shader
	sp::StrongRef<sp::GL::Shader>  vert = sp::GL::Shader::CreateFromFile(&ctx, sp::GL::ShaderType_Vertex,   "Resources/Shaders/Test.vert");
	sp::StrongRef<sp::GL::Shader>  frag = sp::GL::Shader::CreateFromFile(&ctx, sp::GL::ShaderType_Fragment, "Resources/Shaders/Test.frag");
	sp::StrongRef<sp::GL::Program> program = sp::GL::Program::Create(&ctx);
	program->attach(vert);
	program->attach(frag);
	program->link();
	program->setAttributes(quadVertexBuffer->format());
	ctx.bindProgram(program);
	
	sp::GL::TextureBinding diffuseBinding(&ctx, 0, diffuse);
	program->setUniform("Diffuse", int(0));
	
	sp::GL::TextureBinding gradientBinding(&ctx, 1, gradient);
	program->setUniform("Gradient", int(1));
	
	sp::GL::TextureBinding additiveBinding(&ctx, 2, additive);
	program->setUniform("Additive", int(2));
	
	program->validate();
	
	glOrtho(0,1,0,1,0,1);
	
	
	/// Loop
	for(;;)
	{
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		program->setUniform("Time", float(sp::RuntimeInSeconds()));
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		sp::GL::CheckGl();
		
		ctx.swapBuffers();
		glfwPollEvents();
		
		if(glfwGetKey(GLFW_KEY_ESC))
			break;
		
		if(!glfwGetWindowParam(GLFW_OPENED))
			break;
	}
}
