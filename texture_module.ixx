module;

#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include <d3d9.h>

export module texture;
export class Dx9Texture {
    LPDIRECT3DTEXTURE9 tex = nullptr;

public:
    Dx9Texture() = default;
    explicit Dx9Texture(LPDIRECT3DTEXTURE9 t) : tex(t) {}

    //delete copy and copy assignment:
    Dx9Texture(const Dx9Texture&) = delete;
    Dx9Texture& operator=(const Dx9Texture&) = delete;

    Dx9Texture(Dx9Texture&& other) noexcept
        : tex(other.tex) {
        other.tex = nullptr;
    }
    Dx9Texture& operator=(Dx9Texture&& other) noexcept {
        if (this != &other) {
            Release();
            tex = other.tex;
            other.tex = nullptr;
        }
        return *this;
    }
    ~Dx9Texture() noexcept {
        Release();
    }
    void Release() noexcept { 
        if (tex) {
            tex->Release();
            tex = nullptr;
        } 
    }
    void Show(ImVec2 size) const {
        if (tex) {
            ImGui::Image((void*)tex, size);
        }
    }
    LPDIRECT3DTEXTURE9 Get() const noexcept {
        return tex;
    }
    explicit operator bool() const noexcept {
        return tex != nullptr;
    }
};