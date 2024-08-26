#pragma once
#pragma warning (disable : 4251)
#pragma warning (disable : 5208)




#include <d3d11.h>      // d3d11


#include "Effect11/d3dx11effect.h"

#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h" 


#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"




#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
using namespace DirectX;



#include "PhysX/include/PxPhysicsAPI.h"
using namespace physx;




#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;


#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"


namespace Engine
{
	const _wstring		g_strTransformTag = TEXT("Com_Transform");

	const _uint			g_iMaxMeshBones = 512;
}

using namespace Engine;


#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "ImGuizmo.h"




#define EPSILON 0.01f

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG