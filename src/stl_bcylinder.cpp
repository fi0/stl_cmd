/*

Copyright 2018 Allwine Designs, LLC (stl_cmd@allwinedesigns.com)

    This file is part of stl_cmd.

    stl_cmd is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <math.h>
#include "stl_util.h"

#define BUFFER_SIZE 4096

void print_usage() {
    fprintf(stderr, "stl_bcylinder prints bounding cylinder information about an STL file.\n\n");
    fprintf(stderr, "usage: stl_bcylinder <input file>\n");
    fprintf(stderr, "    Prints bounding cylinder information for the given binary STL file.\n");
}

int main(int argc, char** argv) {
    if(argc >= 2) {
        if(strcmp(argv[1], "--help") == 0) {
            print_usage();
            exit(2);
        }
    }

    char *file = argv[1];

    if(!is_valid_binary_stl(file)) {
        fprintf(stderr, "%s is not a binary stl file.\n", file);
        exit(2);
    }

    FILE *f;

    f = fopen(file, "rb");
    if(!f) {
        fprintf(stderr, "Can't read file: %s\n", file);
        exit(2);
    }

    fseek(f, 80, SEEK_SET);

    uint32_t num_tris;
    size_t readBytes = fread(&num_tris, 4, 1, f);

    vec point;
    point.w = 1;

    vec min;
    vec max;

    for(int i = 0; i < num_tris; i++) {
        fseek(f, 12, SEEK_CUR); // normal

        for(int j = 0; j < 3; j++) {
            readBytes = fread(&point, 1, 12,f);
            if(i == 0 && j == 0) {
                min.x = point.x;
                min.y = point.y;
                min.z = point.z;
                max.x = point.x;
                max.y = point.y;
                max.z = point.z;
            } else {
                if(min.x > point.x) min.x = point.x;
                if(min.y > point.y) min.y = point.y;
                if(min.z > point.z) min.z = point.z;
                if(max.x < point.x) max.x = point.x;
                if(max.y < point.y) max.y = point.y;
                if(max.z < point.z) max.z = point.z;
            }
        }
        fseek(f, 2, SEEK_CUR);
    }

    vec center;
    vec pointOnCylinder;

    center.x = .5*(max.x+min.x);
    center.y = .5*(max.y+min.y);
    center.z = .5*(max.z+min.z);

    float maxR = 0;

    fseek(f, 84, SEEK_SET);

    for(int i = 0; i < num_tris; i++) {
        fseek(f, 12, SEEK_CUR); // normal

        for(int j = 0; j < 3; j++) {
            readBytes = fread(&point, 1, 12,f);

            vec R;

            R.x = point.x-center.x;
            R.y = 0;
            R.z = point.z-center.z;

            float rMag = vec_magnitude(&R);

            if(rMag > maxR) {
              maxR = rMag;
              pointOnCylinder.x = point.x;
              pointOnCylinder.y = point.y;
              pointOnCylinder.z = point.z;
            }
        }
        fseek(f, 2, SEEK_CUR);
    }

    fclose(f);

    printf("Center: (%f, %f, %f)\n", center.x, center.y, center.z);
    printf("Point On Cylinder: (%f, %f, %f)\n", pointOnCylinder.x, pointOnCylinder.y, pointOnCylinder.z);
    printf("R: %f\n", maxR);
    printf("H: %f\n", max.y-min.y);

    return 0;
}
