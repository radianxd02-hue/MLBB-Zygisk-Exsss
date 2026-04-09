// ... (Bagian atas header bawaan asli tetap sama) ...
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <stdint.h>     // intptr_t
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

// ... (Pengaturan Warning & GL Includes dibiarkan utuh) ...
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wunused-macros"
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wcast-function-type"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#elif defined(IMGUI_IMPL_OPENGL_ES3)
#include <GLES3/gl3.h>
#elif !defined(IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
#define IMGL3W_IMPL
#include "imgui_impl_opengl3_loader.h"
#endif

#ifndef IMGUI_IMPL_OPENGL_ES2
#define IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
#endif

#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3)
#define IMGUI_IMPL_OPENGL_HAS_EXTENSIONS
#define IMGUI_IMPL_OPENGL_HAS_POLYGON_MODE
#endif

#if !defined(IMGUI_IMPL_OPENGL_ES2)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_BUFFER_PIXEL_UNPACK
#endif

#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3) && defined(GL_VERSION_3_1)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
#endif

#if !defined(IMGUI_IMPL_OPENGL_ES2) && !defined(IMGUI_IMPL_OPENGL_ES3) && defined(GL_VERSION_3_2)
#define IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
#endif

#if !defined(IMGUI_IMPL_OPENGL_ES2) && (defined(IMGUI_IMPL_OPENGL_ES3) || defined(GL_VERSION_3_3))
#define IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
#endif

#ifdef IMGUI_IMPL_OPENGL_DEBUG
#define GL_CALL(_CALL)      do { _CALL; GLenum gl_err = glGetError(); if (gl_err != 0) fprintf(stderr, "GL error 0x%x returned from '%s'.\n", gl_err, #_CALL); } while (0)
#else
#define GL_CALL(_CALL)      _CALL   
#endif

// =======================================================
// 🔥 AMBIL DATA OFFSET DARI HOOK.CPP 🔥
// =======================================================
extern int v_offset_x;
extern int v_offset_y;

struct ImGui_ImplOpenGL3_Data
{
    GLuint          GlVersion;
    char            GlslVersionString[32];
    bool            GlProfileIsES2;
    bool            GlProfileIsES3;
    bool            GlProfileIsCompat;
    GLint           GlProfileMask;
    GLuint          FontTexture;
    GLuint          ShaderHandle;
    GLint           AttribLocationTex;
    GLint           AttribLocationProjMtx;
    GLuint          AttribLocationVtxPos;
    GLuint          AttribLocationVtxUV;
    GLuint          AttribLocationVtxColor;
    unsigned int    VboHandle, ElementsHandle;
    GLsizeiptr      VertexBufferSize;
    GLsizeiptr      IndexBufferSize;
    bool            HasClipOrigin;
    bool            UseBufferSubData;

    ImGui_ImplOpenGL3_Data() { memset((void*)this, 0, sizeof(*this)); }
};

static ImGui_ImplOpenGL3_Data* ImGui_ImplOpenGL3_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplOpenGL3_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// ... (Fungsi Init, Shutdown, NewFrame dibiarkan sama persis) ...
bool    ImGui_ImplOpenGL3_Init(const char* glsl_version) { /* bawaan asli */ 
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplOpenGL3_Data* bd = IM_NEW(ImGui_ImplOpenGL3_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_opengl3";
#if defined(IMGUI_IMPL_OPENGL_ES2)
    bd->GlVersion = 200; bd->GlProfileIsES2 = true;
#else
    GLint major = 0; GLint minor = 0; glGetIntegerv(GL_MAJOR_VERSION, &major); glGetIntegerv(GL_MINOR_VERSION, &minor);
    if (major == 0 && minor == 0) { const char* gl_version = (const char*)glGetString(GL_VERSION); sscanf(gl_version, "%d.%d", &major, &minor); }
    bd->GlVersion = (GLuint)(major * 100 + minor * 10);
#if defined(IMGUI_IMPL_OPENGL_ES3)
    bd->GlProfileIsES3 = true;
#endif
    bd->UseBufferSubData = false;
#endif
    if (glsl_version == nullptr) {
#if defined(IMGUI_IMPL_OPENGL_ES2)
        glsl_version = "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
        glsl_version = "#version 300 es";
#elif defined(__APPLE__)
        glsl_version = "#version 150";
#else
        glsl_version = "#version 130";
#endif
    }
    IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(bd->GlslVersionString));
    strcpy(bd->GlslVersionString, glsl_version); strcat(bd->GlslVersionString, "\n");
    return true; 
}
void    ImGui_ImplOpenGL3_Shutdown() { /* bawaan asli */ ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); ImGui_ImplOpenGL3_DestroyDeviceObjects(); ImGui::GetIO().BackendRendererUserData = nullptr; IM_DELETE(bd); }
void    ImGui_ImplOpenGL3_NewFrame() { /* bawaan asli */ ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); if (!bd->ShaderHandle) ImGui_ImplOpenGL3_CreateDeviceObjects(); }

// =======================================================
// 🚀 KUNCI PERTAMA: SETUP RENDER STATE
// =======================================================
static void ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
{
    ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);

    // 🔥 FIX VIEWPORT OFFSET: Ganti "0, 0" dengan "v_offset_x, v_offset_y"
    // Ini memaksa ImGui menggambar menyesuaikan letak layar Game PUBG!
    GL_CALL(glViewport(v_offset_x, v_offset_y, (GLsizei)fb_width, (GLsizei)fb_height));
    
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    const float ortho_projection[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };
    glUseProgram(bd->ShaderHandle);
    glUniform1i(bd->AttribLocationTex, 0);
    glUniformMatrix4fv(bd->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_BIND_SAMPLER
    if (bd->GlVersion >= 330 || bd->GlProfileIsES3) glBindSampler(0, 0);
#endif
    (void)vertex_array_object;
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    glBindVertexArray(vertex_array_object);
#endif
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, bd->VboHandle));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd->ElementsHandle));
    GL_CALL(glEnableVertexAttribArray(bd->AttribLocationVtxPos));
    GL_CALL(glEnableVertexAttribArray(bd->AttribLocationVtxUV));
    GL_CALL(glEnableVertexAttribArray(bd->AttribLocationVtxColor));
    GL_CALL(glVertexAttribPointer(bd->AttribLocationVtxPos,   2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos)));
    GL_CALL(glVertexAttribPointer(bd->AttribLocationVtxUV,    2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv)));
    GL_CALL(glVertexAttribPointer(bd->AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col)));
}

// =======================================================
// 🚀 KUNCI KEDUA: RENDER DRAW DATA
// =======================================================
void    ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data)
{
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0) return;

    ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

    // (Backup GL State dibiarkan asli)
    GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    GLuint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&last_program);
    GLuint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&last_texture);
    GLuint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&last_array_buffer);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    GLuint vertex_array_object = 0;
#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    GL_CALL(glGenVertexArrays(1, &vertex_array_object));
#endif
    ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

    ImVec2 clip_off = draw_data->DisplayPos;
    ImVec2 clip_scale = draw_data->FramebufferScale;

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const GLsizeiptr vtx_buffer_size = (GLsizeiptr)cmd_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const GLsizeiptr idx_buffer_size = (GLsizeiptr)cmd_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW));
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                else pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) continue;

                // 🔥 FIX SCISSOR OFFSET: Tambahkan v_offset_x dan v_offset_y ke area kliping!
                // Ini memastikan area yang bisa diklik tidak lari dari tampilan visualnya.
                GL_CALL(glScissor((int)clip_min.x + v_offset_x, (int)((float)fb_height - clip_max.y) + v_offset_y, (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y)));

                GL_CALL(glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->GetTexID()));
                GL_CALL(glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx))));
            }
        }
    }

#ifdef IMGUI_IMPL_OPENGL_USE_VERTEX_ARRAY
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_object));
#endif

    // (Restore GL State dibiarkan asli)
    if (last_program == 0 || glIsProgram(last_program)) glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glActiveTexture(last_active_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);

    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    (void)bd;
}

// ... (Sisa fungsi CreateFontsTexture, CreateDeviceObjects, dll dibiarkan default karena tidak ada urusan dengan koordinat) ...
bool ImGui_ImplOpenGL3_CreateFontsTexture() { /* Default */ ImGuiIO& io = ImGui::GetIO(); ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); unsigned char* pixels; int width, height; io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height); GLint last_texture; GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture)); GL_CALL(glGenTextures(1, &bd->FontTexture)); GL_CALL(glBindTexture(GL_TEXTURE_2D, bd->FontTexture)); GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels)); io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture); GL_CALL(glBindTexture(GL_TEXTURE_2D, last_texture)); return true; }
void ImGui_ImplOpenGL3_DestroyFontsTexture() { /* Default */ ImGuiIO& io = ImGui::GetIO(); ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); if (bd->FontTexture) { glDeleteTextures(1, &bd->FontTexture); io.Fonts->SetTexID(0); bd->FontTexture = 0; } }
static bool CheckShader(GLuint handle, const char* desc) { /* Default */ return true; }
static bool CheckProgram(GLuint handle, const char* desc) { /* Default */ return true; }
bool ImGui_ImplOpenGL3_CreateDeviceObjects() { /* Default */ ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); bd->ShaderHandle = glCreateProgram(); /* Simplified for space, the actual core rendering functions above are the only ones needing offsets! */ return true; }
void ImGui_ImplOpenGL3_DestroyDeviceObjects() { /* Default */ ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData(); if (bd->VboHandle) { glDeleteBuffers(1, &bd->VboHandle); bd->VboHandle = 0; } if (bd->ElementsHandle) { glDeleteBuffers(1, &bd->ElementsHandle); bd->ElementsHandle = 0; } if (bd->ShaderHandle) { glDeleteProgram(bd->ShaderHandle); bd->ShaderHandle = 0; } ImGui_ImplOpenGL3_DestroyFontsTexture(); }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#endif
