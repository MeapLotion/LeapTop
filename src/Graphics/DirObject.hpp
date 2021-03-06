#include <string>
#include "irrlicht/irrlicht.h"


#ifndef DIROBJECT_HPP
#define DIROBJECT_HPP
class DirObject
{
public:
    DirObject();
    DirObject(char i_type, irr::f32 i_x, irr::f32 i_y, const std::wstring& i_name, bool i_isHighlighted, bool i_isSelected );
    ~DirObject();
    char getType();
    irr::f32 getX();
    irr::f32 getY();
    std::wstring getName();
    bool isHighlighted;
    bool isSelected;
    void setNode(irr::scene::IMeshSceneNode*);
    void setNodeText(irr::scene::IBillboardTextSceneNode*);
    irr::scene::ISceneNode* getNode();
    irr::scene::IBillboardTextSceneNode* getNodeText();
    void setIsHighlighted(bool, irr::video::IVideoDriver*);
    void setTranslucent(bool);
private:
    char type;
    irr::f32 x;
    irr::f32 y;
    std::wstring name;
    irr::scene::IMeshSceneNode* node;
    irr::scene::IBillboardTextSceneNode* text_node;
};

#endif //DIROBJECT_HPP
