#include "ParticleRequestQueue.h"
#include <stdexcept>

namespace Renderer
{
	ParticleRequestQueue::ParticleRequestQueue(std::vector<ParticleRequestMessage>* queue)
	{
		_queue = queue;
	}

	ParticleRequestQueue::~ParticleRequestQueue()
	{
		_queue = nullptr;
	}

	void ParticleRequestQueue::Insert(ParticleRequestMessage& msg)
	{
		_queue->push_back(msg);
	}
}
