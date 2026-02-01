import texture;

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION 0 
#endif
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "stb_image.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

#include <d3d9.h>
#include <tchar.h>
#include <shellapi.h>

#include "pch.h"
//#include "calculator.h"
#include "enable_flags.h"
#include "calculator.h"

#define IMGUI_ENABLE_FREETYPE

//GENERATE PHOTOS IMAGE IMPLEMENATION:
//important for working with images png and jpg:
Dx9Texture LoadTextureFromFile(
	LPDIRECT3DDEVICE9 device,
	const char* filename)
{
	int w, h, c;
	unsigned char* data = stbi_load(filename, &w, &h, &c, 4);
	if (!data) {
		return {};   // празен Dx9Texture
	}
	LPDIRECT3DTEXTURE9 raw = nullptr;

	if (FAILED(device->CreateTexture(
		w, h, 1, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&raw, nullptr)))
	{
		stbi_image_free(data);
		return {};
	}

	D3DLOCKED_RECT rect;
	raw->LockRect(0, &rect, nullptr, 0);

	for (int y = 0; y < h; ++y) {
		memcpy((BYTE*)rect.pBits + y * rect.Pitch,
			data + y * w * 4,
			w * 4);
	}
		
	raw->UnlockRect(0);
	stbi_image_free(data);

	return Dx9Texture(raw); //ownership
}
//vlaue for slider: 
static float value = 0.7f;
static bool prgMenu = false;
// texture for picture:
// TODO: PicOne
//TODO: PicTwo
static Dx9Texture g_PicOneTexture;
static Dx9Texture g_PicTwoTexture;


static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static bool g_DeviceLost = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Calculator declaration:
#ifdef ENABLE_CALCULATOR
std::unique_ptr<Program_Two>elka = std::make_unique<Calculator>();
Calculator* e = dynamic_cast<Calculator*>(elka.get());
#endif

void DrawButton(Button& b1) {
	ImGui::SetWindowFontScale(1.6f);
	if (ImGui::Button(b1.lable, ImVec2(150, 50))) {
		b1.isRunning = true;
	}
	ImGui::SetWindowFontScale(1.0f);
	if (b1.isRunning) { b1.func(b1); }
}

static bool darkTheme = false;
ImVec4 color_text = darkTheme
? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
	: ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

static char pragmaOneBuffer[60] = "";
static char outputBuffer[60] = "";
static bool showResult = false;
//Pragma one:
void drawPragmaOne(Button& p1) {
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
	ImGui::Begin("PragmaOne", &p1.isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);
	ImGui::PushItemWidth(250);
	ImGui::PushID("TXT BOX");
	if (ImGui::InputText("", pragmaOneBuffer, sizeof(pragmaOneBuffer),
		ImGuiInputTextFlags_EnterReturnsTrue)) {
		showResult = true;
		sprintf_s(outputBuffer, sizeof(outputBuffer), pragmaOneBuffer);
		memset(pragmaOneBuffer, 0, sizeof(pragmaOneBuffer));
	}
	if (showResult) {
		if (strlen(outputBuffer) < 10) {
			ImGui::Text("You entered: %s", outputBuffer);
		}
		else { ImGui::Text("Too big"); }
		if (ImGui::Button("Reset")) {
			showResult = false;
			memset(outputBuffer, 0, sizeof(outputBuffer));
		}

	}
	if (!p1.isRunning) {
		memset(pragmaOneBuffer, 0, sizeof(pragmaOneBuffer));
		memset(outputBuffer, 0, sizeof(outputBuffer));
		showResult = false;
	}
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::End();
}
//Button for pragma:
Button p1("Pragma", drawPragmaOne);

#ifdef ENABLE_LOGIN_UI
Program log_in;
#endif

//flags
static bool mainWindowShow = true;
//begin framework function:
void BeginFrame() {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
//end framework finctions:
void EndFrame() {
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
static int n = 5;
// Main code:
int main(int, char**)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SodiumInit sodium_init;
	color_text = darkTheme ? ImVec4(1.0f, 1.0f, 0.0f, color_text.w) : ImVec4(0.0f, 0.0f, 0.0f, color_text.w);
	static bool prevDarkTheme = darkTheme;

	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	// Create application window
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Load Fonts
	// - If fonts are not explicitly loaded, Dear ImGui will call AddFontDefault() to select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
	//   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
	// - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//style.FontSizeBase = 20.0f;
	//io.Fonts->AddFontDefaultVector();
	//io.Fonts->AddFontDefaultBitmap();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
	//IM_ASSERT(font != nullptr);
	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done) { break; }

		// Handle lost D3D9 device
		if (g_DeviceLost)
		{
			HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
			if (hr == D3DERR_DEVICELOST)
			{
				::Sleep(10);
				continue;
			}
			if (hr == D3DERR_DEVICENOTRESET)
				ResetDevice();
			g_DeviceLost = false;
		}

		// Handle window resize (we don't resize directly in the WM_SIZE handler) 
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			g_d3dpp.BackBufferWidth = g_ResizeWidth;
			g_d3dpp.BackBufferHeight = g_ResizeHeight;
			g_ResizeWidth = g_ResizeHeight = 0;
			ResetDevice();
		}

		// Start the Dear ImGui frame:

		BeginFrame();
		if (mainWindowShow) {
			ImGui::SetNextWindowBgAlpha(value);
			ImGui::SetNextWindowSize(ImVec2(560, 550), ImGuiCond_Always);
			ImGui::Begin("Second Progect", &mainWindowShow, ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoCollapse);

			ImGui::PushStyleColor(ImGuiCol_Text, color_text);

			ImGui::SetWindowFontScale(1.9f);
			ImGui::Text("Hello world!");
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
			//pragma button:
#ifdef ENABLE_LOGIN_UI
			if (ImGui::Button("LogIn", ImVec2(150, 50))) { log_in.isRunning = true; }
			if (log_in.isRunning) { log_in.drawUI(darkTheme, value); }
#endif
			DrawButton(p1);
			ImVec4 border_color = darkTheme
				? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
				: ImVec4(0.0f, 0.0f, 0.5f, 1.0f);

			if (ImGui::CollapsingHeader("Pictures")) {
				//child window

				ImGui::PushStyleColor(ImGuiCol_Border, border_color);
				ImGui::BeginChild("Child1", ImVec2(540, 200), true); //title/size/?borders:true/false.
				ImGui::SetWindowFontScale(1.9f);
				ImGui::PushStyleColor(ImGuiCol_Text, color_text);
				ImGui::Text("Picture");
				//Image one:
				g_PicOneTexture.Show(ImVec2(526, 300));	
				ImGui::Text("Second picture");
				//Image two:
				g_PicTwoTexture.Show(ImVec2(526, 300));
				ImGui::SetWindowFontScale(1.0f);
				ImGui::PopStyleColor();
				ImGui::EndChild();
				ImGui::PopStyleColor();

			}
			if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_Framed))
			{
				ImGui::Indent();
				ImGui::Separator();

				ImGui::Text("Settings");
				//button for theme dark : ligth:
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
				if (ImGui::Button(darkTheme ? "Ligth" : "Dark", ImVec2(85, 30))) {
					darkTheme = !darkTheme;
				}
				//text color change:
				if (darkTheme != prevDarkTheme) {
					color_text = darkTheme ? ImVec4(1.0f, 1.0f, 0.0f, color_text.w) : ImVec4(0.0f, 0.0f, 0.0f, color_text.w);
					prevDarkTheme = darkTheme;
				}
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::Text("Theme");
				ImGui::SliderFloat("Alpha", &value, 0.2f, 1.0f);

				ImGui::Text("Choose a color:");
				ImGui::ColorEdit4("Color", (float*)&color_text); // 4 = RGBA
				// Или ColorEdit3 за RGB без alpha

				ImGui::Text("R: %.2f, G: %.2f, B: %.2f, A: %.2f", color_text.x, color_text.y, color_text.z, color_text.w);
				ImGui::Separator();
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("Projects")) {
				ImGui::Text("Eren");
#ifdef ENABLE_CALCULATOR
				if (ImGui::Button("claculator")) { e->showCalculator = !e->showCalculator; }
				if (e) {
					if (e->showCalculator) { elka->drawUI(darkTheme, value); }
				}
				else {
					IM_ASSERT(false && "Cast failed");
					std::abort();
				}
#endif
			}
			ImGui::PopStyleColor();
			//switch styles:
			if (darkTheme) { ImGui::StyleColorsDark(); }
			else { ImGui::StyleColorsLight(); }
			ImGui::End();
		}
		EndFrame();

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_XRGB(10, 20, 60), 1.0f, 0);
		if (SUCCEEDED(g_pd3dDevice->BeginScene())) {
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}
	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd) {
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) return false;

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;
	g_PicOneTexture = LoadTextureFromFile(
		g_pd3dDevice,
		"C:/Users/user/Desktop/Projects/guiprojectresources/Project2/Project2/x64/Debug/assets/centre.jpg"
	);

	g_PicTwoTexture = LoadTextureFromFile(
		g_pd3dDevice,
		"C:/Users/user/Desktop/Projects/guiprojectresources/Project2/Project2/x64/Debug/assets/holiday.jpg"
	);

	return true;
}
void CleanupDeviceD3D() {
	//Important.Pictures must be released!
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}
void ResetDevice() {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg) {
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam);
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}