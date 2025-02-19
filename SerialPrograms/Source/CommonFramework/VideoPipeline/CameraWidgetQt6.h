/*  Video Widget (Qt6)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_VideoPipeline_Qt6VideoWidget_H
#define PokemonAutomation_VideoPipeline_Qt6VideoWidget_H

#include <QtGlobal>
#if QT_VERSION_MAJOR == 6

#include <mutex>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QVideoFrame>
#include "Common/Cpp/SpinLock.h"
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/Inference/StatAccumulator.h"
#include "CameraInfo.h"
#include "CameraImplementations.h"
#include "VideoWidget.h"

class QCamera;
class QVideoSink;

namespace PokemonAutomation{
namespace CameraQt6QVideoSink{


class CameraBackend : public PokemonAutomation::CameraBackend{
public:
    virtual std::vector<CameraInfo> get_all_cameras() const override;
    virtual QString get_camera_name(const CameraInfo& info) const override;
    virtual VideoWidget* make_video_widget(
        QWidget& parent,
        LoggerQt& logger,
        const CameraInfo& info,
        const QSize& desired_resolution
    ) const override;
};



class VideoWidget : public PokemonAutomation::VideoWidget{
public:
    VideoWidget(
        QWidget* parent,
        Logger& logger,
        const CameraInfo& info, const QSize& desired_resolution
    );
    virtual ~VideoWidget();
    virtual QSize current_resolution() const override;
    virtual std::vector<QSize> supported_resolutions() const override;
    virtual void set_resolution(const QSize& size) override;

    //  Cannot call from UI thread or it will deadlock.
    virtual VideoSnapshot snapshot() override;

    virtual void resizeEvent(QResizeEvent* event) override;
private:
    void paintEvent(QPaintEvent*) override;

    Logger& m_logger;
    std::vector<QSize> m_resolutions;

    QCameraDevice m_info;
    QCamera* m_camera = nullptr;
    QMediaCaptureSession m_captureSession;
    QVideoSink* m_videoSink = nullptr;
    std::vector<QCameraFormat> m_formats;

    mutable std::mutex m_lock;
    SpinLock m_frame_lock;

    //  Last Frame
    QVideoFrame m_last_frame;
    WallClock m_last_frame_timestamp;
    uint64_t m_last_frame_seqnum = 0;

    //  Last Cached Image
    QImage m_last_image;
    WallClock m_last_image_timestamp;
    uint64_t m_last_image_seqnum = 0;
    PeriodicStatsReporterI32 m_stats_conversion;
};


}
}
#endif
#endif
