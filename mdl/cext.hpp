#pragma once
#pragma warning( disable : 4290 )


#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "extension_utils\error_wrapper.hpp"
#include <stdexcept>
#include <typeinfo>
#include <array>
#include <string>

namespace mdl{

    // Exported APIS
    PyObject* MyInt_add(PyObject *self, PyObject *obj);
    PyObject* MyInt_int(PyObject *self);
    PyObject* MyInt_getstate(PyObject* self, PyObject* args);
    PyObject* MyInt_setstate(PyObject* self, PyObject* arg);
    PyObject* new_MyInt_Object(PyTypeObject *subtype, PyObject *args, PyObject *keywds);
    int init_MyInt_Object(PyObject* self, PyObject* args, PyObject* keywds);
    void MyInt_dealloc(PyObject* self);

    PyObject* cext_refcnt(PyObject* module, PyObject* obj);

    //
    enum class CextAPI: int{
        MyInt_add,
        MyInt_int,
        MyInt_getstate,
        MyInt_setstate,
        new_MyInt_Object,
        init_MyInt_Object,
        MyInt_dealloc,
        API_NUM
    };

    using CextAPI_TYPES = std::tuple<
        decltype(MyInt_add),
        decltype(MyInt_int),
        decltype(MyInt_getstate),
        decltype(MyInt_setstate),
        decltype(new_MyInt_Object),
        decltype(init_MyInt_Object),
        decltype(MyInt_dealloc)
    >;

    class CextCapsule{
    public:
        std::string capsule_name = "mdl.cext._C_API";
        CextCapsule() throw(ExtException);
        ~CextCapsule(){
            Py_XDECREF(capsule);
        }

        template <CextAPI api>
        auto function() throw(ExtException){
            return reinterpret_cast<
                std::tuple_element_t<static_cast<int>(api), CextAPI_TYPES>>>(
                    this->c_apis[static_cast<int>(api)]);
        }

    private:
        PyObject* capsule;
        void **c_apis = nullptr;
    };

    CextCapsule::CextCapsule(){
        PyObject *object = nullptr;
        std::string trace = "";
        size_t dot_pos = dot_pos = this->capsule_name.find_first_of(".", 0);
        size_t last_pos = 0;

        while (dot_pos != std::string::npos) {
            trace = this->capsule_name.substr(last_pos, dot_pos - last_pos);

            if (object == nullptr) {
                object = PyImport_ImportModule(trace.c_str());
                if (!object) {
                    throw ExtException(PyExc_ImportError,
                        "CextCapsule() could not import module \""
                        + this->capsule_name.substr(0, last_pos)
                        + "\"");
                }
            } else {
                PyObject *object2 = PyObject_GetAttrString(object, trace.c_str());
                Py_DECREF(object);
                object = object2;
                if (object == nullptr) {
                    throw ExtException(PyExc_ImportError,
                        "CextCapsule() could not get attribute \""
                        + this->capsule_name.substr(0, last_pos)
                        + "\"");
                }
            }
            last_pos = dot_pos + 1;
            dot_pos = this->capsule_name.find_first_of(".", last_pos);
        }

        /* compare attribute name to module.name by hand */
        if (PyCapsule_IsValid(object, this->capsule_name.c_str())) {
            this->capsule = object;
            this->c_apis = reinterpret_cast<void**>(this->capsule);
        } else {
            throw ExtException(PyExc_AttributeError,
                               "PyCapsule_Import \"" + this->capsule_name +"\" is not valid");
        }
    }


}
