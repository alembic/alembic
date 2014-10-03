#ifndef OVR_INTEGRATION_H
#define OVR_INTEGRATION_H

#include <memory>

/// \file ovr_integration.h
/// \desc Library for integration with Oculus Rift. Assumes OVR SDK 0.3.2
struct GLFWwindow;

namespace ovr_integration {

struct OVRManagerImpl;

/// wrapper around oculus rift code
class OVRManager
{
    public:
        /// Constructor connects to Oculus HMD.
        OVRManager();
        /// Destructor shuts down Oculus HMD.
        ~OVRManager();

        /// configures the GL framebfuffers for rendering into the oculus.
        void initialize_opengl();

        /// render the scene into the oculus.  pass your camera function and
        /// scene rendering function in.
        void draw_gl(
                void(*camera_function)(),
                void(*render_func)(),
                float, // scene_scale
                double, // near plane distance
                double);// far plane distance

        /// When you're not rendering fullscreen, handles window size changes.
        void reshape(GLFWwindow* win, int width, int height);

    private:
        std::unique_ptr<OVRManagerImpl> _impl;
};

};

#endif /* end of include guard: OVR_INTEGRATION_H  */
