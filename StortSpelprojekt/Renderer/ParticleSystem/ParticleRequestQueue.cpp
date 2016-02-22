#include "ParticleRequestQueue.h"
#include <stdexcept>

namespace Renderer
{
	ParticleRequestQueue::ParticleRequestQueue(std::vector<ParticleMessage*>* queue)
	{
		_queue = queue;
	}

	ParticleRequestQueue::~ParticleRequestQueue()
	{
		_queue = nullptr;
	}

	void ParticleRequestQueue::Insert(ParticleMessage* msg)
	{
		_queue->push_back(msg);
	}
}
