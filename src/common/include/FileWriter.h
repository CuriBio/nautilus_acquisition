#ifndef FILEWRITER_H
#define FILEWRITER_H

template<FileConcept F, FrameConcept FRAME>
class FileWriter {
    private:
        uint32_t m_frameIndex{ 0 };

    public:
        FileWriter(std::string name);
        ~FileWriter();

        bool WriteFrame(std::shared_ptr<FRAME> frame);
};
#endif //FILEWRITER_H
