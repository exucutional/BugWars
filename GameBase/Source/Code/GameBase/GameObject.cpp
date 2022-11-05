#include "pch.h"
#include "GameObject.h"
#include "Texture.h"

GameObject::GameObject()
	: visible(true)
{
	static int id_counter = 0;
	id = id_counter++;
}

Point GameObject::GetSize() const
{
	if (auto tex = GetTexture())
		return tex->size;

	return Point(64.0f, 64.0f);
}

float GameObject::GetRadius() const
{
	return GetSize().x / 2.0f;
}