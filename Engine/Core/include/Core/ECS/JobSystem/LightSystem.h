#pragma once

#include "Core/ECS/EntityManager.h"

Light get_light(EntityManager* manager, Entity entity);
void set_light(EntityManager* manager, Entity entity, Light light);