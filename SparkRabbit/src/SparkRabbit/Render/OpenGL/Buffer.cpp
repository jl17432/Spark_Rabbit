#include "PreCompile.h"
#include "Buffer.h"
#include"OpenglBuffer.h"


namespace SparkRabbit {
	vertexBuffer* vertexBuffer::create(float* vertices, uint32_t size)
	{
		return new OpenglVertexBuffer(vertices,size);
	}
	indexBuffer* indexBuffer::create(uint32_t* indices, uint32_t count)
	{
		return new OpenglIndexBuffer(indices,count);
	}
}