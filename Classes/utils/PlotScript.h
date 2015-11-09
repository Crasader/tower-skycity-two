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
        getLuaVarString : 调用lua全局string
 
        luaFileName  = lua文件名
        varName = 所要取Lua中的变量名
     */
    const char* getLuaVarString(const char* luaFileName,const char* varName);
 
    /*
     getLuaVarOneOfTable : 调用lua全局table中的一个元素
 
     luaFileName  = lua文件名
     varName = 所要取Lua中的table变量名
     keyName = 所要取Lua中的table中某一个元素的Key
     */
    const char* getLuaVarOneOfTable(const char* luaFileName,const char* varName,const char* keyName);
 
    /*
     getLuaVarTable : 调用lua全局table
 
     luaFileName  = lua文件名
     varName = 所要取的table变量名
 
     （注：返回的是所有的数据，童鞋们可以自己使用Map等处理）
     */
    const char* getLuaVarTable(const char* luaFileName,const char* varName);
 
    /*
     callLuaFunction : 调用lua函数
 
     luaFileName  = lua文件名
     functionName = 所要调用Lua中的的函数名
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
	// 单件模式的清除工作
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