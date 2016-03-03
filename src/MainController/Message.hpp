#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <irrlicht/Keycodes.h>
#include <thread>
#include <system_error>
#include "FileSystem/FileManager.hpp"
#include "GestureCapture/GestureCapture.hpp"

class Message
{
public:
    enum MessageType
    {
        GESTURE,
        KEYPRESS,
        FILESYSTEM,
        INVALID_MESSAGE
    };

    MessageType getType();
    virtual ~Message();
protected:
    Message ();
    Message (MessageType m);
    MessageType type;
};

class GestureMessage : public Message
{
public:
    GestureMessage(GestureCapture::GestureType g, GestureCapture::Direction d, GestureCapture::Handedness h);
    GestureCapture::GestureType getGesture();
    GestureCapture::Direction getDir();
    GestureCapture::Handedness getHandedness();

private:
    GestureCapture::GestureType gesture;
    GestureCapture::Direction dir;
    GestureCapture::Handedness hand;
};

class KeyMessage : public Message
{
public:
    KeyMessage(irr::EKEY_CODE k, bool s, bool c, bool p);
    irr::EKEY_CODE getKey();
    bool getShift();
    bool getCtrl();
    bool getPressed();

private:
    irr::EKEY_CODE key;
    bool shift;
    bool ctrl;
    bool pressed;
};

class FileSystemMessage : public Message
{
public:
    FileSystemMessage(std::thread::id id, std::error_condition err, FileManager::FileSystemAction act, fs::path p1, fs::path p2 );
    std::thread::id get_t_id();
    std::error_condition getErrCode();
    FileManager::FileSystemAction getAction();
    fs::path getPath1();
    fs::path getPath2();

private:
    std::thread::id t_id;
    std::error_condition errCode;
    FileManager::FileSystemAction action;
    fs::path path1;
    fs::path path2;
};

#endif //MESSAGE_HPP