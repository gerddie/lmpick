#include "landmarklistio.hh"
#include "landmarklist.hh"
#include "landmark.hh"

#include <QDomDocument>
#include <QFile>

using std::make_pair;
using std::make_shared;
using std::pair;

LandmarklistIO::LandmarklistIO()
{

}

template <typename T>
struct read_tag_dispatch {
        static T apply(const QString& value) {
                static_assert(sizeof(T) == 0, "No generic implementation available, needs , Missing specialization");
        }
};

template <>
struct read_tag_dispatch<QString> {
        static QString apply(const QString& value) {
                return value;
        }
};

template <>
struct read_tag_dispatch<float> {
        static float apply(const QString& value) {
                return value.toFloat();
        }
};

template <>
struct read_tag_dispatch<QVector3D> {
        static QVector3D apply(const QString& value) {
                QStringList v = value.split(" ");
                if (v.length() != 3) {
                        throw std::invalid_argument("Failed reading QVector3D");
                }
                float x = v.at(0).toFloat();
                float y = v.at(1).toFloat();
                float z = v.at(2).toFloat();
                return QVector3D(x,y,z);
        }
};

template <>
struct read_tag_dispatch<QQuaternion> {
        static QQuaternion apply(const QString& value) {
                QStringList v = value.split(" ");
                if (v.length() != 4) {
                        throw std::invalid_argument("Failed reading QQuaternion");
                }
                float x = v.at(0).toFloat();
                float y = v.at(1).toFloat();
                float z = v.at(2).toFloat();
                float w = v.at(2).toFloat();
                return QQuaternion(x,y,z, w);
        }
};

template <typename T>
pair<bool, T> read_tag(const QDomElement& parent, const QString& tag)
{
        auto elm = parent.firstChildElement(tag);
        if (elm.isNull())
                return make_pair(false, T());

        T value = read_tag_dispatch<T>::apply(elm.text());
        return make_pair(true, value);
}

static pair<bool, Camera> read_camera(const QDomElement& parent)
{
        Camera camera;

        auto elm = parent.firstChildElement("camera");
        if (elm.isNull())
                return make_pair(false, Camera());

        auto loc = read_tag<QVector3D>(elm, "location");
        if (loc.first)
                camera.set_position(loc.second);

        auto zoom = read_tag<float>(elm, "zoom");
        if (zoom.first)
                camera.set_zoom(zoom.second);

        auto rot = read_tag<QQuaternion>(elm, "rotation");
        if (rot.first)
                camera.set_rotation(rot.second);

        auto dist = read_tag<float>(elm, "distance");
        if (dist.first)
                camera.set_distance(dist.second);

        return make_pair(false, camera);
}

static PLandmark read_landmark(const QDomElement& elm)
{
        auto name = read_tag<QString>(elm, "name");
        if (!name.first)
                return PLandmark();

        PLandmark result = make_shared<Landmark>(name.second);
        auto picfile = read_tag<QString>(elm, "picfile");
        if (picfile.first)
                result->set_template_image_file(picfile.second);

        auto isovalue = read_tag<float>(elm, "isovalue");
        if (isovalue.first)
                result->set_iso_value(isovalue.second);

        auto location = read_tag<QVector3D>(elm, "location");
        if (location.first)
                result->set_location(location.second);

        auto camera = read_camera(elm);
        if (camera.first)
                result->set_camera(camera.second);

        qDebug() << "Read landmark:" << name.second << " at " << location.second;
        return result;
}

PLandmarkList LandmarklistIO::read(const QString& filename)
{
        QDomDocument reader;

        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
                qWarning() << "Unable to open file:"<< filename;
                return nullptr;
        }

        if (!reader.setContent(&file)) {
                qWarning() << "Unable to read file as XML:"<< filename;
                file.close();
                return nullptr;
        }

        file.close();

        auto list_elm = reader.documentElement();
        if (list_elm.tagName() != "list") {
                qWarning() << filename << " not a landmark list, got tag <" << list_elm.tagName() << "> expected <list>";
                return nullptr;
        }
        // read name
        auto name_child = list_elm.firstChildElement("name");
        QString list_name("(unknown)");
        if (!name_child.isNull()) {
                list_name = name_child.text();
        }else{
                qWarning() << filename << ":List has no name entry, assign '(unknown)'";
        }

        PLandmarkList result = make_shared<LandmarkList>(list_name);

        auto landmark_elm = list_elm.firstChildElement("landmark");

        while (!landmark_elm.isNull()) {
                auto lm = read_landmark(landmark_elm);
                if (lm)
                        result->add(lm);

                landmark_elm = landmark_elm.nextSiblingElement("landmark");
        }
        return result;
}

bool LandmarklistIO::write(const QString& filename, const LandmarkList& list)
{
        return false;
}
