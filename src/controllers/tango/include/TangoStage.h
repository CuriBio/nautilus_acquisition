#ifndef TANGO_STAGE_H
#define TANGO_STAGE_H

class CTango;

class TangoStage {
    private:
        CTango* m_tango;
        std::string m_comPort;
        double m_x{0.0}, m_y{0.0};
        bool m_open{false};

    public:
        TangoStage(std::string comPort);
        ~TangoStage();

        bool GetCurrentPos(double& x, double& y);
        bool SetRelativePos(double x, double y, bool block);
        bool SetAbsolutePos(double x, double y, bool block);
        bool Calibrate();
        bool RMeasure();
        bool Connected() const;
        int GetError() const;
};
#endif //TANGO_STAGE_H
