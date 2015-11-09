//
//  PlotScript.h
//  CppLua
//
//  Created by Himi on 13-4-17.
//
//
 
#ifndef __CppLua__HclcData__
#define __CppLua__HclcData__
 
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
 
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
 
class PlotScript : public Ref
{
public:
    static PlotScript* sharedHD();
 
        //------------  c++ -> lua ------------//
 

	int openLuaFile(lua_State* _ls, const char *_luaFile );
    /*
        getLuaVarString : ����luaȫ��string
 
        luaFileName  = lua�ļ���
        varName = ��ҪȡLua�еı�����
     */
    const char* getLuaVarString(const char* luaFileName,const char* varName);
 
    /*
     getLuaVarOneOfTable : ����luaȫ��table�е�һ��Ԫ��
 
     luaFileName  = lua�ļ���
     varName = ��ҪȡLua�е�table������
     keyName = ��ҪȡLua�е�table��ĳһ��Ԫ�ص�Key
     */
    const char* getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName);
 
    /*
     getLuaVarTable : ����luaȫ��table
 
     luaFileName  = lua�ļ���
     varName = ��Ҫȡ��table������
 
     ��ע�����ص������е����ݣ�ͯЬ�ǿ����Լ�ʹ��Map�ȴ���
     */
    const char* getLuaVarTable(const char* luaFileName,const char* varName);
 
    /*
     callLuaFunction : ����lua����
 
     luaFileName  = lua�ļ���
     functionName = ��Ҫ����Lua�еĵĺ�����
     */
    const char* callLuaFunction(const char* luaFileName,const char* functionName);
 
      //------------  lua -> c++ ------------//
 
    void registerCppFunction(const char* luaFileName);
	
	int delayCount;

	static void dispatchAll(const char* str);
private:
    static int dispatch(lua_State* ls);	
	static int setCondition(lua_State* ls);
	static int delayDispatch(lua_State* ls);
	static int setCellModel(lua_State* ls);
	static int addCellWithTrigger(lua_State* ls);
	void delayCallBack(Node* pTarget, void* data);
	static bool _isFirst;
	
    const char* getFileFullPath(const char* fileName);
    ~PlotScript();

	static PlotScript* _shared;
	// ����ģʽ���������
	class Garbo  
	{  
	private:  
		~Garbo()  
		{  
			if (_shared)  
			{  
				_shared->release();  
				_shared = NULL;  
				//log("destroy plotScript!");  
			}  
		}  
	private:  
		static Garbo m_Garbo;
	};
};
 
#endif /* defined(__CppLua__HclcData__) */