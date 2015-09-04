/*
 * Copyright (C) 2006 Sander Canisius
 *
 * This file is part of python-timbl.
 * 
 * python-timbl is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * python-timbl is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with python-timbl; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Linking python-timbl statically or dynamically with other modules
 * is making a combined work based on python-timbl. Thus, the terms
 * and conditions of the GNU General Public License cover the whole
 * combination.
 * 
 * In addition, as a special exception, the copyright holder of
 * python-timbl gives you permission to combine python-timbl with free
 * software programs or libraries that are released under the GNU LGPL
 * and with code included in the standard release of TiMBL under the
 * TiMBL license (or modified versions of such code, with unchanged
 * license). You may copy and distribute such a system following the
 * terms of the GNU GPL for python-timbl and the licenses of the other
 * code concerned, provided that you include the source code of that
 * other code when and as the GNU GPL requires distribution of source
 * code.
 * 
 * Note that people who make modified versions of python-timbl are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so. The GNU General
 * Public License gives permission to release a modified version
 * without this exception; this exception also makes it possible to
 * release a modified version which carries forward this exception.
 *
 */

#ifndef TIMBL_H
#define TIMBL_H

#include "Python.h"

#include <timbl/TimblAPI.h>

#include <boost/python.hpp>
#include <string>
#include <map>
#include <iostream>
#include <pthread.h>

namespace python = boost::python;


class TimblApiWrapper : public Timbl::TimblAPI {
private:
    std::map<pthread_t,Timbl::TimblExperiment *> experimentpool;
    Timbl::TimblExperiment * detachedexp;
    python::dict dist2dict(const Timbl::ValueDistribution * dist,  bool=true,double=0) const;
public:
	TimblApiWrapper(const std::string& args, const std::string& name="") : Timbl::TimblAPI(args, name) { detachedexp = NULL; }
    ~TimblApiWrapper() { 
        PyThreadState * m_thread_state = PyEval_SaveThread(); //release GIL
        pthread_t thisthread = pthread_self();
        //std::cerr << "(Destroying TimblApiWrapper for thread " << (size_t) thisthread << ")" << std::endl;
        pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //global lock
        pthread_mutex_lock(&lock);
        std::map<pthread_t,Timbl::TimblExperiment *>::iterator iter = experimentpool.find(thisthread);
        if (experimentpool.find(thisthread) != experimentpool.end()) {
            delete iter->second;
            experimentpool.erase(iter);
            //std::cerr << "(Freed TimblExperiment for thread " << (size_t) thisthread << ")" << std::endl;
        }
        pthread_mutex_unlock(&lock);
        PyEval_RestoreThread(m_thread_state); //reacquire GIL
    }

    void initthreading();
    void finishthreading();

	python::tuple classify(const std::string& line);
	python::tuple classify2(const std::string& line);
	python::tuple classify3(const std::string& line, bool normalize=true,const unsigned char requireddepth=0);
	python::tuple classify3safe(const std::string& line, bool normalize=true,const unsigned char requireddepth=0);

	std::string bestNeighbours();
	bool showBestNeighbours(python::object& stream);

	std::string options();
	bool showOptions(python::object& stream);

	std::string settings();
	bool showSettings(python::object& stream);

	std::string weights();
	bool showWeights(python::object& stream);

};

#endif
