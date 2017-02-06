#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "DirectXMath.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdlib.h>
#include <wrl.h>			//Microsoft::WRL::ComPtr<>
#include "DXUtil.h"
#include "InputHandler.h"
#include <AntTweakBar.h>

#pragma comment (lib, "d3dcompiler.lib")

class DXBase
{
public:
	DXBase(const HINSTANCE &hInstance, const int &cmdShow, const std::string &windowName, unsigned int windowWidth, unsigned int windowHeight);
	virtual ~DXBase();

	int Run();
	virtual bool Init(bool vsync);
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DirectX::XMFLOAT4X4 ProjMatrix() const { return *projMatrix; }
	DirectX::XMFLOAT4X4 WorldMatrix() const { return *worldMatrix; }
	DirectX::XMFLOAT4X4 OrthoMatrix() const { return *orthoMatrix; }

protected:
	//WIN32
	HWND hAppWnd;
	HINSTANCE hAppInstance;
	UINT posX, posY;
	UINT wndWidth;
	UINT wndHeight;
	UINT screenWidth;
	UINT screenHeight;
	RECT wndRect;
	std::string wndTitle;
	DWORD wndStyle;
	int cmdShow;
	bool initialised;
	BOOL fullscreen;
	InputHandler inputHandler;

	//D3D
	Microsoft::WRL::ComPtr<ID3D11Device> dev;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> devCon;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBuffer; 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthEnabledState, depthDisabledState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterStateBCull, rasterStateFCull, rasterStateNCull;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaBlendState, nAlphaBlendState, particleBlendState;

	D3D_FEATURE_LEVEL featureLevel;

	float fov, aspectRatio;
	const float nearDepth;
	const float farDepth;
	bool vsyncEnabled;
	

	DirectX::XMFLOAT4X4 *projMatrix, *worldMatrix, *orthoMatrix;

	bool InitD3D(bool vsync);
	bool CreateRenderTarget(ID3D11RenderTargetView **renderTarget);
	bool CreateDepthComponents(ID3D11Texture2D **depthBuffer, ID3D11DepthStencilState **depthStencilState, ID3D11DepthStencilView **depthView);
	bool ResizeContext(unsigned int width, unsigned int height);
	bool ResizeWindow(unsigned int width, unsigned int height);
	void BeginDraw();
	void EndDraw();
	void CleanD3D();

private:
	DXBase& operator= (const DXBase&);
	DXBase(const DXBase&);
};

