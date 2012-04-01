/*
 *  Written By: Grant Hutchinson
 *  License: GPLv3.
 *  h.g.utchinson@gmail.com
 *  Copyright (C) 2012 by Grant Hutchinson
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef MYSQLCONN_H_
#define MYSQLCONN_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>
#include <string.h>
#include "songObj.h"
#include <QtSql>
#include "fileobj.h"
using namespace std;

#define MAXART 1000
#define MAXALB 1000
#define MAXSONG 2000
#define MAXVIDEO 1000
#define MAXQRY 1000
class mysqlconn {

public:
        QSqlDatabase db;
	mysqlconn(const char *server, const char* user, const char *pass, const char *database);

        fileObj& connectArtist(int artMenu, fileObj& Artist);
        fileObj& connectAlbum(fileObj& Artist, fileObj& Album);
        fileObj& connectSong(fileObj& Album, fileObj& Song);
        fileObj& connectVidDir(int vidMenu, fileObj& VidDir);
        fileObj& connectVideo(fileObj& VidDir, fileObj& Video);


        /*
	songObj* connectArtist(songObj* Artist, int artMenu, int *mySize);
	songObj* connectSong(songObj* Album, int *albSize, songObj* Song, int *songSize);
	songObj* connectAlbum(songObj* Artist, int *artSize, songObj* Album, int *albSize);
        songObj* connectVidDir(songObj* VidDir, int vidDirMenu, int *vidDirSize);
        songObj* connectVideo(songObj* VidDir, int *vidDirSize, songObj* Video, int *vidSize);
        */
        int connectVidMenu();
        int connectArtMenu();
        void display(ostream& os, int pos);
         virtual ~mysqlconn();
};

#endif /* MYSQLCONN_H_ */