

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\

xcopy	/y		.\Engine\Bin\PhysX_64.dll				.\Client\Bin\
xcopy	/y		.\Engine\Bin\PhysXFoundation_64.dll		.\Client\Bin\

xcopy	/y		.\Engine\Bin\PhysXCommon_64.dll			.\Client\Bin\
xcopy	/y		.\Engine\Bin\PhysXCooking_64.dll		.\Client\Bin\
xcopy	/y		.\Engine\Bin\PVDRuntime_64.dll			.\Client\Bin\
xcopy	/y		.\Engine\Bin\PhysXGpu_64.dll			.\Client\Bin\



xcopy	/y		.\Engine\Bin\ShaderFiles\*.hlsl			.\Client\Bin\ShaderFiles\
xcopy	/y/s		.\Engine\Public\*.*					.\EngineSDK\Inc\