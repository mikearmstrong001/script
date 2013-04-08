#include "resource_core.h"

ResourceManager* ResourceManager::root = NULL;

ResourceManager::ResourceManager()
{
	m_next = s_root;
	s_root = this;
}
