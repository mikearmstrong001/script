#ifndef TESTING_SOUND_WORLD_H
#define TESTING_SOUND_WORLD_H

#include <vector>
#include "vectormath/cpp/vectormath_aos.h"

namespace irrklang
{
	class ISound;
};

struct SoundEmitter
{
	Vectormath::Aos::Point3 point;
	std::vector< irrklang::ISound* > sounds;

	SoundEmitter *next;
	SoundEmitter *prev;

	SoundEmitter() : next(NULL), prev(NULL)
	{
	}

	void Link( SoundEmitter *head )
	{
		prev = head;
		next = head->next;

		next->prev = this;
		head->next = this;
	}

	void Unlink()
	{
		next->prev = prev;
		prev->next = next;

		prev = NULL;
		next = NULL;
	}
};

class SoundWorld
{
	Vectormath::Aos::Transform3 world2Listener;
	SoundEmitter root;
public:

	void SetListener( const Vectormath::Aos::Transform3 &trans );

	void AddEmitter( SoundEmitter *emit );
	void RemoveEmitter( SoundEmitter *emit );
};

#endif
