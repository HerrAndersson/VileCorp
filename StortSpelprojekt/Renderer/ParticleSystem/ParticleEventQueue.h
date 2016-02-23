#pragma once
#define RENDERER_EXPORT __declspec(dllexport)
#include "ParticleUtils.h"
#include <vector>

namespace Renderer
{
	//Wrapper to hide std::vector-functionality from users who should not have access to everything
	class RENDERER_EXPORT ParticleEventQueue
	{

	private:

		std::vector<ParticleMessage*>* _queue;

	public:

		ParticleEventQueue(std::vector<ParticleMessage*>* queue);
		virtual ~ParticleEventQueue();

		void Insert(ParticleMessage* msg);
	};
}
