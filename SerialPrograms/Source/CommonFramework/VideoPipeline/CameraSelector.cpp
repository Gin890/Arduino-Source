/*  Camera Selector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QJsonArray>
#include <QJsonObject>
#include "Common/Compiler.h"
#include "Common/Qt/QtJsonTools.h"
#include "CameraSelector.h"
#include "CameraSelectorWidget.h"

namespace PokemonAutomation{


const QString CameraSelector::JSON_CAMERA       = "Device";
const QString CameraSelector::JSON_RESOLUTION   = "Resolution";


CameraSelector::CameraSelector(QSize default_resolution)
    : m_default_resolution(default_resolution)
    , m_current_resolution(default_resolution)
{}
CameraSelector::CameraSelector(QSize default_resolution, const QJsonValue& json)
    : CameraSelector(default_resolution)
{
    load_json(json);
}

void CameraSelector::load_json(const QJsonValue& json){
    QJsonObject obj = json.toObject();
    QString name;
    if (!json_get_string(name, obj, JSON_CAMERA)){
        return;
    }
    m_camera = CameraInfo(name.toStdString());
    QJsonArray res = json_get_array_nothrow(obj, JSON_RESOLUTION);
    if (res.size() == 2 && res[0].isDouble() && res[1].isDouble()){
        m_current_resolution = QSize(res[0].toInt(), res[1].toInt());
    }
}
QJsonValue CameraSelector::to_json() const{
    QJsonObject root;
    root.insert(JSON_CAMERA, QString::fromStdString(m_camera.device_name()));
    QJsonArray res;
    res += m_current_resolution.width();
    res += m_current_resolution.height();
    root.insert(JSON_RESOLUTION, res);
    return root;
}

CameraSelectorWidget* CameraSelector::make_ui(QWidget& parent, LoggerQt& logger, VideoDisplayWidget& holder){
    return new CameraSelectorWidget(parent, logger, *this, holder);
}











}
