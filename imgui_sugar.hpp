#pragma once

// The MIT License (MIT)
//
// Copyright (c) 2021 Frank D. Martinez M. <aka mnesarco>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <imgui.h>
#include <type_traits>
#include <utility>

// clang-format off

// ----------------------------------------------------------------------------
// [SECTION] RAII guard implemetations
// ----------------------------------------------------------------------------

namespace sugar
{
    using end_fn_ptr = void(*)();

    // RAII scope guard for ImGui Begin* functions returning bool.
    template<bool AlwaysCallEnd>
    struct guard_bool
    {
        guard_bool(const bool state, const end_fn_ptr end) noexcept 
            : _state(state), _end(end) {}
    
        ~guard_bool() noexcept 
        {
            if (AlwaysCallEnd || _state) { _end(); }
        }

        operator bool() const noexcept { return _state; }

        private:
            bool _state;
            end_fn_ptr _end;
    };

    // RAII scope guard for ImGui Begin*/Push* functions returning void
    template<typename ...Args>
    struct guard_void
    {
        using begin_fn_t = void (*)(Args...);

        guard_void(begin_fn_t begin, end_fn_ptr end, Args&&... args) noexcept 
            : _end(end) { begin(std::forward<Args>(args)...); }
        
        ~guard_void() noexcept { _end(); }
        
        constexpr operator bool() const noexcept { return true; }

        private:
            end_fn_ptr _end;
    };
}

// ----------------------------------------------------------------------------
// [SECTION] Utility macros
// ----------------------------------------------------------------------------

// Concatenating symbols with __LINE__ requires two levels of indirection
#define _SUGAR_CONCAT0(A, B) A ## B
#define _SUGAR_CONCAT1(A, B) _SUGAR_CONCAT0(A, B)

// ----------------------------------------------------------------------------
// [SECTION] Generic macros to simplify repetitive declarations
// ----------------------------------------------------------------------------
// 
// +----------------------+-------------------+-----------------+---------------------+
// | BEGIN                | END               | ALWAYS          | __VA_ARGS__         |
// +----------------------+-------------------+-----------------+---------------------+
// | ImGui::Begin*/Push*  | ImGui::End*/Pop*  | Is call to END  | ImGui::Begin*/Push* |
// | function name        | function name     | unconditional?  | function arguments  |
// +----------------------+-------------------+-----------------+---------------------+

#define _SUGAR_SCOPED_BOOL(BEGIN, END, ALWAYS, ...) \
    if (const sugar::guard_bool<ALWAYS> _ui_scope_guard = {ImGui::BEGIN(__VA_ARGS__), &ImGui::END})

#define _SUGAR_SCOPED_VOID_N(BEGIN, END, ...) \
    if (const sugar::guard_void _ui_scope_guard = {&ImGui::BEGIN, &ImGui::END, __VA_ARGS__})

#define _SUGAR_SCOPED_VOID_0(BEGIN, END) \
    if (const sugar::guard_void _ui_scope_guard = {&ImGui::BEGIN, &ImGui::END})

#define _SUGAR_PARENT_SCOPED_VOID_N(BEGIN, END, ...) \
    const sugar::guard_void _SUGAR_CONCAT1(_ui_scope_, __LINE__) = {&ImGui::BEGIN, &ImGui::END, __VA_ARGS__}

// ---------------------------------------------------------------------------
// [SECTION] ImGui DSL
// ----------------------------------------------------------------------------

// Self scoped blocks

#define with_Window(...)             _SUGAR_SCOPED_BOOL(Begin,                   End,               true,  __VA_ARGS__)
#define with_Child(...)              _SUGAR_SCOPED_BOOL(BeginChild,              EndChild,          true,  __VA_ARGS__)
#define with_ChildFrame(...)         _SUGAR_SCOPED_BOOL(BeginChildFrame,         EndChildFrame,     true,  __VA_ARGS__)
#define with_Combo(...)              _SUGAR_SCOPED_BOOL(BeginCombo,              EndCombo,          false, __VA_ARGS__)
#define with_ListBox(...)            _SUGAR_SCOPED_BOOL(BeginListBox,            EndListBox,        false, __VA_ARGS__)
#define with_MenuBarArgs(...)        _SUGAR_SCOPED_BOOL(BeginMenuBar,            EndMenuBar,        false, __VA_ARGS__)
#define with_MainMenuBarArgs(...)    _SUGAR_SCOPED_BOOL(BeginMainMenuBar,        EndMainMenuBar,    false, __VA_ARGS__)
#define with_Menu(...)               _SUGAR_SCOPED_BOOL(BeginMenu,               EndMenu,           false, __VA_ARGS__)
#define with_Popup(...)              _SUGAR_SCOPED_BOOL(BeginPopup,              EndPopup,          false, __VA_ARGS__)
#define with_PopupModal(...)         _SUGAR_SCOPED_BOOL(BeginPopupModal,         EndPopup,          false, __VA_ARGS__)
#define with_PopupContextItem(...)   _SUGAR_SCOPED_BOOL(BeginPopupContextItem,   EndPopup,          false, __VA_ARGS__)
#define with_PopupContextWindow(...) _SUGAR_SCOPED_BOOL(BeginPopupContextWindow, EndPopup,          false, __VA_ARGS__)
#define with_PopupContextVoid(...)   _SUGAR_SCOPED_BOOL(BeginPopupContextVoid,   EndPopup,          false, __VA_ARGS__)
#define with_Table(...)              _SUGAR_SCOPED_BOOL(BeginTable,              EndTable,          false, __VA_ARGS__)
#define with_TabBar(...)             _SUGAR_SCOPED_BOOL(BeginTabBar,             EndTabBar,         false, __VA_ARGS__)
#define with_TabItem(...)            _SUGAR_SCOPED_BOOL(BeginTabItem,            EndTabItem,        false, __VA_ARGS__)
#define with_DragDropSource(...)     _SUGAR_SCOPED_BOOL(BeginDragDropSource,     EndDragDropSource, false, __VA_ARGS__)
#define with_DragDropTargetArgs(...) _SUGAR_SCOPED_BOOL(BeginDragDropTarget,     EndDragDropTarget, false, __VA_ARGS__)
#define with_TreeNode(...)           _SUGAR_SCOPED_BOOL(TreeNode,                TreePop,           false, __VA_ARGS__)
#define with_TreeNodeV(...)          _SUGAR_SCOPED_BOOL(TreeNodeV,               TreePop,           false, __VA_ARGS__)
#define with_TreeNodeEx(...)         _SUGAR_SCOPED_BOOL(TreeNodeEx,              TreePop,           false, __VA_ARGS__)
#define with_TreeNodeExV(...)        _SUGAR_SCOPED_BOOL(TreeNodeExV,             TreePop,           false, __VA_ARGS__)

#define with_DragDropTarget          with_DragDropTargetArgs()
#define with_MainMenuBar             with_MainMenuBarArgs()
#define with_MenuBar                 with_MenuBarArgs()

#define with_Group                   _SUGAR_SCOPED_VOID_0(BeginGroup,   EndGroup)
#define with_Tooltip                 _SUGAR_SCOPED_VOID_0(BeginTooltip, EndTooltip)

#define with_Font(...)               _SUGAR_SCOPED_VOID_N(PushFont,               PopFont,               __VA_ARGS__)
#define with_AllowKeyboardFocus(...) _SUGAR_SCOPED_VOID_N(PushAllowKeyboardFocus, PopAllowKeyboardFocus, __VA_ARGS__)
#define with_ButtonRepeat(...)       _SUGAR_SCOPED_VOID_N(PushButtonRepeat,       PopButtonRepeat,       __VA_ARGS__)
#define with_ItemWidth(...)          _SUGAR_SCOPED_VOID_N(PushItemWidth,          PopItemWidth,          __VA_ARGS__)
#define with_TextWrapPos(...)        _SUGAR_SCOPED_VOID_N(PushTextWrapPos,        PopTextWrapPos,        __VA_ARGS__)
#define with_ID(...)                 _SUGAR_SCOPED_VOID_N(PushID,                 PopID,                 __VA_ARGS__)
#define with_ClipRect(...)           _SUGAR_SCOPED_VOID_N(PushClipRect,           PopClipRect,           __VA_ARGS__)
#define with_TextureID(...)          _SUGAR_SCOPED_VOID_N(PushTextureID,          PopTextureID,          __VA_ARGS__)

// Non self scoped guards (managed by parent scope)

#define set_Font(...)                _SUGAR_PARENT_SCOPED_VOID_N(PushFont,               PopFont,               __VA_ARGS__)
#define set_AllowKeyboardFocus(...)  _SUGAR_PARENT_SCOPED_VOID_N(PushAllowKeyboardFocus, PopAllowKeyboardFocus, __VA_ARGS__)
#define set_ButtonRepeat(...)        _SUGAR_PARENT_SCOPED_VOID_N(PushButtonRepeat,       PopButtonRepeat,       __VA_ARGS__)
#define set_ItemWidth(...)           _SUGAR_PARENT_SCOPED_VOID_N(PushItemWidth,          PopItemWidth,          __VA_ARGS__)
#define set_TextWrapPos(...)         _SUGAR_PARENT_SCOPED_VOID_N(PushTextWrapPos,        PopTextWrapPos,        __VA_ARGS__)
#define set_ID(...)                  _SUGAR_PARENT_SCOPED_VOID_N(PushID,                 PopID,                 __VA_ARGS__)
#define set_ClipRect(...)            _SUGAR_PARENT_SCOPED_VOID_N(PushClipRect,           PopClipRect,           __VA_ARGS__)
#define set_TextureID(...)           _SUGAR_PARENT_SCOPED_VOID_N(PushTextureID,          PopTextureID,          __VA_ARGS__)

// Special case (overloaded functions StyleColor and StyleVar)

namespace sugar {
    inline void PopStyleColor() { ImGui::PopStyleColor(1); };
    inline void PopStyleVar()   { ImGui::PopStyleVar(1); };
}

#define set_StyleColor(ITEM, ...)                                                                                   \
    const sugar::guard_void<ImGuiCol,                                                                               \
        std::conditional<std::is_integral<decltype(__VA_ARGS__)>::value, ImU32, const ImVec4&>::type>               \
        _SUGAR_CONCAT1(_ui_scope_, __LINE__) = {&ImGui::PushStyleColor, &sugar::PopStyleColor, ITEM, __VA_ARGS__}

#define set_StyleVar(ITEM, ...)                                                                                     \
    const sugar::guard_void<ImGuiStyleVar,                                                                          \
        std::conditional<std::is_arithmetic<decltype(__VA_ARGS__)>::value, float, const ImVec2&>::type>             \
        _SUGAR_CONCAT1(_ui_scope_, __LINE__) = {&ImGui::PushStyleVar, &sugar::PopStyleVar, ITEM, __VA_ARGS__}

#define with_StyleColor(...) if (set_StyleColor(__VA_ARGS__))
#define with_StyleVar(...)   if (set_StyleVar(__VA_ARGS__))

// Non RAII 

#define with_CollapsingHeader(...) if (ImGui::CollapsingHeader(__VA_ARGS__))

// clang-format on
