#include "ParticleEventQueue.h"
#include <stdexcept>

namespace Renderer
{
	ParticleEventQueue::ParticleEventQueue(std::vector<ParticleMessage*>* queue)
	{
		_queue = queue;
	}

	ParticleEventQueue::~ParticleEventQueue()
	{
		_queue = nullptr;
	}

	void ParticleEventQueue::Insert(ParticleMessage* msg)
	{
		_queue->push_back(msg);
	}
}
