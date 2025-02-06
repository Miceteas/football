#ifndef MINIMAP_H
#define MINIMAP_H

#include <gf/TileLayer.h>
#include <gf/Tileset.h>
#include <gf/Texture.h>
#include <gf/RenderTarget.h>
#include <gf/Vector.h>
#include "field.h"

class Minimap {
public:
    Minimap(const FootballField& field, float scale);

    void setPosition(float x, float y);
    void render(gf::RenderTarget& target);

private:
    gf::TileLayer m_tileLayer;   // Independent tile layer for the minimap
    gf::Tileset m_tileset;       // Independent tileset for the minimap
    gf::Vector2f m_position;     // Position of the minimap
    float m_scale;               // Scale of the minimap
};

#endif
