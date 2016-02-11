#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include "ParticleUtils.h"
#include <vector>

namespace Renderer
{
	//Wrapper to hide std::vector-functionality from users who should not have access to everything
	class RENDERER_EXPORT ParticleRequestQueue
	{

	private:

		std::vector<ParticleRequestMessage>* _queue;

	public:

		ParticleRequestQueue(std::vector<ParticleRequestMessage>* queue);
		virtual ~ParticleRequestQueue();

		void Insert(ParticleRequestMessage& msg);
	};
}
