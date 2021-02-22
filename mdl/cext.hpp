#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdexcept>
#include <tuple>
#include <string>

#include "../extension_utils/error_wrapper.hpp"
#include "../extension_utils/capi_manager.hpp"

namespace mdl{

    struct MyInt_Object {
        PyObject_HEAD
        int value;
        PyObject* dict;
    };

    // Exported APIS
    PyObject* MyInt_add(PyObject *self, PyObject *obj);
    PyObject* MyInt_int(PyObject *self);
    PyObject* MyInt_getstate(PyObject* self, PyObject* args);
    PyObject* MyInt_setstate(PyObject* self, PyObject* arg);
    PyObject* new_MyInt_Object(PyTypeObject *subtype, PyObject *args, PyObject *keywds);
    int init_MyInt_Object(PyObject* self, PyObject* args, PyObject* keywds);
    void MyInt_dealloc(PyObject* self);

    PyObject* cext_refcnt(PyObject* module, PyObject* obj);



    enum class API: int{
        MyInt_add,
        MyInt_int,
        MyInt_getstate,
        MyInt_setstate,
        new_MyInt_Object,
        init_MyInt_Object,
        MyInt_dealloc,
        cext_refcnt,
        API_NUM
    };

    using API_TYPES = std::tuple<
        decltype(&MyInt_add),
        decltype(&MyInt_int),
        decltype(&MyInt_getstate),
        decltype(&MyInt_setstate),
        decltype(&new_MyInt_Object),
        decltype(&init_MyInt_Object),
        decltype(&MyInt_dealloc),
        decltype(&cext_refcnt)
    >;

    using CextCAPIManager = utils::CAPIManager<API_TYPES, API>;
}
