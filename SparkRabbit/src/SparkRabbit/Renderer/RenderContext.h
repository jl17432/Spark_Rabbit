#pragma once


namespace SparkRabbit {
	
	class RenderContext 
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}