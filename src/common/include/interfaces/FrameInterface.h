#ifndef FRAME_INTERFACE_H
#define FRAME_INTERFACE_H
#include <concepts>

struct FrameInfo {
    FrameInfo& operator=(const FrameInfo& other) {
        if (&other != this)
        {
            frameNr = other.frameNr;
            timestampBOF = other.timestampBOF;
            timestampEOF = other.timestampEOF;
            readoutTime = other.readoutTime;
            expTime = other.expTime;
            colorWbScaleRed = other.colorWbScaleRed;
            colorWbScaleGreen = other.colorWbScaleGreen;
            colorWbScaleBlue = other.colorWbScaleBlue;
        }
        return *this;
    }

    uint32_t frameNr{ 0 };
    uint64_t timestampBOF{ 0 };
    uint64_t timestampEOF{ 0 };
    uint32_t readoutTime{ 0 };

    uint32_t expTime{ 0 };
    float colorWbScaleRed{ 1.0 };
    float colorWbScaleGreen{ 1.0 };
    float colorWbScaleBlue{ 1.0 };
};

template<typename T>
concept FrameConcept = requires(T c, const T& f, T const cc, void* vptr, const FrameInfo& pFrameInfo) {
    { c.SetData(vptr) } -> std::same_as<void>;
    { c.GetData() } -> std::same_as<void*>;
    { c.CopyData() } -> std::same_as<bool>;
    { cc.GetInfo() } -> std::same_as<FrameInfo*>;
    { c.SetInfo(pFrameInfo) } -> std::same_as<void>;
    { c.Copy(f, bool()) } -> std::same_as<bool>;
};

#endif //FRAME_INTERFACE_H
