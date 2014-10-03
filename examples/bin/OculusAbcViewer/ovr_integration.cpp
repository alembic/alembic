#include "ovr_integration.h"

#include <iostream>

#include <GL/glew.h>

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <GLFW/glfw3.h>

#include <OVR.h>
#include <OVR_CAPI_GL.h>

namespace ovr_integration {

struct OVRManagerImpl
{
    ovrHmd hmd;
    ovrHmdDesc hmd_descriptor;
    ovrGLConfig opengl_config;
    GLuint frame_buffer_object_id;
    ovrGLTexture eye_gl_texture[2];
    ovrEyeRenderDesc eye_render_desc[2];
    ovrFovPort eye_fov[2];
};

OVRManager::OVRManager():
    _impl(new OVRManagerImpl)
{
    ovr_Initialize();
    _impl->hmd = ovrHmd_Create(0);

    if (_impl->hmd)
    {
        ovrHmd_GetDesc(_impl->hmd, &_impl->hmd_descriptor);
    }
    else
    {
        std::cout << "No Rift Plugged in, using debug device." << std::endl;

        // If there isn't an HMD plugged in, create a debug device.
        _impl->hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        ovrHmd_GetDesc(_impl->hmd, &_impl->hmd_descriptor);
    }

    ovrHmd_StartSensor(
            _impl->hmd,
            ovrSensorCap_Orientation
            | ovrSensorCap_YawCorrection
            | ovrSensorCap_Position,
            ovrSensorCap_Orientation);
}

OVRManager::~OVRManager()
{
    ovrHmd_Destroy(_impl->hmd);
    ovr_Shutdown();
}

void OVRManager::initialize_opengl()
{
    _impl->opengl_config.OGL.Header.API = ovrRenderAPI_OpenGL;
    _impl->opengl_config.OGL.Header.Multisample = 0;
    _impl->opengl_config.OGL.Header.RTSize.w = _impl->hmd_descriptor.Resolution.w;
    _impl->opengl_config.OGL.Header.RTSize.h = _impl->hmd_descriptor.Resolution.h;

#ifdef __linux__
    _impl->opengl_config.OGL.Win = glfwGetX11Window(_impl->_window);
    _impl->opengl_config.OGL.Disp = glfwGetX11Window();
#endif

    int distortion = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

    if (
            !ovrHmd_ConfigureRendering(
                _impl->hmd,
                &_impl->opengl_config.Config,
                distortion,
                _impl->hmd_descriptor.DefaultEyeFov,
                _impl->eye_render_desc))
    {
        std::cerr << "ovrHmd_ConfigureRendering failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    ovrSizei texture_size = {0,0};
    for (int eye_index=0; eye_index<ovrEye_Count; eye_index++)
    {
        auto eye_fov = _impl->hmd_descriptor.DefaultEyeFov[eye_index];
        _impl->eye_fov[eye_index] = eye_fov;
        ovrSizei this_size = ovrHmd_GetFovTextureSize(
                _impl->hmd,
                ovrEye_Left,
                _impl->hmd_descriptor.DefaultEyeFov[eye_index],
                1.0f);
        texture_size.w += this_size.w;
        texture_size.h = std::max(texture_size.h, this_size.h);
    }

    glGenFramebuffers(1, &(_impl->frame_buffer_object_id));
    glBindFramebuffer(GL_FRAMEBUFFER, _impl->frame_buffer_object_id);

    /// Texture to render into.
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
            GL_TEXTURE_2D,
            0, GL_RGBA,
            texture_size.w, texture_size.h,
            0, GL_RGBA,
            GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLuint depth_buffer_id;
    glGenRenderbuffers(1, &depth_buffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);

    glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture_size.w, texture_size.h);
    glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id, 0);

    GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer status is not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    _impl->eye_gl_texture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
    _impl->eye_gl_texture[0].OGL.Header.RenderViewport.Pos = {0,0};
    _impl->eye_gl_texture[0].OGL.Header.RenderViewport.Size.w = texture_size.w/2;
    _impl->eye_gl_texture[0].OGL.Header.RenderViewport.Size.h = texture_size.h;
    _impl->eye_gl_texture[0].OGL.Header.TextureSize = texture_size;
    _impl->eye_gl_texture[0].OGL.TexId = texture_id;

    _impl->eye_gl_texture[1] = _impl->eye_gl_texture[0];
    _impl->eye_gl_texture[1].OGL.Header.RenderViewport.Pos.x = (texture_size.w+1)/2;
}

/// Apply the projection matrix to the GL state.
void _applyProjectionMatrix(ovrFovPort fov, double near, double far)
{
    OVR::Matrix4f projection_matrix = ovrMatrix4f_Projection(
            fov, near, far, true);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&(projection_matrix.Transposed().M[0][0]));
}

void OVRManager::reshape( GLFWwindow* win, int width, int height)
{
    _impl->opengl_config.OGL.Header.RTSize = {width, height};

    int distort_caps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;
    ovrHmd_ConfigureRendering(
            _impl->hmd,
            &_impl->opengl_config.Config,
            distort_caps,
            _impl->eye_fov,
            _impl->eye_render_desc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void OVRManager::draw_gl(
        void(*camera_function)(), void(*render_function)(),
        float scene_scale, double near, double far)
{
    ovrFrameTiming m_HmdFrameTiming = ovrHmd_BeginFrame(_impl->hmd, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, _impl->frame_buffer_object_id);

    // Clearing to gray helps see where the Oculus renders.
    glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int eye_index=0; eye_index<ovrEye_Count; eye_index++)
    {
        ovrEyeType eye = _impl->hmd_descriptor.EyeRenderOrder[eye_index];

        // OVR Render block
        {
            ovrPosef eye_pose = ovrHmd_BeginEyeRender(_impl->hmd, eye);

            auto header = _impl->eye_gl_texture[eye].OGL.Header;
            glViewport(
                    header.RenderViewport.Pos.x,  header.RenderViewport.Pos.y,
                    header.RenderViewport.Size.w, header.RenderViewport.Size.h);

            _applyProjectionMatrix(
                    _impl->eye_render_desc[eye].Fov, near, far);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            auto view_adjust = (_impl->eye_render_desc[eye].ViewAdjust);
            glTranslatef(
                    scene_scale * view_adjust.x,
                    scene_scale * view_adjust.y,
                    scene_scale * view_adjust.z);

            OVR::Quatf orientation = OVR::Quatf(eye_pose.Orientation);
            OVR::Matrix4f model_view_matrix = OVR::Matrix4f(orientation.Inverted());
            glMultMatrixf(&(model_view_matrix.Transposed().M[0][0]));

            // User camera and render functions.
            camera_function();
            render_function();

            ovrHmd_EndEyeRender(
                    _impl->hmd,
                    eye,
                    eye_pose,
                    &_impl->eye_gl_texture[eye].Texture);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ovrHmd_EndFrame(_impl->hmd);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

}; // namespace ovr_integration
