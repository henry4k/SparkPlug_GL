#include <SparkPlug/Common.h>
#include <SparkPlug/Image.h>
#include <SparkPlug/GL/Context.h>
#include <SparkPlug/GL/Texture.h>
#include <SparkPlug/ImageIo/Loader.h>
#include <GL/glfw3.h>

namespace sp = SparkPlug;
namespace spgl = SparkPlug::GL;

class GlfwContext : public spgl::Context
{
public:
	GlfwContext()
	{
		if(!glfwInit())
			sp::FatalError("GLFW init failed.");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		m_Window = glfwCreateWindow(640, 480, "SparkPlug", NULL, NULL);
		if(!m_Window)
			sp::FatalError("Window creation failed.");

		glfwMakeContextCurrent(m_Window);

		glfwSwapInterval(1);

		glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_TRUE);

		postInit();
	}

	~GlfwContext()
	{
		glfwTerminate();
	}

	void swapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}

	GLFWwindow* m_Window;
};

int main()
{
	GlfwContext ctx;

	sp::Image image;
	if(!sp::ImageIo::LoadFromFile(&image, "test.png", sp::ImageIo::ImageFormat_Png))
		sp::FatalError("Can't load image from file.");

	sp::StrongRef<sp::GL::Texture> texture;
	texture = sp::GL::Texture::CreateFromImage(
		&ctx,
		sp::GL::TextureType_2D,
		image
	);

	glEnable(GL_TEXTURE);
	glEnable(GL_TEXTURE_2D);

	ctx.bindTexture(texture, 0);


	for(;;)
	{
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_QUADS);
#define VTX(x, y) glVertex2f(x,y); glTexCoord2f(x,y);
			VTX(0,0);
			VTX(0,1);
			VTX(1,1);
			VTX(1,0);
#undef VTX
		glEnd();

		ctx.swapBuffers();
		glfwPollEvents();

		if(glfwGetKey(ctx.m_Window, GLFW_KEY_ESCAPE))
			break;

		if(glfwWindowShouldClose(ctx.m_Window) == GL_TRUE)
			break;
	}
}
