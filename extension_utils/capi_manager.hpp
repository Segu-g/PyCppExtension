#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>
#include <tuple>

#include "error_wrapper.hpp"

namespace utils{
    template<typename APITypeTuple, typename TypeEnum>
    class CAPIManager{
    public:
        std::string get_capsule_name();
        CAPIManager(std::string name);
        ~CAPIManager();
        void import();
        template <TypeEnum api>
        auto function();

    private:
        std::string capsule_name;
        bool is_imported;
        PyObject* capsule;
        void** c_apis;
    };






    template<typename APITypeTuple, typename TypeEnum>
    CAPIManager<APITypeTuple, TypeEnum>::CAPIManager(std::string name){
        this->capsule_name = name;
    }

    template<typename APITypeTuple, typename TypeEnum>
    CAPIManager<APITypeTuple, TypeEnum>::~CAPIManager(){
        Py_XDECREF(this->capsule);
    }

    template<typename APITypeTuple, typename TypeEnum>
    std::string CAPIManager<APITypeTuple, TypeEnum>::get_capsule_name(){
        return this->capsule_name;
    }


    template<typename APITypeTuple, typename TypeEnum>
    void CAPIManager<APITypeTuple, TypeEnum>::import(){
        PyObject *object = nullptr;
        std::string trace = "";
        size_t dot_pos = 0;
        size_t last_pos = 0;

        while (dot_pos != std::string::npos) {
            dot_pos = this->capsule_name.find_first_of(".", last_pos);
            trace = this->capsule_name.substr(last_pos, dot_pos - last_pos);

            if (object == nullptr) {
                object = PyImport_ImportModule(trace.c_str());
                if (!object) {
                    throw utils::ExtException(PyExc_ImportError,
                        "CAPIManager::import() could not import module \""
                        + this->capsule_name.substr(0, last_pos)
                        + "\"");
                }
            } else {
                PyObject *object2 = PyObject_GetAttrString(object, trace.c_str());
                Py_DECREF(object);
                object = object2;
                if (object == nullptr) {
                    throw utils::ExtException(PyExc_ImportError,
                        "CAPIManager::import() could not get attribute \""
                        + this->capsule_name.substr(0, last_pos)
                        + "\"");
                }
            }
            last_pos = dot_pos + 1;
        }

        /* compare attribute name to module.name by hand */
        if (PyCapsule_IsValid(object, this->capsule_name.c_str())) {
            this->capsule = object;
            this->c_apis = reinterpret_cast<void**>(PyCapsule_GetPointer(this->capsule, this->capsule_name.c_str()));
        } else {
            throw utils::ExtException(PyExc_AttributeError,
                               "PyCapsule_Import \"" + this->capsule_name +"\" is not valid");
        }
    }

    template <typename APITypeTuple, typename TypeEnum>
    template <TypeEnum api>
    auto CAPIManager<APITypeTuple, TypeEnum>::function(){
        return reinterpret_cast<std::tuple_element_t<static_cast<int>(api), APITypeTuple>>(
            this->c_apis[static_cast<int>(api)]);
    }
}
