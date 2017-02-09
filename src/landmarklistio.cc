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
#include "qruntimeexeption.hh"

#include <QObject>
#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <cassert>

using std::make_pair;
using std::make_shared;
using std::pair;

inline QString _(const char *text)
{
        return QCoreApplication::translate("lmio", text);
}


class LandmarkReader : protected QObject {
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
                QString msg(_("Unable to open file: %1"));
                throw QRuntimeExeption(msg.arg(filename));
        }

        if (!reader.setContent(&file)) {
                file.close();
                QString msg(_("Unable to read file as XML: %1"));
                throw QRuntimeExeption(msg.arg(filename));
         }
        file.close();

        auto list_elm = reader.documentElement();
        if (list_elm.tagName() != "list") {
                QString msg(_("%1 not a landmark list file, got tag <%2> but expected <list>"));
                throw QRuntimeExeption(msg.arg(filename).arg(list_elm.tagName()));
        }
        // try to read version attribute
        int version = list_elm.attribute("version", "1").toInt();

        std::unique_ptr<LandmarkReader> reader_backend;

        if (version < 2)
                reader_backend.reset(new LandmarkReaderV1(filename));
        else
                reader_backend.reset(new LandmarkReaderV2(filename));

        auto result = reader_backend->read(list_elm);

        result->setFilename(filename);
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
                        throw QRuntimeExeption(_("Failed to read '%1' as QVector3D").arg(value));
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
                        throw QRuntimeExeption(_("Failed to read '%1' as QQuaternion").arg(value));
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
        // when saving to version 1

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
        result->setFilename(m_filename);

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
                result->setTemplateImageFile(picfile.second);


        auto isovalue = read_tag<float>(elm, "isovalue");
        auto location = read_tag<QVector3D>(elm, "location");
        auto camera = read_camera(elm);
        bool complete = isovalue.first && location.first && camera.first;
        if (complete) {
                result->set(location.second, isovalue.second, camera.second);
        }else{
                if (isovalue.first)
                        result->setIsoValue(isovalue.second);
                if (location.first)
                        result->setLocation(location.second);
                if (camera.first)
                        result->setCamera(camera.second);
        }
        return result;
}

class LandmarkSaver {
public:
        bool save(const QString& filename, const LandmarkList& list);

public:
        void save_landmark(QDomDocument& xml, QDomElement& parent, const Landmark& lm);
        virtual void save_camera(QDomDocument& xml, QDomElement& parent, const Camera& c) = 0;
};

class LandmarkSaverV1 : public LandmarkSaver {
        void save_camera(QDomDocument& xml, QDomElement& parent, const Camera& c) override;
};

bool write_landmarklist(const QString& filename, const LandmarkList& list, int prefer_version)
{
        std::unique_ptr<LandmarkSaver> saver_backend;

        if (prefer_version == 1) {
                saver_backend.reset(new LandmarkSaverV1);
        }else{
                assert(0 && "Only landmark saver V1 implmented");
        }

        return saver_backend->save(filename, list);
}


bool LandmarkSaver::save(const QString& filename, const LandmarkList& list)
{
        QFile save_file(filename);
        if (!save_file.open(QFile::WriteOnly| QFile::Text)) {
                throw QRuntimeExeption(_("Unable to open '%1'' for writing.").arg(filename));
        }

        QDomDocument xml;
        xml.createProcessingInstruction( "xml", "version=\"1.0\"" );
        auto root = xml.createElement("list");
        xml.appendChild(root);

        auto  name = xml.createElement("name");
        root.appendChild(name);
        auto name_text = xml.createTextNode(list.getName());
        name.appendChild(name_text);

        for (auto i: list) {
                save_landmark(xml, root, *i);
        }

        QTextStream s(&save_file);
        s << xml.toString();
        save_file.close();

        return s.status() == QTextStream::Ok;
}

template <typename T>
struct to_string {
        static QString apply(T value) {
                static_assert(sizeof(T) == 0, "Needs specialization");
                return QString();
        }
};

template <>
struct to_string<float> {
        static QString apply(float value) {
                QString s;
                QTextStream ts(&s);
                ts << value;
                return s;
        }
};

template <>
struct to_string<int> {
        static QString apply(int value) {
                QString s;
                QTextStream ts(&s);
                ts << value;
                return s;
        }
};

template <>
struct to_string<QVector3D> {
        static QString apply(const QVector3D& v) {
                QString s;
                QTextStream ts(&s);
                ts << v.x() << " " << v.y() << " " << v.z();
                return s;
        }
};


template <>
struct to_string<QQuaternion> {
        static QString apply(const QQuaternion& v) {
                QString s;
                QTextStream ts(&s);
                ts << v.x() << " " << v.y() << " " << v.z() << " " << v.scalar();
                return s;
        }
};


void LandmarkSaver::save_landmark(QDomDocument& xml, QDomElement& parent, const Landmark& lm)
{
        auto xml_lm = xml.createElement("landmark");
        if (lm.has(Landmark::lm_name)) {
                auto tag = xml.createElement("name");
                auto content = xml.createTextNode(lm.getName());
                tag.appendChild(content);
                xml_lm.appendChild(tag);
        }
        if (lm.has(Landmark::lm_location)) {
                auto tag = xml.createElement("location");
                auto loc_string = to_string<QVector3D>::apply(lm.getLocation());
                auto content = xml.createTextNode(loc_string);
                tag.appendChild(content);
                xml_lm.appendChild(tag);
        }
        if (lm.has(Landmark::lm_picfile)) {
                auto tag = xml.createElement("picfile");
                auto content = xml.createTextNode(lm.getTemplateFilename());
                tag.appendChild(content);
                xml_lm.appendChild(tag);
        }
        if (lm.has(Landmark::lm_iso_value)) {
                auto tag = xml.createElement("isovalue");
                auto iso_string = to_string<float>::apply(lm.getIsoValue());
                auto content = xml.createTextNode(iso_string);
                tag.appendChild(content);
                xml_lm.appendChild(tag);
        }
        if (lm.has(Landmark::lm_camera)) {
                save_camera(xml, xml_lm, lm.getCamera());
        }
        parent.appendChild(xml_lm);
}

void LandmarkSaverV1::save_camera(QDomDocument& xml, QDomElement& parent, const Camera& c)
{
        auto xml_camera = xml.createElement("camera");

        auto loc_tag = xml.createElement("location");
        QVector3D loc(c.get_position().x(), c.get_position().y(), 0);
        auto loc_string = to_string<QVector3D>::apply(loc);
        auto loc_content = xml.createTextNode(loc_string);
        loc_tag.appendChild(loc_content);
        xml_camera.appendChild(loc_tag);

        auto rot_tag = xml.createElement("rotation");
        auto rot_string = to_string<QQuaternion>::apply(c.get_rotation().inverted());
        auto rot_content = xml.createTextNode(rot_string);
        rot_tag.appendChild(rot_content);
        xml_camera.appendChild(rot_tag);

        auto zoom_tag = xml.createElement("zoom");
        auto zoom_string = to_string<float>::apply(c.get_zoom());
        auto zoom_content = xml.createTextNode(zoom_string);
        zoom_tag.appendChild(zoom_content);
        xml_camera.appendChild(zoom_tag);

        auto dist_tag = xml.createElement("distance");
        auto dist_string = to_string<float>::apply(-c.get_position().z());
        auto dist_content = xml.createTextNode(dist_string);
        dist_tag.appendChild(dist_content);
        xml_camera.appendChild(dist_tag);

        parent.appendChild(xml_camera);
}
