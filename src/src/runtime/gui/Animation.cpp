#include "stratos/gui/Animation.h"
#include "stratos/NativeRegistry.h"
#include <unordered_map>
#include <memory>

namespace stratos {

// Animation storage
static int nextAnimId = 1;
static std::unordered_map<int, gui::AnimationManager::TweenPtr> animRegistry;

void NativeRegistry::initAnimation() {
    // Create a tween animation
    registerFunction("gui", "__gui_anim_create",
        [](const std::vector<std::any>& args) -> std::any {
            float start = static_cast<float>(std::any_cast<double>(args[0]));
            float end = static_cast<float>(std::any_cast<double>(args[1]));
            float duration = static_cast<float>(std::any_cast<double>(args[2]));
            std::string easingStr = args.size() > 3 ? std::any_cast<std::string>(args[3]) : "linear";

            gui::EasingType easing = gui::EasingType::Linear;
            if (easingStr == "easeIn") easing = gui::EasingType::EaseIn;
            else if (easingStr == "easeOut") easing = gui::EasingType::EaseOut;
            else if (easingStr == "easeInOut") easing = gui::EasingType::EaseInOut;

            auto tween = gui::AnimationManager::instance().createTween(start, end, duration, easing);
            int id = nextAnimId++;
            animRegistry[id] = tween;
            return id;
        });

    // Start an animation
    registerFunction("gui", "__gui_anim_start",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = animRegistry.find(id);
            if (it != animRegistry.end()) {
                it->second->start();
            }
            return true;
        });

    // Stop an animation
    registerFunction("gui", "__gui_anim_stop",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = animRegistry.find(id);
            if (it != animRegistry.end()) {
                it->second->stop();
            }
            return true;
        });

    // Reset an animation
    registerFunction("gui", "__gui_anim_reset",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = animRegistry.find(id);
            if (it != animRegistry.end()) {
                it->second->reset();
            }
            return true;
        });

    // Get current animation value
    registerFunction("gui", "__gui_anim_get_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = animRegistry.find(id);
            if (it != animRegistry.end()) {
                return static_cast<double>(it->second->getCurrentValue());
            }
            return 0.0;
        });

    // Check if animation is running
    registerFunction("gui", "__gui_anim_is_running",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = animRegistry.find(id);
            if (it != animRegistry.end()) {
                return it->second->isRunning();
            }
            return false;
        });

    // Destroy an animation
    registerFunction("gui", "__gui_anim_destroy",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            animRegistry.erase(id);
            return true;
        });
}

} // namespace stratos
