//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#include <SPARK_Core.h>
#include "Extensions/Actions/SPK_SpawnParticlesAction.h"

namespace SPK
{
	SpawnParticlesAction::SpawnParticlesAction(
		unsigned int minNb,
		unsigned int maxNb,
		const Ref<Group>& group,
		const Ref<Emitter>& emitter) :
		minNb(minNb),
		maxNb(maxNb),
		baseEmitter(),
		targetGroup(group)
	{
		setEmitter(emitter);
	}

	SpawnParticlesAction::SpawnParticlesAction(const SpawnParticlesAction& action) :
        Action(action),
		minNb(action.minNb),
		maxNb(action.maxNb),
		targetGroup(action.targetGroup),
		emitterPool()
	{
		targetGroup = action.copyChild(action.targetGroup);
		baseEmitter = action.copyChild(action.baseEmitter);
	}

	void SpawnParticlesAction::setNb(unsigned int min,unsigned int max)
	{
		if (min <= max)
		{
			this->minNb = min;
			this->maxNb = max;
		}
		else
		{
			SPK_LOG_WARNING("SpawnParticlesAction::setNb(unsigned int,unsigned int) - min is higher than max - Values are swapped");
			this->minNb = max;
			this->maxNb = min;
		}
	}

	void SpawnParticlesAction::setEmitter(const Ref<Emitter>& emitter)
	{
		resetPool();
		baseEmitter = emitter;
	}

	void SpawnParticlesAction::apply(Particle& particle) const
	{
		if (!checkValidity())
			return;

		const Ref<Emitter>& emitter = getNextAvailableEmitter();
		emitter->setTank(baseEmitter->getMinTank(),baseEmitter->getMaxTank());

		const Ref<Zone>& zone = emitter->getZone();
		zone->getTransform().setPosition(particle.position());
		zone->updateTransform();

		targetGroup->addParticles(SPK_RANDOM(minNb,maxNb + 1),emitter);
	}

	bool SpawnParticlesAction::checkValidity() const
	{
		if (!targetGroup)
		{
			SPK_LOG_ERROR("SpawnParticlesAction::checkValidity() - The target group is NULL and cannot be used");
			return false;
		}
		else if (!targetGroup->getSystem())
		{
			SPK_LOG_ERROR("SpawnParticlesAction::checkValidity() - The target group is invalid (it is not bound to a system) and cannot be used");
			return false;
		}

		if (!baseEmitter)
		{
			SPK_LOG_ERROR("SpawnParticlesAction::checkValidity() - The base emitter is NULL and cannot be used");
			return false;
		}
		else if (baseEmitter->getZone()->isShared())
		{
			SPK_LOG_ERROR("SpawnParticlesAction::checkValidity() - The base emitter is invalid (its zone is shared) and cannot be used");
			return false;
		}

		return true;
	}

	const Ref<Emitter>& SpawnParticlesAction::getNextAvailableEmitter() const
	{
		for (size_t i = 0; i < emitterPool.size(); ++i)
		{
			emitterPool.push_back(emitterPool.front());
			emitterPool.pop_front();
			if (emitterPool.back()->getNbReferences() == 1) // the emitter is available
				return emitterPool.back();
		}

		// No emitter is available in the pool, a new one must be created
		emitterPool.push_back(copy(baseEmitter));
		emitterPool.back()->getTransform().reset();
		return emitterPool.back();
	}

	void SpawnParticlesAction::resetPool()
	{
		emitterPool.clear();
	}

	Ref<SPKObject> SpawnParticlesAction::findByName(const std::string& name)
	{
		Ref<SPKObject> object = Action::findByName(name);
		if (object) return object;

		if (baseEmitter)
			object = baseEmitter->findByName(name);

		return object;
	}
}
