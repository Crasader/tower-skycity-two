//  SendLuaData.cpp
//
//  Lua and C++/C ������

#ifndef __PublicSendLuaData__
#define __PublicSendLuaData__

#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

class SendLuaData{
public:
    
    static SendLuaData* getInstance();
    
    /*
     ֱ�ӻ�ȡLua�еñ�����ֵ
     �ļ��� luaFileName
     ������ varName
     */
    const char* getLuaVarString(const char* luaFileName,const char* varName);
    
    /*
     ��ȡLua��һ�����еýڵ�������������
     �ļ���luaFileName
     ������varName
     �ڵ���
     */
    const char* getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName);
    
    /*
     ����Luaȫ��Table
     lua�ļ���luaFileName
     table������varName
     */
    const char* getLuaVarTable(const char* luaFileName,const char* varName);
    
    /*
     �������з���
     �ļ���luaFileName
     ������functionName
     ��������arraypar
     ��������arraypartype
     */
    const char* callLuaFuncParReturn(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype);
    
    /*
     �������޷���
     �ļ���luaFileName
     ������functionName
     ��������arraypar
     ��������arraypartype
     */
    const void callLuaFuncPar(const char* luaFileName,const char* functionName,CCArray* arraypar,CCArray* arraypartype);
    
    
private:
    
    static bool _isFirst;
    static SendLuaData* m_instance;
    const char* getFileFullPath(const char* fileName);
    ~SendLuaData();
};

#endif