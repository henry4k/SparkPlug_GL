#ifndef __SPARKPLUG_GL_BUFFER__
#define __SPARKPLUG_GL_BUFFER__

#include <SparkPlug/Pixel.h>
#include <SparkPlug/Image.h>
#include <SparkPlug/GL/Object.h>
#include <SparkPlug/GL/VertexFormat.h>


namespace SparkPlug
{
namespace GL
{

enum PrimitiveType
{
	PrimitiveType_PointList,
	PrimitiveType_LineList,
	PrimitiveType_LineStrip,
	PrimitiveType_LineLoop,
	PrimitiveType_TriangleList,
	PrimitiveType_TriangleStrip,
	PrimitiveType_TriangleFan,
	PrimitiveType_QuadList,
	PrimitiveType_QuadStrip
};
const char* AsString( PrimitiveType type );
GLenum ConvertToGL( PrimitiveType type );


enum BufferMapMode
{
	BufferMapMode_ReadOnly,
	BufferMapMode_WriteOnly,
	BufferMapMode_ReadWrite
};
const char* AsString( BufferMapMode type );
GLenum ConvertToGL( BufferMapMode type );

/**
 * Usage hints
 */
enum BufferUsage
{
	BufferUsage_Static,
	BufferUsage_Stream,
	BufferUsage_Dynamic
};
const char* AsString( BufferUsage type );
GLenum ConvertToGL( BufferUsage type );


enum BufferTarget
{
	BufferTarget_Vertex,
	BufferTarget_Index,
	BufferTarget_PixelPacker,
	BufferTarget_PixelUnpacker,
	BufferTarget_Count
};
const char* AsString( BufferTarget type );
GLenum ConvertToGL( BufferTarget type );


enum IndexType
{
	IndexType_UInt8,
	IndexType_UInt16,
	IndexType_UInt32
};
const char* AsString( IndexType type );
GLenum ConvertToGL( IndexType type );
int SizeOf( IndexType type );



class Buffer : public Object
{
public:
	virtual ~Buffer();

	bool isReady() const;
	BufferTarget target() const;

	void* map( BufferMapMode type );
	void unmap();

	void copyFrom( const void* source, int count, int start = 0 );
	void copyTo( void* destination, int count, int start = 0 );

	int elementSize() const;
	int elementCount() const;
	int size() const;

protected:
	Buffer( Context* context, BufferTarget target, BufferUsage usage, int count, int elementSize );

private:
	BufferTarget m_Target;
	BufferUsage m_Usage;
	int m_Count;
	int m_ElementSize;
	bool m_Mapped;
};



class VertexBuffer : public Buffer
{
public:
	static StrongRef<VertexBuffer> Create( Context* context, const VertexFormat& format, int count, BufferUsage usage );

	const VertexFormat& format() const;

private:
	VertexBuffer( Context* context, const VertexFormat& format, int count, BufferUsage usage );

	VertexFormat m_Format;
};



class IndexBuffer : public Buffer
{
public:
	static StrongRef<IndexBuffer> Create( Context* context, int count, BufferUsage usage );

	IndexType indexType() const;

private:
	IndexBuffer( Context* context, int count, BufferUsage usage, IndexType indexType );

	IndexType m_IndexType;
};

class PixelBuffer : public Buffer
{
public:
	/*
	static StrongRef<PixelBuffer> Create( Context* context, const PixelFormat& format, int width, int height, int depth );
	static StrongRef<PixelBuffer> CreateFromImage( const Image& image );
	*/

	const PixelFormat& format() const;
	int width() const;
	int height() const;
	int depth() const;

private:
	PixelBuffer( Context* context, BufferTarget target, BufferUsage usage, const PixelFormat& format, int width, int height, int depth );
	// TODO: Über PBOs erkundigen .. also was man jetzt genau damit anstellen kann und so. :)

	PixelFormat m_Format;
	int m_Width;
	int m_Height;
	int m_Depth;
};


/*
// PrimitiveRange <- Schiebt vertices in ein VBO und indices in ein IBO

class FrameBuffer : public Buffer
{
public:
private:
};

// ScreenFrameBuffer
// TextureFrameBuffer
*/

}
}

#endif
