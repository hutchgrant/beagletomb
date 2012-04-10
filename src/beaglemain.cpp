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


#include "beaglemain.h"
#include "ui_beaglemain.h"

BeagleMain::BeagleMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BeagleMain)
{
    ui->setupUi(this);
    Sync(0);
    ui->MenuList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TitleList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->PlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->list_radio->setEditTriggers(QAbstractItemView::NoEditTriggers);
    widget.setSeekSlider(ui->SEEK_slider);
    widget.setVolumeSlider(ui->VOL_dial);
    CON_MODE = 0;
}

/*
  *  SYNCHRONIZE ALL DATA
  */
void BeagleMain::Sync(int type){
    initCueID(0,100,0);
    initCueID(1,100,0);
    artSize = 0;
    albSize = 0;
    songSize = 0;
    vidDirSize = 0;
    vidSize = 0;
    radSize = 0;

    if(type == 0){
        if(pref.initDB() == false){
            prefDg.show();
            if (prefDg.exec()==QDialog::Accepted) {
                //create cache directory
                pref.createCache();
                pref = prefDg.getPref();
                //create custom sql db
                pref.createPrefDB();
                /// write preferences to sql db
                pref.writeDB();
                pref.setInitDB();
            }
        }
        else{
            fillRemoteFiles();
            fillLocalFiles(0);
        }
        /// init playlist
        plMode= 0; // set playlist mode to browse
        pl.setLocation(pref.getSQL());

    }
    else if(type == 1){
        cout << "syncing.... " << pref.getServ() << "\t" << pref.getPort()<<  endl;
        /// read from remote mysql write to local sqlite
        syncMe sy(pref.getServ().c_str(), pref.getUser().c_str(), pref.getPass().c_str(), pref.getTable().c_str(), pref.getSQL().c_str());
        fillRemoteFiles();
         cout << "Database synced!" << endl;
    }
}

/*
  * Fill All objects from sqlite
  */
void BeagleMain::fillRemoteFiles(){
    /// set the default db location from preference cache
    rDB.setDB(pref.getSQL().c_str());
    //// read from sql and fill songObjs
    Artist = rDB.RemoteFill(Artist, 1);
    Album = rDB.RemoteFill(Album, 2);
    Song = rDB.RemoteFill(Song, 3);
    VidDir = rDB.RemoteFill(VidDir, 4);
    Video = rDB.RemoteFill(Video, 5);
}

/*
  * Fill All local files from sqlite
  * Mode: 0 ALL 1 song Dirs, 2 songs, 3 vid dirs, 4, videos
  */
void BeagleMain::fillLocalFiles(int mode){
    if(mode == 0){
        DirecLocal = rDB.LocalFill(DirecLocal, 1);
        SongLocal = rDB.LocalFill(SongLocal, 2);
        vidDirecLocal = rDB.LocalFill(vidDirecLocal, 3);
        VideoLocal = rDB.LocalFill(VideoLocal, 4);
    }
    else if(mode == 1){
         DirecLocal = rDB.LocalFill(DirecLocal, 1);
    }
    else if(mode == 2){
        SongLocal = rDB.LocalFill(SongLocal, 2);
    }
    else if(mode == 3){
        vidDirecLocal = rDB.LocalFill(vidDirecLocal, 3);
    }
    else if(mode == 4){
        VideoLocal = rDB.LocalFill(VideoLocal, 4);
    }
}


/*
  *When Mode iu button is changed, change the mode, update lists
  */
void BeagleMain::on_MODE_combo_currentIndexChanged(int index)
{
    if(index == 1){   //  update lists with artists
        updateMenu(1);
        MenuMode=1;
    }
    else if(index == 2){  // update lists with albums
        updateMenu(2);
        MenuMode=3;
    }
    else if(index == 3){  // update lists with songs
        updateTitle();
        TitleMode = 1;
    }
    else if(index == 4){  // update lists with vid directories
        MenuMode = 4;
        updateMenu(3);
    }
}

/*
  *When a track is double clicked from the TitleList, depending on which mode we are in
  */
void BeagleMain::on_TitleList_doubleClicked(QModelIndex index)
{
    int selID = 0;
    int selected = 0;
    selected = ui->TitleList->currentIndex().row();
    int FinParentID = 0;
    int finSongSize = 0;
    int finPathSize = 0;

    char *finPath;
    char *finSong;
    if(CON_MODE == 1) {


        if(MenuMode == 2 || MenuMode == 3){
            selID = curSongID[selected];
            finSongSize = strlen(checkSongObjByID(selID, Song));
            finSong = new char[finSongSize + 1];

            FinParentID = checkSongObjParByID(selID,Song);
            finSong = checkSongObjByID(selID, Song);
        }
        else if(MenuMode == 4){
            selID = curVidID[selected];
            finSongSize = strlen(checkSongObjByID(selID, Video));
            finSong = new char[finSongSize + 1];
            FinParentID = checkSongObjParByID(selID,Video);
            finSong = checkSongObjByID(selID, Video);
        }
        else{
            selID = curSongID[selected];
            finSongSize = strlen(checkSongObjByID(selID, Song));
            finSong = new char[finSongSize + 1];
            FinParentID = checkSongObjParByID(selID,Song);
            finSong = checkSongObjByID(selID, Song);
        }
        // start song
        startSong(finSong, selID);
    }
    else{

        if(MenuMode != 4){

            selID = curSongID[selected];

            finSongSize = strlen(checkSongObjByID(selID, SongLocal));
            finPathSize = strlen(checkSongObjPathByID(selID, SongLocal));
            finSong = new char[finSongSize + 1];
            finPath = new char[finPathSize + 10];

            FinParentID = checkSongObjParByID(selID,SongLocal);
            finSong = checkSongObjByID(selID, SongLocal);
            finPath = checkSongObjPathByID(selID,SongLocal);

        }
        else{
            selID = curVidID[selected];

            finSongSize = strlen(checkSongObjByID(selID, VideoLocal));
            finPathSize = strlen(checkSongObjPathByID(selID, VideoLocal));
            finSong = new char[finSongSize + 1];
            finPath = new char[finPathSize + 1];

            finSong = checkSongObjByID(selID, VideoLocal);
            FinParentID = checkSongObjParByID(selID,VideoLocal);
            finPath = checkSongObjPathByID(selID,VideoLocal);
        }
        startLocal(finSong, finPath);
    }
}

/*
  *When the Main list is selected, depending on which mode we are in
  */
void BeagleMain::on_MenuList_clicked(QModelIndex index)
{
    int selected = ui->MenuList->currentIndex().row();

    if(MenuMode == 1){
        if(CON_MODE == 1){  // REMOTE MODE
            MenuMode =2;
            updateTitle(selected);

        }
        else{
            updateTitle(selected);
        }
    }
    else if(MenuMode == 4){
        if(CON_MODE == 1){ //  REMOTE MODE
            updateTitle(selected);
        }
        else{
            updateTitle(selected);
        }
    }
    else{
        updateTitle(selected);
    }
}

/*
  * when the playlist list is doubleclick get which item we've selected and play it
  */
void BeagleMain::on_PlayList_doubleClicked(QModelIndex index)
{
    int selected;
    selected = ui->PlayList->currentIndex().row();
    int selID = 0;
    char *FinSong;

    if(plMode == 1){   // folder mode
        pl.setMode(1);
        RefillMainPL(0);
        plMode = 2;
    }
    else{   // item mode
        pl_selected = 0;
        pl_selected = ui->PlayList->currentIndex().row();
        selID = playlist.getID(pl_selected);
        PlaylistPlay(selID);
    }
}
void BeagleMain::on_PlayList_clicked(QModelIndex index)
{
    pl_selected = ui->PlayList->currentIndex().row();
}

/*
  *  When the radio list is double clicked
  */
void BeagleMain::on_list_radio_doubleClicked(QModelIndex index)
{
    int pos =0;
    pos = ui->list_radio->currentIndex().row();
    string finalUrl, finalName;
    finalUrl = Radio.getUrl(pos);
    finalName = Radio.getName(pos);
    ui->SONG_lbl->setText((QString)finalName.c_str());
    widget.show();
    widget.start(QStringList(finalUrl.c_str()));
}


/*
  *  UPDATE MENU LIST WITH ALL ARTISTS OR ALL ALBUMS
  */
void BeagleMain::updateMenu(int type){
    QStringList curMenu;
    m_Model = new QStringListModel(this);

    if(CON_MODE == 1){ /// REMOTE MODE
        if(type == 1){
            /// update with artist
            for(int i=0; i< Artist.getSize(); i++){
                curMenu << Artist.getName(i);
            }
        }
        else if(type == 2){
            /// update with album
            for(int i=0; i< Album.getSize(); i++){
                curMenu << Album.getName(i);
            }
        }
        else if(type == 3){
            /// update with directories
            for(int i=0; i< VidDir.getSize(); i++){
                curMenu << VidDir.getName(i);
            }
        }
    }else{
        if(type == 1){
            /// update with Artist Directories
            for(int i=0; i< DirecLocal.getSize(); i++){
                curMenu << DirecLocal.getName(i);
            }
        }
        else if(type == 3){
            /// update with video Directories
            for(int i=0; i< vidDirecLocal.getSize(); i++){
                curMenu << vidDirecLocal.getName(i);
            }
        }
    }

    m_Model->setStringList(curMenu);
    ui->MenuList->setModel(m_Model);
}

void BeagleMain::setMainPref(preferences my_pref){
    pref = my_pref;
}


/*
  *  UPDATE TITLE LIST WITH ALL SONGS
  */
void BeagleMain::updateTitle(){
    t_Model = new QStringListModel(this);
    QStringList songList;

    for(int i = 0; i<= Song.getSize(); i++){
        songList << Song.getName(i);
    }
    t_Model->setStringList(songList);
    ui->TitleList->setModel(t_Model);

}

/*
  *  UPDATE TITLE LIST WITH SONGS FROM SELECTED
  */
void BeagleMain::updateTitle(int selected){
    songCount = 0;
    vidCount = 0;
    int selID = 0;
    QStringList curSong;
    t_Model = new QStringListModel(this);
    if(CON_MODE == 1){  /// IN REMOTE MODE
        if(MenuMode == 3){     ///  ALL ALBUM
            selID = Album.getID(selected);
            initCueID(0, Song.getSize(),1);
            for(int i = 0; i< Song.getSize(); i++){
                if(Song.getPar(i) == selID){
                    curSong << Song.getName(i);
                    curSongID[songCount] = Song.getID(i);
                    songCount++;
                }
            }
        }
        else if(MenuMode == 2){ // specific album mode
            albCount = 0;
            int selID = 0;
            QStringList curAlb;
            m_Model = new QStringListModel(this);
            curAlbID = new int[MAX];
            selID = Artist.getID(selected);    /// selected ID

            for(int i=0; i< Album.getSize(); i++){
                if(Album.getPar(i) == selID ){
                    curAlb << Album.getName(i);
                    curAlbID[albCount] = Album.getID(i);
                    albCount++;
                }
            }

            m_Model->setStringList(curAlb);
            ui->MenuList->setModel(m_Model);
            MenuMode = 5;
        }
        else if(MenuMode == 4){    ///  VIDEO DIR MODE
            selID = VidDir.getID(selected);
            initCueID(1, Video.getSize(),1);
            for(int i = 0; i<= Video.getSize(); i++){
                if(Video.getPar(i) == selID){
                    curSong << Video.getName(i);
                    curVidID[vidCount] = Video.getID(i);
                    vidCount++;
                }
            }
        }
        else{   /// ALBUM DIR MODE
            selID = curAlbID[selected];
            initCueID(0, Song.getSize(),1);
            for(int i = 0; i<= Song.getSize(); i++){
                if(Song.getPar(i) == selID){
                    curSong << Song.getName(i);
                    curSongID[songCount] = Song.getID(i);
                    songCount++;
                }
            }
        }
    }else{    /// IN LOCAL MODE
        if(MenuMode == 1){
            selID = DirecLocal.getID(selected);
            initCueID(0, SongLocal.getSize(),1);
            for(int i = 0; i<SongLocal.getSize(); i++){
                if(SongLocal.getPar(i) == selID){
                    curSong << SongLocal.getName(i);
                    curSongID[songCount] = SongLocal.getID(i);
                    songCount++;
                }
            }
        }
        if(MenuMode == 4){
            selID = vidDirecLocal.getID(selected);
            initCueID(1, VideoLocal.getSize(),1);
            for(int i = 0; i<VideoLocal.getSize(); i++){
                if(VideoLocal.getPar(i) == selID){
                    curSong << VideoLocal.getName(i);
                    curVidID[vidCount] = VideoLocal.getID(i);
                    vidCount++;
                }
            }
        }
    }

    t_Model->setStringList(curSong);
    ui->TitleList->setModel(t_Model);

}

/*
  * refill playlist list with playlist model from playlist object
  *  type: 0 fill with playlists, 1 fill with playlist items
    */
void BeagleMain::RefillMainPL(int type){
    QStringList updatedList;
    playlist = rDB.PlaylistFill(playlist, type);
    t_Model = new QStringListModel(this);
    updatedList = pl.fillPlaylist(playlist);
    t_Model->setStringList(updatedList);
    ui->PlayList->setModel(t_Model);
}

/*
  * refill playlist list with playlist model from playlist object
  */
void BeagleMain::RefillRadioPL(){
    QStringList updatedList;
    r_Model = new QStringListModel(this);
    updatedList = Radio.RefillPlaylist();
    r_Model->setStringList(updatedList);
    ui->list_radio->setModel(r_Model);
}

/*
  * When vol dial has changed
  */
void BeagleMain::on_VOL_dial_valueChanged(int value)
{

}
/*
  *When Sync Button is pressed
  */
void BeagleMain::on_SYNC_but_clicked()
{
    Sync(1);
}

/*
  *When Stop Button is pressed
  */
void BeagleMain::on_STOP_but_clicked()
{
    widget.stop();
}

void BeagleMain::on_FFWD_but_clicked()
{
    int selID = 0;
    pl_selected++;
    selID = playlist.getID(pl_selected);
    PlaylistPlay(selID);
}

void BeagleMain::on_RRWD_but_clicked()
{
    int selID = 0;
    pl_selected--;
    selID = playlist.getID(pl_selected);
    PlaylistPlay(selID);
}

/*
  * when seek slider has moved
  */
void BeagleMain::on_SEEK_slider_sliderMoved(int position)
{
    //   widget.seek((double)position, 0);
}

/*
  * When play button is pressed and held, seek
  */
void BeagleMain::on_FWD_but_pressed()
{
    int curTime = ui->SEEK_slider->value();
    int FutTime = curTime + 10;
    widget.seek((double)FutTime, curTime);
}

/*
  * When Play button clicked, play
  */
void BeagleMain::on_FWD_but_clicked()
{
    widget.play();
}
/*
  * When Pause button clicked, pause
  */
void BeagleMain::on_PAUSE_but_clicked()
{
    widget.pause();
}

/*
  * When preferences menu item selected, open preference window
  */
void BeagleMain::on_actionPreferences_2_activated()
{
    pref.readDB();
    prefDg.setPref(pref);
    prefDg.show();
    if (prefDg.exec()==QDialog::Accepted) {
        pref = prefDg.getPref();
        //delete custom sql db
        pref.deletePrefDB();
        //create custom sql db
        pref.createPrefDB();
        /// write preferences to sql db
        pref.writeDB();
    }
}

/*
  * When About menu item selected, open about dialog
  */
void BeagleMain::on_actionAbout_activated()
{
    ab.show();
    if(ab.exec()==QDialog::Accepted) {
        ab.close();
    }
}

/*
  * When Sync Button is selected , sync
  */
void BeagleMain::on_actionSync_activated()
{
    Sync(1);
}


/*
  * When Add button is selected, get selected item (depending on mode),
  * add to list, refill list
  */
void BeagleMain::on_ADD_but_clicked()
{
    int selID = 0, iPar = 0;
    int selected = 0;
    char * strBuffer, *strPathBuffer;
    strBuffer= new char[100];
    selected = ui->TitleList->currentIndex().row();

    if(MenuMode == 1 || MenuMode == 2 || MenuMode == 3){
        selID = curSongID[selected];
        if(CON_MODE == 1){
            strBuffer=checkSongObjByID(selID, Song);
            iPar = checkSongObjParByID(selID, Song);

        }
        else{
            strBuffer=checkSongObjByID(selID, SongLocal);
            iPar = checkSongObjParByID(selID, SongLocal);

        }
    }
    else if(MenuMode == 3 || MenuMode == 4){
        selID = curVidID[selected];

        if(CON_MODE == 1){
            strBuffer=checkSongObjByID(selID, Video);
            strPathBuffer=checkSongObjPathByID(selID,Video);

            iPar = checkSongObjParByID(selID,Video);

        }
        else{
            strBuffer=checkSongObjByID(selID, VideoLocal);
            strPathBuffer=checkSongObjPathByID(selID,VideoLocal);
            iPar = checkSongObjParByID(selID,VideoLocal);
        }
    }
    else{
        selID = Song.getID(selected);
        strBuffer=checkSongObjByID(selID, Song);
        strPathBuffer=checkSongObjPathByID(selID,Song);
        iPar = checkSongObjParByID(selID, Song);

    }
    pl.AddTo(selID, iPar, strBuffer, strPathBuffer, playlist);
    pl.setMode(1);
    RefillMainPL(1);
}

/*
  * When Remove button is clicked
  */
void BeagleMain::on_REMOVE_but_clicked()
{
    int selected = 0;
    pl.RemoveFrom(pl_selected);
    RefillMainPL(1);
}

void BeagleMain::on_SAVE_but_clicked()
{
    pl.writeToDB();
}

void BeagleMain::on_OPEN_but_clicked()
{
    plMode = 0;
    pl.setMode(0);
    /// list playlists
    RefillMainPL(0);
}


void BeagleMain::on_UP_but_clicked()
{
    pl.Move(2, pl_selected);
    pl_selected--;
    RefillMainPL(1);
}

void BeagleMain::on_DOWN_but_clicked()
{
    pl.Move(1, pl_selected);
    pl_selected++;
    RefillMainPL(1);
}

void BeagleMain::closeEvent(QCloseEvent *event)
{
    widget.close();
}

void BeagleMain::on_actionDonate_2_triggered()
{
    QDesktopServices::openUrl(QUrl("https://flattr.com/profile/hutchgrant", QUrl::TolerantMode));
}

void BeagleMain::on_ADMIN_but_clicked()
{
    char mediatombAdd[100];
    sprintf(mediatombAdd, "http://%s:%s", pref.getServ().c_str(), pref.getPort().c_str());
    QDesktopServices::openUrl(QUrl(mediatombAdd, QUrl::TolerantMode));
}

void BeagleMain::on_but_RadAdd_clicked()
{
    QString Radio_url;
    QString Radio_name;
    Radio_url = ui->entry_radURL->text();
    Radio_name = ui->entry_radName->text();

    Radio.Add(Radio_name.toStdString(),Radio_url.toStdString());
    radSize++;
    Radio.writeDB();
    RefillRadioPL();
}

void BeagleMain::on_but_RemRad_clicked()
{
    int pos = 0;
    string pl_ItemName;
    pos = ui->list_radio->currentIndex().row();
    pl_ItemName = Radio.getName(pos);
    Radio.Remove(pl_ItemName, pos);
  //  Radio = rDB.RadioFill(&radSize);
    RefillRadioPL();
}

void BeagleMain::on_list_radio_clicked(QModelIndex index)
{

}

/*
  * Import Audio folder button
  */
void BeagleMain::on_but_import_aud_clicked()
{
    QDir usrDir = QString(getenv("HOME"));
    usrDir = QFileDialog::getExistingDirectory(this, tr("Import a directory"), QDir::currentPath());  // get folder import directory
    SyncAudioLocal.Sync(usrDir, 0);
    fillLocalFiles(1);
    fillLocalFiles(2);
    CON_MODE = 0;
    ui->but_remote_tog->setChecked(false);
    ui->MODE_combo->setCurrentIndex(1);
    updateMenu(1);
}

/*
  * Import Video folder button
  */
void BeagleMain::on_but_import_vid_clicked()
{
    QDir usrDir = QString(getenv("HOME"));
    usrDir = QFileDialog::getExistingDirectory(this, tr("Import a directory"), QDir::currentPath());  // get folder import directory
    SyncVideoLocal.Sync(usrDir, 1);
    fillLocalFiles(3);
    fillLocalFiles(4);
    CON_MODE = 0;
    ui->but_remote_tog->setChecked(false);
    ui->MODE_combo->setCurrentIndex(4);
    updateMenu(3);
}

/*
  *  Toggle button for remote / local
  */
void BeagleMain::on_but_remote_tog_clicked()
{
    if(CON_MODE == 1){
        CON_MODE = 0;  // set connection mode local
        cout << "viewing local library" << endl;
    }
    else{
        CON_MODE = 1; // set connection mode remote
        cout << "viewing remote library" << endl;
    }
}

/*
  *  Control for Start of remote File
  */
void BeagleMain::startSong(char *FinSong, int selID){

    char * strBuffer;
    strBuffer= new char[100];
    sprintf(strBuffer, "http://%s:%s/content/media/object_id/%d/res_id/0", pref.getServ().c_str(), pref.getPort().c_str(), selID);
    ui->SONG_lbl->setText((QString)FinSong);
    widget.show();
    widget.start(QStringList(strBuffer));
}
/*
  *  Control for Start of local File
  */
void BeagleMain::startLocal(char *finSong, char *finPath){

    char *final;
    final = new char[strlen(finPath) + 10];
    sprintf(final, "%s", finPath);
    cout << "Final File Playing: " << final << endl;
    ui->SONG_lbl->setText((QString)finSong);
    widget.show();
    widget.start(QStringList(final));
}

/*
  *  Control for Playlist File
  */
void BeagleMain::PlaylistPlay(int selID){
    char* FinSong, *FinPath;
    for(int i = 0; i<= playlist.getSize(); i++){
        if(playlist.getID(i) == selID){
            FinSong = new char[strlen(playlist.getName(i))+1];
            FinPath = new char[strlen(playlist.getPath(i))+1];
            strcpy(FinSong,playlist.getName(i));
            cout << FinSong << endl;
            strcpy(FinPath,playlist.getPath(i));
            cout << FinPath << endl;
        }
    }
    if(CON_MODE == 1){
        // start song
        startSong(FinSong, selID);
    }
    else{
        startLocal(FinSong, FinPath);
    }
}

/*
  *INIT CUE ID LIST
  */
void BeagleMain::initCueID(int type, int newsize, int inital)
{
    if(inital != 0){    // we're reallocating
        if(type == 0){
            delete [] curSongID;
            curSongID = new int[newsize+1];
            for(int i = 0; i<newsize; i++){
                curSongID[i] = 0;
            }
        }
        else if(type == 1){
            delete [] curVidID;
            curVidID = new int[newsize+1];
            for(int i = 0; i<newsize; i++){
                curVidID[i] = 0;
            }
        }
    }
    else{
        curSongID = new int[newsize+1];
        curVidID = new int[newsize+1];
        for(int i = 0; i<newsize; i++){
            curSongID[i] = 0;
            curVidID[i] = 0;
        }
    }
}

/*
  * MEM CLEANUP
  */
BeagleMain::~BeagleMain()
{
    delete ui;
}
