#ifndef LANDMARKLISTIO_HH
#define LANDMARKLISTIO_HH

#include "landmarklist.hh"

class LandmarkList;

class LandmarklistIO
{
public:
        LandmarklistIO();

        PLandmarkList read(const QString& filename);

        bool write(const QString& filename, const LandmarkList& list);
};

#endif // LANDMARKLISTIO_HH
