/* -*- mia-c++  -*-
 *
 * This file is part of qtlmpick- a tool for landmark picking and
 * visualization in volume data
 * Copyright (c) Genoa 2017,  Gert Wollny
 *
 * qtlmpick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "landmarklistio.hh"
#include "landmarklist.hh"
#include "landmark.hh"
#include "errormacro.hh"

#include <QDomDocument>
#include <QFile>

using std::make_pair;
using std::make_shared;
using std::pair;


class LandmarkReader {
public:
        LandmarkReader(const QString& filename);
        PLandmarkList read(const QDomElement& root);
private:
        PLandmark read_landmark(const QDomElement& root);
        virtual pair<bool, Camera> read_camera(const QDomElement& elm) = 0;
        QString m_filename;

};

class LandmarkReaderV1 : public LandmarkReader {
public:
        using LandmarkReader::LandmarkReader;
private:
        pair<bool, Camera> read_camera(const QDomElement& elm) override;
};

class LandmarkReaderV2 : public LandmarkReader {
public:
        using LandmarkReader::LandmarkReader;
private:
        pair<bool, Camera> read_camera(const QDomElement& elm) override;
};


PLandmarkList read_landmarklist(const QString& filename)
{
        QDomDocument reader;

        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
                throw create_exception<std::runtime_error>("Unable to open file:", filename);
        }

        if (!reader.setContent(&file)) {
                file.close();
                throw create_exception<std::runtime_error>("Unable to read file as XML:", filename);
        }
        file.close();

        auto list_elm = reader.documentElement();
        if (list_elm.tagName() != "list") {
                throw create_exception<std::runtime_error>(filename, " not a landmark list, got tag <",
                                                           list_elm.tagName(),"> but expected <list>");
        }
        // try to read version attribute
        int version = list_elm.attribute("version", "1").toInt();

        std::unique_ptr<LandmarkReader> reader_backend;

        if (version < 2)
                reader_backend.reset(new LandmarkReaderV1(filename));
        else
                reader_backend.reset(new LandmarkReaderV2(filename));

        auto result = reader_backend->read(list_elm);

        result->set_filename(filename);
        return result;

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
struct read_tag_dispatch<int> {
        static int apply(const QString& value) {
                return value.toInt();
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
                float w = v.at(3).toFloat();
                return QQuaternion(w, x, y, z);
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

LandmarkReader::LandmarkReader(const QString& filename):
        m_filename(filename)
{
}

pair<bool, Camera> LandmarkReaderV1::read_camera(const QDomElement& parent)
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

        // Other than QT the old code uses
        // the inverse rotation, correct this here and
        // when savong to version 1

        auto rot = read_tag<QQuaternion>(elm, "rotation");
        if (rot.first) {
                rot.second = rot.second.inverted();
                camera.set_rotation(rot.second);
        }

        // the camera position is interpreted
        // differently with the old code, correct here
        // when loading
        auto dist = read_tag<float>(elm, "distance");
        if (dist.first)
                camera.set_distance(-dist.second);

        return make_pair(true, camera);
}

pair<bool, Camera> LandmarkReaderV2::read_camera(const QDomElement& parent)
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

        return make_pair(true, camera);
}


PLandmarkList LandmarkReader::read(const QDomElement& root)
{
        auto name_child = root.firstChildElement("name");
        QString list_name("(unknown)");
        if (!name_child.isNull()) {
                list_name = name_child.text();
        }else{
                qWarning() << m_filename << ":List has no name entry, assign '(unknown)'";
        }

        PLandmarkList result = make_shared<LandmarkList>(list_name);
        result->set_filename(m_filename);

        auto landmark_elm = root.firstChildElement("landmark");

        while (!landmark_elm.isNull()) {
                auto lm = read_landmark(landmark_elm);
                if (lm)
                        result->add(lm);
                else
                        qWarning() << m_filename << ": Skipped empty landmark tag";
                landmark_elm = landmark_elm.nextSiblingElement("landmark");
        }
        return result;
}

PLandmark LandmarkReader::read_landmark(const QDomElement& elm)
{
        auto name = read_tag<QString>(elm, "name");
        if (!name.first)
                return PLandmark();

        PLandmark result = make_shared<Landmark>(name.second);
        auto picfile = read_tag<QString>(elm, "picfile");
        if (picfile.first)
                result->set_template_image_file(picfile.second);


        auto isovalue = read_tag<float>(elm, "isovalue");
        auto location = read_tag<QVector3D>(elm, "location");
        auto camera = read_camera(elm);
        bool complete = isovalue.first && location.first && camera.first;
        if (complete) {
                result->set(location.second, isovalue.second, camera.second);
        }else{
                if (isovalue.first)
                        result->set_iso_value(isovalue.second);
                if (location.first)
                        result->set_location(location.second);
                if (camera.first)
                        result->set_camera(camera.second);
        }
        return result;
}

class LandmarkSaver {
public:
        bool save(const QString& filename, const LandmarkList& list);

public:
        void save_landmark(QDomDocument& xml, QDomElement& parent, const Landmark& lm);
        virtual void write_camera(QDomDocument& xml, QDomElement& parent, const Camera& c) = 0;
};

bool write_landmarklist(const QString& filename, const LandmarkList& list, int prefer_version)
{
        std::unique_ptr<LandmarkSaver> saver_backend;

        if (prefer_version == 1) {
                saver_backend.reset(new LandmarkSaverV1)
        }else{
                assert(0 && "Only landmark saver V1 implmented");
        }

        return saver_backend->save(filename, list);
}


bool LandmarkSaver::save(const QString& filename, const LandmarkList& list)
{
        QFile save_file(filename);
        if (!save_file.open(QFile::WriteOnly| QFile::Text)) {
                throw create_exception<std::runtime_error>(tr("Unable to open '"), filename,
                                                           tr("' for writing."));
        }

        QDomDocument xml;
        xml.createProcessingInstruction( "xml", "version=\"1.0\"" );
        auto root = xml.createElement("list");
        xml.appendChild(list);

        auto  name = xml.createElement("name");
        root.appendChild(name);
        auto name_text = xml.createTextNode(list.get_name());
        name.appendChild(name_text);

        for (auto i: list) {
                save_landmark(xml, root, *i);
        }

}

template <typename T>
struct to_string {
        static QString apply(T value) {
                static_assert(sizeof(T) == 0, "Needs specialization");
        }
};

template <>
struct to_string<float> {
        static QString apply(float value) {
                QString s;
                QTextStream ts(s);
                ts << s;
                return s;
        }
};

template <>
struct to_string<int> {
        static QString apply(int value) {
                QString s;
                QTextStream ts(s);
                ts << s;
                return s;
        }
};

template <>
struct to_string<QVector3D> {
        static QString apply(const QVector3D& v) {
                QString s;
                QTextStream ts(s);
                ts << v.x() << " " << v.y() << " " << v.z();
                return s;
        }
};


template <>
struct to_string<QQuaternion> {
        static QString apply(const QQuaternion& v) {
                QString s;
                QTextStream ts(s);
                ts << v.x() << " " << v.y() << " " << v.z() << " " << v.scalar();
                return s;
        }
};


void LandmarkSaver::save_landmark(QDomDocument& xml, QDomElement& parent, const Landmark& lm)
{
        auto lm_flags = lm.get_flags();
        auto xml_lm = xml.createElement("landmark");
        auto lm_loc_tag = xml.createElement("location");
        auto lm_name_tag = xml.createElement("name");
        auto lm_pic_tag = xml.createElement("picfile");
        auto lm_iso_tag = xml.createElement("isovalue");





}


