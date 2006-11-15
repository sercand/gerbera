/*MT*
    
    MediaTomb - http://www.mediatomb.org/
    
    autoscan.h - this file is part of MediaTomb.
    
    Copyright (C) 2005 Gena Batyan <bgeradz@mediatomb.org>,
                       Sergey Bostandzhyan <jin@mediatomb.org>
    Copyright (C) 2006 Gena Batyan <bgeradz@mediatomb.org>,
                       Sergey Bostandzhyan <jin@mediatomb.org>,
                       Leonhard Wimmer <leo@mediatomb.org>
    
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    $ID: autoscan_directory.h 1126 2006-11-03 01:53:53Z jin $
*/

/// \file autoscan_directory.h
///\brief Definitions of the Autoscan classes. 

#ifndef __AUTOSCAN_H__
#define __AUTOSCAN_H__

#include "zmmf/zmmf.h"
#include "sync.h"
#include "timer.h"

///\brief Scan level - the way how exactly directories should be scanned.
typedef enum scan_level_t
{
    BasicScanLevel, // file was added or removed from the directory
    FullScanLevel   // file was modified/added/removed
};

///\brief Scan mode - type of scan (timed, inotify, fam, etc.)
typedef enum scan_mode_t
{
    TimedScanMode
};

class AutoscanDirectory;

class AutoscanList : public zmm::Object
{
public:
    AutoscanList();

    /// \brief Adds a new AutoscanDirectory to the list. 
    /// 
    /// The scanID of the directory is invalidated and set to 
    /// the index in the AutoscanList.
    /// 
    /// \param dir AutoscanDirectory to add to the list.
    /// \return scanID of the newly added AutoscanDirectory
    int add(zmm::Ref<AutoscanDirectory> dir);

    zmm::Ref<AutoscanDirectory> get(int id);

    void remove(int id);
    void remove(zmm::String location);

    /// \brief Add timer for each directory in the list.
    /// \param obj instance of the class that will receive notifications.
    void subscribeAll(zmm::Ref<TimerSubscriber> obj);

    /// \brief Send notification for each directory that is stored in the list.
    /// 
    /// \param obj instance of the class that will receive the notifications.
    void notifyAll(zmm::Ref<TimerSubscriber> obj);
        
    
    /// \brief Add timer for given directory.
    /// \param obj instance of the class that will receive notifications.
    /// \param id dir id.
    void subscribeDir(zmm::Ref<TimerSubscriber> obj, int id, bool once = true);

protected:
    zmm::Ref<Mutex> mutex;
    zmm::Ref<zmm::Array<AutoscanDirectory> > list;
};

/// \brief Provides information about one autoscan directory.
class AutoscanDirectory : public zmm::Object
{
public:
    /// \brief Creates a new AutoscanDirectory object.
    /// \param location autoscan path
    /// \param mode scan mode
    /// \param level scan level
    /// \param recursive process directories recursively
    /// \param interval rescan interval in seconds (only for timed scan mode),
    /// zero means none.
    AutoscanDirectory(zmm::String location, scan_mode_t mode, 
                      scan_level_t level, bool recursive,
                      bool from_config,
                      int id = -1, unsigned int interval=0);

    zmm::String getLocation() { return location; }

    scan_mode_t getScanMode() { return mode; }

//    void setScanMode(scan_mode_t mode) { this->mode = mode; }

    scan_level_t getScanLevel() { return level; }

    void setScanLevel(scan_level_t level) { this->level = level; }

    bool getRecursive() { return recursive; }

//    void setRecursive(bool recursive) { this->recursive = recursive; }
    
    unsigned int getInterval() { return interval; }

    void setInterval(unsigned int interval) { this->interval = interval; }

    /// \brief Increments the task count. 
    ///
    /// When recursive autoscan is in progress, we only want to subcribe to
    /// a timer event when the scan is finished. However, recursive scans
    /// spawn tasks for each directory. When adding a rescan task for 
    /// subdirectories, the taskCount will be incremented. When a task is
    /// done the count will be decremented. When timer gets to zero, 
    /// we will resubscribe.
    void incTaskCount() { taskCount++; }
    
    void decTaskCount() { taskCount--; }

    int getTaskCount() { return taskCount; }

    void setTaskCount(int taskCount) { this->taskCount = taskCount; }
  
    /// \brief Sets the task ID.
    ///
    /// The task ID helps us to identify to which scan a particular task
    /// belongs. Recursive scans spawn new tasks - they all should have
    /// the same id.
    void setScanID(int id) { scanID = id; }

    int getScanID() { return scanID; }

    void setObjectID(int id) { objectID = id; }

    int getObjectID() { return objectID; }

    bool fromConfig() { return from_config; }

protected:
    zmm::String location;
    scan_mode_t mode;
    scan_level_t level;
    bool recursive;
    bool from_config;
    unsigned int interval;
    int taskCount;
    int scanID;
    int objectID;
    
};

#endif
