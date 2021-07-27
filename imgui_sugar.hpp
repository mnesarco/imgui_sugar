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

#include <imgui/imgui.h>

// clang-format off

// ----------------------------------------------------------------------------
// [SECTION] RAII guard implemetations
// ----------------------------------------------------------------------------

namespace ImGuiSugar
{
    using ScopeEndCallback = void(*)();

    // RAII scope guard for ImGui Begin* functions returning bool.
    template<bool AlwaysCallEnd>
    struct BooleanGuard
    {
        BooleanGuard(const bool state, const ScopeEndCallback end) noexcept
            : m_state(state), m_end(end) {}
    
        ~BooleanGuard() noexcept 
        {
            if (AlwaysCallEnd || m_state) { m_end(); }
        }

        operator bool() const & noexcept { return m_state; }

        private:
            const bool m_state;
            const ScopeEndCallback m_end;
    };

    // For special cases, transform void(*)(int) to void(*)()
    inline void PopStyleColor() { ImGui::PopStyleColor(1); };
    inline void PopStyleVar()   { ImGui::PopStyleVar(1); };
} // namespace ImGuiSugar

// ----------------------------------------------------------------------------
// [SECTION] Utility macros
// ----------------------------------------------------------------------------

// Portable Expression Statement, calls void function and returns true
#define IMGUI_SUGAR_ES(FN, ...) ([&]() -> bool { FN(__VA_ARGS__); return true; }())
#define IMGUI_SUGAR_ES_0(FN) ([&]() -> bool { FN(); return true; }())

// Concatenating symbols with __LINE__ requires two levels of indirection
#define IMGUI_SUGAR_CONCAT0(A, B) A ## B
#define IMGUI_SUGAR_CONCAT1(A, B) IMGUI_SUGAR_CONCAT0(A, B)

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

#define IMGUI_SUGAR_SCOPED_BOOL(BEGIN, END, ALWAYS, ...) \
    if (const ImGuiSugar::BooleanGuard<ALWAYS> _ui_scope_guard = {ImGui::BEGIN(__VA_ARGS__), &ImGui::END})

#define IMGUI_SUGAR_SCOPED_BOOL_0(BEGIN, END, ALWAYS) \
    if (const ImGuiSugar::BooleanGuard<ALWAYS> _ui_scope_guard = {ImGui::BEGIN(), &ImGui::END})

#define IMGUI_SUGAR_SCOPED_VOID_N(BEGIN, END, ...) \
    if (const ImGuiSugar::BooleanGuard<true> _ui_scope_guard = {IMGUI_SUGAR_ES(ImGui::BEGIN, __VA_ARGS__), &ImGui::END})

#define IMGUI_SUGAR_SCOPED_VOID_0(BEGIN, END) \
    if (const ImGuiSugar::BooleanGuard<true> _ui_scope_guard = {IMGUI_SUGAR_ES_0(ImGui::BEGIN), &ImGui::END})

#define IMGUI_SUGAR_PARENT_SCOPED_VOID_N(BEGIN, END, ...) \
    const ImGuiSugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) = {IMGUI_SUGAR_ES(ImGui::BEGIN, __VA_ARGS__), &ImGui::END}

// ---------------------------------------------------------------------------
// [SECTION] ImGui DSL
// ----------------------------------------------------------------------------

// Self scoped blocks

#define with_Window(...)             IMGUI_SUGAR_SCOPED_BOOL(Begin,                   End,               true,  __VA_ARGS__)
#define with_Child(...)              IMGUI_SUGAR_SCOPED_BOOL(BeginChild,              EndChild,          true,  __VA_ARGS__)
#define with_ChildFrame(...)         IMGUI_SUGAR_SCOPED_BOOL(BeginChildFrame,         EndChildFrame,     true,  __VA_ARGS__)
#define with_Combo(...)              IMGUI_SUGAR_SCOPED_BOOL(BeginCombo,              EndCombo,          false, __VA_ARGS__)
#define with_ListBox(...)            IMGUI_SUGAR_SCOPED_BOOL(BeginListBox,            EndListBox,        false, __VA_ARGS__)
#define with_Menu(...)               IMGUI_SUGAR_SCOPED_BOOL(BeginMenu,               EndMenu,           false, __VA_ARGS__)
#define with_Popup(...)              IMGUI_SUGAR_SCOPED_BOOL(BeginPopup,              EndPopup,          false, __VA_ARGS__)
#define with_PopupModal(...)         IMGUI_SUGAR_SCOPED_BOOL(BeginPopupModal,         EndPopup,          false, __VA_ARGS__)
#define with_PopupContextItem(...)   IMGUI_SUGAR_SCOPED_BOOL(BeginPopupContextItem,   EndPopup,          false, __VA_ARGS__)
#define with_PopupContextWindow(...) IMGUI_SUGAR_SCOPED_BOOL(BeginPopupContextWindow, EndPopup,          false, __VA_ARGS__)
#define with_PopupContextVoid(...)   IMGUI_SUGAR_SCOPED_BOOL(BeginPopupContextVoid,   EndPopup,          false, __VA_ARGS__)
#define with_Table(...)              IMGUI_SUGAR_SCOPED_BOOL(BeginTable,              EndTable,          false, __VA_ARGS__)
#define with_TabBar(...)             IMGUI_SUGAR_SCOPED_BOOL(BeginTabBar,             EndTabBar,         false, __VA_ARGS__)
#define with_TabItem(...)            IMGUI_SUGAR_SCOPED_BOOL(BeginTabItem,            EndTabItem,        false, __VA_ARGS__)
#define with_DragDropSource(...)     IMGUI_SUGAR_SCOPED_BOOL(BeginDragDropSource,     EndDragDropSource, false, __VA_ARGS__)
#define with_TreeNode(...)           IMGUI_SUGAR_SCOPED_BOOL(TreeNode,                TreePop,           false, __VA_ARGS__)
#define with_TreeNodeV(...)          IMGUI_SUGAR_SCOPED_BOOL(TreeNodeV,               TreePop,           false, __VA_ARGS__)
#define with_TreeNodeEx(...)         IMGUI_SUGAR_SCOPED_BOOL(TreeNodeEx,              TreePop,           false, __VA_ARGS__)
#define with_TreeNodeExV(...)        IMGUI_SUGAR_SCOPED_BOOL(TreeNodeExV,             TreePop,           false, __VA_ARGS__)

#define with_DragDropTarget          IMGUI_SUGAR_SCOPED_BOOL_0(BeginDragDropTarget,   EndDragDropTarget, false)
#define with_MainMenuBar             IMGUI_SUGAR_SCOPED_BOOL_0(BeginMainMenuBar,      EndMainMenuBar,    false)
#define with_MenuBar                 IMGUI_SUGAR_SCOPED_BOOL_0(BeginMenuBar,          EndMenuBar,        false)

#define with_Group                   IMGUI_SUGAR_SCOPED_VOID_0(BeginGroup,   EndGroup)
#define with_Tooltip                 IMGUI_SUGAR_SCOPED_VOID_0(BeginTooltip, EndTooltip)

#define with_Font(...)               IMGUI_SUGAR_SCOPED_VOID_N(PushFont,               PopFont,               __VA_ARGS__)
#define with_AllowKeyboardFocus(...) IMGUI_SUGAR_SCOPED_VOID_N(PushAllowKeyboardFocus, PopAllowKeyboardFocus, __VA_ARGS__)
#define with_ButtonRepeat(...)       IMGUI_SUGAR_SCOPED_VOID_N(PushButtonRepeat,       PopButtonRepeat,       __VA_ARGS__)
#define with_ItemWidth(...)          IMGUI_SUGAR_SCOPED_VOID_N(PushItemWidth,          PopItemWidth,          __VA_ARGS__)
#define with_TextWrapPos(...)        IMGUI_SUGAR_SCOPED_VOID_N(PushTextWrapPos,        PopTextWrapPos,        __VA_ARGS__)
#define with_ID(...)                 IMGUI_SUGAR_SCOPED_VOID_N(PushID,                 PopID,                 __VA_ARGS__)
#define with_ClipRect(...)           IMGUI_SUGAR_SCOPED_VOID_N(PushClipRect,           PopClipRect,           __VA_ARGS__)
#define with_TextureID(...)          IMGUI_SUGAR_SCOPED_VOID_N(PushTextureID,          PopTextureID,          __VA_ARGS__)

// Non self scoped guards (managed by parent scope)

#define set_Font(...)                IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushFont,               PopFont,               __VA_ARGS__)
#define set_AllowKeyboardFocus(...)  IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushAllowKeyboardFocus, PopAllowKeyboardFocus, __VA_ARGS__)
#define set_ButtonRepeat(...)        IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushButtonRepeat,       PopButtonRepeat,       __VA_ARGS__)
#define set_ItemWidth(...)           IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushItemWidth,          PopItemWidth,          __VA_ARGS__)
#define set_TextWrapPos(...)         IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushTextWrapPos,        PopTextWrapPos,        __VA_ARGS__)
#define set_ID(...)                  IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushID,                 PopID,                 __VA_ARGS__)
#define set_ClipRect(...)            IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushClipRect,           PopClipRect,           __VA_ARGS__)
#define set_TextureID(...)           IMGUI_SUGAR_PARENT_SCOPED_VOID_N(PushTextureID,          PopTextureID,          __VA_ARGS__)

// Special case (overloaded functions StyleColor and StyleVar)

#define set_StyleColor(...)                                                                                 \
    const ImGuiSugar::BooleanGuard<true>                                                                    \
        IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) =                                                        \
        {IMGUI_SUGAR_ES(ImGui::PushStyleColor, __VA_ARGS__), &ImGuiSugar::PopStyleColor}

#define set_StyleVar(...)                                                                                   \
    const ImGuiSugar::BooleanGuard<true>                                                                    \
        IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) =                                                        \
        {IMGUI_SUGAR_ES(ImGui::PushStyleVar, __VA_ARGS__), &ImGuiSugar::PopStyleVar}

#define with_StyleColor(...)                                                                                \
    if (const ImGuiSugar::BooleanGuard<true>                                                                \
        IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) =                                                        \
        {IMGUI_SUGAR_ES(ImGui::PushStyleColor, __VA_ARGS__), &ImGuiSugar::PopStyleColor})

#define with_StyleVar(...)                                                                                  \
    if (const ImGuiSugar::BooleanGuard<true>                                                                \
        IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) =                                                        \
        {IMGUI_SUGAR_ES(ImGui::PushStyleVar, __VA_ARGS__), &ImGuiSugar::PopStyleVar})

// Non RAII 

#define with_CollapsingHeader(...) if (ImGui::CollapsingHeader(__VA_ARGS__))
#define with_MenuItem(...) if (ImGui::MenuItem(__VA_ARGS__))

// clang-format on
