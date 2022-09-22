#ifndef COLOR_CONFIG_INTERFACE_H
#define COLOR_CONFIG_INTERFACE_H
#include <concepts>

template<typename T>
concept ColorConfigConcept = requires(T c) {
    { c.Debayer() } -> std::same_as<void>;
};

/* template<typename T, typename A> */
/* concept ColorConfigConcept = requires(T c, A* pctx) { */
/*     { c.Debayer(pctx) } -> std::same_as<void>; */
/* }; */


#endif //COLOR_CONFIG_INTERFACE_H
