/*  Video Widget (Qt5)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_VideoPipeline_Qt5VideoWidget_H
#define PokemonAutomation_VideoPipeline_Qt5VideoWidget_H

#include <QtGlobal>
#if QT_VERSION_MAJOR == 5

#include <condition_variable>
#include <QThread>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVideoProbe>
#include "Common/Cpp/SpinLock.h"
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/Inference/StatAccumulator.h"
#include "CameraInfo.h"
#include "CameraImplementations.h"
#include "VideoWidget.h"

namespace PokemonAutomation{
namespace CameraQt5QCameraViewfinder{


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
        LoggerQt& logger,
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
    //  All of these must be called under the lock.
    QImage direct_snapshot_image(std::unique_lock<std::mutex>& lock);
    QImage direct_snapshot_probe(bool flip_vertical);

    VideoSnapshot snapshot_image(std::unique_lock<std::mutex>& lock);
    VideoSnapshot snapshot_probe();

    bool determine_frame_orientation(std::unique_lock<std::mutex>& lock);

private:
    friend class FrameReader;

    enum class CaptureStatus{
        PENDING,
        COMPLETED,
        CANCELED,
    };
    struct PendingCapture{
        CaptureStatus status = CaptureStatus::PENDING;
        QImage image;
        std::condition_variable cv;
    };

    LoggerQt& m_logger;
    QCamera* m_camera = nullptr;
    QCameraViewfinder* m_camera_view = nullptr;

    size_t m_max_frame_rate;
    std::chrono::milliseconds m_frame_period;
    std::vector<QSize> m_resolutions;

    mutable std::mutex m_lock;
    QSize m_resolution;

    QCameraImageCapture* m_capture = nullptr;
    std::map<int, PendingCapture> m_pending_captures;

//    SpinLock m_capture_lock;
    QVideoProbe* m_probe = nullptr;
    WallClock m_last_orientation_attempt;
    bool m_orientation_known = false;
//    bool m_use_probe_frames = false;
    bool m_flip_vertical = false;

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
