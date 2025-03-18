#ifndef FIELD_H
#define FIELD_H

#include <gf/TileLayer.h>
#include <gf/Tileset.h>
#include <gf/Texture.h>
#include <vector>

#include "settings.h"

class FootballField {
    public:
        FootballField(const std::string& texturePath, const std::vector<int>& tileOrder, int tilesX, int tilesY, int displayTileSize);
        
        gf::TileLayer& getTileLayer();

    private:
        gf::Texture m_texture;         // Texturespritesheet
        gf::TileLayer m_tileLayer;     
        int m_tilesX;                  // Number of tiles X (width)
        int m_tilesY;                  // Number of tiles Y (height)
        int m_displayTileSize;         // Display size of each tile (on the screen => because different)
};

#endif
