#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment( lib, "xinput.lib")

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <vector>
#include <list>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d12.h>
#include <map>
#include <functional>
#include <algorithm>
#include <DirectXMath.h>
#include <Xinput.h>

#include "graphics/d3dx12.h"
#include "math/Math.h"
#include "math/Vector.h"
#include "math/Matrix.h"

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"
#include "graphics/IShaderResource.h"
#include "graphics/IUnorderAccessResource.h"
#include "graphics/Texture.h"
#include "graphics/RWStructuredBuffer.h"
#include "graphics/Shader.h"
#include "graphics/PipelineState.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/RootSignature.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/DescriptorHeap.h"
#include "graphics/RenderContext_inline.h"
#include "graphics/DescriptorHeap_inline.h"

#include "graphics/Sprite.h"
#include "graphics/Model.h"
#include "HID/GamePad.h"

#include "kgEngine2.h"

#include "graphics/animation/Animation.h"
#include "graphics/animation/AnimationClip.h"

#include "graphics/ModelRender.h"

const UINT FRAME_BUFFER_W = 1280;				//フレームバッファの幅。
const UINT FRAME_BUFFER_H = 720;				//フレームバッファの高さ。

static const int MAX_BONE = 512;	//!<ボーンの最大数。