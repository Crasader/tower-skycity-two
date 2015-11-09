//  SendLuaData.cpp
//
//  Created by ZISOU-YSJ
//
//  Lua and C++/c ������

#include "SendLuaData.h"
#include "CCLuaEngine.h"

SendLuaData* SendLuaData::m_instance = NULL;
SendLuaData* SendLuaData::getInstance(){
    if(!m_instance)
    {
        
        m_instance = new SendLuaData();
    }
    return m_instance;
}
//��ȡ������ֵ
const char* SendLuaData::getLuaVarString(const char* luaFileName,const char* varName){
    
    lua_State*  ls = LuaEngine::getInstance()->getLuaStack()->getLuaState();
    
    int isOpen = luaL_dofile(ls, luaFileName);
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
        return NULL;
    }
    
    lua_settop(ls, 0);
    lua_getglobal(ls, varName);
    
    int statesCode = lua_isstring(ls, 1);
    if(statesCode!=1){
        CCLOG("Open Lua Error: %i", statesCode);
        return NULL;
    }
    
    const char* str = lua_tostring(ls, 1);
    lua_pop(ls, 1);
    
    return str;
}

const char* SendLuaData::getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName){
    
    lua_State*  ls = LuaEngine::defaultEngine()->getLuaStack()->getLuaState();
    
    int isOpen = luaL_dofile(ls, luaFileName);
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
        return NULL;
    }
    
    lua_getglobal(ls, varName);
    
    int statesCode = lua_istable(ls, -1);
    if(statesCode!=1){
        CCLOG("Open Lua Error: %i", statesCode);
        return NULL;
    }
    
    lua_pushstring(ls, keyName);
    lua_gettable(ls, -2);
    const char* valueString = lua_tostring(ls, -1);
    
    lua_pop(ls, -1);
    
    return valueString;
}
//ִ��Lua�����ر�ṹ
const char* SendLuaData::getLuaVarTable(const char* luaFileName,const char* varName){
    lua_State*  ls = LuaEngine::defaultEngine()->getLuaStack()->getLuaState();
    
    int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
        return NULL;
    }
    
    lua_getglobal(ls, varName);
    
    int it = lua_gettop(ls);
    lua_pushnil(ls);
    
    string result="";
    
    while(lua_next(ls, it))
    {
        string key = lua_tostring(ls, -2);
        string value = lua_tostring(ls, -1);
        
        result=result+key+":"+value+"\t";
        
        lua_pop(ls, 1);
    }
    lua_pop(ls, 1);
    
    return result.c_str();
}

//����ִ��Lua�����з���ֵ
const char* SendLuaData::callLuaFuncParReturn(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype)
{
    lua_State*  ls = LuaEngine::defaultEngine()->getLuaStack()->getLuaState();
    
    int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
        return NULL;
    }
    
    lua_getglobal(ls, functionName);
    int countnum = arraypar->count();
    if(countnum>0)
    {
        for (int i = 0; i<arraypar->count(); i++) {
            CCString* typestr = (CCString*)arraypartype->objectAtIndex(i);
            CCString* strnr = (CCString*)arraypar->objectAtIndex(i);
            if(typestr->isEqual(CCString::create("string")))
            {
                lua_pushstring(ls, strnr->getCString());
            }
            else if(typestr->isEqual(CCString::create("int")))
            {
                lua_pushnumber(ls, strnr->intValue());
            }
            else if(typestr->isEqual(CCString::create("bool")))
            {
                lua_pushboolean(ls, strnr->boolValue());
            }
        }
    }
    /*
     lua_call
     ��һ������:�����Ĳ�������
     �ڶ�������:��������ֵ����
     */
    lua_call(ls, countnum, 1);
    
    const char* iResult = lua_tostring(ls, -1);
    
    return iResult;
}

//����ִ��Lua�����޷���ֵ
const void SendLuaData::callLuaFuncPar(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype)
{
    lua_State*  ls = LuaEngine::defaultEngine()->getLuaStack()->getLuaState();
    
    int isOpen = luaL_dofile(ls, getFileFullPath(luaFileName));
    if(isOpen!=0){
        CCLOG("Open Lua Error: %i", isOpen);
    }
    
    lua_getglobal(ls, functionName);
    int countnum = arraypar->count();
    if(countnum>0)
    {
        for (int i = 0; i<arraypar->count(); i++) {
            CCString* typestr = (CCString*)arraypartype->objectAtIndex(i);
            CCString* strnr = (CCString*)arraypar->objectAtIndex(i);
            if(typestr->isEqual(CCString::create("string")))
            {
                lua_pushstring(ls, strnr->getCString());
            }
            else if(typestr->isEqual(CCString::create("int")))
            {
                lua_pushnumber(ls, strnr->intValue());
            }
            else if(typestr->isEqual(CCString::create("bool")))
            {
                lua_pushboolean(ls, strnr->boolValue());
            }
        }
    }
    /*
     lua_call
     ��һ������:�����Ĳ�������
     �ڶ�������:��������ֵ����
     */
    lua_call(ls, countnum, 0);
 
}


const char* SendLuaData::getFileFullPath(const char* fileName){
	//auto path = FileUtils::sharedFileUtils()->fullPathForFilename(fileName);
    return fileName;
}

SendLuaData::~SendLuaData(){
    
    CC_SAFE_DELETE(m_instance);
    m_instance=NULL;
}