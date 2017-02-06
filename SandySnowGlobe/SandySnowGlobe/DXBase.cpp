#include "DXBase.h"

namespace
{
	DXBase *appPointer = nullptr;
}

/// <summary>
/// Main event handler
/// </summary>
/// <param name="hwnd">Window handle</param>
/// <param name="msg">Message</param>
/// <param name="wParam">Additional info on msg</param>
/// <param name="lParam">Additional info on msg</param>
/// <returns>Action/result from passed msg</returns>
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(TwEventWin(hwnd, msg, wParam, lParam)) // send event to AntTweakBar
	{
		return 0;
	}

	if(appPointer)
	{
		return appPointer->MsgProc(hwnd, msg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

DXBase::DXBase(const HINSTANCE &hInstance, const int &nShowCmd, const std::string &windowName, unsigned int windowWidth, unsigned int windowHeight) : nearDepth(0.1f), farDepth(1000.0f)
{
	//WIN32
	hAppInstance = hInstance;
	hAppWnd = NULL;
	wndWidth = windowWidth;
	wndHeight = windowHeight;
	wndTitle = windowName;
	wndStyle = WS_OVERLAPPEDWINDOW;
	cmdShow = nShowCmd;
	appPointer = this;
	initialised = false;
	fullscreen = false;
	inputHandler = InputHandler();

	//D3D
	dev = nullptr;
	swapChain = nullptr;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	devCon = nullptr;
	backBuffer = nullptr;
	depthStencilBuffer = nullptr;
	depthStencilView = nullptr;
	depthEnabledState = nullptr;
	depthDisabledState = nullptr;
	rasterStateBCull = nullptr;
	rasterStateFCull = nullptr;
	rasterStateNCull = nullptr;
	inputLayout = nullptr;
	vertBuffer = nullptr;
	projMatrix = new DirectX::XMFLOAT4X4();
	worldMatrix = new DirectX::XMFLOAT4X4();
	orthoMatrix = new DirectX::XMFLOAT4X4();
	fov = DirectX::XM_PI / 4.0f;
	aspectRatio = (float)wndWidth / (float)wndHeight; //aspectRatioHByW = (float)wndHeight / (float)wndWidth;
	vsyncEnabled = false;
	posX = 0;
	posY = 0;
	screenWidth = 0;
	screenHeight = 0;
}

DXBase::~DXBase()
{
	try
	{
		Memory::SafeDelete(projMatrix);
		Memory::SafeDelete(worldMatrix);
		Memory::SafeDelete(orthoMatrix);
	}

	catch (int &e)
	{
		delete projMatrix;
		delete worldMatrix;
		delete orthoMatrix;
	}
}

/// <summary>
/// Main "game loop". Keeps running until quit msg is received
/// </summary>
/// <returns>msg.wParam</returns>
int DXBase::Run()
{
	MSG msg = {0};

	while(WM_QUIT != msg.message)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}

	CleanD3D();

	return msg.wParam;
}

/// <summary>
/// Clears render target/depth buffer and sets clear colour
/// </summary>
void DXBase::BeginDraw()
{
	float c[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	devCon->ClearRenderTargetView(backBuffer.Get(), c);
	devCon->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);	//1.0f default depth value
}

/// <summary>
/// Swap back buffer to display new scene (interval depends on vsync)
/// </summary>
void DXBase::EndDraw()
{
	if(vsyncEnabled)
		swapChain->Present(1, 0);
	else
		swapChain->Present(0, 0);
}

/// <summary>
/// Main init method for Win32 variables/settings - calls InitD3D at the end
/// e.g. Window desc
/// </summary>
/// <param name="vsync">Vsync on/off</param>
/// <returns>True if successful</returns>
bool DXBase::Init(bool vsync)
{
	if(!initialised)
	{
		srand(time(0));

		Logger::InitLogFile();

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.hInstance = hAppInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.lpszClassName = "WindowClass";
		      
		RegisterClassEx(&wc);

		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		posX = (screenWidth - wndWidth) / 2;
		posY = (screenHeight - wndHeight) / 2;

		wndRect = { posX, posY, wndWidth, wndHeight };
		AdjustWindowRect(&wndRect, wndStyle, FALSE);

		fullscreen = FALSE;

		hAppWnd = CreateWindow("WindowClass",
			wndTitle.c_str(),
			wndStyle,
			posX,
			posY,
			wndWidth,
			wndHeight,
			NULL,
			NULL,
			hAppInstance,
			NULL);

		if(!ShowWindow(hAppWnd, cmdShow))
		{
			Logger::Log(("Initialised " + std::to_string(wndWidth) + "x" + std::to_string(wndHeight) + " window"));
			initialised = true;
		}

		if(SetForegroundWindow(hAppWnd))
		{
			int i = 0;
		}

		if(SetFocus(hAppWnd))
		{
			int i = 0;
		}

		InitD3D(vsync);
	}

	return true;
}

/// <summary>
/// Init for various Direct3D variables/settings.
/// e.g. feature levels, swapchain, back buffers, blend states.
/// </summary>
/// <param name="vsync">Vsync on/off</param>
/// <returns>True if successful</returns>
bool DXBase::InitD3D(bool vsync)
{
	vsyncEnabled = vsync;

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = wndWidth;
	swapDesc.BufferDesc.Height = wndHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if(vsyncEnabled)
	{
		swapDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		swapDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.OutputWindow = hAppWnd;
	swapDesc.SampleDesc.Count = 1;	//MSAA
	swapDesc.SampleDesc.Quality = 0;	//MSAA
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = TRUE;

	HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		featureLevels,
		4,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&dev,
		&featureLevel,
		&devCon);
	
	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create device and swap chain"))
	{
		return false;
	}

	if(!CreateRenderTarget(backBuffer.GetAddressOf()))
		return false;

	if(!CreateDepthComponents(depthStencilBuffer.GetAddressOf(), depthEnabledState.GetAddressOf(), depthStencilView.GetAddressOf()))
		return false;

	devCon->OMSetRenderTargets(1, backBuffer.GetAddressOf(), depthStencilView.Get());

	D3D11_RASTERIZER_DESC rasterStateDesc;
	ZeroMemory(&rasterStateDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterStateDesc.AntialiasedLineEnable = false;
	rasterStateDesc.CullMode = D3D11_CULL_BACK;
	rasterStateDesc.DepthBias = 0;
	rasterStateDesc.DepthBiasClamp = 0.0f;
	rasterStateDesc.DepthClipEnable = FALSE;
	rasterStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterStateDesc.FrontCounterClockwise = FALSE;
	rasterStateDesc.MultisampleEnable = FALSE;
	rasterStateDesc.ScissorEnable = FALSE;
	rasterStateDesc.SlopeScaledDepthBias = 0.0f;

	result = dev->CreateRasterizerState(&rasterStateDesc, rasterStateBCull.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create rasteriser state (back cull)"))
	{
		return false;
	}

	rasterStateDesc.CullMode = D3D11_CULL_FRONT;

	result = dev->CreateRasterizerState(&rasterStateDesc, rasterStateFCull.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create rasteriser state (front cull)"))
	{
		return false;
	}

	rasterStateDesc.CullMode = D3D11_CULL_NONE;

	result = dev->CreateRasterizerState(&rasterStateDesc, rasterStateNCull.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create rasteriser state (no cull)"))
	{
		return false;
	}

	devCon->RSSetState(rasterStateBCull.Get());

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

// 	blendDesc.RenderTarget[0].BlendEnable = TRUE;
// 	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
// 	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
// 	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
// 	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
// 	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
// 	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
// 	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

	result = dev->CreateBlendState(&blendDesc, alphaBlendState.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create blender state (alpha enabled)"))
	{
		return false;
	}

	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blendDesc.AlphaToCoverageEnable = true;

	result = dev->CreateBlendState(&blendDesc, particleBlendState.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create blender state (alpha enabled)"))
	{
		return false;
	}

	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	//blendDesc.AlphaToCoverageEnable = false;

	result = dev->CreateBlendState(&blendDesc, nAlphaBlendState.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create blender state (alpha disabled)"))
	{
		return false;
	}

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = wndWidth;
	viewport.Height = wndHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	devCon->RSSetViewports(1, &viewport);

	DirectX::XMMATRIX matrixP = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearDepth, farDepth);
	DirectX::XMMATRIX matrixW = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX matrixO = DirectX::XMMatrixOrthographicLH(wndWidth, wndHeight, nearDepth, farDepth);

	DirectX::XMStoreFloat4x4(projMatrix, matrixP);
	DirectX::XMStoreFloat4x4(worldMatrix, matrixW);
	DirectX::XMStoreFloat4x4(orthoMatrix, matrixO);

	return true;
}

bool DXBase::CreateRenderTarget(ID3D11RenderTargetView **renderTarget)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> bBuffer;
	HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bBuffer);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Get back buffer"))
	{
		return false;
	}

	result = dev->CreateRenderTargetView(bBuffer.Get(), NULL, renderTarget);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create render target view (back buffer)"))
	{
		return false;
	}

	return true;
}

bool DXBase::CreateDepthComponents(ID3D11Texture2D **depthBuffer, ID3D11DepthStencilState **depthStencilState, ID3D11DepthStencilView **depthView)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthBufferDesc.Width = wndWidth;
	depthBufferDesc.Height = wndHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT result = dev->CreateTexture2D(&depthBufferDesc, 0, depthBuffer);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create depth stencil buffer"))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = dev->CreateDepthStencilState(&depthStencilDesc, depthStencilState);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create depth stencil state"))
	{
		return false;
	}

	devCon->OMSetDepthStencilState(*depthStencilState, 1);

	depthStencilDesc.DepthEnable = FALSE;

	result = dev->CreateDepthStencilState(&depthStencilDesc, depthDisabledState.GetAddressOf());

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create depth stencil state"))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
	ZeroMemory(&depthViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = dev->CreateDepthStencilView(*depthBuffer, NULL, depthView);

	if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Create depth stencil view"))
	{
		return false;
	}

	return true;
}

bool DXBase::ResizeWindow(unsigned int width, unsigned int height)
{
	wndWidth = width;
	wndHeight = height;
	posX = (screenWidth - wndWidth) / 2;
	posY = (screenHeight - wndHeight) / 2;

	wndRect = { posX, posY, wndWidth, wndHeight };
	if(!AdjustWindowRect(&wndRect, wndStyle, FALSE))
		return false;
	if(!MoveWindow(hAppWnd, posX, posY, wndWidth, wndHeight, TRUE))
		return false;

	return true;
}

/// <summary>
/// Triggered when window is resized.
/// Edit swapchain desc and reset buffers and set new viewport
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
bool DXBase::ResizeContext(unsigned int width, unsigned int height)
{
	if(initialised)
	{
		wndWidth = width;
		wndHeight = height;

		TwWindowSize(wndWidth, wndHeight);

		BOOL prevState = fullscreen;
		HRESULT result = swapChain->GetFullscreenState(&fullscreen, nullptr);

		if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Get fullscreen state"))
		{
			return false;
		}

		DXGI_SWAP_CHAIN_DESC swapDesc;

		swapChain->GetDesc(&swapDesc);

		swapDesc.Windowed = !fullscreen;
		swapDesc.BufferDesc.Width = wndWidth;
		swapDesc.BufferDesc.Height = wndHeight;
		swapChain->ResizeTarget(&swapDesc.BufferDesc);

		depthStencilView.Reset();
		depthStencilBuffer.Reset();
		backBuffer.Reset();

		result = swapChain->ResizeBuffers(swapDesc.BufferCount, wndWidth, wndHeight, swapDesc.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		if(!Validation::ErrCheck(result, __FILE__, __LINE__, "Resize buffers"))
		{
			return false;
		}

		CreateRenderTarget(backBuffer.GetAddressOf());
		CreateDepthComponents(depthStencilBuffer.GetAddressOf(), depthEnabledState.GetAddressOf(), depthStencilView.GetAddressOf());

		devCon->OMSetRenderTargets(1, backBuffer.GetAddressOf(), depthStencilView.Get());

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = wndWidth;
		viewport.Height = wndHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		devCon->RSSetViewports(1, &viewport);

		aspectRatio = (float)wndWidth / (float)wndHeight;
		Logger::Log("Application resized to " + std::to_string(wndWidth) + "x" + std::to_string(wndHeight));
	}
}

/// <summary>
/// Message processing from MainWndProc
/// </summary>
/// <param name="hwnd">Window handle</param>
/// <param name="msg">Message</param>
/// <param name="wParam">Additional info on msg</param>
/// <param name="lParam">Additional info on msg</param>
/// <returns></returns>
LRESULT DXBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_SIZE:
			ResizeContext(LOWORD(lParam), HIWORD(lParam));
		break;
		case WM_KEYDOWN:
			inputHandler.KeyDown((unsigned int)wParam);
			return 0;
		break;
		case WM_KEYUP:
			inputHandler.KeyUp((unsigned int)wParam);
			return 0;
		break;
		case WM_DESTROY:
			Logger::Log("Application quitting");
			PostQuitMessage(0);
			return 0;
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

void DXBase::CleanD3D()
{
	swapChain->SetFullscreenState(FALSE, NULL);
}
