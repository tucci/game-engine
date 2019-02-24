#pragma once

#include "Core/ECS/EntityManager.h"

void set_render_visibility(EntityManager* manager, Entity entity, bool visibility);

void set_render_material(EntityManager* manager, Entity entity, MaterialID id);
MaterialID get_render_material(EntityManager* manager, Entity entity);

