#pragma once

#include "Core/ECS/EntityManager.h"


void set_render_material(EntityManager* manager, Entity entity, MaterialID id);
MaterialID get_render_material(EntityManager* manager, Entity entity);

