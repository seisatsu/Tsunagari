#include "entity.h"

Entity::Entity(Resourcer* rc, wstring img_fn)
{
	img = rc->get_image(img_fn);
}

void Entity::draw()
{
	img->draw(0, 0, 0);
}

