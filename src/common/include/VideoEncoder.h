#ifndef __VIDEO_ENCODER__H__
#define __VIDEO_ENCODER__H__

extern "C" {
    #include <libavutil/avassert.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/opt.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/timestamp.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/frame.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
}

class VideoEncoder {
    private:
        std::filesystem::path m_out;
        std::string m_codecName;
        bool m_open;

        int m_framerate;
        size_t m_width{0}, m_height{0};

        AVFormatContext *m_oc = nullptr;

        const AVCodec* m_codec = nullptr;
        AVCodecContext* m_codecCtx = nullptr;

        AVStream* m_stream = nullptr;
        AVFrame* m_frame = nullptr;
        AVFrame* m_tframe = nullptr;
        AVPacket* m_pkt = nullptr;
        AVDictionary *m_fmtOpts = NULL;

    public:
        VideoEncoder(std::filesystem::path outdir, std::string codec, int framerate, size_t width, size_t height) :
            m_out(outdir), m_codecName(codec), m_framerate(framerate), m_height(height), m_width(width), m_open(false) {
        }

        ~VideoEncoder() { close(); }

        bool Initialize() {
            if (!initOutputCtx()) { return false; }
            if (!initCodec()) { return false; }
            if (!initStream()) { return false; }
            if (!initFile()) { return false; }

            if (!(m_pkt = av_packet_alloc())) {
                spdlog::error("Could not allocate packet");
                return false;
            }
            //Allocate frames
            if(!(m_frame = allocFrame(AV_PIX_FMT_GRAY8, m_width, m_height))) {
                spdlog::error("Could not allocate frame");
                return false;
            }

            if (!(m_tframe = allocFrame(m_codecCtx->pix_fmt, m_width, m_height))) {
                spdlog::error("Could not allocate frame");
                return false;
            }

            m_open = true;
            spdlog::info("AVOutputFormat - codec_id: {}, extensions: {}", m_oc->oformat->video_codec, m_oc->oformat->extensions);
            return true;
        }

        void close() {
            if (m_open) {
                flushEncoder();

                if (av_write_trailer(m_oc) != 0) {
                    spdlog::error("Could not write trailer");
                    return;
                }

                if (!(m_oc->oformat->flags & AVFMT_NOFILE)) {
                    avio_closep(&m_oc->pb);
                }

                av_packet_free(&m_pkt);
                avcodec_free_context(&m_codecCtx);
                av_frame_free(&m_frame);
                av_frame_free(&m_tframe);
                avformat_free_context(m_oc);
                m_open = false;
            }
        }

        void writeFrame(uint8_t* data, size_t frameNr) {
            m_frame->pts = frameNr;
            m_tframe->pts = frameNr;
            std::memcpy(m_frame->data[0], data, m_width*m_height);

            if (m_frame->format != m_tframe->format) {
                SwsContext* gray8_to_pix_fmt = nullptr;
                gray8_to_pix_fmt = sws_getContext(
                                        m_frame->width, m_frame->height, (AVPixelFormat) m_frame->format,
                                        m_tframe->width, m_tframe->height, (AVPixelFormat) m_tframe->format,
                                        SWS_BICUBIC, NULL,NULL,NULL);

                sws_scale(gray8_to_pix_fmt, m_frame->data, m_frame->linesize, 0, m_height, m_tframe->data, m_tframe->linesize);
                encodeFrame(m_tframe);
            } else {
                encodeFrame(m_frame);
            }
        }

    private:
        bool initOutputCtx() {
            avformat_alloc_output_context2(&m_oc, nullptr, nullptr, m_out.string().c_str());
            if (m_oc == nullptr) {
                spdlog::error("Could not allocate output context");
                return false;
            }

            if (m_oc->oformat->video_codec == AV_CODEC_ID_NONE) {
                spdlog::error("Video codec not available");
                return false;
            }
            return true;
        }

        bool initCodec() {
            if (!(m_codec = avcodec_find_encoder_by_name(m_codecName.c_str()))) {
                spdlog::error("Count not find encoder");
                return false;
            }

            if (m_codec->id == AV_CODEC_ID_H264) {
                av_opt_set(m_codecCtx->priv_data, "preset", "slow", 0);
                av_dict_set(&m_fmtOpts, "movflags", "faststart", 0);
            }

            if (m_codec->id == AV_CODEC_ID_MPEG4) {
                av_dict_set(&m_fmtOpts, "movflags", "faststart", 0);
                av_dict_set(&m_fmtOpts, "brand", "mp42", 0);
                av_dict_set(&m_fmtOpts, "preset", "medium", 0 );
            }

            if (!(m_codecCtx = avcodec_alloc_context3(m_codec))) {
                spdlog::error("Could not allocate encoding context");
                return false;
            }

            if ((m_oc->oformat->flags & AVFMT_GLOBALHEADER) > 0) {
                m_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }

            m_codecCtx->bit_rate = 24000000;
            m_codecCtx->width = m_width;
            m_codecCtx->height = m_height;

            m_codecCtx->framerate = {m_framerate, 1};
            m_codecCtx->time_base = {1, m_framerate};

            m_codecCtx->gop_size = 3;
            m_codecCtx->max_b_frames = 2;
            m_codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

            if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
                spdlog::error("CouldNotOpenVideoCodec");
                return false;
            }

            return true;
        }

        bool initStream() {
            if(!(m_stream = avformat_new_stream(m_oc, m_codec))) {
                spdlog::error("Could not allocate stream");
                return false;
            }

            if (avcodec_parameters_from_context(m_stream->codecpar, m_codecCtx) < 0) {
                spdlog::error("Could not copy stream parameters");
                return false;
            }

            m_stream->id = m_oc->nb_streams - 1;
            m_stream->time_base = m_codecCtx->time_base;

            return true;
        }

        bool initFile() {
            av_dump_format(m_oc, 0, m_out.string().c_str(), 1);
            if (avio_open(&m_oc->pb, m_out.string().c_str(), AVIO_FLAG_WRITE) < 0) {
                spdlog::error("Could not open file");
                return false;
            }

            if (avformat_write_header(m_oc, &m_fmtOpts) != 0) {
                spdlog::error("Could not write header");
                return false;
            }
            return true;
        }

        AVFrame* allocFrame(AVPixelFormat fmt, int width, int height) {
            //Allocate frame
            AVFrame* frame = av_frame_alloc();
            if (frame == nullptr) { return nullptr; }

            frame->format = fmt;
            frame->width = width;
            frame->height = height;

            if (av_frame_get_buffer(frame, 0) < 0) {
                spdlog::error("Failed to get frame buffer");
                av_frame_free(&frame);
                return nullptr;
            }

            if (av_frame_make_writable(frame) < 0) {
                spdlog::error("Make writeable failed");
                av_frame_free(&frame);
                return nullptr;
            }
            return frame;
        }

        bool encodeFrame(AVFrame* frame) {
            av_packet_unref(m_pkt);
            if (avcodec_send_frame(m_codecCtx, frame) < 0) {
                spdlog::error("Could not send frame to encoder");
                return false;
            }

            while (true) {
                const int r = avcodec_receive_packet(m_codecCtx, m_pkt);
                if (r == AVERROR(EAGAIN) || r == AVERROR_EOF) {
                    break;
                } else if (r < 0) {
                    spdlog::error("Could not encode frame");
                    return false;
                }

                av_packet_rescale_ts(m_pkt, m_codecCtx->time_base, m_stream->time_base);
                m_pkt->stream_index = m_stream->index;

                if (av_interleaved_write_frame(m_oc, m_pkt) < 0) {
                    spdlog::error("Could not write output packet");
                    return false;
                }
            }
            return true;
        }

       bool flushEncoder() {
            if (!(m_codecCtx->codec->capabilities & AV_CODEC_CAP_DELAY)) {
                return false;
            }
 
            return encodeFrame(nullptr);
       }
};

#endif //__VIDEO_ENCODER__H__
