#ifndef TANGO_STAGE_H
#define TANGO_STAGE_H

class CTango;

class TangoStage {
    private:
        CTango* m_tango;
        double m_x{0.0}, m_y{0.0};

    public:
        TangoStage();
        ~TangoStage();

        bool GetCurrentPos(double& x, double& y);
        bool SetRelativePos(double x, double y, bool block);
        bool SetAbsolutePos(double x, double y, bool block);
        int GetError() const;
};
#endif //TANGO_STAGE_H
