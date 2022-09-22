#ifndef PM_COLOR_CONFIG_H
#define PM_COLOR_CONFIG_H
#include <pvcam/pvcam_helper_color.h>

#include <interfaces/ColorConfigInterface.h>

namespace pm {
    template<typename ColorCtx>
    class ColorConfig {
        private:
            ColorCtx* m_colorCtx{nullptr};
        public:
            ColorConfig(ColorCtx* colorCtx) : m_colorCtx(colorCtx) { }
            ~ColorConfig() = default;

            void Debayer() {
                return;
            }
    };
};

/* static_assert(ColorConfigConcept<pm::ColorConfig<ph_color_context>, ph_color_context>); */
#endif //PM_COLOR_CONFIG_H
