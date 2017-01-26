#ifndef LANDMARKLISTIO_HH
#define LANDMARKLISTIO_HH

#include "landmarklist.hh"

class LandmarkList;

class LandmarklistIO
{
public:
        LandmarklistIO();

        PLandmarkList read(const char *filename);

        bool write(const char *filename, const LandmarkList& list);
};

#endif // LANDMARKLISTIO_HH
